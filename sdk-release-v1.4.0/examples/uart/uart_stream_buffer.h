#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "uart/uart.h"

// buffer size for stream buffer
#define UART_RECEIVE_BUFFER_SIZE 128
#define UART_TRANSMIT_BUFFER_SIZE 128

typedef struct {
    int port;

    // 创建 StreamBuffer 用于接收和发送数据
    StaticStreamBuffer_t rxStreamBufferStruct;
    uint8_t rxStreamBufferStorageArea[UART_RECEIVE_BUFFER_SIZE];
    StreamBufferHandle_t RxStreamBuffer;

    StaticStreamBuffer_t txStreamBufferStruct;
    uint8_t txStreamBufferStorageArea[UART_TRANSMIT_BUFFER_SIZE];
    StreamBufferHandle_t TxStreamBuffer;
} uart_handle_t;

int uart_async_open(uart_handle_t *handle, int port);
int uart_async_close(uart_handle_t *handle, int port);
int uart_read_async(uart_handle_t *handle, char *data, size_t len);
int uart_write_async(uart_handle_t *handle, char *data, size_t len);
