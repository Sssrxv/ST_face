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

#ifndef ELPCLP800_HW_H_
#define ELPCLP800_HW_H_

/* Control/status registers */
enum {
   CLP800_CTRL,
   CLP800_STAT,
   CLP800_MODE,
   CLP800_SMODE,
   CLP800_IE,
   CLP800_ISTAT,
   CLP800_BUILD_ID,
   CLP800_FEATURES,

   CLP800_RAND_BASE = 0x20>>2,
   CLP800_SEED_BASE = 0x40>>2,

   CLP800_AUTO_RQST = 0x60>>2,
   CLP800_AUTO_AGE,

   CLP800_IA_RDATA = 0x70>>2,
   CLP800_IA_WDATA,
   CLP800_IA_ADDR,
   CLP800_IA_CMD,
};

#define CLP800_REG_MAX 0x80

/* IRQ bitfields (used for both IE and ISTAT registers) */
#define CLP800_IRQ_GLBL_EN    31
#define CLP800_IRQ_RQST_ALARM  3
#define CLP800_IRQ_AGE_ALARM   2
#define CLP800_IRQ_SEED_DONE   1
#define CLP800_IRQ_RAND_RDY    0

#define CLP800_IRQ_GLBL_EN_MASK    (1ul<<CLP800_IRQ_GLBL_EN)
#define CLP800_IRQ_RQST_ALARM_MASK (1ul<<CLP800_IRQ_RQST_ALARM)
#define CLP800_IRQ_AGE_ALARM_MASK  (1ul<<CLP800_IRQ_AGE_ALARM)
#define CLP800_IRQ_SEED_DONE_MASK  (1ul<<CLP800_IRQ_SEED_DONE)
#define CLP800_IRQ_RAND_RDY_MASK   (1ul<<CLP800_IRQ_RAND_RDY)

#define CLP800_IRQ_ALL_MASK ( CLP800_IRQ_GLBL_EN_MASK \
                           | CLP800_IRQ_RQST_ALARM_MASK \
                           | CLP800_IRQ_AGE_ALARM_MASK \
                           | CLP800_IRQ_SEED_DONE_MASK \
                           | CLP800_IRQ_RAND_RDY_MASK )

/* CTRL register commands */
enum {
   CLP800_CMD_NOP,
   CLP800_CMD_GEN_RAND,
   CLP800_CMD_RAND_RESEED,
   CLP800_CMD_NONCE_RESEED,
};

/* STAT register bitfields */
#define CLP800_STAT_RESEEDING          31
#define CLP800_STAT_GENERATING         30
#define CLP800_STAT_SRVC_RQST          27
#define CLP800_STAT_RESEED_REASON      16
#define CLP800_STAT_RESEED_REASON_BITS  3
#define CLP800_STAT_SEEDED              9
#define CLP800_STAT_SECURE              8
#define CLP800_STAT_R256                3
#define CLP800_STAT_NONCE_MODE          2

/* STAT.RESEED_REASON values */
enum {
   CLP800_RESEED_HOST     = 0,
   CLP800_RESEED_NONCE    = 3,
   CLP800_RESEED_PIN      = 4,
   CLP800_RESEED_UNSEEDED = 7,
};

/* MODE register bitfields */
#define CLP800_MODE_R256               3

/* SMODE register bitfields */
#define CLP800_SMODE_MAX_REJECTS      16
#define CLP800_SMODE_MAX_REJECTS_BITS  8
#define CLP800_SMODE_SECURE            8
#define CLP800_SMODE_NONCE             2

/* FEATURES register bitfields */
#define CLP800_FEATURES_DIAG_LEVEL      4
#define CLP800_FEATURES_DIAG_LEVEL_BITS 3
#define CLP800_FEATURES_SECURE_RST      3
#define CLP800_FEATURES_RAND_SEED       2
#define CLP800_FEATURES_RAND_LEN        0
#define CLP800_FEATURES_RAND_LEN_BITS   2

/* BUILD_ID register bitfields */
#define CLP800_BUILD_ID_EPN           0
#define CLP800_BUILD_ID_EPN_BITS     16
#define CLP800_BUILD_ID_STEPPING     28
#define CLP800_BUILD_ID_STEPPING_BITS 4

/* AUTO_RQST register bitfields */
#define CLP800_AUTO_RQST_RQSTS        0
#define CLP800_AUTO_RQST_RQSTS_BITS  16
#define CLP800_AUTO_RQST_RESOLUTION  16ul

/* AUTO_AGE register bitfields */
#define CLP800_AUTO_AGE_AGE           0
#define CLP800_AUTO_AGE_AGE_BITS     16
#define CLP800_AUTO_AGE_RESOLUTION   (1ul << 26)

/* IA_CMD register bitfields */
#define CLP800_IA_CMD_GO   31
#define CLP800_IA_CMD_WRITE 0

#endif
