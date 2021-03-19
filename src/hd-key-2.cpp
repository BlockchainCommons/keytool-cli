#include "hd-key-2.hpp"
#include "wally.hpp"

HDKey2::HDKey2(
    const std::string& name,
    bool is_master,
    KeyType key_type,
    const ByteVector& key_data,
    std::optional<ByteVector> chain_code,
    const Asset& asset,
    std::optional<DerivationPath> origin,
    std::optional<DerivationPath> children,
    std::optional<uint32_t> parent_fingerprint
)
    : _name(name)
    , _is_master(is_master)
    , _key_type(key_type)
    , _key_data(key_data)
    , _chain_code(chain_code)
    , _asset(asset)
    , _origin(origin)
    , _children(children)
    , _parent_fingerprint(parent_fingerprint)
{ }

static ext_key wally_ext_key(const HDKey2& hd_key) {
    ext_key k;

    if(hd_key.origin().has_value()) {
        auto origin = *hd_key.origin();
        // k.depth =
    }
    return k;
}
