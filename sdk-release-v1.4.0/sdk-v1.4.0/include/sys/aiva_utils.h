#ifndef _AIVA_UTILS_H
#define _AIVA_UTILS_H
#include <stddef.h>
#include <stdint.h>
#include "aiva_printf.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AIVA_MIN(a, b) ((a) > (b) ? (b) : (a))
#define AIVA_MAX(a, b) ((a) > (b) ? (a) : (b))

#define AIVA_ARRAY_LEN(r)           (sizeof(r) / sizeof(r[0]))
#define __ATTR_ALIGN__(a)           __attribute__ ((aligned(a)))

#define BIT(nr)                     (1UL << (nr))
#define BIT_ULL(nr)                 (1ULL << (nr))

#define ROUND_8(n)                  (((n) + 7UL)  & ~7UL)
#define ROUND_16(n)                 (((n) + 15UL) & ~15UL)
#define ROUND_32(n)                 (((n) + 31UL) & ~31UL)

/* Silence warnings about unused parameters */
#define UNUSED_PARAM(x) 		    (void) x

/* Swap pointers */
#define SWAP_PTRS(a, b) do {         \
    void *tmp = (void *)(a);         \
    (a) = (b);                       \
    (b) = tmp;                       \
} while (0)

/* Unified operation for endianness */
typedef uint8_t     ubyte_t;
typedef uint8_t     uword_t[2];
typedef uint8_t     udword_t[4];

#define UNI_GETW(w)    ((w)[0] | ((w)[1] << 8))
#define UNI_SETW(w,v)  ((w)[0] = (uint8_t)(v), (w)[1] = (uint8_t)((v) >> 8))
#define UNI_GETDW(w)   ((w)[0] | ((w)[1] << 8) | ((w)[2] << 16) | ((w)[3] << 24))
#define UNI_SETDW(w,v) ((w)[0] = (uint8_t)(v), \
		     (w)[1] = (uint8_t)((v) >> 8),  \
		     (w)[2] = (uint8_t)((v) >> 16), \
		     (w)[3] = (uint8_t)((v) >> 24))

#ifdef __ASSEMBLY__
#define AIVA_CAST(type, ptr) ptr
#else /* __ASSEMBLY__ */
/**
 * @brief       Cast the pointer to specified pointer type.
 *
 * @param[in]   type        The pointer type to cast to
 * @param[in]   ptr         The pointer to apply the type cast to
 */
#define AIVA_CAST(type, ptr) ((type)(ptr))
#endif /* __ASSEMBLY__ */

#define container_of(ptr, type, member) ({\
    const typeof( ((type *)0)->member ) *__mptr = (ptr);\
    (type *)( (char *)__mptr - offsetof(type,member) );})

/**
 * @addtogroup      UTIL_RW_FUNC Memory Read/Write Utilities
 *
 * This section implements read and write functionality for various
 * memory untis. The memory unit terms used for these functions are
 * consistent with those used in the ARM Architecture Reference Manual
 * ARMv7-A and ARMv7-R edition manual. The terms used for units of memory are:
 *
 *  Unit of Memory | Abbreviation | Size in Bits
 * :---------------|:-------------|:------------:
 *  Byte           | byte         |       8
 *  Half Word      | hword        |      16
 *  Word           | word         |      32
 *  Double Word    | dword        |      64
 *
 */

/**
 * @brief       Write the 8 bit byte to the destination address in device memory.
 *
 * @param[in]   dest        Write destination pointer address
 * @param[in]   src         8 bit data byte to write to memory
 */
#define aiva_write_byte(dest, src) \
    (*AIVA_CAST(volatile uint8_t*, (dest)) = (src))

/**
 * @brief       Read and return the 8 bit byte from the source address in device memory.
 *
 * @param[in]   src     Read source pointer address
 *
 * @return      8 bit data byte value
 */
#define aiva_read_byte(src) (*AIVA_CAST(volatile uint8_t*, (src)))

/**
 * @brief       Write the 16 bit half word to the destination address in device memory.
 *
 * @param[in]   dest        Write destination pointer address
 * @param[in]   src         16 bit data half word to write to memory
 */
#define aiva_write_hword(dest, src) \
    (*AIVA_CAST(volatile uint16_t*, (dest)) = (src))

/**
 * @brief       Read and return the 16 bit half word from the source address in device
 *
 * @param[in]   src     Read source pointer address
 *
 * @return      16 bit data half word value
 */
#define aiva_read_hword(src) (*AIVA_CAST(volatile uint16_t*, (src)))

/**
 * @brief       Write the 32 bit word to the destination address in device memory.
 *
 * @param[in]   dest        Write destination pointer address
 * @param[in]   src         32 bit data word to write to memory
 */
#define aiva_write_word(dest, src) \
    (*AIVA_CAST(volatile uint32_t*, (dest)) = (src))

/**
 * @brief       Read and return the 32 bit word from the source address in device memory.
 *
 * @param[in]   src     Read source pointer address
 *
 * @return      32 bit data half word value
 */
#define aiva_read_word(src) (*AIVA_CAST(volatile uint32_t*, (src)))

/**
 * @brief       Write the 64 bit double word to the destination address in device memory.
 *
 * @param[in]   dest        Write destination pointer address
 * @param[in]   src         64 bit data word to write to memory
 */
#define aiva_write_dword(dest, src) \
    (*AIVA_CAST(volatile uint64_t*, (dest)) = (src))

/**
 * @brief       Read and return the 64 bit double word from the source address in device
 *
 * @param[in]   src     Read source pointer address
 *
 * @return      64 bit data half word value
 */
#define aiva_read_dword(src) (*AIVA_CAST(volatile uint64_t*, (src)))

/**
 * @brief       Set selected bits in the 8 bit byte at the destination address in device
 *
 * @param[in]   dest        Destination pointer address
 * @param[in]   bits        Bits to set in destination byte
 */
#define aiva_setbits_byte(dest, bits) \
    (aiva_write_byte(dest, aiva_read_byte(dest) | (bits)))

/**
 * @brief       Clear selected bits in the 8 bit byte at the destination address in device
 *
 * @param[in]   dest        Destination pointer address
 * @param[in]   bits        Bits to clear in destination byte
 */
#define aiva_clrbits_byte(dest, bits) \
    (aiva_write_byte(dest, aiva_read_byte(dest) & ~(bits)))

/**
 * @brief       Change or toggle selected bits in the 8 bit byte at the destination address
 *
 * @param[in]   dest        Destination pointer address
 * @param[in]   bits        Bits to change in destination byte
 */
#define aiva_xorbits_byte(dest, bits) \
    (aiva_write_byte(dest, aiva_read_byte(dest) ^ (bits)))

/**
 * @brief       Replace selected bits in the 8 bit byte at the destination address in device
 *
 * @param[in]   dest        Destination pointer address
 * @param[in]   msk         Bits to replace in destination byte
 * @param[in]   src         Source bits to write to cleared bits in destination byte
 */
#define aiva_replbits_byte(dest, msk, src) \
    (aiva_write_byte(dest, (aiva_read_byte(dest) & ~(msk)) | ((src) & (msk))))

/**
 * @brief       Set selected bits in the 16 bit halfword at the destination address in
 *
 * @param[in]   dest        Destination pointer address
 * @param[in]   bits        Bits to set in destination halfword
 */
#define aiva_setbits_hword(dest, bits) \
    (aiva_write_hword(dest, aiva_read_hword(dest) | (bits)))

/**
 * @brief       Clear selected bits in the 16 bit halfword at the destination address in
 *
 * @param[in]   dest        Destination pointer address
 * @param[in]   bits        Bits to clear in destination halfword
 */
#define aiva_clrbits_hword(dest, bits) \
    (aiva_write_hword(dest, aiva_read_hword(dest) & ~(bits)))

/**
 * @brief       Change or toggle selected bits in the 16 bit halfword at the destination
 *
 * @param[in]   dest        Destination pointer address
 * @param[in]   bits        Bits to change in destination halfword
 */
#define aiva_xorbits_hword(dest, bits) \
    (aiva_write_hword(dest, aiva_read_hword(dest) ^ (bits)))

/**
 * @brief       Replace selected bits in the 16 bit halfword at the destination address in
 *
 * @param[in]   dest        Destination pointer address
 * @param[in]   msk         Bits to replace in destination byte
 * @param[in]   src         Source bits to write to cleared bits in destination halfword
 */
#define aiva_replbits_hword(dest, msk, src) \
    (aiva_write_hword(dest, (aiva_read_hword(dest) & ~(msk)) | ((src) & (msk))))

/**
 * @brief       Set selected bits in the 32 bit word at the destination address in device
 *
 * @param[in]   dest        Destination pointer address
 * @param[in]   bits        Bits to set in destination word
 */
#define aiva_setbits_word(dest, bits) \
    (aiva_write_word(dest, aiva_read_word(dest) | (bits)))

/**
 * @brief       Clear selected bits in the 32 bit word at the destination address in device
 *
 * @param[in]   dest        Destination pointer address
 * @param[in]   bits        Bits to clear in destination word
 */
#define aiva_clrbits_word(dest, bits) \
    (aiva_write_word(dest, aiva_read_word(dest) & ~(bits)))

/**
 * @brief       Change or toggle selected bits in the 32 bit word at the destination address
 *
 * @param[in]   dest        Destination pointer address
 * @param[in]   bits        Bits to change in destination word
 */
#define aiva_xorbits_word(dest, bits) \
    (aiva_write_word(dest, aiva_read_word(dest) ^ (bits)))

/**
 * @brief       Replace selected bits in the 32 bit word at the destination address in
 *
 * @param[in]   dest        Destination pointer address
 * @param[in]   msk         Bits to replace in destination word
 * @param[in]   src         Source bits to write to cleared bits in destination word
 */
#define aiva_replbits_word(dest, msk, src) \
    (aiva_write_word(dest, (aiva_read_word(dest) & ~(msk)) | ((src) & (msk))))

/**
 * @brief      Set selected bits in the 64 bit doubleword at the destination address in
 *
 * @param[in]   dest     Destination pointer address
 * @param[in]   bits     Bits to set in destination doubleword
 */
#define aiva_setbits_dword(dest, bits) \
    (aiva_write_dword(dest, aiva_read_dword(dest) | (bits)))

/**
 * @brief       Clear selected bits in the 64 bit doubleword at the destination address in
 *
 * @param[in]   dest        Destination pointer address
 * @param[in]   bits        Bits to clear in destination doubleword
 */
#define aiva_clrbits_dword(dest, bits) \
    (aiva_write_dword(dest, aiva_read_dword(dest) & ~(bits)))

/**
 * @brief       Change or toggle selected bits in the 64 bit doubleword at the destination
 *
 * @param[in]   dest        Destination pointer address
 * @param[in]   bits        Bits to change in destination doubleword
 */
#define aiva_xorbits_dword(dest, bits) \
    (aiva_write_dword(dest, aiva_read_dword(dest) ^ (bits)))

/**
 * @brief       Replace selected bits in the 64 bit doubleword at the destination address in
 *
 * @param[in]   dest        Destination pointer address
 * @param[in]   msk         its to replace in destination doubleword
 * @param[in]   src         Source bits to write to cleared bits in destination word
 */
#define aiva_replbits_dword(dest, msk, src) \
    (aiva_write_dword(dest, (aiva_read_dword(dest) & ~(msk)) | ((src) & (msk))))


/**
 * @brief       Set value by mask
 *
 * @param[in]   bits        The one be set
 * @param[in]   mask        mask value
 * @param[in]   value       The value to set
 */
void set_bit(volatile uint32_t *bits, uint32_t mask, uint32_t value);

/**
 * @brief       Set value by mask
 *
 * @param[in]   bits        The one be set
 * @param[in]   mask        Mask value
 * @param[in]   offset      Mask's offset
 * @param[in]   value       The value to set
 */
void set_bit_offset(volatile uint32_t *bits, uint32_t mask, size_t offset, uint32_t value);

/**
 * @brief       Set bit for gpio, only set one bit
 *
 * @param[in]   bits        The one be set
 * @param[in]   idx         Offset value
 * @param[in]   value       The value to set
 */
void set_gpio_bit(volatile uint32_t *bits, size_t idx, uint32_t value);

/**
 * @brief      Get bits value of mask
 *
 * @param[in]   bits        The source data
 * @param[in]   mask        Mask value
 * @param[in]   offset      Mask's offset
 *
 * @return      The bits value of mask
 */
uint32_t get_bit(volatile uint32_t *bits, uint32_t mask, size_t offset);

/**
 * @brief       Get a bit value by offset
 *
 * @param[in]   bits        The source data
 * @param[in]   offset      Bit's offset
 *
 *
 * @return      The bit value
 */
uint32_t get_gpio_bit(volatile uint32_t *bits, size_t offset);

void *aiva_memcpy ( uint8_t  *dest, const uint8_t *src, uint32_t  count);

void *aiva_memset ( uint8_t *ptr, uint8_t  data, uint32_t count);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _DRIVER_COMMON_H */

