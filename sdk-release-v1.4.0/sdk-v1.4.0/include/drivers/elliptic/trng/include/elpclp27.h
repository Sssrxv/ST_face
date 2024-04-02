/*
 * Copyright (c) 2015 Elliptic Technologies Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#ifndef _ELPCLP27_H_
#define _ELPCLP27_H_

#include "elppdu.h"
#include "elpclp27_hw.h"

/* Error codes */
enum
{
   ELPCLP27_OK = 0,
   ELPCLP27_NOT_INITIALIZED,
   ELPCLP27_TIMEOUT,
   ELPCLP27_INVALID_ARGUMENT,
};

/* wait flag */
enum
{
   ELPCLP27_NO_WAIT = 0,
   ELPCLP27_WAIT,
};

/* reseed flag */
enum
{
   ELPCLP27_NO_RESEED = 0,
   ELPCLP27_RESEED,
};

/* IRQ flag */
enum
{
   ELPCLP27_IRQ_PIN_DISABLE = 0,
   ELPCLP27_IRQ_PIN_ENABLE,
};

/* init status */
enum
{
   ELPCLP27_NOT_INIT = 0,
   ELPCLP27_INIT,
};

typedef struct elpclp27_hw {
#if (defined CLP27_HDAC)
   uint32_t * elpclp27_hdac_ctrl;
   uint32_t * elpclp27_hdac_stat;
#endif
   uint32_t * elpclp27_ctrl;
   uint32_t * elpclp27_irq_stat;
   uint32_t * elpclp27_irq_en;
   uint32_t * elpclp27_data;
   uint32_t * elpclp27_cfg;
   uint32_t initialized;
   PDU_LOCK_TYPE lock;
} elpclp27_hw;

/* useful constants */
#define CLP27_DATA_SIZE_WORDS 4
#define CLP27_DATA_SIZE_BYTES 16
#define CLP27_NONCE_SIZE_WORDS 8
#define CLP27_NONCE_SIZE_BYTES 32

#define LOOP_WAIT      1000000   /* Configurable delay */

/* Function definitions for external use */
void elpclp27_dump_registers (elpclp27_hw * hw);
int32_t elpclp27_wait_for_done (elpclp27_hw * hw, uint32_t cycles);
int32_t elpclp27_reseed_random (elpclp27_hw * hw, uint32_t wait, int lock);
int32_t elpclp27_reseed_nonce (elpclp27_hw * hw, uint32_t seed[CLP27_NONCE_SIZE_WORDS], int lock);
int32_t elpclp27_init (elpclp27_hw * hw, uint32_t base_addr, uint32_t enable_irq, uint32_t reseed);
void elpclp27_close (elpclp27_hw * hw);
int32_t elpclp27_rand (elpclp27_hw * hw, uint8_t * randbuf, uint32_t size, int lock);

#if (defined CLP27_HDAC)
void elpclp27_enable(elpclp27_hw * hw);
#endif

/* kernel supplied */
elpclp27_hw *elpclp27_get_device(void);

/* user supplied functions */
void elpclp27_user_init_wait(elpclp27_hw *hw);
void elpclp27_user_wait(elpclp27_hw *hw);



#endif

