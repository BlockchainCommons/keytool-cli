#ifndef KEYTOOL_HD_KEY_2_HPP
#define KEYTOOL_HD_KEY_2_HPP

#include <string>
#include <optional>
#include "utils.hpp"
#include "asset.hpp"
#include "network.hpp"
#include "derivation-path-2.hpp"

enum KeyType {
    private_key,
    public_key
};

struct ext_key;

class HDKey2 final {
public:
    HDKey2(
        const std::string& name,
        bool is_master,
        KeyType key_type,
        const ByteVector& key_data,
        std::optional<ByteVector> chain_code,
        const Asset& asset,
        std::optional<DerivationPath2> origin,
        std::optional<DerivationPath2> children,
        std::optional<uint32_t> parent_fingerprint
    );

    const std::string& name() const { return _name; }
    bool is_master() const { return _is_master; }
    KeyType key_type() const { return _key_type; }
    const ByteVector& key_data() const { return _key_data; }
    std::optional<ByteVector> chain_code() const { return _chain_code; }
    const Asset& asset() const { return _asset; }
    std::optional<DerivationPath2> origin() const { return _origin; }
    std::optional<DerivationPath2> children() const { return _children; }
    std::optional<uint32_t> parent_fingerprint() const { return _parent_fingerprint; }

private:
    std::string _name;
    bool _is_master;
    KeyType _key_type;
    ByteVector _key_data;
    std::optional<ByteVector> _chain_code;
    Asset _asset;
    std::optional<DerivationPath2> _origin;
    std::optional<DerivationPath2> _children;
    std::optional<uint32_t> _parent_fingerprint;

    ext_key wally_ext_key() const;
};

#endif
