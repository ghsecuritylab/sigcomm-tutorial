/*
 * Copyright (C) 2018-2019 Zhiyi Zhang, Tianyuan Yu, Edward Lu
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NDN_LITE_DEFAULT_HMAC_IMPL_H
#define NDN_LITE_DEFAULT_HMAC_IMPL_H

#include <stddef.h>
#include <stdint.h>

struct abstract_hmac_key {
  /**
   * The key bytes buffer of current key.
   */
  uint8_t key_value[32];
  /**
   * The key size of key bytes.
   */
  uint32_t key_size;
};

void
ndn_lite_default_hmac_load_backend(void);

#endif // NDN_LITE_DEFAULT_HMAC_IMPL_H
