#ifndef _ISP_BASE_TYPES_H_
#define _ISP_BASE_TYPES_H_

#include <stdint.h>

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef NULL
#define NULL (0)
#endif

#define  RET_SUCCESS           (0)
#define  RET_FAILURE           (-1)
#define  RET_NOTSUPP           (-2)
#define  RET_BUSY              (-3)
#define  RET_CANCELED          (-4)
#define  RET_OUTOFMEM          (-5)
#define  RET_OUTOFRANGE        (-6)
#define  RET_IDLE              (-7)
#define  RET_WRONG_HANDLE      (-8)
#define  RET_NULL_POINTER      (-9)
#define  RET_NOTAVAILABLE      (-10)
#define  RET_DIVISION_BY_ZERO  (-11)
#define  RET_WRONG_STATE       (-12)
#define  RET_INVALID_PARM      (-13)
#define  RET_PENDING           (-14)
#define  RET_WRONG_CONFIG      (-15)
#define  RET_BUFFER_EMPTY      (-16)
#define  RET_NOTBLOCK_WAIT     (-17)
#define  RET_TIMEOUT           (-18)


#endif
