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

#include "elppdu.h"
#include "elpclp850_hw.h"

#define CLP850_RETRY_MAX 5000000UL

typedef struct {
   uint32_t *base;   
   
   struct {
      struct { 
         unsigned diag_level_trng3,
                  diag_level_st_hlt,
                  secure_rst_state;
      } features;

      struct {
         unsigned stepping,
                  epn;
      } build_id;
   } config;
   
   struct {
      volatile unsigned 
         last_alarm,
         seeded,
         rnd_rdy,
         kat_done,
         zeroized;
   } status;

   PDU_LOCK_TYPE lock;
} elpclp850_state;

#define clp850_zero_status(x) memset(&(x->status), 0, sizeof (x->status))

/* functions users will call */
int elpclp850_init(elpclp850_state *clp850, uint32_t *base);

int elpclp850_hw_read(elpclp850_state *clp850, uint32_t *out);
int elpclp850_nonce_read(elpclp850_state *clp850, uint32_t *seed, uint32_t *out);

int elpclp850_set_keylen(elpclp850_state *clp850, int aes256, int lock);
int elpclp850_set_secure(elpclp850_state *clp850, int secure, int lock);
int elpclp850_set_kat(elpclp850_state *clp850, int kat, int lock);
int elpclp850_set_wait_for_ht(elpclp850_state *clp850, int ht, int lock);
int elpclp850_set_nonce(elpclp850_state *clp850, int nonce, int lock);

int elpclp850_ia_write(elpclp850_state *clp850, uint32_t addr, uint32_t val);
int elpclp850_ia_read(elpclp850_state *clp850, uint32_t addr, uint32_t *val);

/* Functions we use to manage the device */
int elpclp850_handle_irq(elpclp850_state *clp850);
