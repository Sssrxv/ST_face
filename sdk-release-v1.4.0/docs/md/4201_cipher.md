# 硬件加解密算法引擎一

## 概述

​    硬件加密算法引擎, 支持AES, 3DES, DES 加密算法；

## 特性

- 支持AES(CBC, ECB, CFB, OFB, CTR) 算法
- 支持3DES(CBC, ECB)算法
- 支持DES(CBC, ECB)算法


## API 参考

   对应头文件 cipher.h

   为用户提供以下接口

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
<div id="cipher_init"></div>

#### 描述

    初始化加密算法引擎

#### 定义
```c
    int cipher_init(void)
```
#### 参数

    无

#### [返回值](#返回值定义)

    0 - 成功, 其他 - 失败

### aes_cbc128_encrypt
<div id="cipher_init"></div>

#### 描述

    使用AES128加密引擎进行加密

#### 定义
```c
    int aes_cbc128_encrypt(uint8_t *initial_vector, uint8_t *input_key,
         uint8_t *input_data, uint32_t input_len, uint8_t *output_data)
```
#### 参数

| 参数        | 描述                     |  方向     |
| ----------- | ------------------------ | ---------|
| iv          |  初始化向量              | 输入     |
| key         | 加密算法密钥             | 输入     |
| input_data  | 待数据的数据             | 输入     |
| input_len   | 输入数据的大小           | 输入     |
| output_data | 输出数据的缓冲区         | 输出     |

#### [返回值](#返回值定义)

    0 - 成功, 其他 - 失败

#### 备注

    输出数据的缓冲区必须有足够的空间保存数据；

## 数据类型定义

### 返回值定义
<div id="返回值定义"></div>

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

# 硬件加解密算法引擎二

## 概述

    硬件加密算法引擎, 支持AES, 3DES, DES 加密算法；

## 特性

- 支持AES(CBC, ECB, CFB, OFB, CTR) 算法
- 支持3DES(CBC, ECB)算法
- 支持DES(CBC, ECB)算法


## API 参考

    对应头文件 cipher.h

    为用户提供以下接口

- [cipher_init](#cipher_init)
- [cipher](#cipher)


### cipher_init
<div id="cipher_init"></div>

#### 描述

    初始化加密算法引擎

#### 定义
```c
    int cipher_init(void)
```
#### 参数

    无

#### [返回值](#返回值定义)

    0 - 成功, 其他 - 失败

### cipher
<div id="cipher"></div>

#### 描述

    使用AES128加密引擎进行加解密

#### 定义
```c
    int cipher(cipher_mode_t mode, int is_encryption, uint8_t *iv, uint8_t *key,
               uint8_t *input_data, uint32_t input_len, uint8_t *output_data)
```
#### 参数

| 参数        | 描述                     |  方向     |
| ----------- | ------------------------ | --------- |
| mode        | 加密模式                 | input     |
| is_encryption| 加密还是解密             |input     |
| iv          |  初始化向量              | 输入     |
| key         | 加密算法密钥             | 输入     |
| input_data  | 待数据的数据             | 输入     |
| input_len   | 输入数据的大小           | 输入     |
| output_data | 输出数据的缓冲区         | 输出     |

#### [返回值](#返回值定义)

    0 - 成功, 其他 - 失败

#### 注意

    AES_ECB模式不要初始向量，因此输入向量参数将被忽略并且是NULL；

#### 备注

    输出数据的缓冲区必须有足够的空间保存数据；

## 数据类型定义

### 返回值定义
<div id="返回值定义"></div>

#### 描述

    加解密API的返回值

#### 定义

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
<div id="cipher_mode_t"></div>

#### 描述

    运行加密算法的输入模式

#### 定义

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

### 使用AES_CBC_128加密算法

```c
// 准备数据
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

// 初始化加密算法引擎
ret = cipher_init();
ASSERT(ret == CIPHER_OK);

// 加密
is_encryption = 1;
ret = cipher(mode, is_encryption, iv, key, 
         origin_data, sizeof origin_data, encrypt_data);
ASSERT(ret == CIPHER_OK);

// 解密
is_encryption = 0;
ret = cipher(mode, is_encryption, iv, key, 
         encrypt_data, sizeof encrypt_data, decrypt_data);
ASSERT(ret == CIPHER_OK);

// 比较解密后的数据与原始数据
ASSERT(0 == memcmp(decrypt_data, origin_data, sizeof origin_data));
```


