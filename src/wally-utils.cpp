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

HDKey Wally::bip32_key_from_base58(const std::string& base58) const {
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
    if(!parent.is_private() && (is_private || is_hardened)) {
        throw UnsupportedDerivation();
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
        throw std::domain_error("Cannot derive private key from public key.");
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
    if(wally_wif_to_bytes(wif.c_str(), prefix, WALLY_WIF_FLAG_COMPRESSED, &bytes_out.front(), bytes_out.size()) != WALLY_OK) {
        throw std::domain_error("Invalid WIF.");
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
