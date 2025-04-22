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


#ifndef _ELPCLP27_HW_H_
#define _ELPCLP27_HW_H_

/* Register offset addresses */
#if (defined CLP27_HDAC)
#define CLP27_CTRL       0x0040
#else
#define CLP27_CTRL       0x0000
#endif

#if (defined CLP27_HDAC)
#define CLP27_IRQ_STAT   0x0044
#define CLP27_IRQ_EN     0x0048
#elif (defined CLP27_SASPA)
#define CLP27_IRQ_STAT   0x0800
#define CLP27_IRQ_EN     0x0804
#else
#define CLP27_IRQ_STAT   0x0004
#define CLP27_IRQ_EN     0x0008
#endif

#define CLP27_CFG        0x000C
#define CLP27_CFG_RINGS      30
#define CLP27_CFG_RINGS_BITS  2

/* start offset of 4 32-bit words */
#if (defined CLP27_HDAC)
#define CLP27_DATA       0x0050
#else
#define CLP27_DATA       0x0010
#endif

#ifdef CLP27_HDAC
#define CLP27_HDAC_CTRL 0x0000
#define CLP27_HDAC_STAT 0x0004
#endif

/* CLP27_CTRL register definitions */
#define CLP27_CTRL_RAND_RESEED          31
#define CLP27_CTRL_NONCE_RESEED         30
#define CLP27_CTRL_NONCE_RESEED_LD      29
#define CLP27_CTRL_NONCE_RESEED_SELECT  28
#define CLP27_CTRL_GEN_NEW_RANDOM        0

#ifdef CLP27_HDAC
#define CLP27_HDAC_CTRL_IE_BIT     30
#define CLP27_HDAC_CTRL_IRQ_ENABLE (1 << CLP27_HDAC_CTRL_IE_BIT)
#define CLP27_HDAC_STAT_IRQ_BIT    30
#define CLP27_HDAC_STAT_IRQ_DONE   (1 << CLP27_HDAC_STAT_IRQ_BIT)
#endif

#define CLP27_RAND_RESEED            (1 << CLP27_CTRL_RAND_RESEED)
#define CLP27_NONCE_RESEED           (1 << CLP27_CTRL_NONCE_RESEED)
#define CLP27_NONCE_RESEED_LD        (1 << CLP27_CTRL_NONCE_RESEED_LD)
#define CLP27_NONCE_RESEED_SELECT    (1 << CLP27_CTRL_NONCE_RESEED_SELECT)
#define CLP27_GET_NEW                (1 << CLP27_CTRL_GEN_NEW_RANDOM)
#define CLP27_BUSY                   (1 << CLP27_CTRL_GEN_NEW_RANDOM)

#define CLP27_IRQ_STAT_BIT              27
#define CLP27_IRQ_DONE               (1 << CLP27_IRQ_STAT_BIT)

#define CLP27_IRQ_EN_BIT                27
#define CLP27_IRQ_ENABLE             (1 << CLP27_IRQ_EN_BIT)

#endif
