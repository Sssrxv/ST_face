/**************************************************************************//**
 * @file     AI3100.h
 * @brief    CMSIS Cortex-A Core Peripheral Access Layer Header File
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

#ifndef AI3100_H      /* ToDo: replace '<AI3100>' with your device name */
#define AI3100_H

#ifdef __cplusplus
extern "C" {
#endif

/* ToDo: replace '<AIVA>' with vendor name; add your doxyGen comment   */
/** @addtogroup <AIVA>
  * @{
  */


/* ToDo: replace '<Device>' with device name; add your doxyGen comment   */
/** @addtogroup <Device>
  * @{
  */


/** @addtogroup Configuration_of_CMSIS
  * @{
  */


/* =========================================================================================================================== */
/* ================                                Interrupt Number Definition                                ================ */
/* =========================================================================================================================== */

typedef enum IRQn
{
/* =======================================  ARM Cortex-A Specific Interrupt Numbers  ========================================= */

  /* Software Generated Interrupts */
  SGI0_IRQn                          =  0,      /*!< Software Generated Interrupt  0                                           */
  SGI1_IRQn                          =  1,      /*!< Software Generated Interrupt  1                                           */
  SGI2_IRQn                          =  2,      /*!< Software Generated Interrupt  2                                           */
  SGI3_IRQn                          =  3,      /*!< Software Generated Interrupt  3                                           */
  SGI4_IRQn                          =  4,      /*!< Software Generated Interrupt  4                                           */
  SGI5_IRQn                          =  5,      /*!< Software Generated Interrupt  5                                           */
  SGI6_IRQn                          =  6,      /*!< Software Generated Interrupt  6                                           */
  SGI7_IRQn                          =  7,      /*!< Software Generated Interrupt  7                                           */
  SGI8_IRQn                          =  8,      /*!< Software Generated Interrupt  8                                           */
  SGI9_IRQn                          =  9,      /*!< Software Generated Interrupt  9                                           */
  SGI10_IRQn                         = 10,      /*!< Software Generated Interrupt 10                                           */
  SGI11_IRQn                         = 11,      /*!< Software Generated Interrupt 11                                           */
  SGI12_IRQn                         = 12,      /*!< Software Generated Interrupt 12                                           */
  SGI13_IRQn                         = 13,      /*!< Software Generated Interrupt 13                                           */
  SGI14_IRQn                         = 14,      /*!< Software Generated Interrupt 14                                           */
  SGI15_IRQn                         = 15,      /*!< Software Generated Interrupt 15                                           */
  
/******  Cortex-A5 Processor Exceptions Numbers ****************************************/
  GlobalTimer_IRQn     = 27,        /*!< Global Timer Interrupt                        */
  PrivTimer_IRQn       = 29,        /*!< Private Timer Interrupt                       */
  PrivWatchdog_IRQn    = 30,        /*!< Private Watchdog Interrupt                    */

/******  Platform Exceptions Numbers ***************************************************/
  DMA_IRQn               = 32,
  RSZ_RBC_CORE_IRQn      = 33,
  RSZ_RBC_MIR_IRQn       = 34,
  RSZ_MIW_IRQn           = 35,
  RSZ_MIR_IRQn           = 36,
  SPACC_IRQn             = 37,
  PKA_IRQn               = 38,
  TRNG_IRQn              = 39,
  PWM0_IRQn              = 40,
  PWM1_IRQn              = 41,
  PWM2_IRQn              = 42,
  PWM3_IRQn              = 43,
  PWM4_IRQn              = 44,
  PWM5_IRQn              = 45,
  PWM6_IRQn              = 46,
  PWM7_IRQn              = 47,
  PWM8_IRQn              = 48,
  PWM9_IRQn              = 49,
  PWM10_IRQn             = 50,
  PWM11_IRQn             = 51,
  UART0_IRQn             = 52,
  UART1_IRQn             = 53,
  UART2_IRQn             = 54,
  UART3_IRQn             = 55,
  UART4_IRQn             = 56,
  I2CM0_IRQn             = 57,
  I2CM1_IRQn             = 58,
  I2CM2_IRQn             = 59,
  I2CM3_IRQn             = 60,
  I2CM4_IRQn             = 61,
  RTC_IRQn               = 62,
  SMU_IRQn               = 63,
  GPIO_IRQn              = 64,
  SPIM0_IRQn             = 65,
  SPIM1_IRQn             = 66,
  QSPIS_IRQn             = 67,
    
  MIPI0_MIPI_IRQn        = 68,
  MIPI0_FRAME_START_IRQn = 69,
  MIPI0_FRAME_END_IRQn   = 70,
  MIPI0_MI_IRQn          = 71,
  MIPI0_FORM0_IRQn       = 72,
  MIPI0_FORM1_IRQn       = 73,
  MIPI0_FORM2_IRQn       = 74,

  MIPI1_MIPI_IRQn        = 75,
  MIPI1_FRAME_START_IRQn = 76,
  MIPI1_FRAME_END_IRQn   = 77,
  MIPI1_MI_IRQn          = 78,
  MIPI1_FORM0_IRQn       = 79,
  MIPI1_FORM1_IRQn       = 80,
  MIPI1_FORM2_IRQn       = 81,
  
  MIPI2_MIPI_IRQn        = 82,
  MIPI2_FRAME_START_IRQn = 83,
  MIPI2_FRAME_END_IRQn   = 84,
  MIPI2_MI_IRQn          = 85,
  MIPI2_FORM0_IRQn       = 86,
  MIPI2_FORM1_IRQn       = 87,
  MIPI2_FORM2_IRQn       = 88,
  
  DVPI0_CORE_IRQn        = 89,
  DVPI0_MIW_IRQn         = 90,
  
  DVPO_CORE_RGB_IRQn     = 91,
  DVPO_MIR_IRQn          = 92,
  DVPO_CORE_MCU_IRQn     = 93,
  ETH_IRQn               = 94,
  USB_IRQn               = 95,
  
  PWC0_IRQn              = 96,
  PWC1_IRQn              = 97,
  PWC2_IRQn              = 98,
  PWC3_IRQn              = 99,
  PWC4_IRQn              = 100,
  PWC5_IRQn              = 101,
  
  I2S_IRQn               = 102,
  
  DDR_IRQn               = 103,
  
  XNN_RBC_CORE_IRQn      = 104,
  XNN_RBC_MIR_IRQn       = 105,
  XNN_MIW0_IRQn          = 106,
  XNN_MIW1_IRQn          = 107,
  XNN_MIR0_IRQn          = 108,
  XNN_MIR1_IRQn          = 109,
  
  VDE_MIW0_IRQn          = 110,
  VDE_MIW1_IRQn          = 111,
  VDE_MIW2_IRQn          = 112,
  VDE_MIR_IRQn           = 113,

  VTE_RBC_CORE_IRQn      = 114,
  VTE_RBC_MIR_IRQn       = 115,
  VTE_CORE_IRQn          = 116,
  VTE_MIW0_IRQn          = 117,
  VTE_MIW1_IRQn          = 118,
  VTE_MIR_IRQn           = 119,

  ISP_PREP_IRQn          = 120,
  ISP_STNR_IRQn          = 121,
  ISP_ISP_IRQn           = 122,
  ISP_MI_MP_IRQn         = 123,
  ISP_MI_VP_IRQn         = 124,
  ISP_MI_PP_IRQn         = 125,
  ISP_MI_RD_IRQn         = 126,

  JPEG_STATUS_IRQn       = 127,
  JPEG_ERROR_IRQn        = 128,
  JPEG_MIW_IRQn          = 129,
  JPEG_MIR_IRQn          = 130,

  ROT_MIW_IRQn           = 131,
  ROT_MIR_IRQn           = 132,

  HDR_MIR0_IRQn          = 133,
  HDR_MIR1_IRQn          = 134,
  HDR_MIW0_IRQn          = 135,
  HDR_MIW1_IRQn          = 136,
// #if USE_AI3102 
  VIDEO_CODEC_IRQn       = 137,
// #endif

  IRQN_MAX               = 256,
} IRQn_Type;


/* =========================================================================================================================== */
/* ================                          Device Specific Peripheral Address Map                           ================ */
/* =========================================================================================================================== */


/* ToDo: add here your device peripherals base addresses
         following is an example for timer */
/** @addtogroup Device_Peripheral_peripheralAddr
  * @{
  */

/* Peripheral memory map */
#include "mem_AI3100.h"
#include "memory_map.h"

#define AI3100_GIC_BASE_ADDR                (0x2FFFE000UL)
/* Core specific Interrupt interface registers */
/* 0x0100 ~ 0x01FF */
#define AI3100_GIC_DISTRIBUTOR_BASE         (0x00001000UL + AI3100_GIC_BASE_ADDR)         /*!< (GIC DIST  ) Base Address */
/* Interrupt Distributor Registers */
/* 0x1000 ~ 0x1FFF */
#define AI3100_GIC_INTERFACE_BASE           (0x00000100UL + AI3100_GIC_BASE_ADDR)         /*!< (GIC CPU IF) Base Address */
/* Private Timer */
/* 0x0600 ~ 0x06FF */
#define AI3100_PRIVATE_TIMER                (0x00000600UL + AI3100_GIC_BASE_ADDR)         /*!< (PTIM      ) Base Address */
/* Global Timer */
/* 0x0200 ~ 0x02FF */
#define AI3100_GLOBAL_TIMER                 (0x00000200UL + AI3100_GIC_BASE_ADDR)         /*!< (PTIM      ) Base Address */

#define GIC_DISTRIBUTOR_BASE                AI3100_GIC_DISTRIBUTOR_BASE
#define GIC_INTERFACE_BASE                  AI3100_GIC_INTERFACE_BASE
#define TIMER_BASE                          AI3100_PRIVATE_TIMER
#define GLOBAL_TIMER_BASE                   AI3100_GLOBAL_TIMER

/** @} */ /* End of group Device_Peripheral_peripheralAddr */




/* =========================================================================================================================== */
/* ================                           Processor and Core Peripheral Section                           ================ */
/* =========================================================================================================================== */

/* ===========================  Configuration of the Arm Cortex-A Processor and Core Peripherals  ============================ */
/* ToDo: set the defines according your Device */
/* ToDo: define the correct core revision              
         5U if your device is a CORTEX-A5 device
         7U if your device is a CORTEX-A7 device
         9U if your device is a CORTEX-A9 device */
#define __CORTEX_A                    5U      /*!< Cortex-A# Core                              */
#define __CA_REV                 0x0001U      /*!< Core revision r0p0                          */
/* ToDo: define the correct core features for the <Device> */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 1U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */

/** @} */ /* End of group Configuration_of_CMSIS */

/* ToDo: include the correct core_ca#.h file
         core_ca5.h if your device is a CORTEX-A5 device
         core_ca7.h if your device is a CORTEX-A7 device
         core_ca9.h if your device is a CORTEX-A9 device */
#include <core_ca.h>                         /*!< Arm Cortex-A# processor and core peripherals */
/* ToDo: include your system_<Device>.h file
         replace '<Device>' with your device name */
#include "system_AI3100.h"                  /*!< <Device> System */



/** @} */ /* End of group <Device> */

/** @} */ /* End of group <AIVA> */

#ifdef __cplusplus
}
#endif

#endif  /* <Device>_H */
