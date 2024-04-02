#include "syslog.h"
#include "os_startup.h"
#include "os_init_app.h"
#include "os_shell_app.h"
#include "net.h"
#include "aiva_malloc.h"
#include "sysctl.h"
#include "fs_file.h"
#include "xnn_hal_substract_mean_normalize.h"
#include <stdlib.h>
#include <stdbool.h>

static xnn::Net m_net;
static const char* xfile_name = "close_eye_detection.xfile";
static const char* input_file_name[] = {"close_eye_32x32_rgba.bin", "open_eye_32x32_rgba.bin"};
static const int input_w = 32;
static const int input_h = 32;
static const int input_c = 4;

/*
 说明： 本示例程序演示在芯片上使用xnn进行模型推理，本示例程序配合《aiva_xnn_tools》使用
 1.模型文件由《aiva_xnn_tools》生成
 2.xnn使用的时候需要特别注意内存的对齐问题和cache问题:本示例程序中可以搜索“CACHE NOTE”查看关键代码位置
 (1) 模型文件
     xfile内存需要满足16字节对齐，需要使用aiva_malloc_aligned(size, 32)申请（该函数最低支持32字节对齐，所以这里填写32）
     加载完成xfile之后，需要调用dcache_flush函数刷cache
 (2) xnn work buffer，用于xnn保存中间feature map
     xnn work buffer同xfile一样需要满足16字节对齐要求，aiva_malloc_aligned(size, 32)申请
     申请完成后调用dcache_flush函数刷cache
（3）xnn网络的输入图像，在完成cpu预处理之后，输入到xnn之前，需要调用dcache_flush函数刷cache
*/

static int load_model(const uint8_t* xfile_mem, int len)
{
    int ret;
    ret = m_net.load_xfile(xfile_mem, len);
    if (ret != 0)
    {
        LOGE(__func__, "liveness ir load model fail: addr=0x%x, len=%d", xfile_mem, len);
        return -1;
    }

    return 0;
}

static int run_model(const xnn::Mat &input_img, float &score)
{
    float mean[3];
    float norm[3];

    m_net.get_mean_value(mean);
    m_net.get_norm_value(norm);

    sysctl_clock_enable(SYSCTL_CLOCK_XNN);

    xnn::Mat in;
    if (input_img.dataformat == xnn::Mat::DATA_FORMAT_HWC_BGR) {
        in = xnn::Mat::from_pixels((unsigned char*)input_img.data, xnn::Mat::PIXEL_BGR2RGB, input_img.w, input_img.h);
        in.substract_mean_normalize(mean, norm); // CACHE NOTE: this is cpu process
        dcache_flush(in.data, in.cstep * in.c * in.elemsize); // CACHE NOTE: in.substract_mean_normalize is cpu process, next in buffer will be send to xnn, so we should flush cache
    } else if (input_img.dataformat == xnn::Mat::DATA_FORMAT_HWC_RGBA) {
        float input_blob_top_scale;
        if (m_net.get_input_blob_top_scale(input_blob_top_scale) != 0) {
            LOGE(__func__, "get input blob top scale fail");
            sysctl_clock_disable(SYSCTL_CLOCK_XNN);
            return -1;
        }
        //LOGI(__func__, "IR input_blob_top_scale: %f", input_blob_top_scale);
        // CACHE NOTE: xnn::xnn_hal_substract_mean_normalize will handle cache, so we don't flush cache here 
        xnn::xnn_hal_substract_mean_normalize((const uint8_t*)input_img.data,
                                              (int)input_img.w * input_c,
                                              (mean[0] + mean[1] + mean[2]) / 3,
                                              (norm[0] + norm[1] + norm[2]) / 3,
                                              input_blob_top_scale,
                                              input_img.w,
                                              input_img.h,
                                              in);
        // Note: the output xnn Mat in is C16N format
    } else {
        LOGE(__func__, "invalid input data format: %d", input_img.dataformat);
        sysctl_clock_disable(SYSCTL_CLOCK_XNN);
        return -1;
    }

    xnn::Extractor ex = m_net.create_extractor();
    ex.input("input", in);

    xnn::Mat output;
    ex.extract("output", output);

    if (output.elemsize != 4) {
        LOGE(__func__, "output elemsize error: %d", output.elemsize);
        sysctl_clock_disable(SYSCTL_CLOCK_XNN);
        return -1;
    }

    float val0 = ((float*)output.data)[0];
    float val1 = ((float*)output.data)[1];
    float max = std::max(val0, val1);
    val0 = std::exp(val0-max);
    val1 = std::exp(val1-max);
    float sum = val0 + val1;
    score = val1 / sum;
    sysctl_clock_disable(SYSCTL_CLOCK_XNN);

    return 0;
}

static uint8_t* file2buf(const char *file, bool flush_cache)
{
    fs_file_t *fp = fs_file_open(file, FS_O_RDONLY);
    if (!fp) {
        LOGE(__func__, "read file %s failed", file);
        return NULL;
    }

    uint32_t file_size = fs_file_size(fp);

    uint8_t *buf = (uint8_t*)aiva_malloc_aligned(file_size, 32);
    if (buf == NULL) {
        LOGE(__func__, "malloc failed with size:%u", file_size);
        return NULL;
    }

    fs_file_read(fp, buf, file_size);

    if (flush_cache)
        dcache_flush(buf, file_size);

    if (fp) {
        fs_file_close(fp);
    }

    return buf;
}

static int get_filesize(const char* file)
{
    fs_file_t *fp = fs_file_open(file, FS_O_RDONLY);
    if (!fp) {
        LOGE(__func__, "read file %s failed", file);
        return 0;
    }

    return fs_file_size(fp);
}

static int xnn_inference_entry(void)
{
    int ret = 0;
    uint8_t* xfile = NULL;
    uint8_t* xnn_workbuffer = NULL;

    os_mount_fs();
    //
    os_shell_app_entry();


    do
    {
        xnn::Mat xnn_input;

        // 1. read xnn xfile from file system
        bool flush_cache = true; // CACHE NOTE: must make sure xfile buffer been dcache_flushed
        xfile = file2buf(xfile_name, flush_cache);
        if (xfile == NULL) {
            LOGE(__func__, "read xfile failed");
            ret = -1;
            break;
        }
        // 2. load model
        ret = load_model(xfile, get_filesize(xfile_name));
        if (ret) {
            LOGE(__func__, "load model failed");
            ret = -1;
            break;
        }
        // 3. prepare work buffer for this model; only need to provide maximum workbuffer if there are multipy xnn models
        int workbuffer_size = m_net.get_workbuffer_size();
        xnn_workbuffer = (uint8_t*)aiva_malloc_aligned(workbuffer_size, 32);
        // TODO: release old workbuffer when call set_workbuffer
        dcache_flush((void*)xnn_workbuffer, workbuffer_size); // CACHE NOTE: must make sure xnn work buffer been dcache_flushed
        m_net.set_workbuffer((uint32_t)xnn_workbuffer, workbuffer_size);

        // 4. inference
        for (unsigned int i = 0; i < sizeof(input_file_name)/sizeof(input_file_name[0]); i++)
        {
            const char *file_name = input_file_name[i];
            flush_cache = false;
            uint8_t* input_img = file2buf(file_name, flush_cache); // CACHE NOTE: since run_model will handle cache flush, so here we not flush cache
            if (input_img == NULL) {
                LOGE(__func__, "read input_img failed");
                continue;
            }
            // Mat(int w, int h, int c, void* data, size_t elemsize, int packing, DataFormat dataformat, Allocator* allocator = 0);
            xnn::Mat input(input_w, input_h, input_c, input_img, 1u, 1, xnn::Mat::DATA_FORMAT_HWC_RGBA);
            float score = 0;
            ret = run_model(input, score);
            if (ret == 0) {
                LOGI(__func__, "img:%s is eye closed:%d score is:%f", file_name, (int)(score < 0.5), score);
            } else {
                LOGE(__func__, "run model failed");
            }
            if (input_img) {
                free(input_img);
                input_img = NULL;
            }
        }
    } while (0);

    // release buffers
    if (xfile) {
        free(xfile);
        xfile = NULL;
    }

    if (xnn_workbuffer) {
        free(xnn_workbuffer);
        xnn_workbuffer = NULL;
    }

    return ret;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    /* Register custom entry */
    os_app_set_custom_entry(xnn_inference_entry);

    /* Start scheduler, dead loop */
    os_app_main_entry();

    return 0;
}
