#include "hd-key-2.hpp"
#include "wally.hpp"
#include "wally-utils.hpp"

HDKey2::HDKey2(
    const std::string& name,
    bool is_master,
    KeyType key_type,
    const ByteVector& key_data,
    std::optional<ByteVector> chain_code,
    const Asset& asset,
    std::optional<DerivationPath2> origin,
    std::optional<DerivationPath2> children,
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

ext_key HDKey2::wally_ext_key() const {
    ext_key k;

    if(auto opt_origin = origin()) {
        DerivationPath2 origin = *opt_origin;
        k.depth = origin.effective_depth();

        if(!origin.steps().empty()) {
            auto last_step = origin.steps().back();
            if(last_step.spec().is_index()) {
                auto value = last_step.spec().index().value();
                auto is_hardened = last_step.is_hardened();
                auto child_num = value | (is_hardened ? 0x80000000 : 0);
                k.child_num = child_num;
            }
        }
    }

    switch(key_type()) {
    case KeyType::private_key:
        store_into(k.priv_key, key_data());
        assert(wally_ec_public_key_from_private_key(k.priv_key + 1, EC_PRIVATE_KEY_LEN, k.pub_key, EC_PUBLIC_KEY_LEN) == WALLY_OK);
        if(asset().network() == Network::mainnet()) {
            k.version = BIP32_VER_MAIN_PRIVATE;
        } else if(asset().network() == Network::testnet()) {
            k.version = BIP32_VER_TEST_PRIVATE;
        }
        break;
    case KeyType::public_key:
        k.priv_key[0] = 0x01;
        store_into(k.pub_key, key_data());
        if(asset().network() == Network::mainnet()) {
            k.version = BIP32_VER_MAIN_PUBLIC;
        } else if(asset().network() == Network::testnet()) {
            k.version = BIP32_VER_TEST_PUBLIC;
        }
        break;
    }

    assert(wally_hash160(k.pub_key, EC_PUBLIC_KEY_LEN, k.hash160, sizeof(k.hash160)) == WALLY_OK);

    if(auto opt_chain_code = chain_code()) {
        store_into(k.chain_code, *opt_chain_code);
    }

    if(auto opt_parent_fingerprint = parent_fingerprint()) {
        store_into(k.parent160, big_endian_data(*opt_parent_fingerprint));
    }

    Wally::instance.check_valid(k);

    return k;
}
