#ifndef _DRIVER_I2S_H
#define _DRIVER_I2S_H

#include <stdint.h>
#include "dmac/dmac.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2S_INTR_PRI              (19 << 3)

typedef enum _i2s_device_number
{
    I2S_DEVICE_0 = 0,
    I2S_DEVICE_MAX
} i2s_device_number_t;

typedef enum _i2s_channel_num
{
    I2S_CHANNEL_0 = 0,
    I2S_CHANNEL_1 = 1,
    I2S_CHANNEL_2 = 2,
    I2S_CHANNEL_3 = 3,
    I2S_CHANNEL_MAX
} i2s_channel_num_t;

typedef enum _i2s_work_mode
{
    I2S_SLAVE  = 0,
    I2S_MASTER = 1
} i2s_work_mode_t;

typedef enum _i2s_rxtx_mode
{
    I2S_RX   = 0x1 << 0,
    I2S_TX   = 0x1 << 1,
    I2S_RXTX = (0x1 << 0 | 0x1 << 1)
} i2s_rxtx_mode_t;

typedef enum _i2s_work_format
{
    STANDARD_MODE = 1,
    RIGHT_JUSTIFYING_MODE = 2,
    LEFT_JUSTIFYING_MODE = 4
} i2s_work_format_t;

typedef enum _sclk_gating_cycles
{
    /* Clock gating is diable */
    NO_CLOCK_GATING = 0x0,
    /* Gating after 12 sclk cycles */
    CLOCK_CYCLES_12 = 0x1,
    /* Gating after 16 sclk cycles */
    CLOCK_CYCLES_16 = 0x2,
    /* Gating after 20 sclk cycles */
    CLOCK_CYCLES_20 = 0x3,
    /* Gating after 24 sclk cycles */
    CLOCK_CYCLES_24 = 0x4
} i2s_sclk_gating_cycles_t;

typedef enum _word_select_cycles
{
    /* 16 sclk cycles */
    SCLK_CYCLES_16 = 0x0,
    /* 24 sclk cycles */
    SCLK_CYCLES_24 = 0x1,
    /* 32 sclk cycles */
    SCLK_CYCLES_32 = 0x2
} i2s_word_select_cycles_t;

typedef enum _word_length
{
    /* Ignore the word length */
    IGNORE_WORD_LENGTH = 0x0,
    /* 12-bit data resolution of the receiver */
    RESOLUTION_12_BIT = 0x1,
    /* 16-bit data resolution of the receiver */
    RESOLUTION_16_BIT = 0x2,
    /* 20-bit data resolution of the receiver */
    RESOLUTION_20_BIT = 0x3,
    /* 24-bit data resolution of the receiver */
    RESOLUTION_24_BIT = 0x4,
    /* 32-bit data resolution of the receiver */
    RESOLUTION_32_BIT = 0x5
} i2s_word_length_t;

typedef enum _fifo_threshold
{
    /* Interrupt trigger when FIFO level is 1 */
    TRIGGER_LEVEL_1 = 0x0,
    /* Interrupt trigger when FIFO level is 2 */
    TRIGGER_LEVEL_2 = 0x1,
    /* Interrupt trigger when FIFO level is 3 */
    TRIGGER_LEVEL_3 = 0x2,
    /* Interrupt trigger when FIFO level is 4 */
    TRIGGER_LEVEL_4 = 0x3,
    /* Interrupt trigger when FIFO level is 5 */
    TRIGGER_LEVEL_5 = 0x4,
    /* Interrupt trigger when FIFO level is 6 */
    TRIGGER_LEVEL_6 = 0x5,
    /* Interrupt trigger when FIFO level is 7 */
    TRIGGER_LEVEL_7 = 0x6,
    /* Interrupt trigger when FIFO level is 8 */
    TRIGGER_LEVEL_8 = 0x7,
    /* Interrupt trigger when FIFO level is 9 */
    TRIGGER_LEVEL_9 = 0x8,
    /* Interrupt trigger when FIFO level is 10 */
    TRIGGER_LEVEL_10 = 0x9,
    /* Interrupt trigger when FIFO level is 11 */
    TRIGGER_LEVEL_11 = 0xa,
    /* Interrupt trigger when FIFO level is 12 */
    TRIGGER_LEVEL_12 = 0xb,
    /* Interrupt trigger when FIFO level is 13 */
    TRIGGER_LEVEL_13 = 0xc,
    /* Interrupt trigger when FIFO level is 14 */
    TRIGGER_LEVEL_14 = 0xd,
    /* Interrupt trigger when FIFO level is 15 */
    TRIGGER_LEVEL_15 = 0xe,
    /* Interrupt trigger when FIFO level is 16 */
    TRIGGER_LEVEL_16 = 0xf
} i2s_fifo_threshold_t;

typedef enum _i2s_channel_mode
{
    CHANNEL_MONO = 0,
    CHANNEL_STEREO = 1
} i2s_channel_mode_t;

typedef struct {
    i2s_device_number_t dev_num;
    i2s_channel_num_t ch_num;
    i2s_work_mode_t work_mode;
    i2s_rxtx_mode_t rxtx_mode;
    uint32_t sample_rate;
    uint32_t xfs;
    i2s_word_select_cycles_t word_select_cycles;
    i2s_word_length_t rx_word_length;
    i2s_fifo_threshold_t rx_fifo_threshold;
    i2s_word_length_t tx_word_length;
    i2s_fifo_threshold_t tx_fifo_threshold;
} i2s_config_t;

typedef struct {
    uint8_t use_dma;
    uint8_t auto_clear;
    i2s_rxtx_mode_t rxtx_mode;
    uint32_t rx_dma_queue_size;
    uint32_t tx_dma_queue_size;
    uint32_t rx_dma_queue_num;
    uint32_t tx_dma_queue_num;
} i2s_dma_config_t;

typedef struct {
    i2s_config_t i2s_config;
    i2s_dma_config_t i2s_dma_config;
} i2s_init_config_t;

/**
* @brief       I2s init
*
* @return      result
*     - 0      Success
*     - Other  Fail
*
*/
int i2s_init(i2s_device_number_t device_num);

/**
 * @brief       I2S receive channel configure
 *
 * @param[in]   device_num      The device number
 * @param[in]   channel_num     The channel number
 * @param[in]   word_length     The word length
 * @param[in]   trigger_level   The trigger level
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int i2s_rx_channel_config(i2s_device_number_t device_num,
                    i2s_channel_num_t channel_num,
                    i2s_word_length_t word_length,
                    i2s_fifo_threshold_t trigger_level);

/**
 * @brief       I2S transmit channel configure
 *
 * @param[in]   device_num      The device number
 * @param[in]   channel_num     The channel number
 * @param[in]   word_length     The word length
 * @param[in]   trigger_level   The trigger level
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int i2s_tx_channel_config(i2s_device_number_t device_num,
                    i2s_channel_num_t channel_num,
                    i2s_word_length_t word_length,
                    i2s_fifo_threshold_t trigger_level);

/**
 * @brief       Set sample rate
 *
 * @param[in]   device_num              The I2S device number
 * @param[in]   sample_rate             The sample rate
 * @param[in]   xfs                     The multiple of mclk to sample rate, such as 128, 256, 384, 512...
 * @param[in]   word_select_cycles      The word select cycles
 *
 * @return      result
 *     - <0     Fail
 *     - other  The actual sample rate
 */
int i2s_set_sample_rate(i2s_device_number_t device_num, uint32_t sample_rate,
                     uint32_t xfs, i2s_word_select_cycles_t word_select_cycles);

/**
 * @brief       I2S master clock configure
 *
 * @param[in]   device_num              The I2S device number
 * @param[in]   word_select_cycles      Word select cycles
 * @param[in]   sclk_gating_cycles      SCLK gating cycles
 *
 * @return      void
 */
void i2s_master_clock_config(i2s_device_number_t device_num,
                        i2s_word_select_cycles_t word_select_cycles,
                        i2s_sclk_gating_cycles_t sclk_gating_cycles);

/**
 * @brief       Register i2s irq.
 *
 * @param[in]   device_num              The I2S device number
 * @param[in]   channel_num             The I2S channel number
 * @param[in]   rxtx_mode               The mode of interrupt
 * @param[in]   i2s_callback            The callback of i2s
 * @param[in]   ctx                     Param of call back
 * @param[in]   priority                The priority of the i2s irq
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int i2s_irq_register(i2s_device_number_t device_num, i2s_channel_num_t channel_num,
                    i2s_rxtx_mode_t rxtx_mode, xvic_irq_callback_t i2s_callback, void *ctx, uint32_t priority);

/**
 * @brief       Enable I2S interrupt.
 *
 * @param[in]   device_num              The I2S device number
 * @param[in]   channel_num             The I2S channel number
 * @param[in]   rx_available_int_enable RX FIFO available interrupt enable
 * @param[in]   rx_overrun_int_enable   RX FIFO overrun interrupt enable
 * @param[in]   tx_empty_int_enable     TX FIFO empty interrupt enable
 * @param[in]   tx_overrun_int_enable   TX FIFO overrun interrupt enable
 *
 * @return      void
 */
void i2s_irq_enable(i2s_device_number_t device_num, i2s_channel_num_t channel_num,
               uint32_t rx_available_int_enable, uint32_t rx_overrun_int_enable,
               uint32_t tx_empty_int_enable, uint32_t tx_overrun_int_enable);

/**
 * @brief       I2S channel configure.
 *
 * @param[in]   i2s_config              I2S channel config
 *
 * @return      void
 */
void i2s_channel_config(i2s_config_t *i2s_config);

/**
 * @brief       I2S channel configure.
 *
 * @param[in]   i2s_config              I2S channel config
 *
 * @return      void
 */
void i2s_channel_init(i2s_init_config_t *i2s_init_config);

/**
 * @brief       Read I2S data via FIFO mode
 *
 * @param[in]   device_num      The I2S device number
 * @param[in]   channel_num     The I2S channel number
 * @param[in]   buf             Data buffer to save read data
 * @param[in]   buf_len_words   The word length read
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int i2s_receive_data(i2s_device_number_t device_num, i2s_channel_num_t channel_num,
                    uint32_t *buf, size_t buf_len);

/**
 * @brief       Read I2S data via DMA mode, will waiting for dmac done
 *
 * @param[in]   device_num      The I2S device number
 * @param[in]   channel_num     The DMA channel number
 * @param[in]   buf             Data buffer to save read data
 * @param[in]   buf_len_words   The word length read
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int i2s_receive_data_dma(i2s_device_number_t device_num, dmac_channel_number_t channel_num,
                    uint32_t *buf, size_t buf_len_words);

/**
 * @brief       Send I2S data via FIFO mode
 *
 * @param[in]   device_num      The I2S device number
 * @param[in]   channel_num     The I2S channel number
 * @param[in]   buf             Data buffer to send data
 * @param[in]   buf_len_words   The word length read
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int i2s_send_data(i2s_device_number_t device_num, i2s_channel_num_t channel_num,
                    const uint32_t *buf, size_t buf_len);

/**
 * @brief       Send I2S data via DMA mode, will waiting for dmac done
 *
 * @param[in]   device_num      The I2S device number
 * @param[in]   channel_num     The DMA channel number
 * @param[in]   buf             Data buffer to send data
 * @param[in]   buf_len_words   The word length read
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int i2s_send_data_dma(i2s_device_number_t device_num, dmac_channel_number_t channel_num,
                    const uint32_t *buf, size_t buf_len);

/**
* @brief       I2s lookback test
*
* @param[in]   device_num              The I2S device number
* @param[in]   channel_num             The I2S channel number
*
* @return      result
*     - 0      Success
*     - Other  Fail
*
*/
int i2s_loopback_test(i2s_device_number_t device_num, i2s_channel_num_t channel_num);

/**
 * @brief       Initialize I2S work with DMA.
 *
 * @param[in]   i2s_dev_num             The I2S device number
 * @param[in]   i2s_ch_num              The I2S channel number
 * @param[in]   rxtx_mode               The work mode of I2S channel
 * @param[in]   dma_ch_num              The DMA channel number
 * @param[in]   dma_buf_cnt             The DMA buffer count
 * @param[in]   dma_buf_size            The DMA buffer size
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int i2s_dma_init(i2s_device_number_t i2s_dev_num, i2s_dma_config_t *i2s_dma_config);

/**
 * @brief       Send I2S data via DMA mode, it is non-blocking, can realize continuously send.
 *
 * @param[in]   src                      Data buffer need to send
 * @param[in]   size                     Data size
 * @param[in]   bytes_written            Data size of actual sent 
 * @param[in]   timeout_ms               Timeout in ms 
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int i2s_dma_write(uint8_t *src, size_t size, size_t *bytes_written, uint32_t timeout_ms);

/**
 * @brief       Receive I2S data via DMA mode, it is non-blocking, can realize continuously receive.
 *
 * @param[in]   src                      Data buffer used to receive
 * @param[in]   size                     Data size
 * @param[in]   bytes_written            Data size of actual read
 * @param[in]   timeout_ms               Timeout in ms 
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int i2s_dma_read(uint8_t *dest, size_t size, size_t *bytes_read, uint32_t timeout_ms);

/**
 * @brief       Clear dma buffer to avoid repeat redundant audio data.
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int i2s_dma_clear(void);

/**
 * @brief       Convert PCM data to raw data that I2S can send directly.
 *
 * @param[in]   pcm_data                 PCM data
 * @param[in]   pcm_data_len_bytes       PCM data length in bytes
 * @param[in]   raw_data                 Raw data
 * @param[in]   raw_data_len_words       Raw data length in words
 * @param[in]   word_length              Word length (Bits per sample)
 * @param[in]   channel_mode             Channel mode(mono or stereo)
 *
 * @return      void
 */
void i2s_convert_pcm_to_raw_data(const uint8_t *pcm_data, size_t pcm_data_len_bytes, uint32_t *raw_data, uint32_t *raw_data_len_words,
                        i2s_word_length_t word_length, i2s_channel_mode_t channel_mode);

/**
 * @brief       Convert raw data that received from I2S to PCM data.
 *
 * @param[in]   raw_data                 Raw data
 * @param[in]   raw_data_len_words       Raw data length in words
 * @param[in]   pcm_data                 PCM data
 * @param[in]   pcm_data_len_bytes       PCM data length in bytes
 * @param[in]   word_length              Word length (Bits per sample)
 * @param[in]   channel_mode             Channel mode(mono or stereo)
 *
 * @return      void
 */
void i2s_convert_raw_data_to_pcm(const uint32_t *raw_data, uint32_t raw_data_len_words, uint8_t *pcm_data, uint32_t *pcm_data_len_bytes,
                        i2s_word_length_t word_length, i2s_channel_mode_t channel_mode);

/**
 * @brief       Get I2S lock.
 *
 * @param[in]   timeout_ms               Timeout in ms 
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int i2s_dma_get_lock(uint32_t timeout_ms);

/**
 * @brief       Release I2S lock.
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int i2s_dma_release_lock(void);

#ifdef __cplusplus
}
#endif

#endif
