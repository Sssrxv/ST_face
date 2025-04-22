#ifndef _DRIVER_DMAC_H
#define _DRIVER_DMAC_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "io.h"
#include "xvic/xvic.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DMAC_INTR_PRI              (19 << 3)

/**
 * @brief Enumeration of DMA channel number
 *
 */
typedef enum dmac_channel_number
{
    DMAC_CHANNEL0 = 0, /*!< DMA channel 0 */
    DMAC_CHANNEL1 = 1, /*!< DMA channel 1 */
    DMAC_CHANNEL2 = 2, /*!< DMA channel 2 */
    DMAC_CHANNEL3 = 3, /*!< DMA channel 3 */
    DMAC_CHANNEL4 = 4, /*!< DMA channel 4 */
    DMAC_CHANNEL5 = 5, /*!< DMA channel 5 */
    DMAC_CHANNEL_MAX
} dmac_channel_number_t;

/**
 * @brief Enumeration of DMA address access mode
 *
 */
typedef enum dmac_address_increment
{
    DMAC_ADDR_INCREMENT = 0x0, /*!< Address increment mode: DMA write or read data from buffer */
    DMAC_ADDR_NOCHANGE  = 0x1  /*!< Address nochange mode: DMA write or read data from a fixed address */
} dmac_address_increment_t;

/**
 * @brief Enumeration of DMA transfer width mode
 *
 */
typedef enum dmac_transfer_width
{
    DMAC_TRANS_WIDTH_8   = 0x0, /*!< 8 bits width mode */
    DMAC_TRANS_WIDTH_16  = 0x1, /*!< 16 bits width mode */
    DMAC_TRANS_WIDTH_32  = 0x2, /*!< 32 bits width mode */
    DMAC_TRANS_WIDTH_64  = 0x3, /*!< 64 bits width mode */
    DMAC_TRANS_WIDTH_128 = 0x4, /*!< 128 bits width mode */
    DMAC_TRANS_WIDTH_256 = 0x5  /*!< 256 bits width mode */
} dmac_transfer_width_t;

/**
 * @brief Enumeration of DMA transfer flow control
 *
 */
typedef enum dmac_transfer_flow
{
    DMAC_MEM2MEM_DMA    = 0x0, /*!< DMA flow control: memory to memory */
    DMAC_MEM2PRF_DMA    = 0x1, /*!< DMA flow control: memory to peripheral */
    DMAC_PRF2MEM_DMA    = 0x2, /*!< DMA flow control: peripheral to memory */
    DMAC_PRF2PRF_DMA    = 0x3, /*!< DMA flow control: peripheral to peripheral */
    DMAC_PRF2MEM_PRF    = 0x4, /*!< Peripheral flow control: peripheral to memory */
    DMAC_PRF2PRF_SRCPRF = 0x5, /*!< Source flow control: peripheral to peripheral */
    DMAC_MEM2PRF_PRF    = 0x6, /*!< Peripheral flow control: memory to peripheral */
    DMAC_PRF2PRF_DSTPRF = 0x7  /*!< Destination flow control: peripheral to peripheral */
} dmac_transfer_flow_t;

/**
 * @brief Enumeration of DMA transfer burst length
 *
 */
typedef enum dmac_burst_trans_length
{
    DMAC_MSIZE_1   = 0x0, /*!< 1 Data Item in the burst transaction */
    DMAC_MSIZE_4   = 0x1, /*!< 4 Data Item in the burst transaction */
    DMAC_MSIZE_8   = 0x2, /*!< 8 Data Item in the burst transaction */
    DMAC_MSIZE_16  = 0x3, /*!< 16 Data Item in the burst transaction */
    DMAC_MSIZE_32  = 0x4, /*!< 32 Data Item in the burst transaction */
    DMAC_MSIZE_64  = 0x5, /*!< 64 Data Item in the burst transaction */
    DMAC_MSIZE_128 = 0x6, /*!< 128 Data Item in the burst transaction */
    DMAC_MSIZE_256 = 0x7  /*!< 256 Data Item in the burst transaction */
} dmac_burst_trans_length_t;

/**
 * @brief DMA linked list item, must be 64 bytes aligned
 *
 */
typedef struct dmac_lli_item
{
    uint64_t sar;           /*!< Source Address               */
    uint64_t dar;           /*!< Destination Address          */
    uint64_t ch_block_ts;   /*!< Block Transfer Size          */
    uint64_t llp;           /*!< Linked List Pointer          */
    uint64_t ctl;           /*!< Dma Transfer Control         */
    uint64_t sstat;         /*!< Source Status Register       */
    uint64_t dstat;         /*!< Destination Status Register  */
    uint64_t resv;
} __attribute__((packed, aligned(64))) dmac_lli_item_t;

/**
 * @brief DMA transfer with single mode
 * 
 * @param ch_num            DMA channel number
 * @param src               Source buffer address
 * @param dest              Destination buffer address
 * @param src_inc           Source address access mode
 * @param dest_inc          Destination address access mode
 * @param dmac_burst_size   Burst size
 * @param dmac_trans_width  Transfer bit width
 * @param block_size        Data block size
 * 
 * @return void
 */
void dmac_set_single_mode(dmac_channel_number_t ch_num,
        const void                 *src, 
        void                       *dest, 
        dmac_address_increment_t    src_inc,
        dmac_address_increment_t    dest_inc,
        dmac_burst_trans_length_t   dmac_burst_size,
        dmac_transfer_width_t       dmac_trans_width,
        size_t                      block_size
        );

/**
 * @brief Waiting DMA transfer done
 * 
 * @param ch_num DMA channel number
 * 
 * @return 0 - success, -1 - fail
 * 
 * @note If you want to use this function to waiting for DMA done, you need to call "dmac_irq_register" when initialize DMA channel
 */
int dmac_wait_done(dmac_channel_number_t ch_num);

/**
 * @brief DMA IRQ register, will enable transfer done interrupt by default
 * 
 * @param ch_num                 DMA channel number
 * @param dmac_callback          Interrupt callback
 * @param ctx                    Parameter for callback
 * @param priority               Interrupt priority
 * @param enable_block_tfr_done  Enable block transfer done interrupt
 * 
 * @return void
 */
void dmac_irq_register(dmac_channel_number_t ch_num, xvic_irq_callback_t dmac_callback, void *ctx, uint32_t priority, bool enable_block_tfr_done);

/**
 * @brief DMA IRQ unregister
 * 
 * @param ch_num DMA channel number
 * 
 * @return void
 */
void dmac_irq_unregister(dmac_channel_number_t ch_num);

/**
 * @brief Setup linked list for DMA transfer
 * 
 * @param ch_num            DMA channel number
 * @param LLI_head          Linked list item header
 * @param LLI_idx           Linked list item index
 * @param LLI_is_last       Whether this linked list item is the last one
 * @param src_addr          Source buffer address
 * @param dst_addr          Destination buffer address
 * @param src_inc           Source address access mode
 * @param dest_inc          Destination address access mode
 * @param dmac_burst_size   Burst size
 * @param dmac_trans_width  Transfer bit width
 * @param block_size        Data block size
 * 
 * @return void
 */
void dmac_setup_list_item(dmac_channel_number_t ch_num,
        dmac_lli_item_t            *LLI_head,
        uint8_t                     LLI_idx, 
        int8_t                      LLI_is_last,
        void                       *src_addr,
        void                       *dst_addr,
        dmac_address_increment_t    src_inc,
        dmac_address_increment_t    dst_inc,
        dmac_burst_trans_length_t   dmac_burst_size,
        dmac_transfer_width_t       dmac_trans_width,
        uint32_t                    block_size
        );

/**
 * @brief DMA transfer with linked list mode
 * 
 * @param ch_num                 DMA channel number
 * @param flow_control           DMA flow control mode
 * @param first_link_item        First linked list item
 * @param enable_block_tfr_done  Enable block transfer done interrupt
 * 
 * @return void
 */
void dmac_set_linked_list_mode(dmac_channel_number_t ch_num,
        dmac_transfer_flow_t flow_control,
        dmac_lli_item_t *first_link_item,
        bool enable_block_tfr_done
        );

/**
 * @brief Set source hardware handshake interface
 * 
 * @param ch_num DMA channel number
 * @param hs     Hardware handshake interface index (refer to enum "dmac_hs_if_t")
 * 
 * @return void
 */
void dmac_set_hs_src(int ch_num, uint8_t hs);

/**
 * @brief Set destination hardware handshake interface
 * 
 * @param ch_num DMA channel number
 * @param hs     Hardware handshake interface index (refer to enum "dmac_hs_if_t")
 * 
 * @return void
 */
void dmac_set_hs_dst(int ch_num, uint8_t hs);

/**
 * @brief Alloc dma channel
 * 
 * @param void
 * 
 * @return DMA channel number
 */
dmac_channel_number_t dmac_channel_alloc(void);

/**
 * @brief Free dma channel
 * 
 * @param ch_num DMA channel number
 * 
 * @return 0 - success, -1 - fail
 */
int dmac_channel_free(dmac_channel_number_t ch_num);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_DMAC_H */
