#ifndef KEYTOOL_HD_KEY_2_HPP
#define KEYTOOL_HD_KEY_2_HPP

#include <string>
#include <optional>
#include "utils.hpp"
#include "asset.hpp"
#include "network.hpp"

enum KeyType {
    private_key,
    public_key
};

struct UseInfo {
    Asset asset;
    Network network;

    UseInfo(Asset asset, Network network);
};

class HDKey2 final {
public:
    HDKey2(
        const std::string& name,
        bool is_master,
        KeyType key_type,
        const ByteVector& key_data,
        std::optional<ByteVector> chain_code
    );

    const std::string& name() const { return _name; }
    bool is_master() const { return _is_master; }
    KeyType key_type() const { return _key_type; }
    const ByteVector& key_data() const { return _key_data; }
    std::optional<ByteVector> chain_code() const { return _chain_code; }

private:
    std::string _name;
    bool _is_master;
    KeyType _key_type;
    ByteVector _key_data;
    std::optional<ByteVector> _chain_code;
};

#endif
