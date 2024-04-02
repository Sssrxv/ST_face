#ifndef _DRIVER_I2C_H
#define _DRIVER_I2C_H

#include <stdint.h>
#include <stddef.h>
#include "dmac/dmac.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_MAX_NUM 5

#define I2C0_DMA_TX_CH      (DMAC_CHANNEL2)
#define I2C0_DMA_RX_CH      (DMAC_CHANNEL3)


typedef enum _i2c_device_number
{
    I2C_DEVICE_0 = 0,
    I2C_DEVICE_1,
    I2C_DEVICE_2,
    I2C_DEVICE_3,
    I2C_DEVICE_4,
    I2C_DEVICE_5,
    I2C_DEVICE_MAX,
} i2c_device_number_t;

typedef enum _i2c_bus_speed_mode
{
    I2C_BS_STANDARD,
    I2C_BS_FAST,
    I2C_BS_HIGHSPEED
} i2c_bus_speed_mode_t;

typedef enum _i2c_event
{
    I2C_EV_START,
    I2C_EV_RESTART,
    I2C_EV_STOP
} i2c_event_t;

typedef struct _i2c_slave_handler
{
    void(*on_receive)(uint32_t data);
    uint32_t(*on_transmit)();
    void(*on_event)(i2c_event_t event);
} i2c_slave_handler_t;

/**
 * @brief       Definitions for the i2c interrupt callbacks
 */
typedef int (*i2c_irq_callback_t)(void * ctx);

/**
 * @brief       I2c interrupt context
 *
 * @param[in]                   i2c_num          i2c number
 * @param[i2c_irq_callback_t]   i2c_callback     customized i2c interrupt callback function pointer
 * @param[void*]                data             private data pointer
*/
typedef struct _i2c_irq_context
{
    i2c_device_number_t i2c_num;    
    i2c_irq_callback_t callback;
    void * data;
} i2c_irq_context_t;

/**
 * @brief       Set i2c params
 *
 * @param[in]   i2c_num             i2c number
 * @param[in]   slave_address       i2c slave device address
 * @param[in]   address_width       address width 7bit or 10bit
 * @param[in]   i2c_clk             i2c clk rate
 */
void i2c_init(i2c_device_number_t i2c_num, uint32_t slave_address, uint32_t address_width,
              uint32_t i2c_clk);

/**
 * @brief       I2c send data
 *
 * @param[in]   i2c_num         i2c number
 * @param[in]   send_buf        send data
 * @param[in]   send_buf_len    send data length
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int i2c_send_data(i2c_device_number_t i2c_num, uint32_t slave_address, const uint8_t *send_buf, size_t send_buf_len);


/**
 * @brief       I2c send data by dma
 *
 * @param[in]   i2c_num             i2c number
 * @param[in]   slave_address       i2c slave device address
 * @param[in]   send_buf            send data
 * @param[in]   send_buf_len        send data length
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int i2c_send_data_dma(i2c_device_number_t i2c_num, uint32_t slave_address, const uint8_t *send_buf, size_t send_buf_len);

/** 
 * @brief  i2c_send_multi_addr_data_dma 
 * 
 * @param dma_channel_num
 * @param i2c_num
 * @param buffer                    buffer of mutliple addrs and data
 * @param addr_len                  addr bytes in each table item
 * @param data_len                  data bytes in each table item
 * @param stride                    table item stride in bytes
 * @param cnt                       table item count
 * 
 * @return   
 *     - 0      Success
 *     - Other  Fail
 */
int i2c_send_multi_addr_data_dma(dmac_channel_number_t dma_channel_num, 
        i2c_device_number_t i2c_num, const void *buffer,
        uint8_t  addr_len, uint8_t data_len, uint8_t stride,
        uint16_t cnt
        );

/**
 * @brief       I2c receive data
 *
 * @param[in]   i2c_num             i2c number
 * @param[in]   send_buf            send data address
 * @param[in]   send_buf_len        length of send buf
 * @param[in]   receive_buf         receive buf address
 * @param[in]   receive_buf_len     length of receive buf
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 *
 * @note  this function will auto generate restart (not stop + start)
 *        after send data, before receive data, this will not work on
 *        some devices. you can use i2c_recv_data which will generate
 *        stop + start after send data
*/
int i2c_recv_data(i2c_device_number_t i2c_num, uint32_t slave_address, const uint8_t *send_buf, size_t send_buf_len, uint8_t *receive_buf,
                  size_t receive_buf_len);

/**
 * @brief       I2c send receive data
 *
 * @param[in]   i2c_num             i2c number
 * @param[in]   send_buf            send data address
 * @param[in]   send_buf_len        length of send buf
 * @param[in]   receive_buf         receive buf address
 * @param[in]   receive_buf_len     length of receive buf
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 *
 * @note  this function will will generate
 *        stop + start after send data
*/
int i2c_send_recv_data(i2c_device_number_t i2c_num, uint32_t slave_address, const uint8_t *send_buf, size_t send_buf_len, uint8_t *receive_buf,
                  size_t receive_buf_len);

/**
 * @brief       I2c receive data by dma
 *
 * @param[in]   i2c_num             i2c number
 * @param[in]   slave_address       i2c slave device address
 * @param[in]   send_buf            send data address
 * @param[in]   send_buf_len        length of send buf
 * @param[in]   receive_buf         receive buf address
 * @param[in]   receive_buf_len     length of receive buf
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
*/
int i2c_recv_data_dma(i2c_device_number_t i2c_num, uint32_t slave_address, const uint8_t *send_buf, size_t send_buf_len,
                       uint8_t *receive_buf, size_t receive_buf_len);

/**
 * @brief       I2c interrupt initialize
 *
 * @param[in]   i2c_num          i2c number
 * @param[i2c_irq_callback_t]   i2c_callback     customized i2c interrupt callback function pointer
 * @param[void*]   data       private data pointer
 * @param[in]   priority         interrupt priority
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
*/
int i2c_irq_init(i2c_device_number_t i2c_num, i2c_irq_callback_t i2c_callback, void * data, uint32_t priority);

/**
 * @brief       I2c interrupt uninitialize
 *
 * @param[in]   i2c_num        i2c number
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
*/
int i2c_irq_uninit(i2c_device_number_t i2c_num);


/**
 * @brief       scan devices on the i2c bus
 *
 * @param[in]   i2c_num             i2c number
 * @param[in]   address_width       address width 7bit or 10bit
 * @param[in]   i2c_clk             i2c clk rate
 *
 * @return      result
 */
int i2c_scan_dev_addr(i2c_device_number_t i2c_num, uint32_t address_width, uint32_t i2c_clk);

/**
 * @brief       ping a device on the i2c bus
 *
 * @param[in]   i2c_num             i2c number
 * @param[in]   dev_addr            device address to ping
 * @param[in]   address_width       address width 7bit or 10bit
 * @param[in]   i2c_clk             i2c clk rate
 *
 * @return      result
 */
int i2c_ping_dev(i2c_device_number_t i2c_num, uint16_t dev_addr, uint32_t address_width, uint32_t i2c_clk);


void i2c_disable_sleep(void);


void i2c_enable_sleep(void);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_I2C_H */
