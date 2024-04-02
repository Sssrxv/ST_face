#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "uart/uart.h"
#include "uart_stream_buffer.h"

static void uart_port_init(int port)
{
    // initialize the uart port
    uart_init(port);
    uart_config(port, 115200, UART_BITWIDTH_8BIT, UART_STOP_1, UART_PARITY_NONE);
}

static void uart_io_send(uart_handle_t *handle, char *data, size_t size)
{
    // uart port send data
    uart_send_data(handle->port, data, size);
}

static size_t uart_io_receive(uart_handle_t *handle, char *buffer, size_t size)
{
    // read data from uart port
    int len = uart_receive_data(handle->port, buffer, size);
    // return actual length of received bytes
    return len; 
}

// uart receive task
static void uartReceiveTask(void *pvParameters)
{
    uart_handle_t *handle = (uart_handle_t *)pvParameters;
    char receivedData[UART_RECEIVE_BUFFER_SIZE];

    while (1) {
        // read data from uart port
        size_t bytesRead = uart_io_receive(handle, receivedData, UART_RECEIVE_BUFFER_SIZE);

        // save to RxStreamBuffer
        // could be read use uart_read_async()
        if (bytesRead > 0) {
            xStreamBufferSend(handle->RxStreamBuffer, receivedData, bytesRead, portMAX_DELAY);
        }

        vTaskDelay(1);
    }
}

// uart send task
static void uartSendTask(void *pvParameters)
{
    uart_handle_t *handle = (uart_handle_t *)pvParameters;
    char sendData[UART_TRANSMIT_BUFFER_SIZE];

    while (1) {
        // get data from TxStreamBuffer
        // the data is from uart_write_async()
        size_t bytesToTransmit = xStreamBufferReceive(handle->TxStreamBuffer, sendData, UART_TRANSMIT_BUFFER_SIZE, portMAX_DELAY);

        // uart port send data
        if (bytesToTransmit > 0) {
            uart_io_send(handle, sendData, bytesToTransmit);
        }

        vTaskDelay(1);
    }
}

int uart_async_open(uart_handle_t *handle, int port)
{
    if (!handle) {
        // handle should not be NULL
        return -1;
    }

    // init uart port 
    uart_port_init(port);
    handle->port = port;

    // create RxStreamBuffer and TxStreamBuffer
    handle->RxStreamBuffer = xStreamBufferCreateStatic(UART_RECEIVE_BUFFER_SIZE, 1, handle->rxStreamBufferStorageArea, &handle->rxStreamBufferStruct);
    handle->TxStreamBuffer = xStreamBufferCreateStatic(UART_TRANSMIT_BUFFER_SIZE, 1, handle->txStreamBufferStorageArea, &handle->txStreamBufferStruct);

    // create uart rx task
    xTaskCreate(uartReceiveTask, "uart_rx_tsk", configMINIMAL_STACK_SIZE, (void *)handle, tskIDLE_PRIORITY + 1, NULL);

    // create uart tx task
    xTaskCreate(uartSendTask, "uart_tx_tsk", configMINIMAL_STACK_SIZE, (void *)handle, tskIDLE_PRIORITY + 1, NULL);

    return 0;
}

int uart_async_close(uart_handle_t *handle, int port)
{
    (void)(handle);
    (void)(port);
    // TODO:
    // 1. wait the 2 tasks exit
    // 2. close uart port
    // 3. release stream buffer if needed
    return 0;
}

int uart_read_async(uart_handle_t *handle, char *data, size_t len)
{
    size_t bytes = xStreamBufferReceive(handle->RxStreamBuffer, data, len, portMAX_DELAY);

    return bytes;
}

int uart_write_async(uart_handle_t *handle, char *data, size_t len)
{
    xStreamBufferSend(handle->TxStreamBuffer, data, len, portMAX_DELAY);

    return len;
}
