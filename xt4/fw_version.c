#include <stdint.h>

#define XT4_VERSION_PRE     "AT101"
#define XT4_VERSION_SUF     ""

void os_facelock_get_fw_version_info(const char **version_prefix, const char **version_suffix, const char **build_time, uint32_t *version)
{
    const char* time_str = XT4_TARGET_FW_BUILD_TIME;

    *version_prefix = XT4_VERSION_PRE;
    *version_suffix = XT4_VERSION_SUF;
    *version        = XT4_TARGET_FW_VER;
    *build_time     = time_str;
}
