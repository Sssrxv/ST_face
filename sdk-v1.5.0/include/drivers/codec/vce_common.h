#ifndef _VCE_COMMON_H_
#define _VCE_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
*  enable_vce_sysctl_clock just enable once, if vce clock enabled, will not reenable.
*/
void enable_vce_sysctl_clock(void);
/**
 * reset_vce_sysctl_clock will force reenable vce clock.
 */
void reset_vce_sysctl_clock(void);
/**
 *
 */
void disable_vce_sysctl_clock(void);


#ifdef __cplusplus
}
#endif

#endif //_VCE_COMMON_H_