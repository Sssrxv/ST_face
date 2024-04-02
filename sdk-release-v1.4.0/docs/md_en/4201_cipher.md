# hardware cipher algorithm engine

## brief introduction

​    hardware cipher algorithm engine, supports AES, 3DES, DES cipher algorithm.

## feature

- support AES(CBC, ECB, CFB, OFB, CTR) algorithm
- support 3DES(CBC, ECB) algorithm
- support DES(CBC, ECB) algorithm


## API reference

header file cipher.h

​function list:

- cipher_init
- aes_cbc128_encrypt
- aes_cbc128_decrypt
- aes_cbc192_encrypt
- aes_cbc192_decrypt
- aes_cbc256_encrypt
- aes_cbc256_decrypt
- aes_ecb128_encrypt
- aes_ecb128_decrypt
- aes_ecb192_encrypt
- aes_ecb192_decrypt
- aes_ecb256_encrypt
- aes_ecb256_decrypt
- aes_cfb128_encrypt
- aes_cfb128_decrypt
- aes_cfb192_encrypt
- aes_cfb192_decrypt
- aes_cfb256_encrypt
- aes_cfb256_decrypt
- aes_ofb128_encrypt
- aes_ofb128_decrypt
- aes_ofb192_encrypt
- aes_ofb192_decrypt
- aes_ofb256_encrypt
- aes_ofb256_decrypt
- aes_ctr128_encrypt
- aes_ctr128_decrypt
- aes_ctr192_encrypt
- aes_ctr192_decrypt
- aes_ctr256_encrypt
- aes_ctr256_decrypt
- _3des_cbc_encrypt
- _3des_cbc_decrypt
- _3des_ecb_encrypt
- _3des_ecb_decrypt
- des_cbc_encrypt
- des_cbc_decrypt
- des_ecb_encrypt
- des_ecb_decrypt


### cipher_init

#### description

   initialize cipher algorithm engine

#### prototype

`int cipher_init(void)`

#### parameter

   no parameter

#### return value

   0 - success, other - fail

   for detail please refer to the [return value definetion](#return_value_definetion)

### aes_cbc128_encrypt

#### description

   run aes_cbc_128 encryption algorithm

#### prototype

`int aes_cbc128_encrypt(uint8_t *initial_vector, uint8_t *input_key, uint8_t *input_data, uint32_t input_len, uint8_t *output_data)`

#### parameter

| parameter   | description              | direction |
| ----------- | ------------------------ | --------- |
| iv          | initialization vector    | input     |
| key         | key for cipher algorithm | input     |
| input_data  | pointer of input data    | input     |
| input_len   | size of input data       | input     |
| output_data | pointer of output data   | output    |

#### return value

   0 - success, other - fail

   for detail please refer to the [return value definetion](#return_value_definetion)

#### attention  

   the size of output_data buf must have enough space to save the output data

## data type definition

### return value definetion
<div id="return_value_definetion"></div>

```c
#define CIPHER_OK                               (   0)
#define CIPHER_FAILED                           (  -1)
#define CIPHER_INPROGRESS                       (  -2)
#define CIPHER_INVALID_HANDLE                   (  -3)
#define CIPHER_INVALID_CONTEXT                  (  -4)
#define CIPHER_INVALID_SIZE                     (  -5)
#define CIPHER_NOT_INITIALIZED                  (  -6)
#define CIPHER_NO_MEM                           (  -7)
#define CIPHER_INVALID_ALG                      (  -8)
#define CIPHER_INVALID_KEY_SIZE                 (  -9)
#define CIPHER_INVALID_ARGUMENT                 ( -10)
#define CIPHER_MODULE_DISABLED                  ( -11)
#define CIPHER_NOT_IMPLEMENTED                  ( -12)
#define CIPHER_INVALID_BLOCK_ALIGNMENT          ( -13)
#define CIPHER_INVALID_MODE                     ( -14)
#define CIPHER_INVALID_KEY                      ( -15)
#define CIPHER_AUTHENTICATION_FAILED            ( -16)
#define CIPHER_INVALID_IV_SIZE                  ( -17)
#define CIPHER_MEMORY_ERROR                     ( -18)
#define CIPHER_LAST_ERROR                       ( -19)
#define CIPHER_HALTED                           ( -20)
#define CIPHER_TIMEOUT                          ( -21)
#define CIPHER_SRM_FAILED                       ( -22)
#define CIPHER_COMMON_ERROR_MAX                 (-100)
#define CIPHER_INVALID_ICV_KEY_SIZE             (-100)
#define CIPHER_INVALID_PARAMETER_SIZE           (-101)
#define CIPHER_SEQUENCE_OVERFLOW                (-102)
#define CIPHER_DISABLED                         (-103)
#define CIPHER_INVALID_VERSION                  (-104)
#define CIPHER_FATAL                            (-105)
#define CIPHER_INVALID_PAD                      (-106)
#define CIPHER_FIFO_FULL                        (-107)
#define CIPHER_INVALID_SEQUENCE                 (-108)
#define CIPHER_INVALID_FIRMWARE                 (-109)
#define CIPHER_NOT_FOUND                        (-110)
#define CIPHER_CMD_FIFO_INACTIVE                (-111)
```
# hardware cipher algorithm engine

## brief introduction

​    hardware cipher algorithm engine, supports AES, 3DES, DES cipher algorithm.

## feature

- support AES(CBC, ECB, CFB, OFB, CTR) algorithm
- support 3DES(CBC, ECB) algorithm
- support DES(CBC, ECB) algorithm


## API reference

header file cipher.h

​function list:

- cipher_init
- cipher


### cipher_init

#### description

   initialize cipher algorithm engine

#### prototype

`int cipher_init(void)`

#### parameter

   no parameter

#### return value

   0 - success, other - fail

   for detail please refer to the [return value definetion](#return_value_definetion)

### cipher

#### description

   run cipher encryption or decryption algorithm

#### prototype

`int cipher(cipher_mode_t mode, int is_encryption, uint8_t *iv, uint8_t *key, uint8_t *input_data, uint32_t input_len, uint8_t *output_data)`

#### parameter

| parameter   | description              | direction |
| ----------- | ------------------------ | --------- |
| mode        | cipher mode              | input     |
| is_encryption| encryption or decryption |input     |
| iv          | initialization vector    | input     |
| key         | key for cipher algorithm | input     |
| input_data  | pointer of input data    | input     |
| input_len   | size of input data       | input     |
| output_data | pointer of output data   | output    |

#### return value

   0 - success, other - fail

   for detail please refer to the [return value definetion](#return_value_definetion)

#### note

   AES_ECB mode doesn't need an initialization vector, so the iv param is ignored and can be NULL

#### attention  

   the size of output_data buf must have enough space to save the output data

## data type definition

<div id="return_value_definetion"></div>

### return value

#### description

   return value of cipher api

#### definition

```c
#define CIPHER_OK                               (   0)
#define CIPHER_FAILED                           (  -1)
#define CIPHER_INPROGRESS                       (  -2)
#define CIPHER_INVALID_HANDLE                   (  -3)
#define CIPHER_INVALID_CONTEXT                  (  -4)
#define CIPHER_INVALID_SIZE                     (  -5)
#define CIPHER_NOT_INITIALIZED                  (  -6)
#define CIPHER_NO_MEM                           (  -7)
#define CIPHER_INVALID_ALG                      (  -8)
#define CIPHER_INVALID_KEY_SIZE                 (  -9)
#define CIPHER_INVALID_ARGUMENT                 ( -10)
#define CIPHER_MODULE_DISABLED                  ( -11)
#define CIPHER_NOT_IMPLEMENTED                  ( -12)
#define CIPHER_INVALID_BLOCK_ALIGNMENT          ( -13)
#define CIPHER_INVALID_MODE                     ( -14)
#define CIPHER_INVALID_KEY                      ( -15)
#define CIPHER_AUTHENTICATION_FAILED            ( -16)
#define CIPHER_INVALID_IV_SIZE                  ( -17)
#define CIPHER_MEMORY_ERROR                     ( -18)
#define CIPHER_LAST_ERROR                       ( -19)
#define CIPHER_HALTED                           ( -20)
#define CIPHER_TIMEOUT                          ( -21)
#define CIPHER_SRM_FAILED                       ( -22)
#define CIPHER_COMMON_ERROR_MAX                 (-100)
#define CIPHER_INVALID_ICV_KEY_SIZE             (-100)
#define CIPHER_INVALID_PARAMETER_SIZE           (-101)
#define CIPHER_SEQUENCE_OVERFLOW                (-102)
#define CIPHER_DISABLED                         (-103)
#define CIPHER_INVALID_VERSION                  (-104)
#define CIPHER_FATAL                            (-105)
#define CIPHER_INVALID_PAD                      (-106)
#define CIPHER_FIFO_FULL                        (-107)
#define CIPHER_INVALID_SEQUENCE                 (-108)
#define CIPHER_INVALID_FIRMWARE                 (-109)
#define CIPHER_NOT_FOUND                        (-110)
#define CIPHER_CMD_FIFO_INACTIVE                (-111)
```

### cipher_mode_t

#### description

   cipher mode input when run cipher algorithm

#### definition

```c
typedef enum {
    CIPHER_AES_CBC_128 = 0,
    CIPHER_AES_CBC_192,
    CIPHER_AES_CBC_256,
    CIPHER_AES_ECB_128,
    CIPHER_AES_ECB_192,
    CIPHER_AES_ECB_256,
    CIPHER_AES_CFB_128,
    CIPHER_AES_CFB_192,
    CIPHER_AES_CFB_256,
    CIPHER_AES_OFB_128,
    CIPHER_AES_OFB_192,
    CIPHER_AES_OFB_256,
    CIPHER_AES_CTR_128,
    CIPHER_AES_CTR_192,
    CIPHER_AES_CTR_256,
    CIPHER_3DES_CBC,
    CIPHER_3DES_ECB,
    CIPHER_DES_CBC,
    CIPHER_DES_ECB,
    CIPHER_MODE_MAX
} cipher_mode_t;
```

## example

### run AES_CBC_128 cipher algorithm

```c
// prepare data
int ret;
cipher_mode_t mode = CIPHER_AES_CBC_128;
int is_encryption;
uint8_t iv[16]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
uint8_t key[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
uint8_t origin_data[32]  = { 0 };
uint8_t encrypt_data[32] = { 0 };
uint8_t decrypt_data[32] = { 0 };
// srand(time(NULL));
for (uint32_t i = 0; i < sizeof origin_data; ++i) {
   origin_data[i] = rand();
}

// initialize cipher engine
ret = cipher_init();
ASSERT(ret == CIPHER_OK);


// encryption
is_encryption = 1;
ret = cipher(mode, is_encryption, iv, key, 
         origin_data, sizeof origin_data, encrypt_data);
ASSERT(ret == CIPHER_OK);


// decryption
is_encryption = 0;
ret = cipher(mode, is_encryption, iv, key, 
         encrypt_data, sizeof encrypt_data, decrypt_data);
ASSERT(ret == CIPHER_OK);


// compare decrtpted data with origin data
ASSERT(0 == memcmp(decrypt_data, origin_data, sizeof origin_data));

```

