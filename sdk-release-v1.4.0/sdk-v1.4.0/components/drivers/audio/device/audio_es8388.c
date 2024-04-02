#include "audio_esxxxx_common.h"
#include "audio_dev.h"
#include "syslog.h"
#include "udevice.h"
#include "sysctl.h"
#include "gpio.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define TAG "ES8388_DRIVER"

/* ES8388 address */
#define ES8388_ADDR 0x20  /*!< 0x22:CE=1;0x20:CE=0*/

/* ES8388 register */
#define ES8388_CONTROL1         0x00
#define ES8388_CONTROL2         0x01

#define ES8388_CHIPPOWER        0x02

#define ES8388_ADCPOWER         0x03
#define ES8388_DACPOWER         0x04

#define ES8388_CHIPLOPOW1       0x05
#define ES8388_CHIPLOPOW2       0x06

#define ES8388_ANAVOLMANAG      0x07

#define ES8388_MASTERMODE       0x08
/* ADC */
#define ES8388_ADCCONTROL1      0x09
#define ES8388_ADCCONTROL2      0x0a
#define ES8388_ADCCONTROL3      0x0b
#define ES8388_ADCCONTROL4      0x0c
#define ES8388_ADCCONTROL5      0x0d
#define ES8388_ADCCONTROL6      0x0e
#define ES8388_ADCCONTROL7      0x0f
#define ES8388_ADCCONTROL8      0x10
#define ES8388_ADCCONTROL9      0x11
#define ES8388_ADCCONTROL10     0x12
#define ES8388_ADCCONTROL11     0x13
#define ES8388_ADCCONTROL12     0x14
#define ES8388_ADCCONTROL13     0x15
#define ES8388_ADCCONTROL14     0x16
/* DAC */
#define ES8388_DACCONTROL1      0x17
#define ES8388_DACCONTROL2      0x18
#define ES8388_DACCONTROL3      0x19
#define ES8388_DACCONTROL4      0x1a
#define ES8388_DACCONTROL5      0x1b
#define ES8388_DACCONTROL6      0x1c
#define ES8388_DACCONTROL7      0x1d
#define ES8388_DACCONTROL8      0x1e
#define ES8388_DACCONTROL9      0x1f
#define ES8388_DACCONTROL10     0x20
#define ES8388_DACCONTROL11     0x21
#define ES8388_DACCONTROL12     0x22
#define ES8388_DACCONTROL13     0x23
#define ES8388_DACCONTROL14     0x24
#define ES8388_DACCONTROL15     0x25
#define ES8388_DACCONTROL16     0x26
#define ES8388_DACCONTROL17     0x27
#define ES8388_DACCONTROL18     0x28
#define ES8388_DACCONTROL19     0x29
#define ES8388_DACCONTROL20     0x2a
#define ES8388_DACCONTROL21     0x2b
#define ES8388_DACCONTROL22     0x2c
#define ES8388_DACCONTROL23     0x2d
#define ES8388_DACCONTROL24     0x2e
#define ES8388_DACCONTROL25     0x2f
#define ES8388_DACCONTROL26     0x30
#define ES8388_DACCONTROL27     0x31
#define ES8388_DACCONTROL28     0x32
#define ES8388_DACCONTROL29     0x33
#define ES8388_DACCONTROL30     0x34

typedef enum audio_ctrl {
    AUDIO_CTRL_STOP  = 0x00,  /** set stop mode */
    AUDIO_CTRL_START = 0x01,  /** set start mode */
} audio_ctrl_t;

static int es_write_reg(const audio_dev_ctrl_interface_t *i2c_iface, uint8_t reg_addr, uint8_t reg_val)
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

static int es_read_reg(const audio_dev_ctrl_interface_t *i2c_iface, uint8_t reg_addr, uint8_t *p_data)
{
    int ret = 0;
    uint8_t addr_buf[1];
    addr_buf[0] = reg_addr & 0xff;

    ret = i2c_recv_data(i2c_iface->i2c_num, i2c_iface->i2c_addr, addr_buf, 1, p_data, 1);
    if (ret < 0) {
        LOGE(__func__, "recv error");
        return -1;
    }
    
    return ret;
}

void es8388_read_all(const audio_dev_ctrl_interface_t *i2c_iface)
{
    for (int i = 0; i < 50; i++) {
        uint8_t reg = 0;
        es_read_reg(i2c_iface, i, &reg);
        LOGD(__func__, "%x: %x", i, reg);
    }
}

/**
 * @brief Configure ES8388 ADC and DAC volume. Basicly you can consider this as ADC and DAC gain
 *
 * @param mode:             set ADC or DAC or all
 * @param volume:           -96 ~ 0              for example Es8388SetAdcDacVolume(ES_MODULE_ADC, 30, 6); means set ADC volume -30.5db
 * @param dot:              whether include 0.5. for example Es8388SetAdcDacVolume(ES_MODULE_ADC, 30, 4); means set ADC volume -30db
 *
 * @return
 *     - (-1) Parameter error
 *     - (0)   Success
 */
static int es8388_set_adc_dac_volume(const audio_dev_ctrl_interface_t *i2c_iface, int mode, int volume, int dot)
{
    int res = 0;
    if ( volume < -96 || volume > 0 ) {
        LOGW(__func__, "Warning: volume < -96! or > 0!");
        if (volume < -96)
            volume = -96;
        else
            volume = 0;
    }
    dot = (dot >= 5 ? 1 : 0);
    volume = (-volume << 1) + dot;
    if (mode == ES_MODULE_ADC || mode == ES_MODULE_ADC_DAC) {
        res |= es_write_reg(i2c_iface, ES8388_ADCCONTROL8, volume);
        res |= es_write_reg(i2c_iface, ES8388_ADCCONTROL9, volume);  //ADC Right Volume=0db
    }
    if (mode == ES_MODULE_DAC || mode == ES_MODULE_ADC_DAC) {
        res |= es_write_reg(i2c_iface, ES8388_DACCONTROL5, volume);
        res |= es_write_reg(i2c_iface, ES8388_DACCONTROL4, volume);
    }
    return res;
}

/**
 * @brief Configure ES8388 DAC mute or not. Basically you can use this function to mute the output or unmute
 *
 * @param enable: enable or disable
 *
 * @return
 *     - (-1) Parameter error
 *     - (0)   Success
 */
int es8388_set_voice_mute(audio_dev_ctrl_interface_t *i2c_iface, bool enable)
{
    int res = 0;
    uint8_t reg = 0;
    res = es_read_reg(i2c_iface, ES8388_DACCONTROL3, &reg);
    reg = reg & 0xFB;
    res |= es_write_reg(i2c_iface, ES8388_DACCONTROL3, reg | (((int)enable) << 2));
    return res;
}

int es8388_get_voice_mute(audio_dev_ctrl_interface_t *i2c_iface)
{
    int res = 0;
    uint8_t reg = 0;
    res = es_read_reg(i2c_iface, ES8388_DACCONTROL3, &reg);
    if (res == 0) {
        reg = (reg & 0x04) >> 2;
    }
    return res == 0 ? reg : res;
}

/**
 * @brief Power Management
 *
 * @param mod:      if ES_POWER_CHIP, the whole chip including ADC and DAC is enabled
 * @param enable:   false to disable true to enable
 *
 * @return
 *     - (-1)  Error
 *     - (0)   Success
 */
int es8388_start(audio_dev_ctrl_interface_t *i2c_iface, es_module_t mode)
{
    int res = 0;
    uint8_t prev_data = 0, data = 0;
    es_read_reg(i2c_iface, ES8388_DACCONTROL21, &prev_data);
    if (mode == ES_MODULE_LINE) {
        res |= es_write_reg(i2c_iface, ES8388_DACCONTROL16, 0x09); // 0x00 audio on LIN1&RIN1,  0x09 LIN2&RIN2 by pass enable
        res |= es_write_reg(i2c_iface, ES8388_DACCONTROL17, 0x50); // left DAC to left mixer enable  and  LIN signal to left mixer enable 0db  : bupass enable
        res |= es_write_reg(i2c_iface, ES8388_DACCONTROL20, 0x50); // right DAC to right mixer enable  and  LIN signal to right mixer enable 0db : bupass enable
        res |= es_write_reg(i2c_iface, ES8388_DACCONTROL21, 0xC0); //enable adc
    } else {
        res |= es_write_reg(i2c_iface, ES8388_DACCONTROL21, 0x80);   //enable dac
    }
    es_read_reg(i2c_iface, ES8388_DACCONTROL21, &data);
    if (prev_data != data) {
        res |= es_write_reg(i2c_iface, ES8388_CHIPPOWER, 0xF0);   //start state machine
        // res |= es_write_reg(i2c_iface, ES8388_CONTROL1, 0x16);
        // res |= es_write_reg(i2c_iface, ES8388_CONTROL2, 0x50);
        res |= es_write_reg(i2c_iface, ES8388_CHIPPOWER, 0x00);   //start state machine
    }
    if (mode == ES_MODULE_ADC || mode == ES_MODULE_ADC_DAC || mode == ES_MODULE_LINE) {
        res |= es_write_reg(i2c_iface, ES8388_ADCPOWER, 0x00);   //power up adc and line in
    }
    if (mode == ES_MODULE_DAC || mode == ES_MODULE_ADC_DAC || mode == ES_MODULE_LINE) {
        res |= es_write_reg(i2c_iface, ES8388_DACPOWER, 0x3c);   //power up dac and line out
        res |= es8388_set_voice_mute(i2c_iface, false);
        LOGD(__func__, "es8388_start default is mode:%d", mode);
    }

    return res;
}

/**
 * @brief Power Management
 *
 * @param mod:      if ES_POWER_CHIP, the whole chip including ADC and DAC is enabled
 * @param enable:   false to disable true to enable
 *
 * @return
 *     - (-1)  Error
 *     - (0)   Success
 */
int es8388_stop(audio_dev_ctrl_interface_t *i2c_iface, es_module_t mode)
{
    int res = 0;
    if (mode == ES_MODULE_LINE) {
        res |= es_write_reg(i2c_iface, ES8388_DACCONTROL21, 0x80); //enable dac
        res |= es_write_reg(i2c_iface, ES8388_DACCONTROL16, 0x00); // 0x00 audio on LIN1&RIN1,  0x09 LIN2&RIN2
        res |= es_write_reg(i2c_iface, ES8388_DACCONTROL17, 0x90); // only left DAC to left mixer enable 0db
        res |= es_write_reg(i2c_iface, ES8388_DACCONTROL20, 0x90); // only right DAC to right mixer enable 0db
        return res;
    }
    if (mode == ES_MODULE_DAC || mode == ES_MODULE_ADC_DAC) {
        res |= es_write_reg(i2c_iface, ES8388_DACPOWER, 0x00);
        res |= es8388_set_voice_mute(i2c_iface, true);
        //res |= Es8388SetAdcDacVolume(ES_MODULE_DAC, -96, 5);      // 0db
        //res |= es_write_reg(i2c_iface, ES8388_DACPOWER, 0xC0);  //power down dac and line out
    }
    if (mode == ES_MODULE_ADC || mode == ES_MODULE_ADC_DAC) {
        //res |= Es8388SetAdcDacVolume(ES_MODULE_ADC, -96, 5);      // 0db
        res |= es_write_reg(i2c_iface, ES8388_ADCPOWER, 0xFF);  //power down adc and line in
    }
    if (mode == ES_MODULE_ADC_DAC) {
        res |= es_write_reg(i2c_iface, ES8388_DACCONTROL21, 0x9C);  //disable mclk
//        res |= es_write_reg(i2c_iface, ES8388_CONTROL1, 0x00);
//        res |= es_write_reg(i2c_iface, ES8388_CONTROL2, 0x58);
//        res |= es_write_reg(i2c_iface, ES8388_CHIPPOWER, 0xF3);  //stop state machine
    }

    return res;
}

static int m_power_pin_inited = 0;

static void es8388_pa_power(bool enable, uint8_t pa_enable_pin, uint8_t spk_enable_pin)
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

/**
 * @return
 *     - (-1)  Error
 *     - (0)   Success
 */
int es8388_init(audio_dev_t *dev, audio_dev_config_t *_cfg)
{
    int res = 0;

    dev->cfg = *_cfg; // override default cfg

    audio_dev_config_t *codec_cfg = &dev->cfg;
    const audio_dev_ctrl_interface_t *i2c_iface = &codec_cfg->i2c_iface;
    i2c_init(i2c_iface->i2c_num, i2c_iface->i2c_addr, i2c_iface->i2c_addr_width, i2c_iface->i2c_clk);

    res |= es_write_reg(i2c_iface, ES8388_DACCONTROL3, 0x04);  // 0x04 mute/0x00 unmute&ramp;DAC unmute and  disabled digital volume control soft ramp
    if (res) return res;
    /* Chip Control and Power Management */
    res |= es_write_reg(i2c_iface, ES8388_CONTROL2, 0x50);
    res |= es_write_reg(i2c_iface, ES8388_CHIPPOWER, 0x00); //normal all and power up all

    // Disable the internal DLL to improve 8K sample rate
    res |= es_write_reg(i2c_iface, 0x35, 0xA0);
    res |= es_write_reg(i2c_iface, 0x37, 0xD0);
    res |= es_write_reg(i2c_iface, 0x39, 0xD0);

    res |= es_write_reg(i2c_iface, ES8388_MASTERMODE, codec_cfg->i2s_iface.mode); //CODEC IN I2S SLAVE MODE

    /* dac */
    res |= es_write_reg(i2c_iface, ES8388_DACPOWER, 0xC0);  //disable DAC and disable Lout/Rout/1/2
    res |= es_write_reg(i2c_iface, ES8388_CONTROL1, 0x12);  //Enfr=0,Play&Record Mode,(0x17-both of mic&paly)
//    res |= es_write_reg(i2c_iface, ES8388_CONTROL2, 0);  //LPVrefBuf=0,Pdn_ana=0
    res |= es_write_reg(i2c_iface, ES8388_DACCONTROL1, 0x18);//1a 0x18:16bit iis , 0x00:24
    res |= es_write_reg(i2c_iface, ES8388_DACCONTROL2, 0x02);  //DACFsMode,SINGLE SPEED; DACFsRatio,256
    res |= es_write_reg(i2c_iface, ES8388_DACCONTROL16, 0x00); // 0x00 audio on LIN1&RIN1,  0x09 LIN2&RIN2
    res |= es_write_reg(i2c_iface, ES8388_DACCONTROL17, 0x90); // only left DAC to left mixer enable 0db
    res |= es_write_reg(i2c_iface, ES8388_DACCONTROL20, 0x90); // only right DAC to right mixer enable 0db
    res |= es_write_reg(i2c_iface, ES8388_DACCONTROL21, 0x80); //set internal ADC and DAC use the same LRCK clock, ADC LRCK as internal LRCK
    res |= es_write_reg(i2c_iface, ES8388_DACCONTROL23, 0x00);   //vroi=0
    res |= es8388_set_adc_dac_volume(i2c_iface, ES_MODULE_DAC, 0, 0);          // 0db
    int tmp = 0;
    if (AUDIO_DAC_OUTPUT_LINE2 == codec_cfg->dac_output) {
        tmp = DAC_OUTPUT_LOUT1 | DAC_OUTPUT_ROUT1;
    } else if (AUDIO_DAC_OUTPUT_LINE1 == codec_cfg->dac_output) {
        tmp = DAC_OUTPUT_LOUT2 | DAC_OUTPUT_ROUT2;
    } else {
        tmp = DAC_OUTPUT_LOUT1 | DAC_OUTPUT_LOUT2 | DAC_OUTPUT_ROUT1 | DAC_OUTPUT_ROUT2;
    }
    res |= es_write_reg(i2c_iface, ES8388_DACPOWER, tmp);  //0x3c Enable DAC and Enable Lout/Rout/1/2
    /* adc */
    res |= es_write_reg(i2c_iface, ES8388_ADCPOWER, 0xFF);
    res |= es_write_reg(i2c_iface, ES8388_ADCCONTROL1, 0xbb); // MIC Left and Right channel PGA gain
    tmp = 0;
    if (AUDIO_ADC_INPUT_LINE1 == codec_cfg->adc_input) {
        tmp = ADC_INPUT_LINPUT1_RINPUT1;
    } else if (AUDIO_ADC_INPUT_LINE2 == codec_cfg->adc_input) {
        tmp = ADC_INPUT_LINPUT2_RINPUT2;
    } else {
        tmp = ADC_INPUT_DIFFERENCE;
    }
    res |= es_write_reg(i2c_iface, ES8388_ADCCONTROL2, tmp);  //0x00 LINSEL & RINSEL, LIN1/RIN1 as ADC Input; DSSEL,use one DS Reg11; DSR, LINPUT1-RINPUT1
    res |= es_write_reg(i2c_iface, ES8388_ADCCONTROL3, 0x02);
    res |= es_write_reg(i2c_iface, ES8388_ADCCONTROL4, 0x0c); // 16 Bits length and I2S serial audio data format
    res |= es_write_reg(i2c_iface, ES8388_ADCCONTROL5, 0x02);  //ADCFsMode,singel SPEED,RATIO=256
    //ALC for Microphone
    res |= es8388_set_adc_dac_volume(i2c_iface, ES_MODULE_ADC, 0, 0);      // 0db
    res |= es_write_reg(i2c_iface, ES8388_ADCPOWER, 0x09); //Power on ADC, Enable LIN&RIN, Power off MICBIAS, set int1lp to low power mode

    res |= es_write_reg(i2c_iface, ES8388_DACCONTROL6, 0x28);

    /* enable es8388 PA */
    es8388_pa_power(true, codec_cfg->codec_pa_pin, codec_cfg->codec_spk_en_pin);

    LOGD(__func__, "init, out:%02x, in:%02x", codec_cfg->dac_output, codec_cfg->adc_input);
    return res;
}

int es8388_deinit(audio_dev_ctrl_interface_t *i2c_iface)
{
    int res = 0;
    res = es_write_reg(i2c_iface, ES8388_CHIPPOWER, 0xFF);  //reset and stop es8388
    return res;
}

/**
 * @brief Configure ES8388 I2S format
 *
 * @param mode:           set ADC or DAC or all
 * @param bitPerSample:   see Es8388I2sFmt
 *
 * @return
 *     - (-1) Error
 *     - (0)  Success
 */
int es8388_config_fmt(audio_dev_ctrl_interface_t *i2c_iface, es_module_t mode, es_i2s_fmt_t fmt)
{
    int res = 0;
    uint8_t reg = 0;
    if (mode == ES_MODULE_ADC || mode == ES_MODULE_ADC_DAC) {
        res = es_read_reg(i2c_iface, ES8388_ADCCONTROL4, &reg);
        reg = reg & 0xfc;
        res |= es_write_reg(i2c_iface, ES8388_ADCCONTROL4, reg | fmt);
    }
    if (mode == ES_MODULE_DAC || mode == ES_MODULE_ADC_DAC) {
        res = es_read_reg(i2c_iface, ES8388_DACCONTROL1, &reg);
        reg = reg & 0xf9;
        res |= es_write_reg(i2c_iface, ES8388_DACCONTROL1, reg | (fmt << 1));
    }
    return res;
}

/**
 * @param volume: 0 ~ 100
 *
 * @return
 *     - (-1)  Error
 *     - (0)   Success
 */
int es8388_set_voice_volume(audio_dev_ctrl_interface_t *i2c_iface, int volume)
{
    int res = 0;
    if (volume < 0)
        volume = 0;
    else if (volume > 100)
        volume = 100;
    volume /= 3;
    res = es_write_reg(i2c_iface, ES8388_DACCONTROL24, volume);
    res |= es_write_reg(i2c_iface, ES8388_DACCONTROL25, volume);
    res |= es_write_reg(i2c_iface, ES8388_DACCONTROL26, 0);
    res |= es_write_reg(i2c_iface, ES8388_DACCONTROL27, 0);
    return res;
}

/**
 *
 * @return
 *           volume
 */
int es8388_get_voice_volume(audio_dev_ctrl_interface_t *i2c_iface, int *volume)
{
    int res = 0;
    uint8_t reg = 0;
    res = es_read_reg(i2c_iface, ES8388_DACCONTROL24, &reg);
    if (res == -1) {
        *volume = 0;
    } else {
        *volume = reg;
        *volume *= 3;
        if (*volume == 99)
            *volume = 100;
    }
    return res;
}

/**
 * @brief Configure ES8388 data sample bits
 *
 * @param mode:             set ADC or DAC or all
 * @param bitPerSample:   see BitsLength
 *
 * @return
 *     - (-1) Parameter error
 *     - (0)   Success
 */
int es8388_set_bits_per_sample(audio_dev_ctrl_interface_t *i2c_iface, es_module_t mode, es_bits_length_t bits_length)
{
    int res = 0;
    uint8_t reg = 0;
    int bits = (int)bits_length;

    if (mode == ES_MODULE_ADC || mode == ES_MODULE_ADC_DAC) {
        res = es_read_reg(i2c_iface, ES8388_ADCCONTROL4, &reg);
        reg = reg & 0xe3;
        res |=  es_write_reg(i2c_iface, ES8388_ADCCONTROL4, reg | (bits << 2));
    }
    if (mode == ES_MODULE_DAC || mode == ES_MODULE_ADC_DAC) {
        res = es_read_reg(i2c_iface, ES8388_DACCONTROL1, &reg);
        reg = reg & 0xc7;
        res |= es_write_reg(i2c_iface, ES8388_DACCONTROL1, reg | (bits << 3));
    }
    return res;
}

/**
 * @param gain: Config DAC Output
 *
 * @return
 *     - (-1) Parameter error
 *     - (0)   Success
 */
int es8388_config_dac_output(audio_dev_ctrl_interface_t *i2c_iface, int output)
{
    int res;
    uint8_t reg = 0;
    res = es_read_reg(i2c_iface, ES8388_DACPOWER, &reg);
    reg = reg & 0xc3;
    res |= es_write_reg(i2c_iface, ES8388_DACPOWER, reg | output);
    return res;
}

/**
 * @param gain: Config ADC input
 *
 * @return
 *     - (-1) Parameter error
 *     - (0)   Success
 */
int es8388_config_adc_input(audio_dev_ctrl_interface_t *i2c_iface, es_adc_input_t input)
{
    int res;
    uint8_t reg = 0;
    res = es_read_reg(i2c_iface, ES8388_ADCCONTROL2, &reg);
    reg = reg & 0x0f;
    res |= es_write_reg(i2c_iface, ES8388_ADCCONTROL2, reg | input);
    return res;
}

/**
 * @param gain: see es_mic_gain_t
 *
 * @return
 *     - (-1) Parameter error
 *     - (0)   Success
 */
int es8388_set_mic_gain(audio_dev_ctrl_interface_t *i2c_iface, es_mic_gain_t gain)
{
    int res, gain_n;
    gain_n = (int)gain / 3;
    gain_n = (gain_n << 4) + gain_n;
    res = es_write_reg(i2c_iface, ES8388_ADCCONTROL1, gain_n); //MIC PGA
    return res;
}

int es8388_ctrl_state(audio_dev_ctrl_interface_t *i2c_iface, audio_codec_mode_t mode, audio_ctrl_t ctrl_state)
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
            LOGW(__func__, "Codec mode not support, default is decode mode");
            break;
    }
    if (AUDIO_CTRL_STOP == ctrl_state) {
        res = es8388_stop(i2c_iface, es_mode_t);
    } else {
        res = es8388_start(i2c_iface, es_mode_t);
        LOGD(__func__, "start default is decode mode:%d", es_mode_t);
    }
    return res;
}

int es8388_config_i2s(audio_dev_ctrl_interface_t *i2c_iface, audio_codec_mode_t mode, audio_codec_i2s_iface_t *iface)
{
    (void)(mode);

    int res = 0;
    int tmp = 0;
    res |= es8388_config_fmt(i2c_iface, ES_MODULE_ADC_DAC, iface->fmt);
    if (iface->bits == AUDIO_BIT_LENGTH_16BITS) {
        tmp = BIT_LENGTH_16BITS;
    } else if (iface->bits == AUDIO_BIT_LENGTH_24BITS) {
        tmp = BIT_LENGTH_24BITS;
    } else {
        tmp = BIT_LENGTH_32BITS;
    }
    res |= es8388_set_bits_per_sample(i2c_iface, ES_MODULE_ADC_DAC, tmp);
    return res;
}

int audio_es8388_codec_init(audio_dev_t *dev, audio_dev_config_t *cfg)
{
    int ret = 0;
    ret = es8388_init(dev, cfg);
    if (ret < 0) return ret;
    ret = es8388_ctrl_state(&dev->cfg.i2c_iface, dev->cfg.codec_mode, AUDIO_CTRL_START);
    if (ret < 0) return ret;
    ret = es8388_set_voice_volume(&dev->cfg.i2c_iface, dev->cfg.default_volume);
    if (ret < 0) return ret;
    /*es8388_read_all(&dev->cfg.i2c_iface);*/
    return ret;
}

int es8388_codec_deinit(audio_dev_t *dev)
{
    es8388_deinit(&dev->cfg.i2c_iface);
    (void)dev;
    return 0;
}

int es8388_codec_set_voice_mute(audio_dev_t *dev, bool enable)
{
    LOGD(__func__, "SetVoiceMute volume");
    es8388_set_voice_mute(&dev->cfg.i2c_iface, enable);
    return 0;
}

int es8388_codec_set_voice_volume(audio_dev_t *dev, int volume)
{
    return es8388_set_voice_volume(&dev->cfg.i2c_iface, volume);
}

int es8388_enable_speaker_power(audio_dev_t *dev, bool enable)
{
    audio_dev_config_t *codec_cfg = &dev->cfg;
    es8388_pa_power(enable, codec_cfg->codec_pa_pin, codec_cfg->codec_spk_en_pin);

    return 0;
}

audio_dev_ops_t es8388_ops = {
    .init = audio_es8388_codec_init,
    .release = es8388_codec_deinit,
    .suspend = NULL, // TODO: add wrapper function to use es8388_suspend,
    .resume = NULL,
    .speaker_mute = es8388_codec_set_voice_mute,
    .speaker_set_volume = es8388_codec_set_voice_volume,
    .mic_mute = NULL,
    .mic_set_volume = NULL,
    .enable_speaker_power = es8388_enable_speaker_power,
};

audio_dev_t es8388_audio_dev = {
    .name = "es8388",
    .ops = &es8388_ops,
    .is_initialized = 0,
    .cfg = AUDIO_DEV_CONFIG_INITIALIZER, // NOTE: this will be ovveride when init
    .priv = NULL,
};

//NOTE: use apis in audio_dev.h
/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_AUDIO_CODEC, es8388,    0,      &es8388_audio_dev,    NULL);
