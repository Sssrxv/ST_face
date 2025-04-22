#include "audio_esxxxx_common.h"
#include "audio_dev.h"
#include "syslog.h"
#include "udevice.h"
#include "sysctl.h"
#include "gpio.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define TAG "ES8311_DRIVER"

/*
 * to define the clock source of MCLK
 */
#define FROM_MCLK_PIN       0
#define FROM_SCLK_PIN       1

/*
 * to define whether to reverse the clock
 */
#define INVERT_MCLK         0 // do not invert
#define INVERT_SCLK         0

#define IS_DMIC             0 // Is it a digital microphone

#define MCLK_DIV_FRE        256

/*
 *   ES8311_REGISTER NAME_REG_REGISTER ADDRESS
 */
#define ES8311_RESET_REG00              0x00  /*reset digital,csm,clock manager etc.*/

/*
 * Clock Scheme Register definition
 */
#define ES8311_CLK_MANAGER_REG01        0x01 /* select clk src for mclk, enable clock for codec */
#define ES8311_CLK_MANAGER_REG02        0x02 /* clk divider and clk multiplier */
#define ES8311_CLK_MANAGER_REG03        0x03 /* adc fsmode and osr  */
#define ES8311_CLK_MANAGER_REG04        0x04 /* dac osr */
#define ES8311_CLK_MANAGER_REG05        0x05 /* clk divier for adc and dac */
#define ES8311_CLK_MANAGER_REG06        0x06 /* bclk inverter and divider */
#define ES8311_CLK_MANAGER_REG07        0x07 /* tri-state, lrck divider */
#define ES8311_CLK_MANAGER_REG08        0x08 /* lrck divider */
/*
 * SDP
 */
#define ES8311_SDPIN_REG09              0x09 /* dac serial digital port */
#define ES8311_SDPOUT_REG0A             0x0A /* adc serial digital port */
/*
 * SYSTEM
 */
#define ES8311_SYSTEM_REG0B             0x0B /* system */
#define ES8311_SYSTEM_REG0C             0x0C /* system */
#define ES8311_SYSTEM_REG0D             0x0D /* system, power up/down */
#define ES8311_SYSTEM_REG0E             0x0E /* system, power up/down */
#define ES8311_SYSTEM_REG0F             0x0F /* system, low power */
#define ES8311_SYSTEM_REG10             0x10 /* system */
#define ES8311_SYSTEM_REG11             0x11 /* system */
#define ES8311_SYSTEM_REG12             0x12 /* system, Enable DAC */
#define ES8311_SYSTEM_REG13             0x13 /* system */
#define ES8311_SYSTEM_REG14             0x14 /* system, select DMIC, select analog pga gain */
/*
 * ADC
 */
#define ES8311_ADC_REG15                0x15 /* ADC, adc ramp rate, dmic sense */
#define ES8311_ADC_REG16                0x16 /* ADC */
#define ES8311_ADC_REG17                0x17 /* ADC, volume */
#define ES8311_ADC_REG18                0x18 /* ADC, alc enable and winsize */
#define ES8311_ADC_REG19                0x19 /* ADC, alc maxlevel */
#define ES8311_ADC_REG1A                0x1A /* ADC, alc automute */
#define ES8311_ADC_REG1B                0x1B /* ADC, alc automute, adc hpf s1 */
#define ES8311_ADC_REG1C                0x1C /* ADC, equalizer, hpf s2 */
/*
 * DAC
 */
#define ES8311_DAC_REG31                0x31 /* DAC, mute */
#define ES8311_DAC_REG32                0x32 /* DAC, volume */
#define ES8311_DAC_REG33                0x33 /* DAC, offset */
#define ES8311_DAC_REG34                0x34 /* DAC, drc enable, drc winsize */
#define ES8311_DAC_REG35                0x35 /* DAC, drc maxlevel, minilevel */
#define ES8311_DAC_REG37                0x37 /* DAC, ramprate */
/*
 *GPIO
 */
#define ES8311_GPIO_REG44               0x44 /* GPIO, dac2adc for test */
#define ES8311_GP_REG45                 0x45 /* GP CONTROL */
/*
 * CHIP
 */
#define ES8311_CHD1_REGFD               0xFD /* CHIP ID1 */
#define ES8311_CHD2_REGFE               0xFE /* CHIP ID2 */
#define ES8311_CHVER_REGFF              0xFF /* VERSION */
#define ES8311_CHD1_REGFD               0xFD /* CHIP ID1 */

#define ES8311_MAX_REGISTER             0xFF

typedef enum {
    ES8311_MIC_GAIN_MIN = -1,
    ES8311_MIC_GAIN_0DB,
    ES8311_MIC_GAIN_6DB,
    ES8311_MIC_GAIN_12DB,
    ES8311_MIC_GAIN_18DB,
    ES8311_MIC_GAIN_24DB,
    ES8311_MIC_GAIN_30DB,
    ES8311_MIC_GAIN_36DB,
    ES8311_MIC_GAIN_42DB,
    ES8311_MIC_GAIN_MAX
} es8311_mic_gain_t;

typedef enum audio_ctrl {
    AUDIO_CTRL_STOP  = 0x00,  /** set stop mode */
    AUDIO_CTRL_START = 0x01,  /** set start mode */
} audio_ctrl_t;

/*
 * Clock coefficient structer
 */
struct _coeff_div {
    uint32_t mclk;        /* mclk frequency */
    uint32_t rate;        /* sample rate */
    uint8_t pre_div;      /* the pre divider with range from 1 to 8 */
    uint8_t pre_multi;    /* the pre multiplier with x1, x2, x4 and x8 selection */
    uint8_t adc_div;      /* adcclk divider */
    uint8_t dac_div;      /* dacclk divider */
    uint8_t fs_mode;      /* double speed or single speed, =0, ss, =1, ds */
    uint8_t lrck_h;       /* adclrck divider and daclrck divider */
    uint8_t lrck_l;
    uint8_t bclk_div;     /* sclk divider */
    uint8_t adc_osr;      /* adc osr */
    uint8_t dac_osr;      /* dac osr */
};

/* codec hifi mclk clock divider coefficients */
static const struct _coeff_div coeff_div[] = {
    //mclk     rate   pre_div  mult  adc_div dac_div fs_mode lrch  lrcl  bckdiv osr
    /* 8k */
    {12288000, 8000 , 0x06, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {18432000, 8000 , 0x03, 0x02, 0x03, 0x03, 0x00, 0x05, 0xff, 0x18, 0x10, 0x20},
    {16384000, 8000 , 0x08, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {8192000 , 8000 , 0x04, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {6144000 , 8000 , 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {4096000 , 8000 , 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {3072000 , 8000 , 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {2048000 , 8000 , 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1536000 , 8000 , 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1024000 , 8000 , 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},

    /* 11.025k */
    {11289600, 11025, 0x04, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {5644800 , 11025, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {2822400 , 11025, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1411200 , 11025, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},

    /* 12k */
    {12288000, 12000, 0x04, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {6144000 , 12000, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {3072000 , 12000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1536000 , 12000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},

    /* 16k */
    {12288000, 16000, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {18432000, 16000, 0x03, 0x02, 0x03, 0x03, 0x00, 0x02, 0xff, 0x0c, 0x10, 0x20},
    {16384000, 16000, 0x04, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {8192000 , 16000, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {6144000 , 16000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {4096000 , 16000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {3072000 , 16000, 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {2048000 , 16000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1536000 , 16000, 0x03, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1024000 , 16000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},

    /* 22.05k */
    {11289600, 22050, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {5644800 , 22050, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2822400 , 22050, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1411200 , 22050, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 24k */
    {12288000, 24000, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 24000, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000 , 24000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000 , 24000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000 , 24000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 32k */
    {12288000, 32000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 32000, 0x03, 0x04, 0x03, 0x03, 0x00, 0x02, 0xff, 0x0c, 0x10, 0x10},
    {16384000, 32000, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {8192000 , 32000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000 , 32000, 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {4096000 , 32000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000 , 32000, 0x03, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2048000 , 32000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000 , 32000, 0x03, 0x08, 0x01, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x10, 0x10},
    {1024000 , 32000, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 44.1k */
    {11289600, 44100, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {5644800 , 44100, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2822400 , 44100, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1411200 , 44100, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 48k */
    {12288000, 48000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 48000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000 , 48000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000 , 48000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000 , 48000, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 64k */
    {12288000, 64000, 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 64000, 0x03, 0x04, 0x03, 0x03, 0x01, 0x01, 0x7f, 0x06, 0x10, 0x10},
    {16384000, 64000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {8192000 , 64000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000 , 64000, 0x01, 0x04, 0x03, 0x03, 0x01, 0x01, 0x7f, 0x06, 0x10, 0x10},
    {4096000 , 64000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000 , 64000, 0x01, 0x08, 0x03, 0x03, 0x01, 0x01, 0x7f, 0x06, 0x10, 0x10},
    {2048000 , 64000, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000 , 64000, 0x01, 0x08, 0x01, 0x01, 0x01, 0x00, 0xbf, 0x03, 0x18, 0x18},
    {1024000 , 64000, 0x01, 0x08, 0x01, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x10, 0x10},

    /* 88.2k */
    {11289600, 88200, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {5644800 , 88200, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2822400 , 88200, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1411200 , 88200, 0x01, 0x08, 0x01, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x10, 0x10},

    /* 96k */
    {12288000, 96000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 96000, 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000 , 96000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000 , 96000, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000 , 96000, 0x01, 0x08, 0x01, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x10, 0x10},
};

static int8_t get_es8311_mclk_src(void)
{
    return FROM_MCLK_PIN;
}

static int es8311_write_reg(audio_dev_ctrl_interface_t *i2c_iface, uint8_t reg_addr, uint8_t reg_val)
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

static int es8311_read_reg(audio_dev_ctrl_interface_t *i2c_iface, uint8_t reg_addr)
{
    int ret = 0;
    uint8_t reg_val = 0;
    uint8_t addr_buf[1];
    addr_buf[0] = reg_addr & 0xff;

    ret = i2c_recv_data(i2c_iface->i2c_num, i2c_iface->i2c_addr, addr_buf, 1, &reg_val, 1);
    if (ret < 0) {
        LOGE(__func__, "recv error");
        return -1;
    }
    
    return reg_val;
}

/*
* look for the coefficient in coeff_div[] table
*/
static int get_coeff(uint32_t mclk, uint32_t rate)
{
    for (size_t i = 0; i < (sizeof(coeff_div) / sizeof(coeff_div[0])); i++) {
        if (coeff_div[i].rate == rate && coeff_div[i].mclk == mclk)
            return i;
    }
    return -1;
}

/*
* set es8311 dac mute or not
* if mute = 0, dac un-mute
* if mute = 1, dac mute
*/
static void es8311_mute(audio_dev_ctrl_interface_t *i2c_iface, bool mute)
{
    uint8_t regv;
    LOGI(TAG, "Enter into es8311_mute(), mute = %d\n", mute);
    regv = es8311_read_reg(i2c_iface, ES8311_DAC_REG31) & 0x9f;
    if (mute) {
        es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG12, 0x02);
        es8311_write_reg(i2c_iface, ES8311_DAC_REG31, regv | 0x60);
        es8311_write_reg(i2c_iface, ES8311_DAC_REG32, 0x00);
        es8311_write_reg(i2c_iface, ES8311_DAC_REG37, 0x08);
    } else {
        es8311_write_reg(i2c_iface, ES8311_DAC_REG31, regv);
        es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG12, 0x00);
    }
}

/*
* set es8311 into suspend mode
*/
static void es8311_suspend(audio_dev_ctrl_interface_t *i2c_iface)
{
    LOGI(TAG, "Enter into es8311_suspend()");
    es8311_write_reg(i2c_iface, ES8311_DAC_REG32, 0x00);
    es8311_write_reg(i2c_iface, ES8311_ADC_REG17, 0x00);
    es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG0E, 0xFF);
    es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG12, 0x02);
    es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG14, 0x00);
    es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG0D, 0xFA);
    es8311_write_reg(i2c_iface, ES8311_ADC_REG15, 0x00);
    es8311_write_reg(i2c_iface, ES8311_DAC_REG37, 0x08);
    es8311_write_reg(i2c_iface, ES8311_GP_REG45, 0x01);
}

/*
* enable pa power
*/
static int m_power_pin_inited = 0;

static void es8311_pa_power(bool enable, uint8_t pa_enable_pin, uint8_t spk_enable_pin)
{
    if (m_power_pin_inited == 0) {
        // sysctl_io_switch_t io_switch = IO_SWITCH_GPIO0 + pa_enable_pin;
        sysctl_set_io_switch(IO_SWITCH_GPIO0 + pa_enable_pin, 1);
        sysctl_set_io_switch(IO_SWITCH_GPIO0 + spk_enable_pin, 1);
        gpio_set_drive_mode(pa_enable_pin, GPIO_DM_OUTPUT);
        gpio_set_drive_mode(spk_enable_pin, GPIO_DM_OUTPUT);
        m_power_pin_inited = 1;
    }

    if (enable) {
        gpio_set_pin(pa_enable_pin, 1);
        gpio_set_pin(spk_enable_pin, 1);
    } else {
        gpio_set_pin(pa_enable_pin, 0);
        gpio_set_pin(spk_enable_pin, 0);
    }
}

int es8311_codec_init(audio_dev_t *dev, audio_dev_config_t *_cfg)
{
    uint8_t datmp, regv;
    int coeff;
    int ret = 0;

    dev->cfg = *_cfg; // override default cfg

    audio_dev_config_t *codec_cfg = &dev->cfg;
    audio_dev_ctrl_interface_t *i2c_iface = &codec_cfg->i2c_iface;
    i2c_init(i2c_iface->i2c_num, i2c_iface->i2c_addr, i2c_iface->i2c_addr_width, i2c_iface->i2c_clk);

    ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG01, 0x30);
    if (ret) return ret;
    ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG02, 0x00);
    ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG03, 0x10);
    ret |= es8311_write_reg(i2c_iface, ES8311_ADC_REG16, 0x24);
    ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG04, 0x10);
    ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG05, 0x00);
    ret |= es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG0B, 0x00);
    ret |= es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG0C, 0x00);
    ret |= es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG10, 0x1F);
    ret |= es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG11, 0x7F);
    ret |= es8311_write_reg(i2c_iface, ES8311_RESET_REG00, 0x80);
    /*
     * Set Codec into Master or Slave mode
     */
    regv = es8311_read_reg(i2c_iface, ES8311_RESET_REG00);
    /*
     * Set master/slave audio interface
     */
    audio_codec_i2s_iface_t *i2s_cfg = &(codec_cfg->i2s_iface);
    switch (i2s_cfg->mode) {
        case AUDIO_MODE_MASTER:    /* MASTER MODE */
            LOGI(TAG, "ES8311 in Master mode");
            regv |= 0x40;
            break;
        case AUDIO_MODE_SLAVE:    /* SLAVE MODE */
            LOGI(TAG, "ES8311 in Slave mode");
            regv &= 0xBF;
            break;
        default:
            regv &= 0xBF;
    }
    ret |= es8311_write_reg(i2c_iface, ES8311_RESET_REG00, regv);
    ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG01, 0x3F);
    /*
     * Select clock source for internal mclk
     */
    switch (get_es8311_mclk_src()) {
        case FROM_MCLK_PIN:
            regv = es8311_read_reg(i2c_iface, ES8311_CLK_MANAGER_REG01);
            regv &= 0x7F;
            ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG01, regv);
            break;
        case FROM_SCLK_PIN:
            regv = es8311_read_reg(i2c_iface, ES8311_CLK_MANAGER_REG01);
            regv |= 0x80;
            ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG01, regv);
            break;
        default:
            regv = es8311_read_reg(i2c_iface, ES8311_CLK_MANAGER_REG01);
            regv &= 0x7F;
            ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG01, regv);
            break;
    }
    int sample_fre = 0;
    int mclk_fre = 0;
    switch (i2s_cfg->samples) {
        case AUDIO_08K_SAMPLES:
            sample_fre = 8000;
            break;
        case AUDIO_11K_SAMPLES:
            sample_fre = 11025;
            break;
        case AUDIO_16K_SAMPLES:
            sample_fre = 16000;
            break;
        case AUDIO_22K_SAMPLES:
            sample_fre = 22050;
            break;
        case AUDIO_24K_SAMPLES:
            sample_fre = 24000;
            break;
        case AUDIO_32K_SAMPLES:
            sample_fre = 32000;
            break;
        case AUDIO_44K_SAMPLES:
            sample_fre = 44100;
            break;
        case AUDIO_48K_SAMPLES:
            sample_fre = 48000;
            break;
        default:
            LOGE(TAG, "Unable to configure sample rate %dHz", sample_fre);
            break;
    }
    mclk_fre = sample_fre * MCLK_DIV_FRE;
    coeff = get_coeff(mclk_fre, sample_fre);
    if (coeff < 0) {
        LOGE(TAG, "Unable to configure sample rate %dHz with %dHz MCLK", sample_fre, mclk_fre);
        return -1;
    }
    /*
     * Set clock parammeters
     */
    if (coeff >= 0) {
        regv = es8311_read_reg(i2c_iface, ES8311_CLK_MANAGER_REG02) & 0x07;
        regv |= (coeff_div[coeff].pre_div - 1) << 5;
        datmp = 0;
        switch (coeff_div[coeff].pre_multi) {
            case 1:
                datmp = 0;
                break;
            case 2:
                datmp = 1;
                break;
            case 4:
                datmp = 2;
                break;
            case 8:
                datmp = 3;
                break;
            default:
                break;
        }

        if (get_es8311_mclk_src() == FROM_SCLK_PIN) {
            datmp = 3;     /* DIG_MCLK = LRCK * 256 = BCLK * 8 */
        }
        regv |= (datmp) << 3;
        ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG02, regv);

        regv = es8311_read_reg(i2c_iface, ES8311_CLK_MANAGER_REG05) & 0x00;
        regv |= (coeff_div[coeff].adc_div - 1) << 4;
        regv |= (coeff_div[coeff].dac_div - 1) << 0;
        ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG05, regv);

        regv = es8311_read_reg(i2c_iface, ES8311_CLK_MANAGER_REG03) & 0x80;
        regv |= coeff_div[coeff].fs_mode << 6;
        regv |= coeff_div[coeff].adc_osr << 0;
        ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG03, regv);

        regv = es8311_read_reg(i2c_iface, ES8311_CLK_MANAGER_REG04) & 0x80;
        regv |= coeff_div[coeff].dac_osr << 0;
        ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG04, regv);

        regv = es8311_read_reg(i2c_iface, ES8311_CLK_MANAGER_REG07) & 0xC0;
        regv |= coeff_div[coeff].lrck_h << 0;
        ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG07, regv);

        regv = es8311_read_reg(i2c_iface, ES8311_CLK_MANAGER_REG08) & 0x00;
        regv |= coeff_div[coeff].lrck_l << 0;
        ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG08, regv);

        regv = es8311_read_reg(i2c_iface, ES8311_CLK_MANAGER_REG06) & 0xE0;
        if (coeff_div[coeff].bclk_div < 19) {
            regv |= (coeff_div[coeff].bclk_div - 1) << 0;
        } else {
            regv |= (coeff_div[coeff].bclk_div) << 0;
        }
        ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG06, regv);
    }

    /*
     * mclk inverted or not
     */
    if (INVERT_MCLK) {
        regv = es8311_read_reg(i2c_iface, ES8311_CLK_MANAGER_REG01);
        regv |= 0x40;
        ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG01, regv);
    } else {
        regv = es8311_read_reg(i2c_iface, ES8311_CLK_MANAGER_REG01);
        regv &= ~(0x40);
        ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG01, regv);
    }
    /*
     * sclk inverted or not
     */
    if (INVERT_SCLK) {
        regv = es8311_read_reg(i2c_iface, ES8311_CLK_MANAGER_REG06);
        regv |= 0x20;
        ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG06, regv);
    } else {
        regv = es8311_read_reg(i2c_iface, ES8311_CLK_MANAGER_REG06);
        regv &= ~(0x20);
        ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG06, regv);
    }

    ret |= es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG13, 0x10);
    ret |= es8311_write_reg(i2c_iface, ES8311_ADC_REG1B, 0x0A);
    ret |= es8311_write_reg(i2c_iface, ES8311_ADC_REG1C, 0x6A);

    ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG02, 0x00);
    ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG04, 0x20);
    ret |= es8311_write_reg(i2c_iface, ES8311_CLK_MANAGER_REG05, 0x11);

    es8311_pa_power(true, codec_cfg->codec_pa_pin, codec_cfg->codec_spk_en_pin);

    LOGI(__func__, "ES8311 init done");
    return 0;
}

int es8311_codec_deinit(audio_dev_t *dev)
{
    (void)dev;

    return 0;
}

int es8311_config_fmt(audio_dev_ctrl_interface_t *i2c_iface, es_i2s_fmt_t fmt)
{
    int ret = 0;
    uint8_t adc_iface = 0, dac_iface = 0;
    dac_iface = es8311_read_reg(i2c_iface, ES8311_SDPIN_REG09);
    adc_iface = es8311_read_reg(i2c_iface, ES8311_SDPOUT_REG0A);
    switch (fmt) {
        case AUDIO_I2S_NORMAL:
            LOGD(TAG, "ES8311 in I2S Format");
            dac_iface &= 0xFC;
            adc_iface &= 0xFC;
            break;
        case AUDIO_I2S_LEFT:
        case AUDIO_I2S_RIGHT:
            LOGD(TAG, "ES8311 in LJ Format");
            adc_iface &= 0xFC;
            dac_iface &= 0xFC;
            adc_iface |= 0x01;
            dac_iface |= 0x01;
            break;
        case AUDIO_I2S_DSP:
            LOGD(TAG, "ES8311 in DSP-A Format");
            adc_iface &= 0xDC;
            dac_iface &= 0xDC;
            adc_iface |= 0x03;
            dac_iface |= 0x03;
            break;
        default:
            dac_iface &= 0xFC;
            adc_iface &= 0xFC;
            break;
    }
    ret |= es8311_write_reg(i2c_iface, ES8311_SDPIN_REG09, dac_iface);
    ret |= es8311_write_reg(i2c_iface, ES8311_SDPOUT_REG0A, adc_iface);

    return ret;
}

int es8311_set_bits_per_sample(audio_dev_ctrl_interface_t *i2c_iface, audio_iface_bits_t bits)
{
    int ret = 0;
    uint8_t adc_iface = 0, dac_iface = 0;
    dac_iface = es8311_read_reg(i2c_iface, ES8311_SDPIN_REG09);
    adc_iface = es8311_read_reg(i2c_iface, ES8311_SDPOUT_REG0A);
    switch (bits) {
        case AUDIO_BIT_LENGTH_16BITS:
            dac_iface |= 0x0c;
            adc_iface |= 0x0c;
            break;
        case AUDIO_BIT_LENGTH_24BITS:
            break;
        case AUDIO_BIT_LENGTH_32BITS:
            dac_iface |= 0x10;
            adc_iface |= 0x10;
            break;
        default:
            dac_iface |= 0x0c;
            adc_iface |= 0x0c;
            break;

    }
    ret |= es8311_write_reg(i2c_iface, ES8311_SDPIN_REG09, dac_iface);
    ret |= es8311_write_reg(i2c_iface, ES8311_SDPOUT_REG0A, adc_iface);

    return ret;
}

int es8311_codec_config_i2s(audio_codec_mode_t mode, audio_codec_i2s_iface_t *iface, audio_dev_ctrl_interface_t *i2c_iface)
{
    (void)mode;

    int ret = 0;
    ret |= es8311_set_bits_per_sample(i2c_iface, iface->bits);
    ret |= es8311_config_fmt(i2c_iface, iface->fmt);
    return ret;
}

int es8311_start(audio_dev_ctrl_interface_t *i2c_iface, es_module_t mode)
{
    int ret = 0;
    uint8_t adc_iface = 0, dac_iface = 0;

    dac_iface = es8311_read_reg(i2c_iface, ES8311_SDPIN_REG09) & 0xBF;
    adc_iface = es8311_read_reg(i2c_iface, ES8311_SDPOUT_REG0A) & 0xBF;
    adc_iface |= (1 << 6);
    dac_iface |= (1 << 6);

    if (mode == ES_MODULE_LINE) {
        LOGE(TAG, "The codec es8311 doesn't support ES_MODULE_LINE mode");
        return -1;
    }
    if (mode == ES_MODULE_ADC || mode == ES_MODULE_ADC_DAC) {
        adc_iface &= ~(1 << 6);
    }
    if (mode == ES_MODULE_DAC || mode == ES_MODULE_ADC_DAC) {
        dac_iface &= ~(1 << 6);
    }

    ret |= es8311_write_reg(i2c_iface, ES8311_SDPIN_REG09, dac_iface);
    ret |= es8311_write_reg(i2c_iface, ES8311_SDPOUT_REG0A, adc_iface);

    ret |= es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG0E, 0x02);
    ret |= es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG12, 0x00);
    ret |= es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG14, 0x18);
    ret |= es8311_write_reg(i2c_iface, ES8311_ADC_REG17, 0xD0);

    /*
     * pdm dmic enable or disable
     */
    int regv = 0;
    if (IS_DMIC) {
        regv = es8311_read_reg(i2c_iface, ES8311_SYSTEM_REG14);
        regv |= 0x40;
        ret |= es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG14, regv);
    } else {
        regv = es8311_read_reg(i2c_iface, ES8311_SYSTEM_REG14);
        regv &= ~(0x40);
        ret |= es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG14, regv);
    }

    ret |= es8311_write_reg(i2c_iface, ES8311_SYSTEM_REG0D, 0x01);
    ret |= es8311_write_reg(i2c_iface, ES8311_ADC_REG15, 0x40);
    ret |= es8311_write_reg(i2c_iface, ES8311_DAC_REG37, 0x48);
    ret |= es8311_write_reg(i2c_iface, ES8311_GP_REG45, 0x00);

    return ret;
}

int es8311_stop(audio_dev_ctrl_interface_t *i2c_iface, es_module_t mode)
{
    (void)mode;

    int ret = 0;
    es8311_suspend(i2c_iface);
    return ret;
}

int es8311_codec_ctrl_state(audio_dev_ctrl_interface_t *i2c_iface, audio_codec_mode_t mode, audio_ctrl_t ctrl_state)
{
    int ret = 0;
    es_module_t es_mode = ES_MODULE_MIN;

    switch (mode) {
        case AUDIO_CODEC_MODE_ENCODE:
            es_mode  = ES_MODULE_ADC;
            break;
        case AUDIO_CODEC_MODE_LINE_IN:
            es_mode  = ES_MODULE_LINE;
            break;
        case AUDIO_CODEC_MODE_DECODE:
            es_mode  = ES_MODULE_DAC;
            break;
        case AUDIO_CODEC_MODE_BOTH:
            es_mode  = ES_MODULE_ADC_DAC;
            break;
        default:
            es_mode = ES_MODULE_DAC;
            LOGW(TAG, "Codec mode not support, default is decode mode");
            break;
    }

    if (ctrl_state == AUDIO_CTRL_START) {
        ret |= es8311_start(i2c_iface, es_mode);
    } else {
        LOGW(TAG, "The codec is about to stop");
        ret |= es8311_stop(i2c_iface, es_mode);
    }

    return ret;
}

int es8311_codec_set_voice_volume(audio_dev_t *dev, int volume)
{
    int res = 0;
    if (volume < 0) {
        volume = 0;
    } else if (volume > 100) {
        volume = 100;
    }
    int vol = (volume) * 2550 / 1000;
    LOGD(TAG, "SET: volume:%d", vol);
    es8311_write_reg(&dev->cfg.i2c_iface, ES8311_DAC_REG32, vol);
    return res;
}

int es8311_codec_get_voice_volume(audio_dev_t *dev, int *volume)
{
    int res = 0;
    int regv = 0;
    regv = es8311_read_reg(&dev->cfg.i2c_iface, ES8311_DAC_REG32);
    if (regv == -1) {
        *volume = 0;
        res = -1;
    } else {
        *volume = regv * 100 / 256;
    }
    LOGD(TAG, "GET: res:%d, volume:%d", regv, *volume);
    return res;
}

int es8311_set_voice_mute(audio_dev_t *dev, bool enable)
{
    LOGD(TAG, "Es8311SetVoiceMute volume");
    es8311_mute(&dev->cfg.i2c_iface, enable);
    return 0;
}

int es8311_get_voice_mute(audio_dev_t *dev, int *mute)
{
    int res = 0;
    uint8_t reg = 0;
    res = es8311_read_reg(&dev->cfg.i2c_iface, ES8311_DAC_REG31);
    if (res != -1) {
        reg = (res & 0x20) >> 5;
    }
    *mute = reg;
    return res;
}

int es8311_set_mic_gain(audio_dev_ctrl_interface_t *i2c_iface, es8311_mic_gain_t gain_db)
{
    int res = 0;
    res = es8311_write_reg(i2c_iface, ES8311_ADC_REG16, gain_db); // MIC gain scale
    return res;
}

void es8311_read_all(audio_dev_ctrl_interface_t *i2c_iface)
{
    for (int i = 0; i <= 0xFF; i++) {
        uint8_t reg = es8311_read_reg(i2c_iface, i);
        LOGI(TAG, "REG:%02x, %02x", i, reg);
    }
}

int audio_es8311_codec_init(audio_dev_t *dev, audio_dev_config_t *cfg)
{
    int ret = 0;
    ret = es8311_codec_init(dev, cfg);
    if (ret < 0) return ret;
    ret = es8311_codec_ctrl_state(&dev->cfg.i2c_iface, dev->cfg.codec_mode, AUDIO_CTRL_START);
    if (ret < 0) return ret;
    ret = es8311_codec_set_voice_volume(dev, dev->cfg.default_volume);
    if (ret < 0) return ret;
    // es8311_read_all(&dev->cfg.i2c_iface);
    return ret;
}

int es8311_enable_speaker_power(audio_dev_t *dev, bool enable)
{
    audio_dev_config_t *codec_cfg = &dev->cfg;
    es8311_pa_power(enable, codec_cfg->codec_pa_pin, codec_cfg->codec_spk_en_pin);

    return 0;
}

audio_dev_ops_t es8311_ops = {
    .init = audio_es8311_codec_init,
    .release = es8311_codec_deinit,
    .suspend = NULL, // TODO: add wrapper function to use es8311_suspend,
    .resume = NULL,
    .speaker_mute = es8311_set_voice_mute,
    .speaker_set_volume = es8311_codec_set_voice_volume,
    .mic_mute = NULL,
    .mic_set_volume = NULL,
    .enable_speaker_power = es8311_enable_speaker_power,
};

audio_dev_t es8311_audio_dev = {
    .name = "es8311",
    .ops = &es8311_ops,
    .is_initialized = 0,
    .cfg = AUDIO_DEV_CONFIG_INITIALIZER, // NOTE: this will be ovveride when init
    .priv = NULL,
};

//NOTE: use apis in audio_dev.h
/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_AUDIO_CODEC, es8311,    0,      &es8311_audio_dev,    NULL);