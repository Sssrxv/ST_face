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

#ifndef _ELPSPACCHW_
#define _ELPSPACCHW_

#define SPACC_INTR_PRI              (19 << 3)

/* max DDT particle size */
#ifndef SPACC_MAX_PARTICLE_SIZE
#define SPACC_MAX_PARTICLE_SIZE     4096
#endif

/* max message size from HW configuration */
/* usually defined in ICD as (2 exponent 16) -1 */
#ifndef _SPACC_MAX_MSG_MALLOC_SIZE
#define _SPACC_MAX_MSG_MALLOC_SIZE     16
#endif
#define SPACC_MAX_MSG_MALLOC_SIZE     (1 << _SPACC_MAX_MSG_MALLOC_SIZE)

#ifndef SPACC_MAX_MSG_SIZE
#define SPACC_MAX_MSG_SIZE     (SPACC_MAX_MSG_MALLOC_SIZE - 1)
#endif

#define SPACC_LOOP_WAIT  1000000

/********* Register Offsets **********/
#define SPACC_REG_IRQ_EN       0x00000L
#define SPACC_REG_IRQ_STAT     0x00004L
#define SPACC_REG_IRQ_CTRL     0x00008L
#define SPACC_REG_FIFO_STAT    0x0000CL
#define SPACC_REG_SDMA_BRST_SZ 0x00010L

/* HSM specific */
#define SPACC_REG_HSM_CMD_REQ      0x00014L
#define SPACC_REG_HSM_CMD_GNT      0x00018L

#define SPACC_REG_SRC_PTR      0x00020L
#define SPACC_REG_DST_PTR      0x00024L
#define SPACC_REG_OFFSET       0x00028L
#define SPACC_REG_PRE_AAD_LEN  0x0002CL
#define SPACC_REG_POST_AAD_LEN 0x00030L

#define SPACC_REG_PROC_LEN     0x00034L
#define SPACC_REG_ICV_LEN      0x00038L
#define SPACC_REG_ICV_OFFSET   0x0003CL
#define SPACC_REG_IV_OFFSET    0x00040L

#define SPACC_REG_SW_CTRL      0x00044L
#define SPACC_REG_AUX_INFO     0x00048L
#define SPACC_REG_CTRL         0x0004CL

#define SPACC_REG_STAT_POP     0x00050L
#define SPACC_REG_STATUS       0x00054L

#define SPACC_REG_STAT_WD_CTRL 0x00080L


#define SPACC_REG_KEY_SZ       0x00100L

#define SPACC_REG_VIRTUAL_RQST         0x00140L
#define SPACC_REG_VIRTUAL_ALLOC        0x00144L
#define SPACC_REG_VIRTUAL_PRIO         0x00148L
#define SPACC_REG_VIRTUAL_RC4_KEY_RQST 0x00150L
#define SPACC_REG_VIRTUAL_RC4_KEY_GNT  0x00154L

#define SPACC_REG_ID            0x00180L
#define SPACC_REG_CONFIG        0x00184L
#define SPACC_REG_CONFIG2       0x00190L
#define SPACC_REG_HSM_VERSION   0x00188L

#define SPACC_REG_SECURE_CTRL    0x001C0L
#define SPACC_REG_SECURE_RELEASE 0x001C4

#define SPACC_REG_SK_LOAD        0x00200L
#define SPACC_REG_SK_STAT        0x00204L
#define SPACC_REG_SK_KEY         0x00240L

#define SPACC_REG_HSM_CTX_CMD    0x00300L
#define SPACC_REG_HSM_CTX_STAT   0x00304L

#define SPACC_REG_SKP           0x800000UL // out 8MB from base of SPACC

#ifndef SPACC_ID_MINOR
#define SPACC_ID_MINOR(x)   ((x)         & 0x0F)
#define SPACC_ID_MAJOR(x)   (((x) >>  4) & 0x0F)
#define SPACC_ID_QOS(x)     (((x) >>  8) & 0x01)
#define SPACC_ID_TYPE(x)    (((x) >>  9) & 0x03)
#define SPACC_ID_AUX(x)     (((x) >> 11) & 0x01)
#define SPACC_ID_VIDX(x)    (((x) >> 12) & 0x07)
#define SPACC_ID_PARTIAL(x) (((x) >> 15) & 0x01)
#define SPACC_ID_PROJECT(x) ((x)>>16)

#define SPACC_CFG_CTX_CNT(x)       ((x) & 0x7F)
#define SPACC_CFG_RC4_CTX_CNT(x)   (((x) >> 8) & 0x7F)
#define SPACC_CFG_VSPACC_CNT(x)    (((x) >> 16) & 0x0F)
#define SPACC_CFG_CIPH_CTX_SZ(x)   (((x) >> 20) & 0x07)
#define SPACC_CFG_HASH_CTX_SZ(x)   (((x) >> 24) & 0x07)
#define SPACC_CFG_DMA_TYPE(x)      (((x) >> 28) & 0x03)

#define SPACC_HSM_CFG_MINOR(x)   ((x) & 0x0F)
#define SPACC_HSM_CFG_MAJOR(x)   (((x)>>4)  & 0x0F)
#define SPACC_HSM_CFG_PARADIGM(x)    (((x)>>8)  & 0x01)
#define SPACC_HSM_CFG_KEY_CNT(x)  (((x)>>16)&0xFF)
#define SPACC_HSM_CFG_KEY_SZ(x)   (((x)>>14)&0x03)

#endif

/********** Context Offsets **********/
#define SPACC_CTX_CIPH_KEY     0x04000L
#define SPACC_CTX_HASH_KEY     0x08000L
#define SPACC_CTX_RC4_CTX      0x20000L

/******** Sub-Context Offsets ********/
#define SPACC_CTX_AES_KEY      0x00
#define SPACC_CTX_AES_IV       0x20

#define SPACC_CTX_DES_KEY      0x08
#define SPACC_CTX_DES_IV       0x00

#define SPACC_CTX_RC4_KEY      0x00
#define SPACC_CTX_RC4_IJ       0x100

#define SPACC_RC4_DMA_CTRL     0x00
#define SPACC_RC4_DMA_STAT     0x04
#define SPACC_RC4_DMA_CTX      0x200

#define SPACC_RC4_CTX_IDX_O    0
#define SPACC_RC4_CTX_IDX_W    8
#define _SPACC_RC4_IMPORT      31
#define SPACC_RC4_IMPORT      (1UL << _SPACC_RC4_IMPORT)

/******** Context Page Sizes *********/

#define SPACC_CTX_RC4_PAGESIZE   512

/* use these to loop over CMDX macros */
#define SPACC_CMDX_MAX        1
#define SPACC_CMDX_MAX_QOS    3
/********** IRQ_EN Bit Masks **********/

#define _SPACC_IRQ_CMD0       0
#define _SPACC_IRQ_STAT       4
#define _SPACC_IRQ_RC4_DMA    8
#define _SPACC_IRQ_STAT_WD    12
#define _SPACC_IRQ_GLBL       31

#define SPACC_IRQ_EN_CMD(x)     (1UL << _SPACC_IRQ_CMD0 << (x))
#define SPACC_IRQ_EN_STAT       (1UL << _SPACC_IRQ_STAT)
#define SPACC_IRQ_EN_RC4_DMA    (1UL << _SPACC_IRQ_RC4_DMA)
#define SPACC_IRQ_EN_STAT_WD    (1UL << _SPACC_IRQ_STAT_WD)
#define SPACC_IRQ_EN_GLBL       (1UL << _SPACC_IRQ_GLBL)

/********* IRQ_STAT Bitmasks *********/

#define SPACC_IRQ_STAT_CMDX(x)     (1UL << _SPACC_IRQ_CMD0 << (x))
#define SPACC_IRQ_STAT_STAT        (1UL << _SPACC_IRQ_STAT)
#define SPACC_IRQ_STAT_STAT_WD     (1UL << _SPACC_IRQ_STAT_WD)
#define SPACC_IRQ_STAT_RC4_DMA     (1UL << _SPACC_IRQ_RC4_DMA)

#define SPACC_IRQ_STAT_CLEAR_STAT(spacc)    pdu_io_write32(spacc->regmap + SPACC_REG_IRQ_STAT, SPACC_IRQ_STAT_STAT)
#define SPACC_IRQ_STAT_CLEAR_STAT_WD(spacc) pdu_io_write32(spacc->regmap + SPACC_REG_IRQ_STAT, SPACC_IRQ_STAT_STAT_WD)
#define SPACC_IRQ_STAT_CLEAR_RC4_DMA(spacc) pdu_io_write32(spacc->regmap + SPACC_REG_IRQ_STAT, SPACC_IRQ_STAT_RC4_DMA)
#define SPACC_IRQ_STAT_CLEAR_CMDX(spacc, x) pdu_io_write32(spacc->regmap + SPACC_REG_IRQ_STAT, SPACC_IRQ_STAT_CMDX(x))


/********* IRQ_CTRL Bitmasks *********/
/* CMD0 = 0; for QOS, CMD1 = 8, CMD2 = 16 */
#define _SPACC_IRQ_CTRL_CMDX_CNT(x)       (8*(x))
#define SPACC_IRQ_CTRL_CMDX_CNT_SET(x,n)  (((n) & 0xFF) << _SPACC_IRQ_CTRL_CMDX_CNT(x))
#define SPACC_IRQ_CTRL_CMDX_CNT_MASK(x)   (0xFF << _SPACC_IRQ_CTRL_CMDX_CNT(x))

/* STAT_CNT is at 16 and for QOS at 24 */
#define _SPACC_IRQ_CTRL_STAT_CNT          16
#define SPACC_IRQ_CTRL_STAT_CNT_SET(n)    ((n) << _SPACC_IRQ_CTRL_STAT_CNT)
#define SPACC_IRQ_CTRL_STAT_CNT_MASK      (0x1FF << _SPACC_IRQ_CTRL_STAT_CNT)

#define _SPACC_IRQ_CTRL_STAT_CNT_QOS         24
#define SPACC_IRQ_CTRL_STAT_CNT_SET_QOS(n)   ((n) << _SPACC_IRQ_CTRL_STAT_CNT_QOS)
#define SPACC_IRQ_CTRL_STAT_CNT_MASK_QOS     (0x7F << _SPACC_IRQ_CTRL_STAT_CNT_QOS)

/******** FIFO_STAT Bitmasks *********/

/* SPACC with QOS */
#define SPACC_FIFO_STAT_CMDX_CNT_MASK(x)      (0x7F << ((x)*8))
#define SPACC_FIFO_STAT_CMDX_CNT_GET(x,y)     (((y) & SPACC_FIFO_STAT_CMDX_CNT_MASK(x)) >> ((x)*8))
#define SPACC_FIFO_STAT_CMDX_FULL(x)          (1UL << (7 + (x)*8))

#define _SPACC_FIFO_STAT_STAT_CNT_QOS         24
#define SPACC_FIFO_STAT_STAT_CNT_MASK_QOS     (0x7F << _SPACC_FIFO_STAT_STAT_CNT_QOS)
#define SPACC_FIFO_STAT_STAT_CNT_GET_QOS(y)   (((y) & SPACC_FIFO_STAT_STAT_CNT_MASK_QOS) >> _SPACC_FIFO_STAT_STAT_CNT_QOS)

/* SPACC without QOS */
#define SPACC_FIFO_STAT_CMD0_CNT_MASK         (0x1FF)
#define SPACC_FIFO_STAT_CMD0_CNT_GET(y)       ((y) & SPACC_FIFO_STAT_CMD0_CNT_MASK)
#define _SPACC_FIFO_STAT_CMD0_FULL            15
#define SPACC_FIFO_STAT_CMD0_FULL             (1ul<<_SPACC_FIFO_STAT_CMD0_FULL)

#define _SPACC_FIFO_STAT_STAT_CNT             16
#define SPACC_FIFO_STAT_STAT_CNT_MASK         (0x1FF << _SPACC_FIFO_STAT_STAT_CNT)
#define SPACC_FIFO_STAT_STAT_CNT_GET(y)       (((y) & SPACC_FIFO_STAT_STAT_CNT_MASK) >> _SPACC_FIFO_STAT_STAT_CNT)

/* both */
#define _SPACC_FIFO_STAT_STAT_EMPTY           31
#define SPACC_FIFO_STAT_STAT_EMPTY            (1UL << _SPACC_FIFO_STAT_STAT_EMPTY)

/******* SDMA_BRST_SZ Bitmasks *******/

#if defined (SPACC_ALG_SHA384) || defined (SPACC_ALG_SHA512)
# define SPACC_SDMA_BRST_SZ_BRST_SZ 0, 5
#else
# define SPACC_SDMA_BRST_SZ_BRST_SZ 0, 4
#endif

/********* SRC_PTR Bitmasks **********/

#define SPACC_SRC_PTR_PTR           0xFFFFFFF8

/********* DST_PTR Bitmasks **********/

#define SPACC_DST_PTR_PTR           0xFFFFFFF8

/********** OFFSET Bitmasks **********/

#define SPACC_OFFSET_SRC_O          0
#define SPACC_OFFSET_SRC_W          16
#define SPACC_OFFSET_DST_O          16
#define SPACC_OFFSET_DST_W          16

#define SPACC_MIN_CHUNK_SIZE        1024
#define SPACC_MAX_CHUNK_SIZE        16384


/********* PKT_LEN Bitmasks **********/

#ifndef _SPACC_PKT_LEN_PROC_LEN
#define _SPACC_PKT_LEN_PROC_LEN    0
#endif
#ifndef _SPACC_PKT_LEN_AAD_LEN
#define _SPACC_PKT_LEN_AAD_LEN     16
#endif


/********* SW_CTRL Bitmasks ***********/

#define _SPACC_SW_CTRL_ID_0          0
#define SPACC_SW_CTRL_ID_W           8
#define SPACC_SW_CTRL_ID_MASK        (0xFF << _SPACC_SW_CTRL_ID_0)
#define SPACC_SW_CTRL_ID_GET(y)      (((y) & SPACC_SW_CTRL_ID_MASK) >> _SPACC_SW_CTRL_ID_0)
#define SPACC_SW_CTRL_ID_SET(id)     (((id) & SPACC_SW_CTRL_ID_MASK) >> _SPACC_SW_CTRL_ID_0)

#define _SPACC_SW_CTRL_PRIO          30
#define SPACC_SW_CTRL_PRIO_MASK      0x3
#define SPACC_SW_CTRL_PRIO_SET(prio) (((prio) & SPACC_SW_CTRL_PRIO_MASK) << _SPACC_SW_CTRL_PRIO)

/* Priorities */
#define SPACC_SW_CTRL_PRIO_HI         0
#define SPACC_SW_CTRL_PRIO_MED        1
#define SPACC_SW_CTRL_PRIO_LOW        2


/*********** SECURE_CTRL bitmasks *********/
#define _SPACC_SECURE_CTRL_MS_SRC    0
#define _SPACC_SECURE_CTRL_MS_DST    1
#define _SPACC_SECURE_CTRL_MS_DDT    2
#define _SPACC_SECURE_CTRL_LOCK     31

#define SPACC_SECURE_CTRL_MS_SRC    (1UL<<_SPACC_SECURE_CTRL_MS_SRC)
#define SPACC_SECURE_CTRL_MS_DST    (1UL<<_SPACC_SECURE_CTRL_MS_DST)
#define SPACC_SECURE_CTRL_MS_DDT    (1UL<<_SPACC_SECURE_CTRL_MS_DDT)
#define SPACC_SECURE_CTRL_LOCK      (1UL<<_SPACC_SECURE_CTRL_LOCK)

/*********** CTX_CMD bitmasks *********/
/* SPAcc HSM Specific */
#define _SPACC_CTX_CMD_IDX          0
#define _SPACC_CTX_CMD_IDX_REQ      30
#define _SPACC_CTX_CMD_REQ          31

#define SPACC_CTX_CMD_IDX           (1UL<<_SPACC_CTX_CMD_IDX)
#define SPACC_CTX_CMD_IDX_REQ       (1UL<<_SPACC_CTX_CMD_IDX_REQ)
#define SPACC_CTX_CMD_REQ           (1UL<<_SPACC_CTX_CMD_REQ)

/*********** CTX_STAT bitmasks *********/
/* SPAcc HSM Specific */
#define _SPACC_CTX_STAT_IDX          0
#define _SPACC_CTX_STAT_SUCCESS      30
#define _SPACC_CTX_STAT_RDY          31

#define SPACC_CTX_STAT_IDX          (1UL<<_SPACC_CTX_STAT_IDX)
#define SPACC_CTX_STAT_SUCCESS      (1UL<<_SPACC_CTX_STAT_SUCCESS)
#define SPACC_CTX_STAT_RDY          (1UL<<_SPACC_CTX_STAT_RDY)

/********* SKP bits **************/
#define _SPACC_SK_LOAD_CTX_IDX       0
#define _SPACC_SK_LOAD_ALG           8
#define _SPACC_SK_LOAD_MODE         12
#define _SPACC_SK_LOAD_SIZE         16
#define _SPACC_SK_LOAD_ENC_EN       30
#define _SPACC_SK_LOAD_DEC_EN       31
#define _SPACC_SK_STAT_BUSY          0

#define SPACC_SK_LOAD_ENC_EN         (1UL<<_SPACC_SK_LOAD_ENC_EN)
#define SPACC_SK_LOAD_DEC_EN         (1UL<<_SPACC_SK_LOAD_DEC_EN)
#define SPACC_SK_STAT_BUSY           (1UL<<_SPACC_SK_STAT_BUSY)

/*********** CTRL Bitmasks ***********/
#define _SPACC_CTRL_CIPH_ALG         0
#define _SPACC_CTRL_CIPH_MODE        4
#define _SPACC_CTRL_HASH_ALG         8
#define _SPACC_CTRL_HASH_MODE       13
#define _SPACC_CTRL_ENCRYPT         15
#define _SPACC_CTRL_CTX_IDX         16
#define _SPACC_CTRL_SEC_KEY         24
#define _SPACC_CTRL_AAD_COPY        25
#define _SPACC_CTRL_ICV_PT          26
#define _SPACC_CTRL_ICV_ENC         27
#define _SPACC_CTRL_ICV_APPEND      28
#define _SPACC_CTRL_KEY_EXP         29
#define _SPACC_CTRL_MSG_BEGIN       30
#define _SPACC_CTRL_MSG_END         31

/* CTRL bitmasks for 4.15+ cores */
#define _SPACC_CTRL_CIPH_ALG_415         0
#define _SPACC_CTRL_CIPH_MODE_415        4
#define _SPACC_CTRL_HASH_ALG_415         8
#define _SPACC_CTRL_HASH_MODE_415       13


/********* Virtual Spacc Priority Bitmasks **********/
#define _SPACC_VPRIO_MODE          0
#define _SPACC_VPRIO_WEIGHT        8

/********* AUX INFO Bitmasks *********/
#define _SPACC_AUX_INFO_DIR        0
#define _SPACC_AUX_INFO_BIT_ALIGN  1
#define _SPACC_AUX_INFO_CBC_CS    16

/********* STAT_POP Bitmasks *********/
#define _SPACC_STAT_POP_POP         0

#define SPACC_STAT_POP_POP         (1UL << _SPACC_STAT_POP_POP)

/********** STATUS Bitmasks **********/
#define _SPACC_STATUS_SW_ID        0
#define _SPACC_STATUS_RET_CODE     24
#define _SPACC_STATUS_SEC_CMD      31
#define SPACC_GET_STATUS_RET_CODE(s)          (((s)>>_SPACC_STATUS_RET_CODE)&0x7)
#define SPACC_STATUS_SW_ID_MASK         (0xFF << _SPACC_STATUS_SW_ID)
#define SPACC_STATUS_SW_ID_GET(y)      (((y) & SPACC_STATUS_SW_ID_MASK) >> _SPACC_STATUS_SW_ID)

/********** KEY_SZ Bitmasks **********/
#define _SPACC_KEY_SZ_SIZE         0
#define _SPACC_KEY_SZ_CTX_IDX      8
#define _SPACC_KEY_SZ_CIPHER       31

#define SPACC_KEY_SZ_CIPHER        (1UL << _SPACC_KEY_SZ_CIPHER)

#define SPACC_SET_CIPHER_KEY_SZ(z) (((z)<< _SPACC_KEY_SZ_SIZE) | (1UL << _SPACC_KEY_SZ_CIPHER))
#define SPACC_SET_HASH_KEY_SZ(z)   ((z) << _SPACC_KEY_SZ_SIZE)
#define SPACC_SET_KEY_CTX(ctx)     ((ctx) << _SPACC_KEY_SZ_CTX_IDX)

/***********************************************************************************/
/***********************************************************************************/

#define CTRL_SET_CIPH_ALG(a)    ((a) << _SPACC_CTRL_CIPH_ALG)
#define CTRL_SET_CIPH_MODE(a)   ((a) << _SPACC_CTRL_CIPH_MODE)
#define CTRL_SET_HASH_ALG(a)    ((a) << _SPACC_CTRL_HASH_ALG)
#define CTRL_SET_HASH_MODE(a)   ((a) << _SPACC_CTRL_HASH_MODE)
#define CTRL_SET_CIPH_ALG_415(a)    ((a) << _SPACC_CTRL_CIPH_ALG_415)
#define CTRL_SET_CIPH_MODE_415(a)   ((a) << _SPACC_CTRL_CIPH_MODE_415)
#define CTRL_SET_HASH_ALG_415(a)    ((a) << _SPACC_CTRL_HASH_ALG_415)
#define CTRL_SET_HASH_MODE_415(a)   ((a) << _SPACC_CTRL_HASH_MODE_415)
#define CTRL_SET_ENCRYPT        (1UL << _SPACC_CTRL_ENCRYPT)
#define CTRL_SET_AAD_COPY       (1UL << _SPACC_CTRL_AAD_COPY)
#define CTRL_SET_ICV_PT         (1UL << _SPACC_CTRL_ICV_PT)
#define CTRL_SET_ICV_ENC        (1UL << _SPACC_CTRL_ICV_ENC)
#define CTRL_SET_ICV_APPEND     (1UL << _SPACC_CTRL_ICV_APPEND)
#define CTRL_SET_KEY_EXP        (1UL << _SPACC_CTRL_KEY_EXP)
#define CTRL_SET_MSG_BEGIN      (1UL << _SPACC_CTRL_MSG_BEGIN)
#define CTRL_SET_MSG_END        (1UL << _SPACC_CTRL_MSG_END)
#define CTRL_SET_CTX_IDX(a)     ((a) << _SPACC_CTRL_CTX_IDX)
#define CTRL_SET_SEC_KEY        (1UL << _SPACC_CTRL_SEC_KEY)

#define AUX_INFO_SET_DIR(a)       ((a) << _SPACC_AUX_INFO_DIR)
#define AUX_INFO_SET_BIT_ALIGN(a) ((a) << _SPACC_AUX_INFO_BIT_ALIGN)
#define AUX_INFO_SET_CBC_CS(a)    ((a) << _SPACC_AUX_INFO_CBC_CS)

#define VPRIO_SET(mode,weight) (((mode)<<_SPACC_VPRIO_MODE)|((weight)<<_SPACC_VPRIO_WEIGHT))

enum ecipher
{
  C_NULL   = 0,
  C_DES    = 1,
  C_AES    = 2,
  C_RC4    = 3,
  C_MULTI2 = 4,
  C_KASUMI = 5,
  C_SNOW3G_UEA2 = 6,
  C_ZUC_UEA3 = 7,

  C_MAX
};

enum eciphermode
{
  CM_ECB = 0,
  CM_CBC = 1,
  CM_CTR = 2,
  CM_CCM = 3,
  CM_GCM = 5,
  CM_OFB = 7,
  CM_CFB = 8,
  CM_F8  = 9,
  CM_XTS = 10,

  CM_MAX
};

enum ehash
{
  H_NULL   = 0,
  H_MD5    = 1,
  H_SHA1   = 2,
  H_SHA224 = 3,
  H_SHA256 = 4,
  H_SHA384 = 5,
  H_SHA512 = 6,
  H_XCBC   = 7,
  H_CMAC   = 8,
  H_KF9    = 9,
  H_SNOW3G_UIA2 = 10,
  H_CRC32_I3E802_3 = 11,
  H_ZUC_UIA3 = 12,
  H_SHA512_224 = 13,
  H_SHA512_256 = 14,
  H_MICHAEL    = 15,
  H_SHA3_224=16,
  H_SHA3_256=17,
  H_SHA3_384=18,
  H_SHA3_512=19,
  H_MAX
};

enum ehashmode
{
  HM_RAW    = 0,
  HM_SSLMAC = 1,
  HM_HMAC   = 2,

  HM_MAX
};

enum spacc_ret_code
{
  SPACC_OK = 0,
  SPACC_ICVFAIL,
  SPACC_MEMERR,
  SPACC_BLOCKERR,
  SPACC_SECERR,
};

#endif  /* _ELPSPACCHW_ */
