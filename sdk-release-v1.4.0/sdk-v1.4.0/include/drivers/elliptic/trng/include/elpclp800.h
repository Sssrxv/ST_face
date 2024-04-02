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

#ifndef ELPTRNG3_H_
#define ELPTRNG3_H_

#include "elppdu.h"

#define ELPCLP800_MAXLEN 32

struct elpclp800_state {
   void     (*writereg)(void *base, unsigned offset, uint32_t val);
   uint32_t (*readreg) (void *base, unsigned offset);

   void *regbase;

   unsigned short epn, stepping, output_len, diag_level;
   unsigned secure_reset:1,
            rings_avail:1;
};

void elpclp800_setup(struct elpclp800_state *clp800, uint32_t *regbase);

/*
 * Enable/disable the IRQ sources specified in irq_mask, which is the
 * bitwise-OR of one or more of the TRNG3_IRQ_xxx_MASK macros in
 * elpclp800_hw.h.
 */
void elpclp800_enable_irqs(struct elpclp800_state *clp800, uint32_t irq_mask);
void elpclp800_disable_irqs(struct elpclp800_state *clp800, uint32_t irq_mask);

/*
 * If secure is non-zero, switch the TRNG into secure mode.  Otherwise,
 * switch the TRNG into promiscuous mode.
 */
void elpclp800_set_secure(struct elpclp800_state *clp800, int secure);

/*
 * Adjust the bit rejection tweak threshold.
 */
int elpclp800_set_max_rejects(struct elpclp800_state *clp800, unsigned rejects);

/*
 * Initiate a reseed of the engine.  If nonce is a null pointer, then a
 * random reseed operation is performed.  Otherwise, nonce should point to
 * a 32-byte buffer which will be used to perform a nonce reseed.
 */
int elpclp800_reseed(struct elpclp800_state *clp800, const void *nonce);

/*
 * Read the last-used seed into a buffer, which must be at least 32 bytes long.
 */
int elpclp800_get_seed(struct elpclp800_state *clp800, void *out);

/*
 * Configure the output length of the engine.  Possible values are 16 and 32
 * bytes.  32-byte mode requires appropriate H/W configuration.
 */
int elpclp800_set_output_len(struct elpclp800_state *clp800, unsigned outlen);

/*
 * Configure the request-based reseed reminder alarm to trigger after the
 * specified number of random numbers generated.
 */
int elpclp800_set_request_reminder(struct elpclp800_state *clp800,
                                  unsigned long requests);

/*
 * Configure the age-based reseed reminder alarm to trigger after the specified
 * number of clock cycles.
 */
int elpclp800_set_age_reminder(struct elpclp800_state *clp800,
                              unsigned long long cycles);

/*
 * Retrieve random data from the engine; out should point to a buffer
 * that is at least 32 bytes long to accomodate all possible output
 * sizes.  On success, returns the number of output bytes written to
 * the buffer (may be either 16 or 32).
 */
int elpclp800_get_random(struct elpclp800_state *clp800, void *out);

/* Helpers to access TRNG registers directly. */
static inline void
elpclp800_writereg(struct elpclp800_state *clp800, unsigned offset, uint32_t val)
{
   clp800->writereg(clp800->regbase, offset, val);
}

static inline uint32_t
elpclp800_readreg(struct elpclp800_state *clp800, unsigned offset)
{
   return clp800->readreg(clp800->regbase, offset);
}

#endif
