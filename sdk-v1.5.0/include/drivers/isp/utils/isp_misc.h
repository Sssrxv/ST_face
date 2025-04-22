#ifndef __MISC_H__
#define __MISC_H__

/* beware of macro side effects! */

#ifndef __FLT_EPSILON__
#define __FLT_EPSILON__     0.000000119209289550781250000000
#endif /* __FLT_EPSILON__ */

#ifndef FLT_EPSILON
#define FLT_EPSILON         __FLT_EPSILON__
#endif /* FLT_EPSILON */

#ifndef FLT_MAX
#define FLT_MAX     ((float)3.40282346638528860e+38)
#endif /* FLT_MAX */

#ifndef MIN
#define MIN(a, b)   ( ((a)<(b)) ? (a) : (b) )
#endif /* MIN */

#ifndef MAX
#define MAX(a, b)   ( ((a)>(b)) ? (a) : (b) )
#endif /* MAX */

#ifndef ABS
#define ABS(a)      ( ((a)<0) ? -(a) : (a) )
#endif /*ABS */

#ifndef SIGN
#define SIGN(a)     ( ((a)<0) ? -1 : ((a)>0) ? 1 : 0 )
#endif /* SIGN */

#define DIVMIN     0.00001f  /**< lowest denominator for divisions  */
#define LOGMIN     0.0001f   /**< lowest value for logarithmic calculations */

#endif /* __MISC_H__ */
