/*
 * Copyright (C) 2018-2019 Tianyuan Yu, Zhiyi Zhang
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "ndn-lite-aes.h"

ndn_aes_backend_t ndn_aes_backend;

ndn_aes_backend_t*
ndn_aes_get_backend(void)
{
  return &ndn_aes_backend;
}

uint32_t
ndn_aes_get_key_size(const ndn_aes_key_t* aes_key)
{
  return ndn_aes_backend.get_key_size(&aes_key->abs_key);
}

const uint8_t*
ndn_aes_get_key_value(const ndn_aes_key_t* aes_key)
{
  return ndn_aes_backend.get_key_value(&aes_key->abs_key);
}

int
ndn_aes_load_key(ndn_aes_key_t* aes_key,
                 const uint8_t* key_value, uint32_t key_size)
{
  return ndn_aes_backend.load_key(&aes_key->abs_key, key_value, key_size);
}

int
ndn_aes_cbc_encrypt(const uint8_t* input_value, uint8_t input_size,
                    uint8_t* output_value, uint8_t output_size,
                    const uint8_t* aes_iv, const ndn_aes_key_t* aes_key)
{
  return ndn_aes_backend.cbc_encrypt(input_value, input_size,
                                     output_value, output_size,
                                     aes_iv, &aes_key->abs_key);
}

int
ndn_aes_cbc_decrypt(const uint8_t* input_value, uint8_t input_size,
                    uint8_t* output_value, uint8_t output_size,
                    const uint8_t* aes_iv, const ndn_aes_key_t* aes_key)
{
  return ndn_aes_backend.cbc_decrypt(input_value, input_size,
                                     output_value, output_size,
                                     aes_iv, &aes_key->abs_key);
}
