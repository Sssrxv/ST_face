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
#ifndef __ELPCRYPTO_API_
#define __ELPCRYPTO_API__

#include <stdint.h>

#define ELPCRYPTO_TRNG_NAME       "stdrng-elp"
#define ELPCRYPTO_TRNG_TYPE       "rng"
#define ELPCRYPTO_TRNG_DRIVER     "crypto-elptrng"
#define ELPCRYPTO_TRNG_PRIORITY    200

#ifdef __cplusplus
extern "C" {
#endif

int spacc_api_init(void);
int spacc_api_uninit(void);

/* HASH */
int spacc_hash_md5   (const char *msg, int msg_bytes, char out_buf[16]);
int spacc_hash_sha1  (const char *msg, int msg_bytes, char out_buf[20]);
int spacc_hash_sha256(const char *msg, int msg_bytes, char out_buf[32]);
int spacc_hash_sha384(const char *msg, int msg_bytes, char out_buf[32]);
int spacc_hash_sha512(const char *msg, int msg_bytes, char out_buf[64]);

/* HMAC-HASH */
int spacc_hmac_md5   (const char *msg, int msg_bytes, char out_buf[16], const char *hmac_key, int key_len);
int spacc_hmac_sha1  (const char *msg, int msg_bytes, char out_buf[20], const char *hmac_key, int key_len);
int spacc_hmac_sha224(const char *msg, int msg_bytes, char out_buf[28], const char *hmac_key, int key_len);
int spacc_hmac_sha256(const char *msg, int msg_bytes, char out_buf[32], const char *hmac_key, int key_len);
int spacc_hmac_sha384(const char *msg, int msg_bytes, char out_buf[48], const char *hmac_key, int key_len);
int spacc_hmac_sha512(const char *msg, int msg_bytes, char out_buf[64], const char *hmac_key, int key_len);

/* AES */
int spacc_aes_set_iv_key(const char *iv, const char *key, int len);
int spacc_encrypt_aes_cbc_128(const char *msg, int msg_bytes, char *out_buf);
int spacc_encrypt_aes_cbc_192(const char *msg, int msg_bytes, char *out_buf);
int spacc_encrypt_aes_cbc_256(const char *msg, int msg_bytes, char *out_buf);

int spacc_encrypt_aes_ecb_128(const char *msg, int msg_bytes, char *out_buf);
int spacc_encrypt_aes_ecb_192(const char *msg, int msg_bytes, char *out_buf);
int spacc_encrypt_aes_ecb_256(const char *msg, int msg_bytes, char *out_buf);

int spacc_encrypt_aes_cfb_128(const char *msg, int msg_bytes, char *out_buf);
int spacc_encrypt_aes_cfb_192(const char *msg, int msg_bytes, char *out_buf);
int spacc_encrypt_aes_cfb_256(const char *msg, int msg_bytes, char *out_buf);

int spacc_encrypt_aes_ofb_128(const char *msg, int msg_bytes, char *out_buf);
int spacc_encrypt_aes_ofb_192(const char *msg, int msg_bytes, char *out_buf);
int spacc_encrypt_aes_ofb_256(const char *msg, int msg_bytes, char *out_buf);

int spacc_decrypt_aes_cbc_128(const char *msg, int msg_bytes, char *out_buf);
int spacc_decrypt_aes_cbc_192(const char *msg, int msg_bytes, char *out_buf);
int spacc_decrypt_aes_cbc_256(const char *msg, int msg_bytes, char *out_buf);

int spacc_decrypt_aes_ecb_128(const char *msg, int msg_bytes, char *out_buf);
int spacc_decrypt_aes_ecb_192(const char *msg, int msg_bytes, char *out_buf);
int spacc_decrypt_aes_ecb_256(const char *msg, int msg_bytes, char *out_buf);

int spacc_decrypt_aes_cfb_128(const char *msg, int msg_bytes, char *out_buf);
int spacc_decrypt_aes_cfb_192(const char *msg, int msg_bytes, char *out_buf);
int spacc_decrypt_aes_cfb_256(const char *msg, int msg_bytes, char *out_buf);

int spacc_decrypt_aes_ofb_128(const char *msg, int msg_bytes, char *out_buf);
int spacc_decrypt_aes_ofb_192(const char *msg, int msg_bytes, char *out_buf);
int spacc_decrypt_aes_ofb_256(const char *msg, int msg_bytes, char *out_buf);

/* 3DES */
int spacc_3des_set_iv_key(const char iv[16], const char *key[16]);
int spacc_encrypt_3des_cbc(const char *msg, int msg_bytes, char *out_buf);
int spacc_encrypt_3des_ecb(const char *msg, int msg_bytes, char *out_buf);
int spacc_decrypt_3des_cbc(const char *msg, int msg_bytes, char *out_buf);
int spacc_decrypt_3des_ecb(const char *msg, int msg_bytes, char *out_buf);

/* DES */
int spacc_des_set_iv_key(const char iv[16], const char *key[16]);
int spacc_encrypt_des_cbc(const char *msg, int msg_bytes, char *out_buf);
int spacc_encrypt_des_ecb(const char *msg, int msg_bytes, char *out_buf);
int spacc_decrypt_des_cbc(const char *msg, int msg_bytes, char *out_buf);
int spacc_decrypt_des_ecb(const char *msg, int msg_bytes, char *out_buf);

uint32_t trng_rand(void);

#ifdef __cplusplus
}
#endif

#endif /* __ELPCRYPTO_API_ */



