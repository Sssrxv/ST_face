/**************************************************************************//**
 * @file     mem_<Device>.h
 * @brief    CMSIS Cortex-A Memory base and size definitions (used in scatter file)
 * @version  V1.00
 * @date     10. January 2018
 ******************************************************************************/
/*
 * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MEM_AI3100_H   /* ToDo: replace '<Device>' with your device name */
#define MEM_AI3100_H

#ifdef NEW_MEM_MAP
#   define ROM_BASE_ADDR                        (0x00000000)
#   define ROM_SIZE                             (8 * 1024)
#else
#   define ROM_BASE_ADDR                        (0x00000000)
#   define ROM_SIZE                             (32 * 1024)
#endif // #ifdef NEW_MEM_MAP

#define SRAM0_BASE_ADDR                         (0x10000000)
#define SRAM0_SIZE                              (256 * 1024)
#define SRAM1_BASE_ADDR                         (0x10040000)
#define SRAM1_SIZE                              (256 * 1024)
#define SRAM2_BASE_ADDR                         (0x10080000)
#define SRAM2_SIZE                              (256 * 1024)


/*
 * SRAM0: 0x1000_0000 ~ 0x1003_FFFF (256KB)
 * SRAM1: 0x1004_0000 ~ 0x1007_FFFF (256KB)
 * SRAM2: 0x1008_0000 ~ 0X100B_FFFF (256KB)
 */

#define __ROM_BASE                              ROM_BASE_ADDR
#define __ROM_SIZE                              ROM_SIZE

#define __SRAM_BASE                             SRAM0_BASE_ADDR
#define __SRAM_SIZE                             (236 * 1024)        /*236KB*/

#ifdef EXECUTE_ON_SRAM
#   define __TTB_BASE                              SRAM2_BASE_ADDR
#else
#   define __TTB_BASE                              SRAM0_BASE_ADDR
#endif // EXECUTE_ON_SRAM

#define __TTB_SIZE                              (16 * 1024)         /*16KB*/

#define __PRIVATE_TABLE_L2_4K_BASE              (__TTB_BASE+__TTB_SIZE)
#define __PRIVATE_TABLE_L2_4K_SIZE              (3 * 1024)          /*3KB*/

#define __PERIPHRAL_TABLE_L2_64K_BASE           (__PRIVATE_TABLE_L2_4K_BASE+__PRIVATE_TABLE_L2_4K_SIZE)
#define __PERIPHRAL_TABLE_L2_64K_SIZE           (1024)              /*1KB*/

#define __PERIPHERAL_REG_BASE_ADDR              (0x20000000) 
#define __PERIPHERAL_REG_SIZE                   (320 * 1024)        /*320KB*/

#define __DDR_BASE                              (0x80000000)
//#define __DDR_SIZE                              (256 * 1024 * 1024 - __DMA_MEM_SIZE)
//#define __DDR_SIZE                              (1024 * 1024 * 1024 - __DMA_MEM_SIZE)
#ifndef CONFIG_DDR_SIZE_MB
#error Should define CONFIG_DDR_SIZE_MB
#endif
#define __DDR_SIZE                              (CONFIG_DDR_SIZE_MB * 1024 * 1024 - __DMA_MEM_SIZE)

#define __UND_STACK_SIZE                        (256) 
#define __ABT_STACK_SIZE                        (1024)
#define __SVC_STACK_SIZE                        (16*1024)
#define __IRQ_STACK_SIZE                        (16*1024)
#define __FIQ_STACK_SIZE                        (256)

#ifdef EXECUTE_ON_ROM
#   define __STACK_SIZE                         (4 * 1024)          /*4KB*/
#   define __HEAP_SIZE                          (8 * 1024)          /*8KB*/
#   define __DMA_MEM_SIZE                       (0)
#endif // EXECUTe_ON_ROM

#ifdef EXECUTE_ON_SRAM
#   define __STACK_SIZE                         ( 4 * 1024)         /*4KB*/
#   define __HEAP_SIZE                          (33 * 1024)         /*33KB*/
#   define __DMA_MEM_SIZE                       (0)
#endif // EXECUTE_ON_SRAM

#ifdef EXECUTE_ON_DDR
#ifdef WITH_DYNAMIC_XNN_WORKBUFFER

#define __FW_SPACE_SIZE                         (CONFIG_FW_SPACE_SIZE_MB * 1024 * 1024)
#   define __FW_CODE_SIZE                       (CONFIG_FW_CODE_SIZE_MB  * 1024 * 1024)
#   define __HEAP_SIZE                          (CONFIG_FW_HEAP_SIZE_MB  * 1024 * 1024)
#   define __STACK_SIZE                         (__FW_SPACE_SIZE - __FW_CODE_SIZE - __HEAP_SIZE)  /*8MB*/

#define __DMA_MEM_BASE                          (__DDR_BASE + __DDR_SIZE)
#   define __DMA_MEM_SIZE                       ((CONFIG_DDR_SIZE_MB - CONFIG_FW_SPACE_SIZE_MB) * 1024 * 1024)
#   define __DMA_HEAP_SIZE                      (CONFIG_DMA_HEAP_SIZE_MB * 1024 * 1024)
#   define IS_DMA_MEMORY(a) ( ((a) >= __DMA_MEM_BASE) && ((a) < (__DMA_MEM_BASE+__DMA_MEM_SIZE)) )
#else
// 0x80000000--------------
//           |            |
//           |    64MB    |
//           |  Firmware  |
//           |            |
// 0x84000000--------------
//           |    16MB    |
//           | XNN Weight |
// 0x85000000--------------
//           |            |
//           |    32MB    |
//           | XNN Working|
//           |   Buffer   |
// 0x87000000--------------
//           |            |
//           |    8MB     |
//           | GDB Loading|
//           |   Buffer   |
// 0x87800000--------------
//           |            |
//           |    8MB     |
//           | FirmwareDMA|
// 0x88000000--------------
#   define __FW_SPACE_SIZE                      (64 * 1024 * 1024)
#   define __FW_CODE_SIZE                       ( 6 * 1024 * 1024)
#   define __HEAP_SIZE                          (54 * 1024 * 1024)  /*54MB*/
#   define __STACK_SIZE                         (__FW_SPACE_SIZE - __FW_CODE_SIZE - __HEAP_SIZE)  /*4MB*/
#   define __DMA_MEM_BASE                       (__DDR_BASE + __DDR_SIZE)
#   define __DMA_MEM_SIZE                       (8 * 1024 * 1024)
#   define __DMA_HEAP_SIZE                      (4 * 1024 * 1024)
#   define IS_DMA_MEMORY(a) ( ((a) >= __DMA_MEM_BASE) && ((a) < (__DMA_MEM_BASE+__DMA_MEM_SIZE)) )
#endif
#endif // EXECUTE_ON_DDR

#define __DMA_ALIGNED__(n) __attribute__ ((__aligned__(n), section(".dma")))

#define FW_MEM_ASSERT (__FW_SPACE_SIZE == (__FW_CODE_SIZE + __HEAP_SIZE + __STACK_SIZE) \
                    && __FW_SPACE_SIZE > 0 \
                    && __FW_CODE_SIZE > 0 \
                    && __HEAP_SIZE > 0 \
                    && __STACK_SIZE > 0)

#define DDR_MEM_ASSERT (__DMA_MEM_SIZE > 0 \
                    &&  __DMA_HEAP_SIZE > 0 \
                    &&  __DMA_HEAP_SIZE < __DMA_MEM_SIZE)

#if (!(FW_MEM_ASSERT) || !(DDR_MEM_ASSERT))
#error wrong mem map
#endif

#endif /* MEM_<Device>_H */
