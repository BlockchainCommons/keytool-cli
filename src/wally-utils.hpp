#ifndef KEYTOOL_WALLY_UTILS_HPP
#define KEYTOOL_WALLY_UTILS_HPP

#include "utils.hpp"
#include "asset.hpp"
#include "hd-key.hpp"
#include "ec-key.hpp"

class Wally final {
public:
    // Requiring access through a singleton ensures
    // that LibWally has been properly initialized.
    static Wally instance;

    struct BIP39InvalidEntropyLength: std::exception { };
    struct UnsupportedDerivation: std::exception { };

    bool bip39_is_entropy_length_valid(size_t len) const;
    std::string bip39_mnemonic_from_entropy(const ByteVector& entropy) const;
    ByteVector bip39_mnemonic_to_entropy(const std::string& mnemonic) const;
    ByteVector bip39_mnemonic_to_bip32_seed(const std::string& mnemonic) const;

    struct HDKeyInvalidSeedLength: std::exception { };
    struct HDKeyInvalidSeed: std::exception { };
    struct HDKeyInvalidKey: std::exception { };

    bool bip32_is_seed_length_valid(size_t len) const;
    HDKey bip32_key_from_base58(const std::string& base58) const;
    HDKey bip32_key_from_seed(const ByteVector& seed, const Network& network) const;
    HDKey bip32_key_from_entropy(const ByteVector& entropy, const Network& network) const;
    HDKey bip32_key_from_parent(const HDKey& parent, uint32_t index, bool is_hardened, bool is_private) const;
    ByteVector bip32_key_serialize(const HDKey& parent, bool is_private) const;
    HDKey bip32_key_unserialize(const ByteVector& serialized) const;
    HDKey bip32_key_to_public(const HDKey& key) const;

    ECPrivateKey bip32_key_to_ec_private(const HDKey& key) const;
    ECCompressedPublicKey bip32_key_to_ec_public(const HDKey& key) const;

    ECCompressedPublicKey ec_key_to_public(const ECPrivateKey& key) const;
    ECUncompressedPublicKey ec_key_uncompress(const ECCompressedPublicKey& key) const;
    ECCompressedPublicKey ec_key_compress(const ECUncompressedPublicKey& key) const;

    std::string ec_key_to_wif(const ECPrivateKey& key, const Network& network) const;
    ECPrivateKey wif_to_ec_key(const std::string& wif, const Network& network) const;

    ByteVector hash160(const ByteVector& bytes) const;
    std::string to_base58(const ByteVector& bytes, bool is_check) const;
    std::string to_address(const ECCompressedPublicKey& key, uint8_t version) const;
    std::string to_address(const ECCompressedPublicKey& key, const Asset& asset, bool is_sh) const;

private:
    Wally();
    ~Wally();

    static uint32_t flags_for_private(bool is_private);

    void test1() const;
    void test2() const;
};

#endif // KEYTOOL_WALLY_UTILS_HPP
