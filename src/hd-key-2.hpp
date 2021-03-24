#ifndef KEYTOOL_HD_KEY_2_HPP
#define KEYTOOL_HD_KEY_2_HPP

#include <string>
#include <optional>
#include "utils.hpp"
#include "use-info.hpp"
#include "derivation-path-2.hpp"
#include "seed.hpp"
#include "key-type.hpp"

struct ext_key;

class HDKey2 final {
public:
    HDKey2(
        bool is_master,
        const KeyType& key_type,
        const ByteVector& key_data,
        std::optional<ByteVector> chain_code,
        const UseInfo& use_info,
        std::optional<DerivationPath2> origin,
        std::optional<DerivationPath2> children,
        std::optional<uint32_t> parent_fingerprint
    );

    bool is_master() const { return _is_master; }
    const KeyType& key_type() const { return _key_type; }
    const ByteVector& key_data() const { return _key_data; }
    std::optional<ByteVector> chain_code() const { return _chain_code; }
    const UseInfo& use_info() const { return _use_info; }
    std::optional<DerivationPath2> origin() const { return _origin; }
    std::optional<DerivationPath2> children() const { return _children; }
    std::optional<uint32_t> parent_fingerprint() const { return _parent_fingerprint; }

    ByteVector key_fingerprint_data() const;
    uint32_t key_fingerprint() const;

    static HDKey2 from_seed(const Seed& seed, const UseInfo& use_info);

    HDKey2 derive(const KeyType& derived_key_type) const;
    HDKey2 derive(const KeyType& derived_key_type, DerivationStep child_derivation);
    HDKey2 derive(const KeyType& derived_key_type, const DerivationPath2& child_derivation_path);

    void encode_cbor(ByteVector& cbor) const;
    void encode_tagged_cbor(ByteVector &cbor) const;

private:
    bool _is_master;
    KeyType _key_type;
    ByteVector _key_data;
    std::optional<ByteVector> _chain_code;
    UseInfo _use_info;
    std::optional<DerivationPath2> _origin;
    std::optional<DerivationPath2> _children;
    std::optional<uint32_t> _parent_fingerprint;

    ext_key wally_ext_key() const;
};

#endif
