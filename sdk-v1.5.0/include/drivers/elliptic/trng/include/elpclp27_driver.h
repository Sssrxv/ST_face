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


#ifndef __ELLIPTIC_DRIVER_CLP27__
#define __ELLIPTIC_DRIVER_CLP27__

#include "elpclp27.h"

#define ELLIPTIC_CLP27_DRIVER_PLATFORM_NAME "clp27"
#define ELLIPTIC_HWRNG_DRIVER_NAME "hwrng-clp27"

#define ELLIPTIC_CLP27_CRYPTO_RESEED_NONCE_SUPPORT 1

typedef struct 
{
  elpclp27_hw elpclp27;
  unsigned int *base_addr;

#ifdef ELPCLP27_HWRANDOM
  void *hwrng_drv;
#endif

#ifdef ELPCLP27_CRYPTO_API
  void *crypto_drv;
#endif
}elliptic_elpclp27_driver;

#ifdef ELPCLP27_HWRANDOM
int elpclp27_hwrng_driver_read(struct hwrng *rng, void *buf, size_t max, bool wait);
#endif

#ifdef ELPCLP27_CRYPTO_API
int elpclp27_crypto_driver_read(struct crypto_rng *tfm, u8 *rdata, unsigned int dlen);
int elpclp27_crypto_driver_reset(struct crypto_rng *tfm, u8 *seed, unsigned int slen);
#endif

#endif /* __ELLIPTIC_DRIVER_CLP27__ */



