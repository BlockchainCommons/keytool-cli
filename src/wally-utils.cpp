#include "wally-utils.hpp"
#include "wally.hpp"

#include <iostream>
#include <algorithm>
#include "model.hpp"

using namespace std;

Wally Wally::instance;

Wally::Wally() {
    wally_init(0);
    auto buf = random_bytes(WALLY_SECP_RANDOMIZE_LEN);
    wally_secp_randomize(&buf[0], buf.size());
}

Wally::~Wally() {
    wally_cleanup(0);
}

bool Wally::bip39_is_entropy_length_valid(size_t len) const {
    vector<size_t> valid_lengths = {BIP39_ENTROPY_LEN_128, BIP39_ENTROPY_LEN_160, BIP39_ENTROPY_LEN_192, BIP39_ENTROPY_LEN_224, BIP39_ENTROPY_LEN_256, BIP39_ENTROPY_LEN_288, BIP39_ENTROPY_LEN_320};
    return any_of(valid_lengths.begin(), valid_lengths.end(), [&](size_t l) { return l == len; });
}

string Wally::bip39_mnemonic_from_entropy(const ByteVector& entropy) const {
    if(!bip39_is_entropy_length_valid(entropy.size())) {
        throw BIP39InvalidEntropyLength();
    }

    char* mnemonic;
    bip39_mnemonic_from_bytes(NULL, &entropy[0], entropy.size(), &mnemonic);
    auto result = string(mnemonic);
    wally_free_string(mnemonic);
    return result;
}

ByteVector Wally::bip39_mnemonic_to_entropy(const string& mnemonic) const {
    ByteVector result(500);
    size_t written = 0;
    bip39_mnemonic_to_bytes(NULL, mnemonic.c_str(), &result[0], result.size(), &written);
    result.resize(written);
    return result;
}

ByteVector Wally::bip39_mnemonic_to_bip32_seed(const string& mnemonic) const {
    ByteVector result(BIP39_SEED_LEN_512);
    size_t written = 0;
    bip39_mnemonic_to_seed(mnemonic.c_str(), NULL, &result[0], result.size(), &written);
    result.resize(written);
    return result;
}

bool Wally::bip32_is_seed_length_valid(size_t len) const {
    vector<size_t> valid_lengths = {BIP32_ENTROPY_LEN_128, BIP32_ENTROPY_LEN_256, BIP32_ENTROPY_LEN_512};
    return any_of(valid_lengths.begin(), valid_lengths.end(), [&](size_t l) { return l == len; });
}

HDKey Wally::bip32_key_from_seed(const ByteVector& seed, const Network& network) const {
    if(!bip32_is_seed_length_valid(seed.size())) {
        throw HDKeyInvalidSeedLength();
    }

    ext_key* key = NULL;

    uint32_t version;
    if(network == Network::mainnet()) {
        version = BIP32_VER_MAIN_PRIVATE;
    } else if(network == Network::testnet()) {
        version = BIP32_VER_TEST_PRIVATE;
    } else {
        assert(false);
    }
    if(bip32_key_from_seed_alloc(&seed[0], seed.size(), version, BIP32_FLAG_SKIP_HASH, &key) != WALLY_OK) {
        throw HDKeyInvalidSeed();
    }

    return HDKey(key);
}

HDKey Wally::bip32_key_from_base58(const string& base58) const {
    ext_key* key = NULL;
    if(::
    bip32_key_from_base58_alloc(base58.c_str(), &key) != WALLY_OK) {
        throw HDKeyInvalidKey();
    }
    return HDKey(key);
}


HDKey Wally::bip32_key_from_entropy(const ByteVector& entropy, const Network& network) const {
    auto mnemonic = bip39_mnemonic_from_entropy(entropy);
    auto seed = bip39_mnemonic_to_bip32_seed(mnemonic);
    return bip32_key_from_seed(seed, network);
}

uint32_t Wally::flags_for_private(bool is_private) {
    return is_private ? BIP32_FLAG_KEY_PRIVATE : BIP32_FLAG_KEY_PUBLIC;
}

HDKey Wally::bip32_key_from_parent(const HDKey& parent, uint32_t index, bool is_hardened, bool is_private) const {
    if(!parent.is_private()) {
        if(is_private) {
            throw domain_error("Cannot derive a private key from a public key.");
        } else if(is_hardened) {
            throw domain_error("Cannot perform a hardened derivation on a public key.");
        }
    }
    ext_key* out_key = NULL;
    if(is_hardened) {
        index += BIP32_INITIAL_HARDENED_CHILD;
    }
    assert(bip32_key_from_parent_alloc(parent._key.get(), index, flags_for_private(is_private), &out_key) == WALLY_OK);
    return HDKey(out_key);
}

ByteVector Wally::bip32_key_serialize(const HDKey& key, bool is_private) const {
    ByteVector bytes(BIP32_SERIALIZED_LEN);
    assert(::bip32_key_serialize(key._key.get(), flags_for_private(is_private), &bytes[0], bytes.size()) == WALLY_OK);
    return bytes;
}

HDKey Wally::bip32_key_unserialize(const ByteVector& serialized) const {
    ext_key* out_key = NULL;
    assert(::bip32_key_unserialize_alloc(&serialized[0], serialized.size(), &out_key) == WALLY_OK);
    return HDKey(out_key);
}

HDKey Wally::bip32_key_to_public(const HDKey& key) const {
    // It's a shame LibWally doesn't appear to have a call to do this in one step.
    auto pub_serialized = bip32_key_serialize(key, false);
    return bip32_key_unserialize(pub_serialized);
}

ByteVector Wally::bip32_key_get_fingerprint(const HDKey& key) const {
    uint8_t buf[BIP32_KEY_FINGERPRINT_LEN];
    assert(::bip32_key_get_fingerprint(key._key.get(), buf, BIP32_KEY_FINGERPRINT_LEN) == WALLY_OK);
    return ByteVector(buf, buf + BIP32_KEY_FINGERPRINT_LEN);
}

ECPrivateKey Wally::bip32_key_to_ec_private(const HDKey& key) const {
    if(!key.is_private()) {
        throw domain_error("Cannot derive private key from public key.");
    }
    auto a = key._key.get()->priv_key + 1;
    ByteVector k = ByteVector(a, a + EC_PRIVATE_KEY_LEN);
    return ECPrivateKey(k);
}

ECCompressedPublicKey Wally::bip32_key_to_ec_public(const HDKey& key) const {
    auto a = key._key.get()->pub_key;
    ByteVector k = ByteVector(a, a + EC_PUBLIC_KEY_LEN);
    return ECCompressedPublicKey(k);
}

ECCompressedPublicKey Wally::ec_key_to_public(const ECPrivateKey& key) const {
    ByteVector key_out(EC_PUBLIC_KEY_LEN);
    assert(wally_ec_public_key_from_private_key(&key.data()[0], key.data().size(), &key_out[0], key_out.size()) == WALLY_OK);
    return ECCompressedPublicKey(key_out);
}

ECUncompressedPublicKey Wally::ec_key_uncompress(const ECCompressedPublicKey& key) const {
    ByteVector key_out(EC_PUBLIC_KEY_UNCOMPRESSED_LEN);
    assert(wally_ec_public_key_decompress(&key.data()[0], key.data().size(), &key_out[0], key_out.size()) == WALLY_OK);
    return ECUncompressedPublicKey(key_out);
}

ECCompressedPublicKey Wally::ec_key_compress(const ECUncompressedPublicKey& key) const {
    ByteVector key_out(EC_PUBLIC_KEY_LEN);
    assert(wally_ec_public_key_negate(&key.data()[0], key.data().size(), &key_out[0], key_out.size()) == WALLY_OK);
    return ECCompressedPublicKey(key_out);
}

string Wally::ec_key_to_wif(const ECPrivateKey& key, const Network& network) const {
    char* wif_out;
    uint32_t prefix = network == Network::mainnet() ? 0x80 : 0xef;
    assert(wally_wif_from_bytes(&key.data()[0], key.data().size(), prefix, WALLY_WIF_FLAG_COMPRESSED, &wif_out) == WALLY_OK);
    auto result = string(wif_out);
    wally_free_string(wif_out);
    return result;
}

ECPrivateKey Wally::wif_to_ec_key(const string& wif, const Network& network) const {
    uint32_t prefix = network == Network::mainnet() ? 0x80 : 0xef;
    ByteVector bytes_out(EC_PRIVATE_KEY_LEN);
    int ret = 0;
    if((ret = wally_wif_to_bytes(wif.c_str(), prefix, WALLY_WIF_FLAG_COMPRESSED, &bytes_out.front(), bytes_out.size())) != WALLY_OK) {
        throw domain_error("Invalid WIF.");
    }
    return ECPrivateKey(bytes_out);
}

ByteVector Wally::hash160(const ByteVector& bytes) const {
    ByteVector bytes_out(HASH160_LEN);
    assert(wally_hash160(&bytes[0], bytes.size(), &bytes_out[0], bytes_out.size()) == WALLY_OK);
    return bytes_out;
}

string Wally::to_base58(const ByteVector& bytes, bool is_check) const {
    uint32_t flags = is_check ? BASE58_FLAG_CHECKSUM : 0;
    char* output = NULL;
    assert(wally_base58_from_bytes(&bytes[0], bytes.size(), flags, &output) == WALLY_OK);
    auto result = string(output);
    wally_free_string(output);
    return result;
}

string Wally::to_address(const ECCompressedPublicKey& key, uint8_t version) const {
    auto data = key.data();
    auto hashed = hash160(data);
    hashed.insert(hashed.begin(), version);
    return to_base58(hashed, true);
}

string Wally::to_address(const ECCompressedPublicKey& key, const Asset& asset, bool is_sh) const {
    auto version = is_sh ? asset.version_sh() : asset.version_pkh();
    return to_address(key, version);
}

string Wally::to_segwit_address(const HDKey& key, const Network& network) const {
    string address_family;
    if(network == Network::mainnet()) {
        address_family = "bc";
    } else if(network == Network::testnet()) {
        address_family = "tb";
    } else {
        assert(false);
    }

    char* addr_segwit = nullptr;
    assert(wally_bip32_key_to_addr_segwit(key._key.get(), address_family.c_str(), 0, &addr_segwit) == WALLY_OK);
    string result(addr_segwit);
    wally_free_string(addr_segwit);
    return result;
}

wally_psbt* Wally::psbt_from_data(const ByteVector& data) const {
    wally_psbt* output = nullptr;
    if(wally_psbt_from_bytes(&data.front(), data.size(), &output) != WALLY_OK) {
        throw domain_error("Invalid PSBT.");
    }
    return output;
}

wally_psbt* Wally::psbt_from_base64(const string& base64) const {
    wally_psbt* output = nullptr;
    if(wally_psbt_from_base64(base64.c_str(), &output) != WALLY_OK) {
        throw domain_error("Invalid PSBT.");
    }
    return output;
}

bool Wally::psbt_is_finalized(wally_psbt* psbt) const {
    size_t output = 0;
    assert(wally_psbt_is_finalized(psbt, &output) == WALLY_OK);
    return output == 1;
}

ByteVector Wally::psbt_to_data(wally_psbt* psbt) const {
    size_t len = 0;
    assert(wally_psbt_get_length(psbt, 0, &len) == WALLY_OK);
    ByteVector result(len);
    size_t written = 0;
    assert(wally_psbt_to_bytes(psbt, 0, &result.front(), len, &written) == WALLY_OK);
    assert(written == len);
    return result;
}

string Wally::psbt_to_base64(wally_psbt* psbt) const {
    char* output = nullptr;
    assert(wally_psbt_to_base64(psbt, 0, &output) == WALLY_OK);
    string result(output);
    wally_free_string(output);
    return result;
}

// /* Get the script to sign with */
// static bool input_get_scriptcode_2(const struct wally_psbt_input *input,
//                                  uint32_t input_index,
//                                  const unsigned char **script,
//                                  size_t *script_len)
// {
//     const struct wally_tx_output *utxo = NULL;
//     const struct wally_tx_output *out;

//     if (!input || !script || !script_len)
//         return false;

//     *script = NULL;
//     *script_len = 0;

//     if (input->utxo) {
//         if (input_index >= input->utxo->num_outputs)
//             return false; /* Invalid input index */
//         utxo = &input->utxo->outputs[input_index];
//     }

//     out = input->witness_utxo ? input->witness_utxo : utxo;
//     if (!out)
//         return false; /* No prevout to get the script from */

//     if (input->redeem_script) {
//         unsigned char p2sh[WALLY_SCRIPTPUBKEY_P2SH_LEN];
//         size_t p2sh_len;

//         if (wally_scriptpubkey_p2sh_from_bytes(input->redeem_script,
//                                                input->redeem_script_len,
//                                                WALLY_SCRIPT_HASH160,
//                                                p2sh, sizeof(p2sh),
//                                                &p2sh_len) != WALLY_OK)
//             return false;

//         if (out->script_len != p2sh_len || memcmp(p2sh, out->script, p2sh_len))
//             return false; /* Script mismatch */

//         *script = input->redeem_script;
//         *script_len = input->redeem_script_len;
//         return true;
//     }

//     *script = out->script;
//     *script_len = out->script_len;
//     return true;
// }

// static bool is_matching_txid_2(const struct wally_tx *tx,
//                              const unsigned char *txid, size_t txid_len)
// {
//     unsigned char src_txid[WALLY_TXHASH_LEN];
//     bool ret;

//     if (!tx || !txid || txid_len != WALLY_TXHASH_LEN)
//         return false;

//     if (wally_tx_get_txid(tx, src_txid, sizeof(src_txid)) != WALLY_OK)
//         return false;

//     ret = memcmp(src_txid, txid, txid_len) == 0;
//     wally_clear(src_txid, sizeof(src_txid));
//     return ret;
// }

// static int psbt_input_sign_2(struct wally_psbt_input *input,
//                            const unsigned char *priv_key, size_t priv_key_len,
//                            const unsigned char *pub_key, size_t pub_key_len,
//                            const unsigned char *bytes, size_t bytes_len,
//                            uint32_t flags)
// {
//     unsigned char sig[EC_SIGNATURE_LEN], der[EC_SIGNATURE_DER_MAX_LEN + 1];
//     size_t der_len;
//     uint32_t sighash = input && input->sighash ? input->sighash : WALLY_SIGHASH_ALL;
//     int ret;

//     if (!input || !priv_key || priv_key_len != EC_PRIVATE_KEY_LEN ||
//         (wally_ec_public_key_verify(pub_key, pub_key_len) != WALLY_OK) ||
//         !bytes || bytes_len != SHA256_LEN || (flags & ~EC_FLAGS_ALL) ||
//         (sighash & 0xffffff00))
//         return WALLY_EINVAL;

//     /* Only grinding flag is relevant */
//     flags = EC_FLAG_ECDSA | (flags & EC_FLAG_GRIND_R);
//     if ((ret = wally_ec_sig_from_bytes(priv_key, priv_key_len,
//                                        bytes, SHA256_LEN, flags,
//                                        sig, sizeof(sig))) != WALLY_OK)
//         return ret;

//     if ((ret = wally_ec_sig_to_der(sig, sizeof(sig), der,
//                                    sizeof(der), &der_len)) != WALLY_OK)
//         return ret;

//     /* Convert sig to DER, add sighash byte and store in the input */
//     der[der_len++] = sighash & 0xff;
//     ret = wally_psbt_input_add_signature(input, pub_key, pub_key_len,
//                                          der, der_len);
//     wally_clear_2(sig, sizeof(sig), der, sizeof(der));
//     return ret;
// }

// static int wally_psbt_sign_2(struct wally_psbt *psbt,
//                     const unsigned char *key, size_t key_len, uint32_t flags)
// {
//     unsigned char pubkey[EC_PUBLIC_KEY_LEN], full_pubkey[EC_PUBLIC_KEY_UNCOMPRESSED_LEN];
//     const size_t pubkey_len = sizeof(pubkey), full_pubkey_len = sizeof(full_pubkey);
//     unsigned char wpkh_sc[WALLY_SCRIPTPUBKEY_P2PKH_LEN];
//     size_t is_elements, i;
//     int ret;

//     if (!psbt || !psbt->tx || !key || key_len != EC_PRIVATE_KEY_LEN ||
//         (flags & ~EC_FLAGS_ALL)) {
//         return WALLY_EINVAL;
//     }

//     if ((ret = wally_psbt_is_elements(psbt, &is_elements)) != WALLY_OK)
//         return ret;
// #ifndef BUILD_ELEMENTS
//     if (is_elements)
//         return WALLY_EINVAL;
// #endif /* ndef BUILD_ELEMENTS */

//     /* Get the pubkey */
//     ret = wally_ec_public_key_from_private_key(key, key_len,
//                                                pubkey, pubkey_len);
//     if (ret == WALLY_OK)
//         ret = wally_ec_public_key_decompress(pubkey, pubkey_len,
//                                              full_pubkey, full_pubkey_len);
//     if (ret != WALLY_OK)
//         return ret;

//     /* Go through each of the inputs */
//     for (i = 0; i < psbt->num_inputs; ++i) {
//         struct wally_psbt_input *input = &psbt->inputs[i];
//         struct wally_tx_input *txin = &psbt->tx->inputs[i];
//         unsigned char signature_hash[SHA256_LEN];
//         const unsigned char *scriptcode;
//         size_t keypath_index = 0, scriptcode_len;
//         uint32_t sighash;

//         /* See if this input has a keypath matching the pubkey of the private key supplied */
//         ret = wally_map_find(&input->keypaths, full_pubkey, full_pubkey_len, &keypath_index);
//         if (ret == WALLY_OK && !keypath_index)
//             ret = wally_map_find(&input->keypaths, pubkey, pubkey_len, &keypath_index);
//         if (ret != WALLY_OK)
//             continue;

//         if (!keypath_index)
//             continue; /* Didn't find a keypath matching this pubkey: skip it */
//         keypath_index -= 1; /* Use 0 based index below */

//         /* Make sure we don't already have a sig for this input */
//         size_t is_found;
//         ret = wally_map_find(&input->signatures, full_pubkey, full_pubkey_len, &is_found);
//         if (ret == WALLY_OK && !is_found)
//             ret = wally_map_find(&input->signatures, pubkey, pubkey_len, &is_found);

//         if (ret != WALLY_OK || is_found)
//             continue; /* Already got a partial sig for this pubkey on this input */

//         /* From this point, any failure to sign returns an error, since we
//         * have the key to sign this input we are expected to be able to */

//         if (!input_get_scriptcode_2(input, txin->index, &scriptcode, &scriptcode_len))
//             return WALLY_EINVAL; /* Couldn't find the script to sign with */

//         sighash = input->sighash ? input->sighash : WALLY_SIGHASH_ALL;

//         if (input->witness_utxo) {
//             size_t type;

//             ret = wally_scriptpubkey_get_type(scriptcode, scriptcode_len, &type);
//             if (ret != WALLY_OK)
//                 return ret;

//             if (type == WALLY_SCRIPT_TYPE_P2WPKH) {
//                 ret = wally_scriptpubkey_p2pkh_from_bytes(&scriptcode[2],
//                                                           HASH160_LEN, 0,
//                                                           wpkh_sc, sizeof(wpkh_sc),
//                                                           &scriptcode_len);
//                 if (ret != WALLY_OK)
//                     return ret;

//                 scriptcode = wpkh_sc;
//             } else if (type == WALLY_SCRIPT_TYPE_P2WSH && input->witness_script) {
//                 unsigned char p2wsh[WALLY_SCRIPTPUBKEY_P2WSH_LEN];
//                 size_t written;

//                 ret = wally_witness_program_from_bytes(input->witness_script,
//                                                        input->witness_script_len,
//                                                        WALLY_SCRIPT_SHA256,
//                                                        p2wsh, sizeof(p2wsh),
//                                                        &written);
//                 if (ret != WALLY_OK)
//                     return ret;

//                 if (scriptcode_len != sizeof(p2wsh) ||
//                     memcmp(p2wsh, scriptcode, sizeof(p2wsh)))
//                     return WALLY_EINVAL;

//                 scriptcode = input->witness_script;
//                 scriptcode_len = input->witness_script_len;
//             } else
//                 return WALLY_EINVAL; /* Unknown scriptPubKey type/not enough info */

// #ifdef BUILD_ELEMENTS
//             if (is_elements)
//                 ret = wally_tx_get_elements_signature_hash(psbt->tx, i,
//                                                            scriptcode, scriptcode_len,
//                                                            input->witness_utxo->value,
//                                                            input->witness_utxo->value_len,
//                                                            sighash,
//                                                            WALLY_TX_FLAG_USE_WITNESS,
//                                                            signature_hash, SHA256_LEN);
//             else
// #endif /* BUILD_ELEMENTS */
//             ret = wally_tx_get_btc_signature_hash(psbt->tx, i,
//                                                   scriptcode, scriptcode_len,
//                                                   input->witness_utxo->satoshi,
//                                                   sighash,
//                                                   WALLY_TX_FLAG_USE_WITNESS,
//                                                   signature_hash, SHA256_LEN);
//             if (ret != WALLY_OK)
//                 return ret;
//         } else if (input->utxo) {
//             if (!is_matching_txid_2(input->utxo,
//                                   txin->txhash, sizeof(txin->txhash)))
//                 return WALLY_EINVAL; /* prevout doesn't match this input */

//             ret = wally_tx_get_btc_signature_hash(psbt->tx, i,
//                                                   scriptcode, scriptcode_len,
//                                                   0, sighash, 0,
//                                                   signature_hash, SHA256_LEN);
//             if (ret != WALLY_OK)
//                 return ret;
//         }

//         ret = psbt_input_sign_2(input, key, key_len,
//                               input->keypaths.items[keypath_index].key,
//                               input->keypaths.items[keypath_index].key_len,
//                               signature_hash, SHA256_LEN, flags);
//         if (ret != WALLY_OK)
//             return ret;
//     }

//     return WALLY_OK;
// }

void Wally::psbt_sign(wally_psbt* psbt, const ECPrivateKey& key) const {
    auto data = key.data();
    auto ret = wally_psbt_sign(psbt, &data.front(), data.size(), 0);
    if(ret != WALLY_OK) {
        throw domain_error("Signing failed (" + to_string(ret) + ").");
    }
}

wally_psbt* Wally::psbt_copy(wally_psbt* psbt) const {
    auto data = psbt_to_data(psbt);
    return psbt_from_data(data);
}

void Wally::psbt_finalize(wally_psbt* psbt) const {
    if(wally_psbt_finalize(psbt) != WALLY_OK) {
        throw domain_error("Could not finalize PSBT.");
    }
}

// #define WALLY_TX_FLAG_USE_WITNESS   0x1 /* Encode witness data if present */
// #define WALLY_TX_FLAG_USE_ELEMENTS  0x2 /* Encode/Decode as an elements transaction */
// #define WALLY_TX_FLAG_ALLOW_PARTIAL 0x4 /* Allow partially complete transactions */
static uint32_t tx_flags = WALLY_TX_FLAG_USE_WITNESS | WALLY_TX_FLAG_ALLOW_PARTIAL;

wally_tx* Wally::tx_from_data(const ByteVector& data) const {
    wally_tx* output = nullptr;
    if(wally_tx_from_bytes(&data.front(), data.size(), tx_flags, &output) != WALLY_OK) {
        throw domain_error("Invalid transaction");
    }
    return output;
}

ByteVector Wally::tx_to_data(wally_tx* tx) const {
    size_t len;
    assert(wally_tx_get_length(tx, tx_flags, &len) == WALLY_OK);
    ByteVector result(len);
    size_t written;
    assert(wally_tx_to_bytes(tx, tx_flags, &result.front(), len, &written) == WALLY_OK);
    assert(written == len);
    return result;
}

wally_tx* Wally::tx_from_psbt(wally_psbt* psbt) const {
    wally_tx* output;
    if(wally_psbt_extract(psbt, &output) != WALLY_OK) {
        throw domain_error("Could not extract transaction from PBST.");
    }
    return output;
}
