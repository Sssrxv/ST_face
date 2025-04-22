#include "fpc1150.h"
#include "spi.h"
#include "gpio.h"
#include "aiva_sleep.h"
#include "syslog.h" // for LOG*
#include <string.h>
#include <stdlib.h>

#define FINGERPRINT_SPI_DEV               SPI_DEVICE_1
#define FINGERPRINT_SPI_WORKMODE          SPI_WORK_MODE_0
#define FINGERPRINT_SPI_CS                SPI_CHIP_SELECT_0
#define FINGERPRINT_SPI_CLK               4*1000*1000
#define FINGERPRINT_RESET_PIN             GPIO_PIN31

#define FPC1150_REG_FPC_STATUS            0x14      /* RO, 1 bytes */
#define FPC1150_REG_RD_ERROR_CLR          0x38      /* RO, 1 byte */
#define FPC1150_REG_MISO_EDGE_RIS_EN      0x40      /* WO, 1 byte */
#define FPC1150_REG_FPC_CONFIG            0x44      /* RW, 1 byte */
#define FPC1150_REG_IMG_SMPL_SETUP        0x4C      /* RW, 3 bytes */
#define FPC1150_REG_CLOCK_CONFIG          0x50      /* RW, 1 byte */
#define FPC1150_REG_IMG_CAPT_SIZE         0x54      /* RW, 4 bytes */
#define FPC1150_REG_IMAGE_SETUP           0x5C      /* RW, 1 byte */
#define FPC1150_REG_ADC_TEST_CTRL         0x60      /* RW, 1 byte */
#define FPC1150_REG_IMG_RD                0x64      /* RW, 1 byte */
#define FPC1150_REG_SAMPLE_PX_DLY         0x68      /* RW, 8 bytes */
#define FPC1150_REG_PXL_RST_DLY           0x6C      /* RW, 1 byte */
#define FPC1150_REG_TST_COL_PATTERN_EN    0x78      /* RW, 2 bytes */
#define FPC1150_REG_CLK_BIST_RESULT       0x7C      /* RW, 4 bytes */
#define FPC1150_REG_ADC_WEIGHT_SETUP      0x84      /* RW, 1 byte */
#define FPC1150_REG_ANA_TEST_MUX          0x88      /* RW, 4 bytes */
#define FPC1150_REG_FINGER_DRIVE_CONF     0x8C      /* RW, 1 byte */
#define FPC1150_REG_FINGER_DRIVE_DLY      0x90      /* RW, 1 byte */
#define FPC1150_REG_OSC_TRIM              0x94      /* RW, 2 bytes */
#define FPC1150_REG_ADC_WEIGHT_TABLE      0x98      /* RW, 10 bytes */
#define FPC1150_REG_ADC_SETUP             0x9C      /* RW, 5 bytes */
#define FPC1150_REG_ADC_SHIFT_GAIN        0xA0      /* RW, 2 bytes */
#define FPC1150_REG_BIAS_TRIM             0xA4      /* RW, 1 byte */
#define FPC1150_REG_PXL_CTRL              0xA8      /* RW, 2 bytes */
#define FPC1150_REG_FPC_DEBUG             0xD0      /* RO, 1 bytes */
#define FPC1150_REG_FINGER_PRESENT_STATUS 0xD4      /* RO, 2 bytes */
#define FPC1150_REG_FNGR_DET_THRES        0xD8      /* RW, 1 byte */
#define FPC1150_REG_FNGR_DET_CNTR         0xDC      /* RW, 2 bytes */
#define FPC1150_REG_HWID                  0xFC      /* RO, 2 bytes */

#define FPC1150_CMD_RD_INT                0x18      /* RO, 1 byte */
#define FPC1150_CMD_RD_INT_CLR            0x1C      /* RO, 1 byte */
#define FPC1150_CMD_FINGER_PRESENT_QUERY  0x20
#define FPC1150_CMD_WAIT_FINGER_PRESENT   0x24

#define FPC1150_CMD_ACT_SLEEP_MODE        0x28
#define FPC1150_CMD_ACT_DEEP_SLEEP_MODE   0x2c
#define FPC1150_CMD_ACT_IDLE_MODE         0x34
#define FPC1150_CMD_CAPTURE_IMAGE         0xC0
#define FPC1150_CMD_RD_IMAGE              0xC4
#define FPC1150_CMD_SOFT_RESET            0xF8

#define FP_IMG_WIDTH                      80
#define FP_IMG_HEIGHT                     208
#define FP_IMG_BUF_SIZE                   (FP_IMG_WIDTH*FP_IMG_HEIGHT)

typedef struct fpc1150_cmd {
    uint8_t address;
    uint8_t read_num;
    uint8_t read_buf[10];
    uint8_t write_buf[10];
} fpc1150_cmd_t;

static fpc1150_cmd_t m_fpc1150_cmd;
static int m_driver_initialized = 0;

static uint8_t m_gain = 0x2;   // 0a
static uint8_t m_shift = 0x0a; // 05

static void fpc1150_cmd_reg_rw()
{
    size_t trans_num_bytes = m_fpc1150_cmd.read_num + 1;
    for (int i = trans_num_bytes - 1; i >= 0; i--)
    {
        // memcpy();
        if (i == 0)
            m_fpc1150_cmd.write_buf[i] = m_fpc1150_cmd.address;
        else
            m_fpc1150_cmd.write_buf[i] = m_fpc1150_cmd.write_buf[i - 1];
    }
    int ret = spi_dup_send_receive_data_dma(FINGERPRINT_SPI_DEV, FINGERPRINT_SPI_CS, &m_fpc1150_cmd.write_buf[0], trans_num_bytes, &m_fpc1150_cmd.read_buf[0], trans_num_bytes);
    if (ret != 0)
        LOGE(__func__, "spi error!");
    for (int i = 0; i < m_fpc1150_cmd.read_num; i++)
    {
        // memcpy();
        m_fpc1150_cmd.read_buf[i] = m_fpc1150_cmd.read_buf[i + 1];
    }
    return;
}

static void fpc1150_scan_image()
{
    m_fpc1150_cmd.address = FPC1150_CMD_CAPTURE_IMAGE;
    m_fpc1150_cmd.read_num = 0;
    fpc1150_cmd_reg_rw();
}

static void fpc1150_com_init()
{
    spi_init(FINGERPRINT_SPI_DEV, FINGERPRINT_SPI_WORKMODE, SPI_FF_STANDARD, 8);
    spi_set_clk_rate(FINGERPRINT_SPI_DEV, FINGERPRINT_SPI_CLK);

    gpio_init();
    gpio_set_drive_mode(FINGERPRINT_RESET_PIN, GPIO_DM_OUTPUT);
}

static void fpc1150_reset()
{
    gpio_set_pin(FINGERPRINT_RESET_PIN, GPIO_PV_HIGH);
    aiva_msleep(5);
    gpio_set_pin(FINGERPRINT_RESET_PIN, GPIO_PV_LOW);
    aiva_msleep(3);
}

static int fpc1150_check_id()
{
    m_fpc1150_cmd.address = FPC1150_REG_HWID;
    m_fpc1150_cmd.read_num = 2;
    m_fpc1150_cmd.write_buf[0] = 0x00;
    m_fpc1150_cmd.write_buf[1] = 0x00;
    fpc1150_cmd_reg_rw();
    //	if((m_fpc1150_cmd.read_buf[0]==0x15) &&(m_fpc1150_cmd.read_buf[1] == 0x0B
    if (m_fpc1150_cmd.read_buf[0] == 0x15)
        return 0;
    else
        return -1;
}

static int is_finger_present()
{
    unsigned short temp, temp2;
    uint8_t flag = 0;
    uint8_t i = 0;
    temp = 0;
    m_fpc1150_cmd.address = FPC1150_CMD_FINGER_PRESENT_QUERY;
    m_fpc1150_cmd.read_num = 0;
    m_fpc1150_cmd.write_buf[0] = 0;
    fpc1150_cmd_reg_rw();
    aiva_msleep(2);

    m_fpc1150_cmd.address = FPC1150_CMD_RD_INT_CLR;
    m_fpc1150_cmd.read_num = 1;
    m_fpc1150_cmd.write_buf[0] = 0;
    fpc1150_cmd_reg_rw();

    if ((m_fpc1150_cmd.read_buf[0] == 0x81) || (m_fpc1150_cmd.read_buf[0] == 0xFF))
    {
        m_fpc1150_cmd.address = FPC1150_CMD_WAIT_FINGER_PRESENT; // again ...
        m_fpc1150_cmd.read_num = 0;
        m_fpc1150_cmd.write_buf[0] = 0;
        fpc1150_cmd_reg_rw();
        aiva_msleep(2);
        m_fpc1150_cmd.address = FPC1150_CMD_RD_INT_CLR;
        m_fpc1150_cmd.read_num = 1;
        m_fpc1150_cmd.write_buf[0] = 0;
        fpc1150_cmd_reg_rw();

        if (m_fpc1150_cmd.read_buf[0] == 0x81)
        {
            m_fpc1150_cmd.address = FPC1150_REG_FINGER_PRESENT_STATUS;
            m_fpc1150_cmd.read_num = 2;
            m_fpc1150_cmd.write_buf[0] = 0;
            m_fpc1150_cmd.write_buf[1] = 0;
            fpc1150_cmd_reg_rw();

            if ((m_fpc1150_cmd.read_buf[0] != 0) || (m_fpc1150_cmd.read_buf[1] != 0))
            {
                flag = 0;
                temp = (m_fpc1150_cmd.read_buf[0] << 8) | (m_fpc1150_cmd.read_buf[1]);
                for (i = 0; i < 12; i++)
                {
                    temp2 = ((temp >> i) & (0x01));
                    if (temp2)
                    {
                        flag++;
                    }
                }
                if (flag >= 0x6)
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

// NOTE: to compatible with origin algorithm, return 1 when fail and 0 when success
#ifdef USE_CAC
#ifndef CAC_IN_IMG
static uint8_t spi_cac_rw(uint8_t cmd, uint8_t dummy_byte_len, uint8_t *cmd_para/* cmd para buffer point,output*/,uint8_t *resp/*resp data buffer point,input*/,unsigned int cmd_para_len/* only cmd para length */)
{
    int trans_bytes = sizeof(cmd) + dummy_byte_len + cmd_para_len;
    uint8_t *trans_buf = (uint8_t*)calloc(1, trans_bytes);
    configASSERT(trans_buf != NULL);
    uint8_t *recv_buf = (uint8_t*)calloc(1, trans_bytes);
    configASSERT(recv_buf != NULL);
    //
    trans_buf[0] = cmd;
    if (cmd_para)
        memcpy(trans_buf + sizeof(cmd) + dummy_byte_len, cmd_para, cmd_para_len);
    int ret = spi_dup_send_receive_data_dma(FINGERPRINT_SPI_DEV, FINGERPRINT_SPI_CS, trans_buf, trans_bytes, recv_buf, trans_bytes);
    if (ret == 0)
    {
        if (resp)
        {
            memcpy(resp, recv_buf + sizeof(cmd) + dummy_byte_len, cmd_para_len);
        }
        else
        {
            LOGE(__func__, "null resp!!!");
        }
    }
    free(trans_buf);
    free(recv_buf);

    return ret == 0 ? 0 : 1;
}
#endif
#endif

static int fpc1150_read_image(uint8_t *img_buf, uint16_t buf_len)
{
    configASSERT(img_buf && buf_len >= FP_IMG_BUF_SIZE);
    if (!m_driver_initialized)
    {
        LOGE(__func__, "must init first!!!");
        return -1;
    }

    unsigned int num = 0;
    fpc1150_scan_image();
    m_fpc1150_cmd.read_buf[0] = 0;
    while (1)
    {
        m_fpc1150_cmd.address = FPC1150_CMD_RD_INT_CLR;
        m_fpc1150_cmd.read_num = 1;
        m_fpc1150_cmd.write_buf[0] = 0;
        fpc1150_cmd_reg_rw();
        aiva_msleep(1);
        num++;
        if (num > 100)
            return -1;
        if (m_fpc1150_cmd.read_buf[0] == 0x20)
            break;
    }

    uint8_t cmd[] = {FPC1150_CMD_RD_IMAGE, 0x00};

    m_fpc1150_cmd.address = FPC1150_REG_RD_ERROR_CLR;
    m_fpc1150_cmd.read_num = 1;
    m_fpc1150_cmd.write_buf[0] = 0;
    fpc1150_cmd_reg_rw();

    int ret = spi_receive_data_standard_dma(FINGERPRINT_SPI_DEV, FINGERPRINT_SPI_CS, &cmd[0], sizeof(cmd), img_buf, FP_IMG_BUF_SIZE);
    if (ret == 0)
    {
        m_fpc1150_cmd.address = FPC1150_REG_RD_ERROR_CLR;
        m_fpc1150_cmd.read_num = 1;
        m_fpc1150_cmd.write_buf[0] = 0;
        fpc1150_cmd_reg_rw();
    }

    return ret;
}

int fpc1150_init(void)
{
    fpc1150_com_init();
    //
    fpc1150_reset();

    int ret = fpc1150_check_id();
    if (ret)
    {
        LOGE(__func__, "read sensor id failed!");
        return ret;
    }

    m_fpc1150_cmd.address = FPC1150_CMD_RD_INT_CLR;
    m_fpc1150_cmd.read_num = 1;
    m_fpc1150_cmd.write_buf[0] = 0x00;
    fpc1150_cmd_reg_rw();
    aiva_msleep(2);

    m_fpc1150_cmd.address = FPC1150_REG_FINGER_DRIVE_CONF;
    m_fpc1150_cmd.read_num = 1;
    m_fpc1150_cmd.write_buf[0] = 0x12; // 0x12;
    fpc1150_cmd_reg_rw();
    aiva_msleep(2);

    m_fpc1150_cmd.address = FPC1150_REG_ADC_SHIFT_GAIN;
    m_fpc1150_cmd.read_num = 2;
    m_fpc1150_cmd.write_buf[0] = m_shift; // 0x06;//m_shift;
    m_fpc1150_cmd.write_buf[1] = m_gain;  // 0x0F;//m_gain;
    fpc1150_cmd_reg_rw();
    aiva_msleep(2);

    // When the pxlCtrl register is accessed, it is required that fSPICLK <= 3MHz
    m_fpc1150_cmd.address = FPC1150_REG_PXL_CTRL;
    m_fpc1150_cmd.read_num = 2;
    m_fpc1150_cmd.write_buf[0] = 0x0F; // 0x0F;//0x0F;
    m_fpc1150_cmd.write_buf[1] = 0x0A; // 0x0A(FPC2050)//0x1A(none FPC2050)
    fpc1150_cmd_reg_rw();
    aiva_msleep(2);

    m_fpc1150_cmd.address = FPC1150_REG_IMAGE_SETUP;
    m_fpc1150_cmd.read_num = 1;
    m_fpc1150_cmd.write_buf[0] = 0x0B;
    fpc1150_cmd_reg_rw();
    aiva_msleep(2);

    m_fpc1150_cmd.address = FPC1150_REG_FNGR_DET_THRES;
    m_fpc1150_cmd.read_num = 1;
    m_fpc1150_cmd.write_buf[0] = 0x30;
    fpc1150_cmd_reg_rw();
    aiva_msleep(2);

    m_fpc1150_cmd.address = FPC1150_REG_FNGR_DET_CNTR;
    m_fpc1150_cmd.read_num = 2;
    m_fpc1150_cmd.write_buf[0] = 0x00;
    m_fpc1150_cmd.write_buf[1] = 0x32; // 0x23;//0x08;//0x32;
    fpc1150_cmd_reg_rw();
    aiva_msleep(2);

#ifdef USE_CAC
#ifndef CAC_IN_IMG
    fpc_cac_init(SENSOR_FPC1155, spi_cac_rw, (uint8_t *)m_finger_image_buffer);
    fpc_cac_SetFingerThres(9);
#endif
#endif

    m_driver_initialized = 1;

    return 0;
}

void fpc1150_get_img_resolution(int *width, int *height)
{
    *width = FP_IMG_WIDTH;
    *height = FP_IMG_HEIGHT;
}

int fpc1150_read_fingerprint(uint8_t *img_buf, uint16_t buf_len)
{
    configASSERT(img_buf && buf_len >= FP_IMG_BUF_SIZE);
    int ret = -1;

    if (!m_driver_initialized)
    {
        LOGE(__func__, "must init first!!!");
        return -1;
    }
    // make sure device is correctly connected
    int i = 0;
    const int test_cnt = 5;
    for(i = 0; i < test_cnt; i++)
    {
        if(0 == fpc1150_check_id())
            break;
    }

    if(i >= test_cnt)
        fpc1150_init();

    if (!is_finger_present())
    {
        return ret;
    }

#ifdef USE_CAC
#ifdef CAC_IN_IMG
    fpc_cac_init(SENSOR_FPC1155, spi_cac_rw, img_buf);
    fpc_cac_SetFingerThres(9);
#endif
    int cac_status = fpc_cac_start();
    if (cac_status != 0) // klz20171114
    {
        cac_status = fpc_cac_stop();
        return 1;
    }
    aiva_msleep(2);
    cac_status = fpc_cac_stop();
    if (cac_status != 0)
    {
        return 0; //
    }
#else
    ret = fpc1150_read_image(img_buf, buf_len);
    return ret;
#endif
}

int fpc1150_sleep()
{
    if (!m_driver_initialized)
    {
        LOGE(__func__, "must init first!!!");
        return -1;
    }

    int ret = -1;

    m_fpc1150_cmd.address = FPC1150_CMD_ACT_SLEEP_MODE;
    m_fpc1150_cmd.read_num = 0;
    fpc1150_cmd_reg_rw();
    for (int i = 0; i < 5; i++)
    {
        m_fpc1150_cmd.address = FPC1150_REG_FPC_STATUS;
        m_fpc1150_cmd.read_num = 2;
        m_fpc1150_cmd.write_buf[0] = 0;
        m_fpc1150_cmd.write_buf[1] = 0;
        fpc1150_cmd_reg_rw();

        if ((m_fpc1150_cmd.read_buf[1] & 0x0E) == 0x00)
        {
            ret = 0;
            break;
        }
        aiva_msleep(10);
    }

    return ret;
}
