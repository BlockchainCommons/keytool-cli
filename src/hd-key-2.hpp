#ifndef KEYTOOL_HD_KEY_2_HPP
#define KEYTOOL_HD_KEY_2_HPP

#include <string>
#include <optional>
#include "utils.hpp"
#include "asset.hpp"
#include "network.hpp"
#include "derivation-path.hpp"

enum KeyType {
    private_key,
    public_key
};

class HDKey2 final {
public:
    HDKey2(
        const std::string& name,
        bool is_master,
        KeyType key_type,
        const ByteVector& key_data,
        std::optional<ByteVector> chain_code,
        const Asset& asset,
        std::optional<DerivationPath> origin,
        std::optional<DerivationPath> children,
        std::optional<uint32_t> parent_fingerprint
    );

    const std::string& name() const { return _name; }
    bool is_master() const { return _is_master; }
    KeyType key_type() const { return _key_type; }
    const ByteVector& key_data() const { return _key_data; }
    std::optional<ByteVector> chain_code() const { return _chain_code; }
    const Asset& asset() const { return _asset; }
    std::optional<DerivationPath> origin() const { return _origin; }
    std::optional<DerivationPath> children() const { return _children; }
    std::optional<uint32_t> parent_fingerprint() const { return _parent_fingerprint; }

private:
    std::string _name;
    bool _is_master;
    KeyType _key_type;
    ByteVector _key_data;
    std::optional<ByteVector> _chain_code;
    Asset _asset;
    std::optional<DerivationPath> _origin;
    std::optional<DerivationPath> _children;
    std::optional<uint32_t> _parent_fingerprint;
};

#endif
