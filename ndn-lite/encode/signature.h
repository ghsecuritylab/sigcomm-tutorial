/*
 * Copyright (C) 2018-2019 Zhiyi Zhang, Tianyuan Yu
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NDN_ENCODING_SIGNATURE_H
#define NDN_ENCODING_SIGNATURE_H

#include "name.h"
#include "../ndn-constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The structure to represent the signature validity period.
 */
typedef struct ndn_validity_period {
  /**
   * Signature is valid not before.
   */
  uint8_t not_before[15];
  /**
   * Signature is valid not after.
   */
  uint8_t not_after[15];
} ndn_validity_period_t;

/**
 * The structure to represent the Signature. We don't support key digest as
 * KeyLocator in ndn-lite.
 */
typedef struct ndn_signature {
  /**
   * Signature type.
   */
  uint8_t sig_type;
  /**
   * The buffer which holds signature value.
   */
  uint8_t sig_value[NDN_SIGNATURE_BUFFER_SIZE];
  /**
   * Signature value size.
   */
  uint32_t sig_size;
  /**
   * Key locator of the signing key.
   */
  ndn_name_t key_locator_name;
  /**
   * The signature info nonce. Used when enable_SignatureInfoNonce > 0.
   */
  uint8_t enable_SignatureInfoNonce;
  uint32_t signature_info_nonce;
  /**
   * The signature timestamp. Used when enable_Timestamp > 0.
   */
  uint8_t enable_Timestamp;
  uint64_t timestamp;
  /**
   * Signature validity period.
   */
  ndn_validity_period_t validity_period;

  uint8_t enable_KeyLocator;
  uint8_t enable_ValidityPeriod;
} ndn_signature_t;

/**
 * Init a Signature structure. set signature type, signature size,
 * and disable keylocator and validity period by default.
 * @param signature. Output. The Signature structure to be inited.
 * @param type. Input. The signature type.
 * @return 0 if there is no error.
 */
static inline int
ndn_signature_init(ndn_signature_t* signature)
{
  signature->enable_KeyLocator = 0;
  signature->enable_ValidityPeriod = 0;
  signature->enable_SignatureInfoNonce = 0;
  signature->signature_info_nonce = 0;
  signature->enable_Timestamp = 0;
  signature->timestamp = 0;
  return 0;
}

static inline int
ndn_signature_set_signature_type(ndn_signature_t* signature, uint8_t type)
{
  switch (type) {
  case NDN_SIG_TYPE_DIGEST_SHA256:
    signature->sig_size = NDN_SEC_SHA256_HASH_SIZE;
    break;
  case NDN_SIG_TYPE_ECDSA_SHA256:
    signature->sig_size = NDN_ASN1_ECDSA_MAX_ENCODED_SIG_SIZE;
    break;
  case NDN_SIG_TYPE_HMAC_SHA256:
    signature->sig_size = NDN_SEC_SHA256_HASH_SIZE;
    break;
  default:
    return NDN_SEC_UNSUPPORT_SIGN_TYPE;
  }
  signature->sig_type = type;
  return 0;
}

/**
 * Set signature value of the Signature. This function will do memory copy.
 * @param signature. Output. The Signature whose signature value will be set.
 * @param sig_value. Input. The buffer which holds the input signature value.
 * @param sig_size. Input. Size of input buffer.
 * @return 0 if there is no error.
 */
static inline int
ndn_signature_set_signature(ndn_signature_t* signature, const uint8_t* sig_value, size_t sig_size)
{
  if (sig_size > NDN_SIGNATURE_BUFFER_SIZE)
    return NDN_OVERSIZE;

  if (signature->sig_type == NDN_SIG_TYPE_ECDSA_SHA256 && sig_size != 64)
    return NDN_SEC_WRONG_SIG_SIZE;

  if (signature->sig_type == NDN_SIG_TYPE_HMAC_SHA256 && sig_size != 32)
    return NDN_SEC_WRONG_SIG_SIZE;

  if (signature->sig_type == NDN_SIG_TYPE_DIGEST_SHA256 && sig_size != 32)
    return NDN_SEC_WRONG_SIG_SIZE;

  signature->sig_size = sig_size;
  memcpy(signature->sig_value, sig_value, sig_size);
  return 0;
}

/**
 * Set keylocator of the Signature. This function will do memory copy.
 * @note This function is NOT recommended. Better to first init signature and
 *       init signature.keylocator_name and set enable_KeyLocator = 1.
 * @param signature. Output. The Signature whose keylocator will be set.
 * @param key_name. Input. The input keylocator.
 */
static inline void
ndn_signature_set_key_locator(ndn_signature_t* signature, const ndn_name_t* key_name)
{
  signature->enable_KeyLocator = 1;
  memcpy(&signature->key_locator_name, key_name, sizeof(ndn_name_t));
}

/**
 * Set Timestamp of the Signature.
 * A Timestamp should be set when using a signed Interest.
 * @param signature. Output. The Signature whose Timestamp will be set.
 * @param timestamp. Input. Timestamp value.
 */
static inline void
ndn_signature_set_timestamp(ndn_signature_t* signature, uint64_t timestamp)
{
  signature->enable_Timestamp = 1;
  signature->timestamp = timestamp;
}

/**
 * Set Nonce of the Signed Interest.
 * @param interest. Output. The Interest whose Nonce will be set.
 * @param nonce. Input. Nonce value.
 */
static inline void
ndn_signature_set_signature_info_nonce(ndn_signature_t* signature, uint32_t nonce)
{
  signature->enable_SignatureInfoNonce = 1;
  signature->signature_info_nonce = nonce;
}

/**
 * Set validity period of the Signature. This function will do memory copy.
 * not before and not after must be ISO 8601 time format, which is 15 bytes long.
 * @param signature. Output. The Signature whose validity period will be set.
 * @param not_before. Input. The input not_before time.
 * @param not_after. Input. The input not_after time.
 */
static inline void
ndn_signature_set_validity_period(ndn_signature_t* signature,
                                  const uint8_t* not_before, const uint8_t* not_after)
{
  signature->enable_ValidityPeriod = 1;
  memcpy(signature->validity_period.not_before, not_before, 15);
  memcpy(signature->validity_period.not_after, not_after, 15);
}

/**
 * Probe the size of a Signature info TLV block before encoding it from a Signature structure.
 * This function is used to check whether the output buffer size is enough or not.
 * @param signature. Input. The signature structure to be probed.
 * @return the length of the expected Signature info TLV block.
 */
static inline uint32_t
ndn_signature_info_probe_block_size(const ndn_signature_t* signature)
{
  // signature type
  uint32_t info_buffer_size = encoder_probe_block_size(TLV_SignatureType, 1);

  if (signature->enable_KeyLocator > 0) {
    uint32_t key_name_block_size = ndn_name_probe_block_size(&signature->key_locator_name);
    info_buffer_size += encoder_probe_block_size(TLV_KeyLocator, key_name_block_size);
  }
  if (signature->enable_ValidityPeriod > 0) {
    uint32_t validity_period_buffer_size = encoder_probe_block_size(TLV_NotBefore, 15);
    validity_period_buffer_size += encoder_probe_block_size(TLV_NotAfter, 15);
    info_buffer_size += encoder_probe_block_size(TLV_ValidityPeriod, validity_period_buffer_size);
  }
  if (signature->enable_SignatureInfoNonce > 0) {
    info_buffer_size += encoder_probe_block_size(TLV_Nonce, 4);
  }
  if (signature->enable_Timestamp > 0) {
    info_buffer_size += encoder_probe_block_size(TLV_SignedInterestTimestamp,
                                                 encoder_probe_uint_length(signature->timestamp));
  }
  return encoder_probe_block_size(TLV_SignatureInfo, info_buffer_size);
}

/**
 * Probe the size of a Signature value TLV block before encoding it from a Signature structure.
 * This function is used to check whether the output buffer size is enough or not.
 * @param signature. Input. The signature structure to be probed.
 * @return the length of the expected Signature value TLV block.
 */
static inline uint32_t
ndn_signature_value_probe_block_size(const ndn_signature_t* signature)
{
  return encoder_probe_block_size(TLV_SignatureValue, signature->sig_size);
}

/**
 * Encode the Signature info into wire format (TLV block) from Signature structure.
 * @param encoder. Output. The encoder who keeps the encoding result and the state.
 * @param signature. Input. The Signature structure whose signature info to be encoded.
 * @return 0 if there is no error.
 */
int
ndn_signature_info_tlv_encode(ndn_encoder_t* encoder, const ndn_signature_t* signature);

/**
 * Encode the Signature value into wire format (TLV block) from Signature structure.
 * @param encoder. Output. The encoder who keeps the encoding result and the state.
 * @param signature. Input. The Signature structure whose signature value to be encoded.
 * @return 0 if there is no error.
 */
int
ndn_signature_value_tlv_encode(ndn_encoder_t* encoder, const ndn_signature_t* signature);

/**
 * Decode an Signature info TLV block into an Signature structure. This function will do memory copy.
 * @param decoder. Input. The decoder who keeps the decoding result and the state.
 * @param signature. Output. The Signature structure whose signature info to be decoded.
 * @return 0 if decoding is successful.
 */
int
ndn_signature_info_tlv_decode(ndn_decoder_t* decoder, ndn_signature_t* signature);

/**
 * Decode an Signature value TLV block into an Signature structure. This function will do memory copy.
 * @param decoder. Input. The decoder who keeps the decoding result and the state.
 * @param signature. Output. The Signature structure whose signature value to be decoded.
 * @return 0 if decoding is successful.
 */
int
ndn_signature_value_tlv_decode(ndn_decoder_t* decoder, ndn_signature_t* signature);

#ifdef __cplusplus
}
#endif

#endif // NDN_ENCODING_SIGNATURE_H
