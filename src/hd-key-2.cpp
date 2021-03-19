#include "hd-key-2.hpp"

HDKey2::HDKey2(
    const std::string& name,
    bool is_master,
    KeyType key_type,
    const ByteVector& key_data,
    std::optional<ByteVector> chain_code
)
    : _name(name)
    , _is_master(is_master)
    , _key_type(key_type)
    , _key_data(key_data)
    , _chain_code(chain_code)
{ }
