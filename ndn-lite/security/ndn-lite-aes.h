/*
 * Copyright (C) 2018-2019 Tianyuan Yu, Zhiyi Zhang
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NDN_SECURITY_AES_H_
#define NDN_SECURITY_AES_H_

#include "../ndn-error-code.h"
#include "ndn-lite-sec-config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The opaque abstract aes key struct to be implemented by the backend.
 */
typedef struct abstract_aes_key abstract_aes_key_t;

/**
 * The APIs that are supposed to be implemented by the backend.
 */
typedef uint32_t (*ndn_aes_get_key_size_impl)(const abstract_aes_key_t* aes_key);
typedef const uint8_t* (*ndn_aes_get_key_value_impl)(const abstract_aes_key_t* aes_key);
typedef int (*ndn_aes_load_key_impl)(abstract_aes_key_t* aes_key,
                                     const uint8_t* key_value, uint32_t key_size);
typedef int (*ndn_aes_cbc_encrypt_impl)(const uint8_t* input_value, uint8_t input_size,
                                        uint8_t* output_value, uint8_t output_size,
                                        const uint8_t* aes_iv, const abstract_aes_key_t* aes_key);
typedef int (*ndn_aes_cbc_decrypt_impl)(const uint8_t* input_value, uint8_t input_size,
                                        uint8_t* output_value, uint8_t output_size,
                                        const uint8_t* aes_iv, const abstract_aes_key_t* aes_key);

/**
 * The structure to represent the backend implementation.
 */
typedef struct ndn_aes_backend {
  ndn_aes_get_key_size_impl get_key_size;
  ndn_aes_get_key_value_impl get_key_value;
  ndn_aes_load_key_impl load_key;
  ndn_aes_cbc_encrypt_impl cbc_encrypt;
  ndn_aes_cbc_decrypt_impl cbc_decrypt;
} ndn_aes_backend_t;

/**
 * The structure to keep an AES-128 key.
 */
typedef struct ndn_aes_key {
  abstract_aes_key_t abs_key;
  /**
   * The KEY ID of current key. Should be unique.
   */
  uint32_t key_id;
} ndn_aes_key_t;

ndn_aes_backend_t*
ndn_aes_get_backend(void);

/**
 * Get aes key size in unit of byte.
 * @param aes_key. Input. NDN aes key.
 */
uint32_t
ndn_aes_get_key_size(const ndn_aes_key_t* aes_key);

/**
 * Get aes key bytes.
 * @param aes_key. Input. NDN aes key.
 */
const uint8_t*
ndn_aes_get_key_value(const ndn_aes_key_t* aes_key);

/**
 * Load in-memory key bits into an NDN aes key.
 * @param aes_key. Output. NDN aes key.
 * @param key_value. Input. Key bytes.
 * @param key_size. Input. The size of the key bytes.
 */
int
ndn_aes_load_key(ndn_aes_key_t* aes_key,
                 const uint8_t* key_value, uint32_t key_size);

/**
 * Initialize an AES-128 key.
 * @param key. Input. The HMAC key whose info will be set.
 * @param key_value. Input. The key value bytes to set.
 * @param key_size. Input. The key size. Should not larger than 32 bytes.
 * @param key_id. Input. The key id to be set with this key.
 * @return 0 if there is no error.
 */
static inline int
ndn_aes_key_init(ndn_aes_key_t* key, const uint8_t* key_value,
                 uint32_t key_size, uint32_t key_id)
{
  ndn_aes_load_key(key, key_value, key_size);
  key->key_id = key_id;
  return 0;
}

/**
 * Use AES-128-CBC algorithm to encrypt a buffer. This function does not perform any padding.
 * The input_size must be a multiple of NDN_AES_BLOCK_SIZE to obtain a successful encryption.
 * @param input_value. Input. Buffer to encrypt.
 * @param input_size. Input. Size of input buffer.
 * @param output_value. Output. Encrypted buffer.
 * @param output_size. Input. Size of encrypted buffer.
 * @param aes_iv. Input. AES Initialization Vector, whose length should be NDN_AES_BLOCK_SIZE.
 * @param key_value. Input. AES-128 key to perform encryption.
 * @param key_size. Input. Size of used AES-128 key.
 * @return NDN_SUCCESS if there is no error.
 */
int
ndn_aes_cbc_encrypt(const uint8_t* input_value, uint8_t input_size,
                    uint8_t* output_value, uint8_t output_size,
                    const uint8_t* aes_iv, const ndn_aes_key_t* aes_key);

/**
 * Use AES-128-CBC algorithm to decrypt an encrypted buffer. This function is implemented without padding.
 * The input_size must be a multiple of NDN_AES_BLOCK_SIZE to obtain a successful decryption.
 * @param input_value. Input. Buffer to decrypt.
 * @param input_size. Input. Size of input buffer.
 * @param output_value. Output. Decrypted buffer.
 * @param output_size. Input. Size of decrypted buffer.
 * @param aes_iv. Input. AES Initialization Vector, whose length should be NDN_AES_BLOCK_SIZE.
 * @param key_value. Input. AES-128 key to perform decryption. Should be same as encryption key.
 * @param key_size. Input. Size of used AES-128 key.
 * @return NDN_SUCCESS if there is no error.
 */
int
ndn_aes_cbc_decrypt(const uint8_t* input_value, uint8_t input_size,
                    uint8_t* output_value, uint8_t output_size,
                    const uint8_t* aes_iv, const ndn_aes_key_t* aes_key);

#ifdef __cplusplus
}
#endif

#endif // NDN_SECURITY_AES_H_
