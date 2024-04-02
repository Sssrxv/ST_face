#include "i2c.h"
#include "gpio.h"
#include "sysctl.h"
#include "udevice.h"
#include "audio_dev.h"
#include "audio_esxxxx_common.h"
#include "syslog.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define ES8374_TAG "ES8374_DRIVER"

typedef enum audio_ctrl {
    AUDIO_CTRL_STOP  = 0x00,  /** set stop mode */
    AUDIO_CTRL_START = 0x01,  /** set start mode */
} audio_ctrl_t;

int audio_es8374_write_reg(audio_dev_ctrl_interface_t *i2c_iface, uint8_t reg_addr, uint8_t reg_val)
{
    int ret;
    uint8_t data_buf[2];
    data_buf[0] = reg_addr & 0xff;
    data_buf[1] = reg_val & 0xff;

    ret = i2c_send_data(i2c_iface->i2c_num, i2c_iface->i2c_addr, data_buf, 2);
    if (ret < 0) {
        LOGE(__func__, "send addr error: ret is %d\n", ret);
    }
    return ret;
}

int audio_es8374_read_reg(audio_dev_ctrl_interface_t *i2c_iface, uint8_t reg_addr, uint8_t *reg_val)
{
    int ret = 0;

    uint8_t addr_buf[1];
    addr_buf[0] = reg_addr & 0xff;
    
    ret = i2c_recv_data(i2c_iface->i2c_num, i2c_iface->i2c_addr, addr_buf, 1, reg_val, 1);
    if (ret < 0) {
        LOGE(__func__, "recv error");
    }
    
    return ret;
}

void audio_es8374_read_all(audio_dev_ctrl_interface_t *i2c_iface)
{
    for (int i = 0; i < 50; i++) {
        uint8_t reg = 0;
        audio_es8374_read_reg(i2c_iface, i, &reg);
        LOGI(ES8374_TAG, "%x: %x", i, reg);
    }
}

int audio_es8374_set_voice_mute(audio_dev_t *dev, bool enable)
{
    int res = 0;
    uint8_t reg = 0;

    audio_dev_ctrl_interface_t *i2c_iface = &dev->cfg.i2c_iface;
    res |= audio_es8374_read_reg(i2c_iface, 0x36, &reg);
    if (res == 0) {
        reg = reg & 0xdf;
        res |= audio_es8374_write_reg(i2c_iface, 0x36, reg | (((int)enable) << 5));
    }

    return res;
}

int audio_es8374_get_voice_mute(audio_dev_t *dev)
{
    int res = 0;
    uint8_t reg = 0;

    res |= audio_es8374_read_reg(&dev->cfg.i2c_iface, 0x36, &reg);
    if (res == 0) {
        reg = reg & 0x40;
    }

    return res == 0 ? reg : res;
}

int audio_es8374_set_bits_per_sample(audio_dev_ctrl_interface_t *i2c_iface, es_module_t mode, es_bits_length_t bit_per_sample)
{
    int res = 0;
    uint8_t reg = 0;
    int bits = (int)bit_per_sample & 0x0f;

    if (mode == ES_MODULE_ADC || mode == ES_MODULE_ADC_DAC) {
        res |= audio_es8374_read_reg(i2c_iface, 0x10, &reg);
        if (res == 0) {
            reg = reg & 0xe3;
            res |=  audio_es8374_write_reg(i2c_iface, 0x10, reg | (bits << 2));
        }
    }
    if (mode == ES_MODULE_DAC || mode == ES_MODULE_ADC_DAC) {
        res |= audio_es8374_read_reg(i2c_iface, 0x11, &reg);
        if (res == 0) {
            reg = reg & 0xe3;
            res |= audio_es8374_write_reg(i2c_iface, 0x11, reg | (bits << 2));
        }
    }

    return res;
}

int audio_es8374_config_fmt(audio_dev_ctrl_interface_t *i2c_iface, es_module_t mode, es_i2s_fmt_t fmt)
{
    int res = 0;
    uint8_t reg = 0;
    int fmt_tmp, fmt_i2s;

    fmt_tmp = ((fmt & 0xf0) >> 4);
    fmt_i2s =  fmt & 0x0f;
    if (mode == ES_MODULE_ADC || mode == ES_MODULE_ADC_DAC) {
        res |= audio_es8374_read_reg(i2c_iface, 0x10, &reg);
        if (res == 0) {
            reg = reg & 0xfc;
            res |= audio_es8374_write_reg(i2c_iface, 0x10, reg | fmt_i2s);
            res |= audio_es8374_set_bits_per_sample(i2c_iface, mode, fmt_tmp);
        }
    }
    if (mode == ES_MODULE_DAC || mode == ES_MODULE_ADC_DAC) {
        res |= audio_es8374_read_reg(i2c_iface, 0x11, &reg);
        if (res == 0) {
            reg = reg & 0xfc;
            res |= audio_es8374_write_reg(i2c_iface, 0x11, reg | (fmt_i2s));
            res |= audio_es8374_set_bits_per_sample(i2c_iface, mode, fmt_tmp);
        }
    }

    return res;
}

int audio_es8374_start(audio_dev_t *dev, es_module_t mode)
{
    int res = 0;
    uint8_t reg = 0;

    audio_dev_ctrl_interface_t *i2c_iface = &dev->cfg.i2c_iface;
    if (mode == ES_MODULE_LINE) {
        res |= audio_es8374_read_reg(i2c_iface, 0x1a, &reg);       //set monomixer
        if (res) return res;
        reg |= 0x60;
        reg |= 0x20;
        reg &= 0xf7;
        res |= audio_es8374_write_reg(i2c_iface,  0x1a, reg);
        res |= audio_es8374_read_reg(i2c_iface, 0x1c, &reg);        // set spk mixer
        reg |= 0x40;
        res |= audio_es8374_write_reg(i2c_iface,  0x1c, reg);
        res |= audio_es8374_write_reg(i2c_iface, 0x1D, 0x02);      // spk set
        res |= audio_es8374_write_reg(i2c_iface, 0x1F, 0x00);      // spk set
        res |= audio_es8374_write_reg(i2c_iface, 0x1E, 0xA0);      // spk on
    }
    if (mode == ES_MODULE_ADC || mode == ES_MODULE_ADC_DAC || mode == ES_MODULE_LINE) {
        res |= audio_es8374_read_reg(i2c_iface, 0x21, &reg);       //power up adc and input
        if (res) return res;
        reg &= 0x3f;
        res |= audio_es8374_write_reg(i2c_iface, 0x21, reg);
        res |= audio_es8374_read_reg(i2c_iface, 0x10, &reg);       //power up adc and input
        reg &= 0x3f;
        res |= audio_es8374_write_reg(i2c_iface, 0x10, reg);
    }

    if (mode == ES_MODULE_DAC || mode == ES_MODULE_ADC_DAC || mode == ES_MODULE_LINE) {
        res |= audio_es8374_read_reg(i2c_iface, 0x1a, &reg);       //disable lout
        if (res) return res;
        reg |= 0x08;
        res |= audio_es8374_write_reg(i2c_iface,  0x1a, reg);
        reg &= 0xdf;
        res |= audio_es8374_write_reg(i2c_iface,  0x1a, reg);
        res |= audio_es8374_write_reg(i2c_iface, 0x1D, 0x12);      // mute speaker
        res |= audio_es8374_write_reg(i2c_iface, 0x1E, 0x20);      // disable class d
        res |= audio_es8374_read_reg(i2c_iface, 0x15, &reg);        //power up dac
        reg &= 0xdf;
        res |= audio_es8374_write_reg(i2c_iface, 0x15, reg);
        res |= audio_es8374_read_reg(i2c_iface, 0x1a, &reg);        //disable lout
        reg |= 0x20;
        res |= audio_es8374_write_reg(i2c_iface,  0x1a, reg);
        reg &= 0xf7;
        res |= audio_es8374_write_reg(i2c_iface,  0x1a, reg);
        res |= audio_es8374_write_reg(i2c_iface, 0x1D, 0x02);      // mute speaker
        res |= audio_es8374_write_reg(i2c_iface, 0x1E, 0xa0);      // disable class d

        res |= audio_es8374_set_voice_mute(dev, false);
    }

    return res;
}

int audio_es8374_stop(audio_dev_t *dev, es_module_t mode)
{
    int res = 0;
    uint8_t reg = 0;

    audio_dev_ctrl_interface_t *i2c_iface = &dev->cfg.i2c_iface;
    if (mode == ES_MODULE_LINE) {
        res |= audio_es8374_read_reg(i2c_iface, 0x1a, &reg);       //disable lout
        if (res) return res;
        reg |= 0x08;
        res |= audio_es8374_write_reg(i2c_iface,  0x1a, reg);
        reg &= 0x9f;
        res |= audio_es8374_write_reg(i2c_iface,  0x1a, reg);
        res |= audio_es8374_write_reg(i2c_iface, 0x1D, 0x12);      // mute speaker
        res |= audio_es8374_write_reg(i2c_iface, 0x1E, 0x20);      // disable class d
        res |= audio_es8374_read_reg(i2c_iface, 0x1c, &reg);        // disable spkmixer
        reg &= 0xbf;
        res |= audio_es8374_write_reg(i2c_iface,  0x1c, reg);
        res |= audio_es8374_write_reg(i2c_iface, 0x1F, 0x00);      // spk set
    }
    if (mode == ES_MODULE_DAC || mode == ES_MODULE_ADC_DAC) {
        res |= audio_es8374_set_voice_mute(dev, true);
        if (res) return res;

        res |= audio_es8374_read_reg(i2c_iface, 0x1a, &reg);        //disable lout
        reg |= 0x08;
        res |= audio_es8374_write_reg(i2c_iface,  0x1a, reg);
        reg &= 0xdf;
        res |= audio_es8374_write_reg(i2c_iface,  0x1a, reg);
        res |= audio_es8374_write_reg(i2c_iface, 0x1D, 0x12);      // mute speaker
        res |= audio_es8374_write_reg(i2c_iface, 0x1E, 0x20);      // disable class d
        res |= audio_es8374_read_reg(i2c_iface, 0x15, &reg);        //power up dac
        reg |= 0x20;
        res |= audio_es8374_write_reg(i2c_iface, 0x15, reg);
    }
    if (mode == ES_MODULE_ADC || mode == ES_MODULE_ADC_DAC) {

        res |= audio_es8374_read_reg(i2c_iface, 0x10, &reg);       //power up adc and input
        if (res) return res;
        reg |= 0xc0;
        res |= audio_es8374_write_reg(i2c_iface, 0x10, reg);
        res |= audio_es8374_read_reg(i2c_iface, 0x21, &reg);       //power up adc and input
        reg |= 0xc0;
        res |= audio_es8374_write_reg(i2c_iface, 0x21, reg);
    }

    return res;
}

int audio_es8374_i2s_config_clock(audio_dev_ctrl_interface_t *i2c_iface, es_i2s_clock_t cfg)
{

    int res = 0;
    uint8_t reg = 0;

    res |= audio_es8374_read_reg(i2c_iface, 0x0f, &reg);       //power up adc and input
    reg &= 0xe0;
    int divratio = 0;
    switch (cfg.sclk_div) {
        case MCLK_DIV_1:
            divratio = 1;
            break;
        case MCLK_DIV_2: // = 2,
            divratio = 2;
            break;
        case MCLK_DIV_3: // = 3,
            divratio = 3;
            break;
        case MCLK_DIV_4: // = 4,
            divratio = 4;
            break;
        case MCLK_DIV_5: // = 20,
            divratio = 5;
            break;
        case MCLK_DIV_6: // = 5,
            divratio = 6;
            break;
        case MCLK_DIV_7: //  = 29,
            divratio = 7;
            break;
        case MCLK_DIV_8: // = 6,
            divratio = 8;
            break;
        case MCLK_DIV_9: // = 7,
            divratio = 9;
            break;
        case MCLK_DIV_10: // = 21,
            divratio = 10;
            break;
        case MCLK_DIV_11: // = 8,
            divratio = 11;
            break;
        case MCLK_DIV_12: // = 9,
            divratio = 12;
            break;
        case MCLK_DIV_13: // = 30,
            divratio = 13;
            break;
        case MCLK_DIV_14: // = 31
            divratio = 14;
            break;
        case MCLK_DIV_15: // = 22,
            divratio = 15;
            break;
        case MCLK_DIV_16: // = 10,
            divratio = 16;
            break;
        case MCLK_DIV_17: // = 23,
            divratio = 17;
            break;
        case MCLK_DIV_18: // = 11,
            divratio = 18;
            break;
        case MCLK_DIV_20: // = 24,
            divratio = 19;
            break;
        case MCLK_DIV_22: // = 12,
            divratio = 20;
            break;
        case MCLK_DIV_24: // = 13,
            divratio = 21;
            break;
        case MCLK_DIV_25: // = 25,
            divratio = 22;
            break;
        case MCLK_DIV_30: // = 26,
            divratio = 23;
            break;
        case MCLK_DIV_32: // = 27,
            divratio = 24;
            break;
        case MCLK_DIV_33: // = 14,
            divratio = 25;
            break;
        case MCLK_DIV_34: // = 28,
            divratio = 26;
            break;
        case MCLK_DIV_36: // = 15,
            divratio = 27;
            break;
        case MCLK_DIV_44: // = 16,
            divratio = 28;
            break;
        case MCLK_DIV_48: // = 17,
            divratio = 29;
            break;
        case MCLK_DIV_66: // = 18,
            divratio = 30;
            break;
        case MCLK_DIV_72: // = 19,
            divratio = 31;
            break;
        default:
            break;
    }
    reg |= divratio;
    res |= audio_es8374_write_reg(i2c_iface, 0x0f, reg);

    int dacratio_l = 0;
    int dacratio_h = 0;

    switch (cfg.lclk_div) {
        case LCLK_DIV_128:
            dacratio_l = 128 % 256;
            dacratio_h = 128 / 256;
            break;
        case LCLK_DIV_192:
            dacratio_l = 192 % 256;
            dacratio_h = 192 / 256;
            break;
        case LCLK_DIV_256:
            dacratio_l = 256 % 256;
            dacratio_h = 256 / 256;
            break;
        case LCLK_DIV_384:
            dacratio_l = 384 % 256;
            dacratio_h = 384 / 256;
            break;
        case LCLK_DIV_512:
            dacratio_l = 512 % 256;
            dacratio_h = 512 / 256;
            break;
        case LCLK_DIV_576:
            dacratio_l = 576 % 256;
            dacratio_h = 576 / 256;
            break;
        case LCLK_DIV_768:
            dacratio_l = 768 % 256;
            dacratio_h = 768 / 256;
            break;
        case LCLK_DIV_1024:
            dacratio_l = 1024 % 256;
            dacratio_h = 1024 / 256;
            break;
        case LCLK_DIV_1152:
            dacratio_l = 1152 % 256;
            dacratio_h = 1152 / 256;
            break;
        case LCLK_DIV_1408:
            dacratio_l = 1408 % 256;
            dacratio_h = 1408 / 256;
            break;
        case LCLK_DIV_1536:
            dacratio_l = 1536 % 256;
            dacratio_h = 1536 / 256;
            break;
        case LCLK_DIV_2112:
            dacratio_l = 2112 % 256;
            dacratio_h = 2112 / 256;
            break;
        case LCLK_DIV_2304:
            dacratio_l = 2304 % 256;
            dacratio_h = 2304 / 256;
            break;
        case LCLK_DIV_125:
            dacratio_l = 125 % 256;
            dacratio_h = 125 / 256;
            break;
        case LCLK_DIV_136:
            dacratio_l = 136 % 256;
            dacratio_h = 136 / 256;
            break;
        case LCLK_DIV_250:
            dacratio_l = 250 % 256;
            dacratio_h = 250 / 256;
            break;
        case LCLK_DIV_272:
            dacratio_l = 272 % 256;
            dacratio_h = 272 / 256;
            break;
        case LCLK_DIV_375:
            dacratio_l = 375 % 256;
            dacratio_h = 375 / 256;
            break;
        case LCLK_DIV_500:
            dacratio_l = 500 % 256;
            dacratio_h = 500 / 256;
            break;
        case LCLK_DIV_544:
            dacratio_l = 544 % 256;
            dacratio_h = 544 / 256;
            break;
        case LCLK_DIV_750:
            dacratio_l = 750 % 256;
            dacratio_h = 750 / 256;
            break;
        case LCLK_DIV_1000:
            dacratio_l = 1000 % 256;
            dacratio_h = 1000 / 256;
            break;
        case LCLK_DIV_1088:
            dacratio_l = 1088 % 256;
            dacratio_h = 1088 / 256;
            break;
        case LCLK_DIV_1496:
            dacratio_l = 1496 % 256;
            dacratio_h = 1496 / 256;
            break;
        case LCLK_DIV_1500:
            dacratio_l = 1500 % 256;
            dacratio_h = 1500 / 256;
            break;
        default:
            break;
    }
    res |= audio_es8374_write_reg(i2c_iface,  0x06, dacratio_h);  //ADCFsMode,singel SPEED,RATIO=256
    res |= audio_es8374_write_reg(i2c_iface,  0x07, dacratio_l);  //ADCFsMode,singel SPEED,RATIO=256

    return res;
}

int audio_es8374_config_dac_output(audio_dev_ctrl_interface_t *i2c_iface, es_dac_output_t output)
{
    (void)output;

    int res = 0;
    uint8_t reg_val = 0;
    res |= audio_es8374_read_reg(i2c_iface, 0x1c, &reg_val); // set spk mixer
    reg_val |= 0x80;
    res |= audio_es8374_write_reg(i2c_iface, 0x1c, reg_val);
    res |= audio_es8374_write_reg(i2c_iface, 0x1D, 0x02); // spk set
    res |= audio_es8374_write_reg(i2c_iface, 0x1F, 0x00); // spk set
    res |= audio_es8374_write_reg(i2c_iface, 0x1E, 0xA0); // spk on

    return res;
}

int audio_es8374_config_adc_input(audio_dev_ctrl_interface_t *i2c_iface, es_adc_input_t input)
{
    (void)input;

    int res = 0;
    uint8_t reg = 0;

    res |= audio_es8374_read_reg(i2c_iface, 0x21, &reg);
    // LOGD(ES8374_TAG, "reg is %d\n", reg);
    if (res == 0) {
        // reg = (reg & 0xcf) | 0x14; // Enable PGA for MIC1
        reg = (reg & 0xcf) | 0x24; // Enable PGA for MIC2
        res |= audio_es8374_write_reg(i2c_iface,  0x21, reg);
    }

    // Increase the ADC gain (ALC control)
    // res |= audio_es8374_write_reg(i2c_iface, 0x26, 0x52);
    res |= audio_es8374_write_reg(i2c_iface, 0x26, 0x4D);
    res |= audio_es8374_write_reg(i2c_iface, 0x27, 0x08);
    res |= audio_es8374_write_reg(i2c_iface, 0x28, 0xa2);

    return res;
}

int audio_es8374_set_mic_gain(audio_dev_ctrl_interface_t *i2c_iface, es_mic_gain_t gain)
{
    int res = 0;

    if (gain > MIC_GAIN_MIN && gain < MIC_GAIN_24DB) {
        int gain_n = 0;
        gain_n = (int)gain / 3;
        res = audio_es8374_write_reg(i2c_iface, 0x22, gain_n | (gain_n << 4)); //MIC PGA
    } else {
        res = -1;
        LOGE(ES8374_TAG, "invalid microphone gain!");
    }

    return res;
}

int audio_es8374_codec_set_voice_volume(audio_dev_t *dev, int volume)
{
    int res = 0;

    if (volume < 0) {
        volume = 192;
    } else if (volume > 96) {
        volume = 0;
    } else {
        volume = 192 - volume * 2;
    }

    res = audio_es8374_write_reg(&dev->cfg.i2c_iface, 0x38, volume);

    return res;
}

int audio_es8374_codec_get_voice_volume(audio_dev_t *dev, int *volume)
{
    int res = 0;
    uint8_t reg = 0;

    res = audio_es8374_read_reg(&dev->cfg.i2c_iface, 0x38, &reg);

    if (res < 0) {
        *volume = 0;
    } else {
        *volume = (192 - reg) / 2;
        if (*volume > 96) {
            *volume = 100;
        }
    }

    return res;
}

static int audio_es8374_set_adc_dac_volume(audio_dev_ctrl_interface_t *i2c_iface, int mode, int volume, int dot)
{
    int res = 0;

    if ( volume < -96 || volume > 0 ) {
        LOGW(ES8374_TAG, "Warning: volume < -96! or > 0!");
        if (volume < -96) {
            volume = -96;
        } else {
            volume = 0;
        }
    }
    dot = (dot >= 5 ? 1 : 0);
    volume = (-volume << 1) + dot;
    if (mode == ES_MODULE_ADC || mode == ES_MODULE_ADC_DAC) {
        res |= audio_es8374_write_reg(i2c_iface, 0x25, volume);
    }
    if (mode == ES_MODULE_DAC || mode == ES_MODULE_ADC_DAC) {
        res |= audio_es8374_write_reg(i2c_iface, 0x38, volume);
    }

    return res;
}
#if 0
static int audio_es8374_set_d2se_pga(audio_dev_ctrl_interface_t *i2c_iface, es_d2se_pga_t gain)
{
    int res = 0;
    uint8_t reg = 0;

    if (gain > D2SE_PGA_GAIN_MIN && gain < D2SE_PGA_GAIN_MAX) {
        res = audio_es8374_read_reg(i2c_iface, 0x21, &reg);
        reg &= 0xfb;
        reg |= gain << 2;
        res = audio_es8374_write_reg(i2c_iface, 0x21, reg); //MIC PGA
    } else {
        res = -1;
        LOGE(ES8374_TAG, "invalid microphone gain!");
    }

    return res;
}
#endif

static int audio_es8374_init_reg(audio_dev_t *dev, audio_iface_mode_t ms_mode, es_i2s_fmt_t fmt, es_i2s_clock_t cfg, es_dac_output_t out_channel, es_adc_input_t in_channel)
{
    int res = 0;
    uint8_t reg;

    audio_dev_ctrl_interface_t *i2c_iface = &dev->cfg.i2c_iface;
    res |= audio_es8374_write_reg(i2c_iface, 0x00, 0x3F); //IC Rst start
    res |= audio_es8374_write_reg(i2c_iface, 0x00, 0x03); //IC Rst stop
    res |= audio_es8374_write_reg(i2c_iface, 0x01, 0x7F); //IC clk on

    res |= audio_es8374_read_reg(i2c_iface, 0x0F, &reg);
    reg &= 0x7f;
    reg |=  (ms_mode << 7);
    res |= audio_es8374_write_reg(i2c_iface,  0x0f, reg); //CODEC IN I2S SLAVE MODE

    res |= audio_es8374_write_reg(i2c_iface, 0x6F, 0xA0); //pll set:mode enable
    res |= audio_es8374_write_reg(i2c_iface, 0x72, 0x41); //pll set:mode set
    // res |= audio_es8374_write_reg(i2c_iface, 0x09, 0x01); //pll set:reset on ,set start
    // res |= audio_es8374_write_reg(i2c_iface, 0x0C, 0x22); //pll set:k
    // res |= audio_es8374_write_reg(i2c_iface, 0x0D, 0x2E); //pll set:k
    // res |= audio_es8374_write_reg(i2c_iface, 0x0E, 0xC6); //pll set:k

    // res |= audio_es8374_write_reg(i2c_iface, 0x0A, 0x3A); //pll set:
    // res |= audio_es8374_write_reg(i2c_iface, 0x0B, 0x07); //pll set:n
    // res |= audio_es8374_write_reg(i2c_iface, 0x09, 0x41); //pll set:reset off ,set stop
    res |= audio_es8374_write_reg(i2c_iface, 0x09, 0x40); //pll bypass, cause master mclk is 12.288M

    res |= audio_es8374_i2s_config_clock(i2c_iface, cfg);

    res |= audio_es8374_write_reg(i2c_iface, 0x24, 0x08); //adc set
    res |= audio_es8374_write_reg(i2c_iface, 0x36, 0x00); //dac set
    res |= audio_es8374_write_reg(i2c_iface, 0x12, 0x30); //timming set
    res |= audio_es8374_write_reg(i2c_iface, 0x13, 0x20); //timming set

    res |= audio_es8374_config_fmt(i2c_iface, ES_MODULE_ADC, fmt);
    res |= audio_es8374_config_fmt(i2c_iface, ES_MODULE_DAC, fmt);

    res |= audio_es8374_write_reg(i2c_iface, 0x21, 0x50); //adc set: SEL LIN1 CH+PGAGAIN=0DB
    res |= audio_es8374_write_reg(i2c_iface, 0x22, 0xFF); //adc set: PGA GAIN=0DB
    res |= audio_es8374_write_reg(i2c_iface, 0x21, 0x14); //adc set: SEL LIN1 CH+PGAGAIN=18DB
    res |= audio_es8374_write_reg(i2c_iface, 0x22, 0x55); //pga = +15db
    res |= audio_es8374_write_reg(i2c_iface, 0x08, 0x21); //set class d divider = 33, to avoid the high frequency tone on laudspeaker
    res |= audio_es8374_write_reg(i2c_iface, 0x00, 0x80); // IC START

    res |= audio_es8374_set_adc_dac_volume(i2c_iface, ES_MODULE_ADC, 0, 0);      // 0db
    res |= audio_es8374_set_adc_dac_volume(i2c_iface, ES_MODULE_DAC, 0, 0);      // 0db

    res |= audio_es8374_write_reg(i2c_iface, 0x14, 0x8A); // IC START
    res |= audio_es8374_write_reg(i2c_iface, 0x15, 0x40); // IC START
    // res |= audio_es8374_write_reg(i2c_iface, 0x1A, 0xA0); // monoout set: MIC1
    res |= audio_es8374_write_reg(i2c_iface, 0x1A, 0xA1); // monoout set: MIC2
    res |= audio_es8374_write_reg(i2c_iface, 0x1B, 0x19); // monoout set
    res |= audio_es8374_write_reg(i2c_iface, 0x1C, 0x90); // spk set
    res |= audio_es8374_write_reg(i2c_iface, 0x1D, 0x01); // spk set
    res |= audio_es8374_write_reg(i2c_iface, 0x1F, 0x00); // spk set
    res |= audio_es8374_write_reg(i2c_iface, 0x1E, 0x20); // spk on
    res |= audio_es8374_write_reg(i2c_iface, 0x28, 0x00); // alc set
    res |= audio_es8374_write_reg(i2c_iface, 0x25, 0x00); // ADCVOLUME on
    res |= audio_es8374_write_reg(i2c_iface, 0x38, 0x00); // DACVOLUME on
    res |= audio_es8374_write_reg(i2c_iface, 0x37, 0x30); // dac set
    res |= audio_es8374_write_reg(i2c_iface, 0x6D, 0x60); //SEL:GPIO1=DMIC CLK OUT+SEL:GPIO2=PLL CLK OUT

    /*
     * The automute function will mute the speaker when the data lower than the threshold.
     * If enable automute, the speaker will mute when we send zero data to codec,
     * but when we start to play audio, a pop-click will be generated, so we
     * disable the automute function. 
     */
    // res |= audio_es8374_write_reg(i2c_iface, 0x71, 0x05); //for automute setting
    // res |= audio_es8374_write_reg(i2c_iface, 0x73, 0x70);

    res |= audio_es8374_config_dac_output(i2c_iface, out_channel);  //0x3c Enable DAC and Enable Lout/Rout/1/2
    res |= audio_es8374_config_adc_input(i2c_iface, in_channel);  //0x00 LINSEL & RINSEL, LIN1/RIN1 as ADC Input; DSSEL,use one DS Reg11; DSR, LINPUT1-RINPUT1

    res |= audio_es8374_set_mic_gain(i2c_iface, MIC_GAIN_6DB);
    // res |= audio_es8374_set_mic_gain(MIC_GAIN_18DB);

    res |= audio_es8374_codec_set_voice_volume(dev, dev->cfg.default_volume);
    // res |= audio_es8374_codec_set_voice_volume(96);

    res |= audio_es8374_write_reg(i2c_iface, 0x37, 0x00); // dac set

    return res;
}

static int m_power_pin_inited = 0;

static int audio_es8374_enable_speaker_power(audio_dev_t *dev, bool enable)
{
    audio_dev_config_t *codec_cfg = &dev->cfg;
    uint8_t spk_enable_pin = codec_cfg->codec_spk_en_pin;

    if (m_power_pin_inited == 0) {
        sysctl_io_switch_t io_switch = IO_SWITCH_GPIO0 + spk_enable_pin;
        sysctl_set_io_switch(io_switch, 1);
        gpio_set_drive_mode(spk_enable_pin, GPIO_DM_OUTPUT);
        m_power_pin_inited = 1;
    }

    if (enable)
        gpio_set_pin(spk_enable_pin, 1);
    else
        gpio_set_pin(spk_enable_pin, 0);

    return 0;
}

int audio_es8374_codec_config_i2s(audio_dev_ctrl_interface_t *i2c_iface, audio_codec_mode_t mode, audio_codec_i2s_iface_t *iface);
int audio_es8374_codec_ctrl_state(audio_dev_t *dev, audio_codec_mode_t mode, audio_ctrl_t ctrl_state);
int audio_es8374_codec_init(audio_dev_t *dev, audio_dev_config_t *_cfg)
{
    int res = 0;
    es_i2s_clock_t clkdiv;

    clkdiv.lclk_div = LCLK_DIV_256;
    clkdiv.sclk_div = MCLK_DIV_4;

    dev->cfg = *_cfg; // override default cfg
    audio_dev_config_t *cfg = &dev->cfg;
    audio_dev_ctrl_interface_t *i2c_iface = &cfg->i2c_iface;
    i2c_init(i2c_iface->i2c_num, i2c_iface->i2c_addr, i2c_iface->i2c_addr_width, i2c_iface->i2c_clk);

    res |= audio_es8374_codec_ctrl_state(dev, cfg->codec_mode, AUDIO_CTRL_START);
    if (res) return res;
    // res |= audio_es8374_stop(i2c_iface, cfg->codec_mode);
    res |= audio_es8374_init_reg(dev, cfg->i2s_iface.mode, (BIT_LENGTH_16BITS << 4) | cfg->i2s_iface.fmt, clkdiv,
                           cfg->dac_output, cfg->adc_input);
    res |= audio_es8374_config_fmt(i2c_iface, cfg->codec_mode, cfg->i2s_iface.fmt);
    res |= audio_es8374_codec_config_i2s(i2c_iface, cfg->codec_mode, &(cfg->i2s_iface));

    int current_volume = 0;
    audio_es8374_codec_get_voice_volume(dev, &current_volume);
    // LOGD(__func__, "current volume: %d", current_volume);

    uint8_t reg_val = 0;
    res |= audio_es8374_read_reg(i2c_iface, 0x1d, &reg_val);
    // LOGD(__func__, "0x1d: %02x", reg_val);

    res |= audio_es8374_read_reg(i2c_iface, 0x21, &reg_val);
    // LOGD(__func__, "0x21: %02x", reg_val);

    // enable speaker
    audio_es8374_enable_speaker_power(dev, true);

    LOGI(__func__, "ES8374 init done");

    return res;
}

int audio_es8374_codec_deinit(audio_dev_t *dev)
{
    return audio_es8374_write_reg(&dev->cfg.i2c_iface, 0x00, 0x7F); // IC Reset and STOP
}

int audio_es8374_codec_config_i2s(audio_dev_ctrl_interface_t *i2c_iface, audio_codec_mode_t mode, audio_codec_i2s_iface_t *iface)
{
    (void)mode;

    int res = 0;
    int tmp = 0;
    res |= audio_es8374_config_fmt(i2c_iface, ES_MODULE_ADC_DAC, iface->fmt);
    if (iface->bits == AUDIO_BIT_LENGTH_16BITS) {
        tmp = BIT_LENGTH_16BITS;
    } else if (iface->bits == AUDIO_BIT_LENGTH_24BITS) {
        tmp = BIT_LENGTH_24BITS;
    } else {
        tmp = BIT_LENGTH_32BITS;
    }
    res |= audio_es8374_set_bits_per_sample(i2c_iface, ES_MODULE_ADC_DAC, tmp);
    return res;
}

int audio_es8374_codec_ctrl_state(audio_dev_t *dev, audio_codec_mode_t mode, audio_ctrl_t ctrl_state)
{
    int res = 0;
    int es_mode_t = 0;
    switch (mode) {
        case AUDIO_CODEC_MODE_ENCODE:
            es_mode_t  = ES_MODULE_ADC;
            break;
        case AUDIO_CODEC_MODE_LINE_IN:
            es_mode_t  = ES_MODULE_LINE;
            break;
        case AUDIO_CODEC_MODE_DECODE:
            es_mode_t  = ES_MODULE_DAC;
            break;
        case AUDIO_CODEC_MODE_BOTH:
            es_mode_t  = ES_MODULE_ADC_DAC;
            break;
        default:
            es_mode_t = ES_MODULE_DAC;
            LOGW(ES8374_TAG, "Codec mode not support, default is decode mode");
            break;
    }
    if (AUDIO_CTRL_STOP == ctrl_state) {
        res = audio_es8374_stop(dev, es_mode_t);
    } else {
        res = audio_es8374_start(dev, es_mode_t);
        LOGD(ES8374_TAG, "start default is decode mode:%d", es_mode_t);
    }
    return res;
}

audio_dev_ops_t es8374_ops = {
    .init = audio_es8374_codec_init,
    .release = audio_es8374_codec_deinit,
    .suspend = NULL,
    .resume = NULL,
    .speaker_mute = audio_es8374_set_voice_mute,
    .speaker_set_volume = audio_es8374_codec_set_voice_volume,
    .mic_mute = NULL,
    .mic_set_volume = NULL,
    .enable_speaker_power = audio_es8374_enable_speaker_power,
};

audio_dev_t es8374_audio_dev = {
    .name = "es8374",
    .ops = &es8374_ops,
    .is_initialized = 0,
    .cfg = AUDIO_DEV_CONFIG_INITIALIZER, // NOTE: this will be ovveride when init
    .priv = NULL,
};

//NOTE: use apis in audio_dev.h
/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_AUDIO_CODEC, es8374,    0,      &es8374_audio_dev,    NULL);