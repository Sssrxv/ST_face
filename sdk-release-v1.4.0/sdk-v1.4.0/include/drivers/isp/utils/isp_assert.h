#ifndef _ISP_ASSERT_H_
#define _ISP_ASSERT_H_


#include "isp_base_types.h"
#include "isp_log.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef CONFIG_ENABLE_ASSERT
#define ISP_ASSERT(exp) \
    do{\
        if (!(exp))\
        {\
            ISP_LOG_ERROR("Assertion Error: File:%s, Line:%s\n", __FILE__, __LINE__);\
            return -1;\
        }\
    }while(0)
#else
#define ISP_ASSERT(exp)
#endif



#ifdef __cplusplus
}
#endif

#endif
