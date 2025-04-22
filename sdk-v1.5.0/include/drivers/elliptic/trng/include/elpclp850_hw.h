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

#ifndef ELPCLP850_HW_H
#define ELPCLP850_HW_H

/* registers */
#define CLP850_REG_CTRL             0x00
#define CLP850_REG_STAT             0x01
#define CLP850_REG_MODE             0x02
#define CLP850_REG_IE               0x03
#define CLP850_REG_ISTAT            0x04
#define CLP850_REG_ALARM            0x05
#define CLP850_REG_BUILD_ID         0x06
#define CLP850_REG_FEATURES         0x07
#define CLP850_REG_RAND0            0x08
#define CLP850_REG_SEED0            0x0C
#define CLP850_REG_IA_RDATA         0x1C
#define CLP850_REG_IA_WDATA         0x1D
#define CLP850_REG_IA_ADDR          0x1E
#define CLP850_REG_IA_CMD           0x1F

/* CTRL REG */
#define CLP850_REG_CTRL_SEED_NOISE  0x01
#define CLP850_REG_CTRL_SEED_NONCE  0x02
#define CLP850_REG_CTRL_KAT         0x03
#define CLP850_REG_CTRL_ZEROIZE     0x04

/* STAT */
#define _CLP850_REG_STAT_BUSY       31
#define _CLP850_REG_STAT_SECURE      4
#define _CLP850_REG_STAT_NONCE_MODE  3
#define _CLP850_REG_STAT_LAST_CMD    0
#define CLP850_REG_STAT_BUSY(x)           (((x) >> _CLP850_REG_STAT_BUSY) & 1)
#define CLP850_REG_STAT_SECURE(x)         (((x) >> _CLP850_REG_STAT_SECURE) & 1)
#define CLP850_REG_STAT_NONCE_MODE(x)     (((x) >> _CLP850_REG_STAT_NONCE_MODE) & 1)
#define CLP850_REG_STAT_LAST_CMD(x)       (((x) >> _CLP850_REG_STAT_LAST_CMD) & 7)

/* ALARM */
#define CLP850_REG_ALARM_OK              0
#define CLP850_REG_ALARM_KAT_STAT        1
#define CLP850_REG_ALARM_KAT             2
#define CLP850_REG_ALARM_MONOBIT         3
#define CLP850_REG_ALARM_RUN             4
#define CLP850_REG_ALARM_LONG_RUN        5
#define CLP850_REG_ALARM_AUTOCORRELATION 6
#define CLP850_REG_ALARM_POKER           7
#define CLP850_REG_ALARM_REPETITION      8
#define CLP850_REG_ALARM_ADAPTIVE        9

/* MODE */
#define _CLP850_REG_MODE_MAX_REJECTS     16
#define _CLP850_REG_MODE_SECURE_EN        8
#define _CLP850_REG_MODE_NONCE            3
#define _CLP850_REG_MODE_KAT              2
#define _CLP850_REG_MODE_SEC_ALG          1
#define _CLP850_REG_MODE_WAIT_FOR_HT      0

#define _CLP850_REG_MODE_MAX_REJECTS_MASK    255UL
#define _CLP850_REG_MODE_SECURE_EN_MASK        1UL
#define _CLP850_REG_MODE_NONCE_MASK            1UL
#define _CLP850_REG_MODE_KAT_MASK              1UL
#define _CLP850_REG_MODE_SEC_ALG_MASK          1UL
#define _CLP850_REG_MODE_WAIT_FOR_HT_MASK      1UL

#define CLP850_REG_MODE_SET_MAX_REJECTS(y, x) (((y) & ~(_CLP850_REG_MODE_MAX_REJECTS_MASK << _CLP850_REG_MODE_MAX_REJECTS)) | ((x) << _CLP850_REG_MODE_MAX_REJECTS))
#define CLP850_REG_MODE_SET_SECURE_EN(y, x)   (((y) & ~(_CLP850_REG_MODE_SECURE_EN_MASK << _CLP850_REG_MODE_SECURE_EN))     | ((x) << _CLP850_REG_MODE_SECURE_EN))
#define CLP850_REG_MODE_SET_NONCE(y, x)       (((y) & ~(_CLP850_REG_MODE_NONCE_MASK << _CLP850_REG_MODE_NONCE))             | ((x) << _CLP850_REG_MODE_NONCE))
#define CLP850_REG_MODE_SET_KAT(y, x)         (((y) & ~(_CLP850_REG_MODE_KAT_MASK << _CLP850_REG_MODE_KAT))                 | ((x) << _CLP850_REG_MODE_KAT))
#define CLP850_REG_MODE_SET_SEC_ALG(y, x)     (((y) & ~(_CLP850_REG_MODE_SEC_ALG_MASK << _CLP850_REG_MODE_SEC_ALG))         | ((x) << _CLP850_REG_MODE_SEC_ALG))
#define CLP850_REG_MODE_SET_WAIT_FOR_HT(y, x) (((y) & ~(_CLP850_REG_MODE_WAIT_FOR_HT_MASK << _CLP850_REG_MODE_WAIT_FOR_HT)) | ((x) << _CLP850_REG_MODE_WAIT_FOR_HT))

#define CLP850_REG_MODE_GET_MAX_REJECTS(x)  (((x) >> _CLP850_REG_MODE_MAX_REJECTS) & _CLP850_REG_MODE_MAX_REJECTS_MASK)
#define CLP850_REG_MODE_GET_SECURE_EN(x)    (((x) >> _CLP850_REG_MODE_SECURE_EN) & _CLP850_REG_MODE_SECURE_EN_MASK)
#define CLP850_REG_MODE_GET_NONCE(x)        (((x) >> _CLP850_REG_MODE_NONCE) & _CLP850_REG_MODE_NONCE_MASK)
#define CLP850_REG_MODE_GET_KAT(x)          (((x) >> _CLP850_REG_MODE_KAT) & _CLP850_REG_MODE_KAT_MASK)
#define CLP850_REG_MODE_GET_SEC_ALG(x)      (((x) >> _CLP850_REG_MODE_SEC_ALG) & _CLP850_REG_MODE_SEC_ALG_MASK)
#define CLP850_REG_MODE_GET_WAIT_FOR_HT(x)  (((x) >> _CLP850_REG_MODE_WAIT_FOR_HT) & _CLP850_REG_MODE_WAIT_FOR_HT_MASK)

/* IE */
#define CLP850_REG_IE_GLOBAL     31
#define CLP850_REG_IE_ALARM       4
#define CLP850_REG_IE_SEEDED      3
#define CLP850_REG_IE_RND_RDY     2
#define CLP850_REG_IE_KAT_DONE    1
#define CLP850_REG_IE_ZEROIZED    0

/* ISTAT */
#define CLP850_REG_ISTAT_ALARMS   (1ul<<4)
#define CLP850_REG_ISTAT_SEEDED   (1ul<<3)
#define CLP850_REG_ISTAT_RND_RDY  (1ul<<2)
#define CLP850_REG_ISTAT_KAT_DONE (1ul<<1)
#define CLP850_REG_ISTAT_ZEROIZED (1ul<<0)

/* BUILD_ID */
#define _CLP850_REG_BUILD_ID_STEPPING 28
#define _CLP850_REG_BUILD_ID_EPN       0
#define CLP850_REG_BUILD_ID_STEPPING(x)  (((x) >> _CLP850_REG_BUILD_ID_STEPPING) & 31)
#define CLP850_REG_BUILD_ID_EPN(x)       (((x) >> _CLP850_REG_BUILD_ID_EPN) & 0xFFFF)

/* FEATURES */
#define _CLP850_REG_FEATURES_DIAG_LEVEL_TRNG3       7
#define _CLP850_REG_FEATURES_DIAG_LEVEL_ST_HLT      4
#define _CLP850_REG_FEATURES_SECURE_RST_STATE       3
#define CLP850_REG_FEATURES_DIAG_LEVEL_TRNG3(x)  (((x) >> _CLP850_REG_FEATURES_DIAG_LEVEL_TRNG3) & 7)
#define CLP850_REG_FEATURES_DIAG_LEVEL_ST_HLT(x) (((x) >> _CLP850_REG_FEATURES_DIAG_LEVEL_ST_HLT) & 7)
#define CLP850_REG_FEATURES_SECURE_RST_STATE(x)  (((x) >> _CLP850_REG_FEATURES_SECURE_RST_STATE) & 1)

/* IA_CMD */
#define _CLP850_REG_IA_CMD_GO      31
#define _CLP850_REG_IA_CMD_W_nR     0
#define CLP850_REG_IA_CMD_GO       (1ul << _CLP850_REG_IA_CMD_GO)
#define CLP850_REG_IA_CMD_W_nR     (1ul << _CLP850_REG_IA_CMD_W_nR)




#endif
