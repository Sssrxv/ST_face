/**
 * @file
 * @brief       Universal Asynchronous Receiver/Transmitter (UART)
 *
 *              The UART peripheral supports the following features:
 *
 *              - 8-N-1 and 8-N-2 formats: 8 data bits, no parity bit, 1 start
 *                bit, 1 or 2 stop bits
 *
 *              - 8-entry transmit and receive FIFO buffers with programmable
 *                watermark interrupts
 *
 *              - 16× Rx oversampling with 2/3 majority voting per bit
 *
 *              The UART peripheral does not support hardware flow control or
 *              other modem control signals, or synchronous serial data
 *              tranfesrs.
 *
 *
 */

#ifndef _DRIVER_UART_H
#define _DRIVER_UART_H

#include <stdint.h>
#include "xvic/xvic.h"
#include "dmac/dmac.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum _uart_device_number
{
    UART_DEVICE_0,
    UART_DEVICE_1,
    UART_DEVICE_2,
    UART_DEVICE_3,
	UART_DEVICE_4,
    UART_DEVICE_MAX,
} uart_device_number_t;

typedef enum _uart_bitwidth
{
    UART_BITWIDTH_5BIT = 5,
    UART_BITWIDTH_6BIT,
    UART_BITWIDTH_7BIT,
    UART_BITWIDTH_8BIT,
} uart_bitwidth_t;

typedef enum _uart_stopbit
{
    UART_STOP_1,
    UART_STOP_1_5,
    UART_STOP_2
} uart_stopbit_t;

typedef enum _uart_rede_sel
{
    DISABLE = 0,
    ENABLE,
} uart_rede_sel_t;

typedef enum _uart_parity
{
    UART_PARITY_NONE,
    UART_PARITY_ODD,
    UART_PARITY_EVEN
} uart_parity_t;

typedef enum _uart_interrupt_mode
{
    UART_SEND = 1,
    UART_RECEIVE = 2,
} uart_interrupt_mode_t;

typedef enum _uart_send_trigger
{
    UART_SEND_FIFO_0,
    UART_SEND_FIFO_2,
    UART_SEND_FIFO_4,
    UART_SEND_FIFO_8,
} uart_send_trigger_t;

typedef enum _uart_receive_trigger
{
    UART_RECEIVE_FIFO_1,
    UART_RECEIVE_FIFO_4,
    UART_RECEIVE_FIFO_8,
    UART_RECEIVE_FIFO_14,
} uart_receive_trigger_t;

/**
 * @brief       Send data from uart
 *
 * @param[in]   channel     Uart index
 * @param[in]   buffer      The data be transfer
 * @param[in]   len         The data length
 *
 * @return      Transfer length
 */
int uart_send_data(uart_device_number_t channel, const char *buffer, size_t buf_len);

/**
 * @brief       Read data from uart
 *
 * @param[in]   channel     Uart index
 * @param[in]   buffer      The Data received
 * @param[in]   len         Receive length
 *
 * @return      Receive length
 */
int uart_receive_data(uart_device_number_t channel, char *buffer, size_t buf_len);

/**
 * @brief       Init uart
 *
 * @param[in]   channel     Uart index
 *
 */
void uart_init(uart_device_number_t channel);

/** 
 * @brief       Disable uart port
 * 
 * @param[in]   channel     Uart index
 */
void uart_deinit(uart_device_number_t channel);

/**
 * @brief       Set uart param
 *
 * @param[in]   channel         Uart index
 * @param[in]   baud_rate       Baudrate
 * @param[in]   data_width      Data width
 * @param[in]   stopbit         Stop bit
 * @param[in]   parity          Odd Even parity
 *
 */
void uart_config(uart_device_number_t channel, uint32_t baud_rate, uart_bitwidth_t data_width, uart_stopbit_t stopbit, uart_parity_t parity);

/**
 * @brief       Set uart param
 *
 * @param[in]   channel         Uart index
 * @param[in]   baud_rate       Baudrate
 * @param[in]   data_width      Data width
 * @param[in]   stopbit         Stop bit
 * @param[in]   parity          Odd Even parity
 *
 */
void uart_configure(uart_device_number_t channel, uint32_t baud_rate, uart_bitwidth_t data_width, uart_stopbit_t stopbit, uart_parity_t parity);

/**
 * @brief       Enable uart receive interrupt
 *
 * @param[in]   channel             Uart index
 *
 */
void uart_receive_irq_enable(uart_device_number_t channel);

/**
 * @brief       Disable uart receive interrupt
 *
 * @param[in]   channel             Uart index
 *
 */
void uart_receive_irq_disable(uart_device_number_t channel);

/**
 * @brief       Register uart interrupt
 *
 * @param[in]   channel             Uart index
 * @param[in]   interrupt_mode      Interrupt Mode receive or send
 * @param[in]   uart_callback       Call back
 * @param[in]   ctx                 Param of call back
 * @param[in]   priority            Interrupt priority
 *
 */
void uart_irq_register(uart_device_number_t channel, uart_interrupt_mode_t interrupt_mode, xvic_irq_callback_t uart_callback, void *ctx, uint32_t priority);

/**
 * @brief       Deregister uart interrupt
 *
 * @param[in]   channel             Uart index
 * @param[in]   interrupt_mode      Interrupt Mode receive or send
 *
 */
void uart_irq_unregister(uart_device_number_t channel, uart_interrupt_mode_t interrupt_mode);

/**
 * @brief       Set send interrupt threshold
 *
 * @param[in]   channel             Uart index
 * @param[in]   trigger             Threshold of send interrupt
 *
 */
void uart_set_send_trigger(uart_device_number_t channel, uart_send_trigger_t trigger);

/**
 * @brief       Set receive interrupt threshold
 *
 * @param[in]   channel             Uart index
 * @param[in]   trigger             Threshold of receive interrupt
 *
 */
void uart_set_receive_trigger(uart_device_number_t channel, uart_receive_trigger_t trigger);

/**
 * @brief       Send data by dma
 *
 * @param[in]   channel             Uart index
 * @param[in]   dmac_channel        Dmac channel
 * @param[in]   buffer              Send data
 * @param[in]   buf_len             Data length
 *
 */
void uart_send_data_dma(uart_device_number_t uart_channel, dmac_channel_number_t dmac_channel, const uint8_t *buffer, size_t buf_len);

/**
 * @brief       Receive data by dma
 *
 * @param[in]   channel             Uart index
 * @param[in]   dmac_channel        Dmac channel
 * @param[in]   buffer              Receive data
 * @param[in]   buf_len             Data length
 *
 */
void uart_receive_data_dma(uart_device_number_t uart_channel, dmac_channel_number_t dmac_channel, uint8_t *buffer, size_t buf_len);


/**
 * @brief       Send data by dma
 *
 * @param[in]   uart_channel        Uart index
 * @param[in]   dmac_channel        Dmac channel
 * @param[in]   buffer              Send data
 * @param[in]   buf_len             Data length
 * @param[in]   uart_callback       Call back
 * @param[in]   ctx                 Param of call back
 * @param[in]   priority            Interrupt priority
 *
 */
void uart_send_data_dma_irq(uart_device_number_t uart_channel, dmac_channel_number_t dmac_channel,
                            const uint8_t *buffer, size_t buf_len, xvic_irq_callback_t uart_callback,
                            void *ctx, uint32_t priority);

/**
 * @brief       Receive data by dma
 *
 * @param[in]   uart_channel        Uart index
 * @param[in]   dmac_channel        Dmac channel
 * @param[in]   buffer              Receive data
 * @param[in]   buf_len             Data length
 * @param[in]   uart_callback       Call back
 * @param[in]   ctx                 Param of call back
 * @param[in]   priority            Interrupt priority
 *
 */
void uart_receive_data_dma_irq(uart_device_number_t uart_channel, dmac_channel_number_t dmac_channel,
                               uint8_t *buffer, size_t buf_len, xvic_irq_callback_t uart_callback,
                               void *ctx, uint32_t priority);

uint32_t uart_get_baudrate(uart_device_number_t channel);

int uart_rdata_ready(uart_device_number_t channel);

int uartapb_getc(uart_device_number_t channel);

void uart_suspend(uart_device_number_t channel);

void uart_resume(uart_device_number_t channel);

void uart_disable_sleep(void);

void uart_enable_sleep(void);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_APBUART_H */
