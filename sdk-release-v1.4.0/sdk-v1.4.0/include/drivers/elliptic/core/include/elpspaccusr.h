/*
 * Copyright (c) 2015 Elliptic Technologies Inc.
 */

// include spacc modes if we are in userspace
#ifndef _ELPSPACCMODES_H_
   #include "elpspaccmodes.h"
#endif

// no __user in userspace
#ifndef KERNEL
   #ifndef __user
      #define __user
   #endif
#endif

enum {
   ELP_SPACC_USR_INIT=100,       // set keys/mode
   ELP_SPACC_USR_PROCESS_IV,     // process packet and reset IV (not used with ivimport)
   ELP_SPACC_USR_PROCESS,        // process packet
   ELP_SPACC_USR_REGISTER,       // register a state key with this connection
   ELP_SPACC_USR_BIND,           // bind this connection to another
   ELP_SPACC_USR_FEATURES,       // get features out of the SPAcc config
};

#define ELP_SPACC_USR_MAX_DDT 8  // max userspace segments per src/dst

struct elp_spacc_usr_ddt {
   void __user *ptr;
   int          len;
};

#define ELP_SPACC_SRC_DDT(fd, x, addr, len) do { (fd)->io.src[x] = (struct elp_spacc_usr_ddt){(addr),(len)}; } while (0);
#define ELP_SPACC_DST_DDT(fd, x, addr, len) do { (fd)->io.dst[x] = (struct elp_spacc_usr_ddt){(addr),(len)}; } while (0);
#define ELP_SPACC_SRC_TERM(fd, x) do { (fd)->io.src[x] = (struct elp_spacc_usr_ddt){NULL,0}; } while (0);
#define ELP_SPACC_DST_TERM(fd, x) do { (fd)->io.dst[x] = (struct elp_spacc_usr_ddt){NULL,0}; } while (0);

#define ELP_SPACC_SRCLEN_SET(fd, x) (fd)->io.srclen = x
#define ELP_SPACC_DSTLEN_SET(fd, x) (fd)->io.dstlen = x

#define ELP_SPACC_SRCLEN_RESET(fd) (fd)->io.srclen = -1
#define ELP_SPACC_DSTLEN_RESET(fd) (fd)->io.dstlen = -1

enum {
   SPACC_MAP_HINT_TEST=0,  // test internally if there is valid overlap
   SPACC_MAP_HINT_USESRC,  // there is overlap and use the source mapping (because the destination mapping is contained inside it)
   SPACC_MAP_HINT_USEDST,  // there is overlap and use the destination mapping
   SPACC_MAP_HINT_NOLAP    // there is no overlap so don't bother trying to test for it
};

// Message begin/end flags
#define ELP_SPACC_USR_MSG_BEGIN 1
#define ELP_SPACC_USR_MSG_END   2

// handy macros for setting the various partial processing states
#define ELP_SPACC_USR_MSG_DEFAULT(fd) (fd)->io.partial = ELP_SPACC_USR_MSG_BEGIN|ELP_SPACC_USR_MSG_END
#define ELP_SPACC_USR_MSG_FIRST(fd)   (fd)->io.partial = ELP_SPACC_USR_MSG_BEGIN
#define ELP_SPACC_USR_MSG_MIDDLE(fd)  (fd)->io.partial = 0
#define ELP_SPACC_USR_MSG_LAST(fd)    (fd)->io.partial = ELP_SPACC_USR_MSG_END

struct elp_spacc_features {
   unsigned
      project,
      partial,
      version,
      ivimport,
      qos,
      max_msg_size;
   unsigned char
      modes[CRYPTO_MODE_LAST];
};

struct elp_spacc_ioctl {
   unsigned char
                cmd,           // ioctl command
                cipher_mode,   // cipher mode
                aad_copy,      // copy the AAD or not
                hash_mode,     // hash mode
                icv_len,       // ICV length (0 for default)
                icv_mode,      // how to handle the ICV
                encrypt,       // 1 to set encrypt mode
                ckeylen,       // cipher key length
                civlen,        // cipher IV length
                hkeylen,       // hash key length
                hivlen,        // hash IV length
                auxinfo_align, // bit align for 3G modes
                auxinfo_dir,   // direction for 3G modes
                partial,       // MSG_BEGIN/MSG_END flags
                map_hint;
   int
                err;

   struct elp_spacc_usr_ddt
                src[ELP_SPACC_USR_MAX_DDT+1],
                dst[ELP_SPACC_USR_MAX_DDT+1];

   int          srclen, dstlen,              // despite the DDTs above these need to be filled in to make things simpler inside the kernel
                src_offset, dst_offset,      // offsets into buffers
                pre_aad_len, post_aad_len,   // AAD lengths before and after plaintext (if any)
                ivoffset;                    // offset in packet for IV, or -1 to not use import

   unsigned char civ[64];                    // order is important, civ must come before ckey
   unsigned char ckey[64], hkey[128], hiv[128];
   unsigned char state_key[16];              // state key used to allow multiple file handle use the same SPAcc handle
}__attribute__((packed));

struct elp_spacc_usr {
   int fd;
   struct elp_spacc_ioctl io;
};

#ifndef KERNEL

int spacc_dev_open(struct elp_spacc_usr *io,
                   int cipher_mode, int hash_mode,    // cipher and hash (see elpspaccmodes.h)
                   int encrypt,                       // non-zero for encrypt (sign) modes
                   int icvmode,                       // ICV mode (see elpspaccmodes.h)
                   int icvlen,                        // length of ICV, 0 for default length (algorithm dependent)
                   int aad_copy,                      // non-zero to copy AAD to dst buffer
                   unsigned char *ckey, int ckeylen, unsigned char *civ, int civlen,  // cipher key and IVs
                   unsigned char *hkey, int hkeylen, unsigned char *hiv, int hivlen); // hash key and IVs (if any)

#define spacc_dev_setaux(f, align, dir) do { (f)->io.auxinfo_align = align; (f)->io.auxinfo_dir = dir; } while (0);

int spacc_dev_register(struct elp_spacc_usr *io);
int spacc_dev_open_bind(struct elp_spacc_usr *master, struct elp_spacc_usr *new);

int spacc_dev_process_multi(
   struct elp_spacc_usr *io,
   unsigned char *new_iv,
   int            iv_offset,
   int            pre_aad,
   int            post_aad,
   int            src_offset,
   int            dst_offset,
   int            map_hint);

int spacc_dev_process(
   struct elp_spacc_usr *io,
   unsigned char *new_iv,                       //pointer to new IV for this job if any, NULL if not
   int            iv_offset,                    //offset of IV for this job within src, -1 if you don't want to use this
   int            pre_aad,                      //PRE AAD size
   int            post_aad,                     //POST AAD size
   int            src_offset,
   int            dst_offset,
   unsigned char *src, unsigned long src_len,
   unsigned char *dst, unsigned long dst_len);

int spacc_dev_close(struct elp_spacc_usr *io);

int spacc_dev_features(struct elp_spacc_features *features);

int spacc_dev_isenabled(struct elp_spacc_features *features, int mode, int keysize);

#endif
