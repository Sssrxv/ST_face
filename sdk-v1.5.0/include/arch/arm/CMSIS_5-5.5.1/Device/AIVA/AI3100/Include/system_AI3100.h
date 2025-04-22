/**************************************************************************//**
 * @file     system_<Device>.h
 * @brief    CMSIS Cortex-A Device Peripheral Access Layer
 * @version  V5.00
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

#ifndef SYSTEM_AI3100_H   /* ToDo: replace '<Device>' with your device name */
#define SYSTEM_AI3100_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

extern uint32_t SystemCoreClock;     /*!< System Clock Frequency (Core Clock)  */

/**
  \brief Setup the microcontroller system.

   Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit (void);

/**
  \brief  Update SystemCoreClock variable.

   Updates the SystemCoreClock with current core Clock retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate (void);

/**
  \brief  Create Translation Table.

   Creates Memory Management Unit Translation Table.
 */
extern void MMU_CreateTranslationTable(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  dcache_inval, invalidate d-cache 
 * 
 * @param p[in], pointer to virtual address
 * @param size[in]
 * 
 */
/* --------------------------------------------------------------------------*/
extern void dcache_inval(const void *p, size_t size);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  dcache_flush, clean and invalidate d-cache 
 * 
 * @param p[in], pointer to the virtual address
 * @param size
 * 
 */
/* --------------------------------------------------------------------------*/
extern void dcache_flush(const void *p, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_<Device>_H */
