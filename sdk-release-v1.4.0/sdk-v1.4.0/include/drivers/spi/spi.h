#ifndef _DRIVER_SPI_H
#define _DRIVER_SPI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "dmac/dmac.h"

#ifdef __cplusplus
extern "C" {
#endif
#define SPI_INTR_PRI              (19 << 3)

typedef enum spi_device_num
{
    SPI_DEVICE_0 = 0, /*!< SPI device 0 */
    SPI_DEVICE_1,     /*!< SPI device 1 */
    SPI_DEVICE_SLAVE, /*!< SPI device 2 (slave) */
    SPI_DEVICE_MAX,
} spi_device_num_t;

typedef enum spi_work_mode
{
    SPI_WORK_MODE_0, /*!< SPI work in mode 0 */
    SPI_WORK_MODE_1, /*!< SPI work in mode 1 */
    SPI_WORK_MODE_2, /*!< SPI work in mode 2 */
    SPI_WORK_MODE_3, /*!< SPI work in mode 3 */
} spi_work_mode_t;

typedef enum spi_frame_format
{
    SPI_FF_STANDARD, /*!< Standard mode frame format */
    SPI_FF_DUAL,     /*!< DUAL mode frame format */
    SPI_FF_QUAD,     /*!< QUAD mode frame format */
    SPI_FF_OCTAL     /*!< OCTAL mode frame format */
} spi_frame_format_t;

typedef enum spi_instruction_address_trans_mode
{
    SPI_AITM_STANDARD = 0,   /*!< Instruction and address will be sent in standard SPI Mode */
    SPI_AITM_ADDR_STANDARD,  /*!< Instruction will be sent in standard SPI mode and address will be sent in specified mode*/
    SPI_AITM_AS_FRAME_FORMAT /*!< Both instruction and address will be sent in specified mode */
} spi_instruction_address_trans_mode_t;

typedef enum spi_transfer_width
{
    SPI_TRANS_CHAR  = 0x1, /*!< SPI transfer width: 1 byte */
    SPI_TRANS_SHORT = 0x2, /*!< SPI transfer width: 2 bytes */
    SPI_TRANS_INT   = 0x4, /*!< SPI transfer width: 4 bytes */
} spi_transfer_width_t;

typedef enum spi_chip_select
{
    SPI_CHIP_SELECT_0, /*!< SPI CS signal 0 */
    SPI_CHIP_SELECT_1, /*!< SPI CS signal 1 */
    SPI_CHIP_SELECT_2, /*!< SPI CS signal 2 */
    SPI_CHIP_SELECT_3, /*!< SPI CS signal 3 */
    SPI_CHIP_SELECT_MAX,
} spi_chip_select_t;

typedef enum spi_transfer_mode
{
    SPI_TMOD_TRANS_RECV, /*!< Transmit and receive mode */
    SPI_TMOD_TRANS,      /*!< Transmit only mode */
    SPI_TMOD_RECV,       /*!< Receive only mode */
    SPI_TMOD_EEROM       /*!< EEPROM read mode */
} spi_transfer_mode_t;

/**
 * @brief       Set spi configuration
 *
 * @param[in]   spi_num             Spi bus number
 * @param[in]   work_mode           Spi work mode
 * @param[in]   frame_format        Spi frame format
 * @param[in]   data_bit_length     Spi data bit length
 *
 * @return      Result
 *     - 0      Success
 *     - Other  Fail
 */
void spi_init(spi_device_num_t spi_num, spi_work_mode_t work_mode, spi_frame_format_t frame_format,
              size_t data_bit_length);

/**
 * @brief       Set multiline configuration
 *
 * @param[in]   spi_num                                 Spi bus number
 * @param[in]   instruction_length                      Instruction length
 * @param[in]   address_length                          Address length
 * @param[in]   wait_cycles                             Wait cycles
 * @param[in]   instruction_address_trans_mode          Spi transfer mode
 *
 */
void spi_init_non_standard(spi_device_num_t spi_num, uint32_t instruction_length, uint32_t address_length,
                           uint32_t wait_cycles, spi_instruction_address_trans_mode_t instruction_address_trans_mode);

/**
 * @brief       Enable or disable the setting of the ssi_oe_n output form serial slave.
 *
 * @param[in]   spi_num         Spi bus number
 * @param[in]   enable          Enable or disable
 *
 * @return      NULL
 *     -Tips:Relevant only when the device is configured as a serial slave device.
 *          1:must be enable before the device will receive.
 *          2:must be disable before the device will send.
 */
void spi_slave_output_enable(uint8_t spi_num, uint32_t enable);

/**
 * @brief       Spi send data
 *
 * @param[in]   spi_num         Spi bus number
 * @param[in]   chip_select     Spi chip select
 * @param[in]   cmd_buff        Spi command buffer point
 * @param[in]   cmd_len         Spi command length
 * @param[in]   tx_buff         Spi transmit buffer point
 * @param[in]   tx_len          Spi transmit buffer length
 *
 * @return      Result
 *     - 0      Success
 *     - Other  Fail
 */
int spi_send_data_standard(spi_device_num_t spi_num, spi_chip_select_t chip_select, const uint8_t *cmd_buff,
                            size_t cmd_len, const uint8_t *tx_buff, size_t tx_len);

/**
 * @brief       Spi receive data
 *
 * @param[in]   spi_num             Spi bus number
 * @param[in]   chip_select         Spi chip select
 * @param[in]   cmd_buff            Spi command buffer point
 * @param[in]   cmd_len             Spi command length
 * @param[in]   rx_buff             Spi receive buffer point
 * @param[in]   rx_len              Spi receive buffer length
 *
 * @return      Result
 *     - 0      Success
 *     - Other  Fail
 */
int spi_receive_data_standard(spi_device_num_t spi_num, spi_chip_select_t chip_select, const uint8_t *cmd_buff,
                               size_t cmd_len, uint8_t *rx_buff, size_t rx_len);

/**
 * @brief       Spi special receive data
 *
 * @param[in]   spi_num         Spi bus number
 * @param[in]   chip_select     Spi chip select
 * @param[in]   cmd_buff        Spi command buffer point
 * @param[in]   cmd_len         Spi command length
 * @param[in]   rx_buff         Spi receive buffer point
 * @param[in]   rx_len          Spi receive buffer length
 *
 * @return      Result
 *     - 0      Success
 *     - Other  Fail
 */
int spi_receive_data_multiple(spi_device_num_t spi_num, spi_chip_select_t chip_select, const uint32_t *cmd_buff,
                               size_t cmd_len, uint8_t *rx_buff, size_t rx_len);

/**
 * @brief       Spi special send data
 *
 * @param[in]   spi_num         Spi bus number
 * @param[in]   chip_select     Spi chip select
 * @param[in]   cmd_buff        Spi command buffer point
 * @param[in]   cmd_len         Spi command length
 * @param[in]   tx_buff         Spi transmit buffer point
 * @param[in]   tx_len          Spi transmit buffer length
 *
 * @return      Result
 *     - 0      Success
 *     - Other  Fail
 */
int spi_send_data_multiple(spi_device_num_t spi_num, spi_chip_select_t chip_select, const uint32_t *cmd_buff,
                            size_t cmd_len, const uint8_t *tx_buff, size_t tx_len);

/**
 * @brief       Spi send data by dma
 *
 * @param[in]   spi_num         Spi bus number
 * @param[in]   chip_select     Spi chip select
 * @param[in]   cmd_buff        Spi command buffer point
 * @param[in]   cmd_len         Spi command length
 * @param[in]   tx_buff         Spi transmit buffer point
 * @param[in]   tx_len          Spi transmit buffer length
 *
 * @return      Result
 *     - 0      Success
 *     - Other  Fail
 */
int spi_send_data_standard_dma(spi_device_num_t spi_num, spi_chip_select_t chip_select,
                                const uint8_t *cmd_buff, size_t cmd_len, const uint8_t *tx_buff, size_t tx_len);

/**
 * @brief       Spi receive data by dma
 *
 * @param[in]   spi_num             Spi bus number
 * @param[in]   chip_select         Spi chip select
 * @param[in]   cmd_buff            Spi command buffer point
 * @param[in]   cmd_len             Spi command length
 * @param[in]   rx_buff             Spi receive buffer point
 * @param[in]   rx_len              Spi receive buffer length
 *
 * @return      Result
 *     - 0      Success
 *     - Other  Fail
 */
int spi_receive_data_standard_dma( spi_device_num_t spi_num, spi_chip_select_t chip_select, const uint8_t *cmd_buff,
                                   size_t cmd_len, uint8_t *rx_buff, size_t rx_len);

/**
 * @brief       Spi special send data by dma
 *
 * @param[in]   spi_num         Spi bus number
 * @param[in]   chip_select     Spi chip select
 * @param[in]   cmd_buff        Spi command buffer point
 * @param[in]   cmd_len         Spi command length
 * @param[in]   tx_buff         Spi transmit buffer point
 * @param[in]   tx_len          Spi transmit buffer length
 *
 * @return      Result
 *     - 0      Success
 *     - Other  Fail
 */
int spi_send_data_multiple_dma( spi_device_num_t spi_num,
                                spi_chip_select_t chip_select,
                                const uint32_t *cmd_buff, size_t cmd_len, const uint8_t *tx_buff, size_t tx_len);

/**
 * @brief       Spi special receive data by dma
 *
 * @param[in]   spi_num                     Spi bus number
 * @param[in]   chip_select                 Spi chip select
 * @param[in]   cmd_buff                    Spi command buffer point
 * @param[in]   cmd_len                     Spi command length
 * @param[in]   rx_buff                     Spi receive buffer point
 * @param[in]   rx_len                      Spi receive buffer length
 *
 * @return      Result
 *     - 0      Success
 *     - Other  Fail
 */
int spi_receive_data_multiple_dma( spi_device_num_t spi_num, spi_chip_select_t chip_select, const uint32_t *cmd_buff,
                                   size_t cmd_len, uint8_t *rx_buff, size_t rx_len);

/**
 * @brief       Spi special receive data by dma
 * @note        almost the same as #spi_receive_data_multiple_dma, except it will copy cmd to rx_buffer to avoid memcpy
 *              call this api only when rx_buffer is 32 bytes aligned
 *
 * @param[in]   spi_num                     Spi bus number
 * @param[in]   chip_select                 Spi chip select
 * @param[in]   cmd_buff                    Spi command buffer point
 * @param[in]   cmd_len                     Spi command length
 * @param[in]   rx_buff                     Spi receive buffer point
 * @param[in]   rx_len                      Spi receive buffer length
 *
 * @return      Result
 *     - 0      Success
 *     - Other  Fail
 */
int spi_receive_data_multiple_dma_aligned32( spi_device_num_t spi_num, spi_chip_select_t chip_select, const uint32_t *cmd_buff,
                                  size_t cmd_len, uint8_t *rx_buff, size_t rx_len);

/** 
 * @brief  spi send data in normal mode, no command and address
 * 
 * @param[in]   spi_num                     Spi bus number
 * @param[in]   chip_select                 Spi chip select
 * @param[in]   tx_buff                     Spi transmit buffer point
 * @param[in]   tx_len                      Spi transmit buffer length
 * 
 */
int spi_send_data_normal(spi_device_num_t spi_num, spi_chip_select_t chip_select, const uint8_t *tx_buff, size_t tx_len);

/**
 * @brief       Set SPI clock rate
 *
 * @param[in]   spi_num         Spi bus number
 * @param[in]   spi_clk         Spi clock rate
 *
 * @return      The real spi clock rate
 */
uint32_t spi_set_clk_rate(spi_device_num_t spi_num, uint32_t spi_clk);

/**
 * @brief       Spi full duplex send receive data by dma
 *
 * @param[in]   spi_num                       Spi bus number
 * @param[in]   chip_select                   Spi chip select
 * @param[in]   tx_buf                        Spi send buffer
 * @param[in]   tx_len                        Spi send buffer length
 * @param[in]   rx_buf                        Spi receive buffer
 * @param[in]   rx_len                        Spi receive buffer length
 *
 */
int spi_dup_send_receive_data_dma( spi_device_num_t spi_num, spi_chip_select_t chip_select,
                               const uint8_t *tx_buf, size_t tx_len, uint8_t *rx_buf, size_t rx_len);

/**
 * @brief       Set RX sample delay
 *
 * @param[in]   spi_num                       Spi bus number
 * @param[in]   val                           RX sample delay register value
 *
 */
void spi_set_rx_samp_delay(spi_device_num_t spi_num, uint32_t val);

/**
 * @brief       Register SPI interrupt
 *
 * @param[in]   spi_num             spi index
 * @param[in]   rxtx_mode           Interrupt Mode receive or send
 * @param[in]   spi_callback        Call back
 * @param[in]   ctx                 Param of call back
 * @param[in]   priority            Interrupt priority
 * @param[in]   bool                Enable corresponding irq or not
 *
 */
void spi_irq_register(spi_device_num_t spi_num, spi_transfer_mode_t rxtx_mode, xvic_irq_callback_t spi_callback, void *ctx, uint32_t priority, bool enable_irq);

/**
 * @brief       Unregister SPI interrupt
 *
 * @param[in]   spi_num             spi index
 * @param[in]   rxtx_mode           Interrupt Mode receive or send
 * @param[in]   bool                Disable corresponding irq or not
 */
void spi_irq_unregister(spi_device_num_t spi_num, spi_transfer_mode_t rxtx_mode, bool disable_irq);

/**
 * @brief       Enable spi receive irq
 *
 * @param[in]   spi_num  spi index
 */
void spi_receive_irq_enable(spi_device_num_t spi_num);

/**
 * @brief       Disable spi receive irq
 *
 * @param[in]   spi_num  spi index
 */
void spi_receive_irq_disable(spi_device_num_t spi_num);

/**
 * @brief       Enable spi send irq
 *
 * @param[in]   spi_num  spi index
 */

void spi_send_irq_enable(spi_device_num_t spi_num);

/**
 * @brief       Disable spi send irq
 *
 * @param[in]   spi_num  spi index
 */
void spi_send_irq_disable(spi_device_num_t spi_num);

/**
 * @brief       Enable spi
 *
 * @param[in]   spi_num  spi index
 */
void spi_enable(spi_device_num_t spi_num);

/**
 * @brief       Disable spi
 *
 * @param[in]   spi_num  spi index
 */
void spi_disable(spi_device_num_t spi_num);

/**
 * @brief       Query data len current in receive buffer
 *
 * @param[in]   spi_num  spi index
 */
uint32_t spi_datalen_in_rx_buffer(spi_device_num_t spi_num);

/**
 * @brief       Directly read one data from spi data register
 *
 * @param[in]   spi_num  spi index
 * @note        you need to convert to uint8_t uint16_t or uint32_t according
 *              to configuration of spi_transfer_width_t
 */
uint32_t spi_read_one_data(spi_device_num_t spi_num);

/**
 * @brief       Directly write one data to spi data register
 *
 * @param[in]   spi_num  spi index
 * @param[in]   val  value to trans
 * @param[in]   trans_width  width of trans value
 * @note        api will convert to uint8_t uint16_t or uint32_t according
 *              to configuration of spi_transfer_width_t
 */
void spi_write_one_data(spi_device_num_t spi_num, uint32_t val, spi_transfer_width_t trans_width);

/**
 * @brief       Query whether transmit buffer is empty
 *
 * @param[in]   spi_num  spi index
 *
 * @return      true - empty, false - not empty
 */
bool spi_is_tx_empty(spi_device_num_t spi_num);

/**
 * @brief       Set baudrate of spi
 *
 * @param[in]   spi_num  spi index
 * @param[in]   spi_baudr  spi baudrate
 *
 * @return      0 for success, other for fail
 */
int spi_set_baudr(spi_device_num_t spi_num, uint32_t spi_baudr);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_SPI_H */
