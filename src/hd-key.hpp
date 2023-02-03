#ifndef KEYTOOL_HD_KEY_2_HPP
#define KEYTOOL_HD_KEY_2_HPP

#include <string>
#include <optional>
#include <limits>
#include "utils.hpp"
#include "use-info.hpp"
#include "derivation-path.hpp"
#include "seed.hpp"
#include "key-type.hpp"
#include "ec-key.hpp"

struct ext_key;

class HDKey final {
public:
    HDKey(
        bool is_master,
        const KeyType& key_type,
        const ByteVector& key_data,
        std::optional<ByteVector> chain_code,
        const UseInfo& use_info,
        std::optional<DerivationPath> origin,
        std::optional<DerivationPath> children,
        std::optional<uint32_t> parent_fingerprint
    );

    bool is_master() const { return _is_master; }
    const KeyType& key_type() const { return _key_type; }
    const ByteVector& key_data() const { return _key_data; }
    std::optional<ByteVector> chain_code() const { return _chain_code; }
    bool is_derivable() const { return _chain_code.has_value(); }
    const UseInfo& use_info() const { return _use_info; }
    std::optional<DerivationPath> origin() const { return _origin; }
    std::optional<DerivationPath> children() const { return _children; }
    std::optional<uint32_t> parent_fingerprint() const { return _parent_fingerprint; }

    ByteVector key_fingerprint_data() const;
    uint32_t key_fingerprint() const;

    static HDKey from_seed(const Seed& seed, const UseInfo& use_info);

    HDKey derive(const KeyType& derived_key_type, bool is_derivable) const;
    HDKey derive(const KeyType& derived_key_type, DerivationStep child_derivation) const;
    HDKey derive(const KeyType& derived_key_type, const DerivationPath& child_derivation_path, bool is_derivable) const;

    ext_key wally_ext_key() const;
    static HDKey from_wally_ext_key(const ext_key& k);

    std::string to_base58(KeyType key_type) const;
    std::string to_base58() const;
    static HDKey from_base58(const std::string& base58);

    void encode_cbor(ByteVector& cbor) const;
    void encode_tagged_cbor(ByteVector &cbor) const;
    ByteVector tagged_cbor() const;
    static HDKey decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);
    static HDKey decode_tagged_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);

    std::string ur() const;
    static HDKey from_ur(const std::string& ur);

    // UR or Base58
    static HDKey parse_key(const std::string& s);

    ECPrivateKey to_ec_private() const;
    ECCompressedPublicKey to_ec_public() const;
    std::string to_segwit_address() const;

private:
    bool _is_master;
    KeyType _key_type;
    ByteVector _key_data;
    std::optional<ByteVector> _chain_code;
    UseInfo _use_info;
    std::optional<DerivationPath> _origin;
    std::optional<DerivationPath> _children;
    std::optional<uint32_t> _parent_fingerprint;
};

#endif
