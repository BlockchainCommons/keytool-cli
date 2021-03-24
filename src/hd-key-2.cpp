#include "hd-key-2.hpp"
#include "wally.hpp"
#include "wally-utils.hpp"
#include <bc-ur/bc-ur.hpp>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

HDKey2::HDKey2(
    bool is_master,
    const KeyType& key_type,
    const ByteVector& key_data,
    optional<ByteVector> chain_code,
    const UseInfo& use_info,
    optional<DerivationPath2> origin,
    optional<DerivationPath2> children,
    optional<uint32_t> parent_fingerprint
)
    : _is_master(is_master)
    , _key_type(key_type)
    , _key_data(key_data)
    , _chain_code(chain_code)
    , _use_info(use_info)
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


    if(key_type() == KeyType::private_key()) {
        store_into(k.priv_key, key_data());
        assert(wally_ec_public_key_from_private_key(k.priv_key + 1, EC_PRIVATE_KEY_LEN, k.pub_key, EC_PUBLIC_KEY_LEN) == WALLY_OK);
        k.version = use_info().network().bip32_private_version();
    } else if(key_type() == KeyType::public_key()) {
        k.priv_key[0] = 0x01;
        store_into(k.pub_key, key_data());
        k.version = use_info().network().bip32_public_version();
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

HDKey2 HDKey2::from_seed(const Seed& seed, const UseInfo& use_info) {
    auto& data = seed.data();
    if(!Wally::instance.bip39_is_entropy_length_valid(data.size())) {
        throw domain_error("Cannot construct HDKey from seed of invalid length.");
    }
    auto mnemonic = Wally::instance.bip39_mnemonic_from_entropy(data);
    auto bip32_seed = Wally::instance.bip39_mnemonic_to_bip32_seed(mnemonic);

    ext_key k;
    bip32_key_from_seed(&data[0], data.size(), use_info.network().bip32_private_version(), BIP32_FLAG_SKIP_HASH, &k);

    auto is_master = true;
    auto key_type = KeyType::private_key();
    auto key_data = data_of(k.priv_key, sizeof(k.priv_key));
    auto chain_code = data_of(k.chain_code, sizeof(k.chain_code));
    auto origin = nullopt;
    auto children = nullopt;
    auto parent_fingerprint = nullopt;

    return HDKey2(is_master, key_type, key_data, chain_code, use_info, origin, children, parent_fingerprint);
}

HDKey2 HDKey2::derive(const KeyType& derived_key_type) const {
    if(key_type() == KeyType::public_key() && derived_key_type == KeyType::private_key()) {
        throw domain_error("Cannot derive private key from public key.");
    }

    if(key_type() == derived_key_type) {
        // private -> private
        // public -> public
        return HDKey2(*this);
    } else {
        // private -> public
        auto pub_key = data_of(wally_ext_key().pub_key, sizeof(ext_key::pub_key));
        return HDKey2(is_master(), derived_key_type, pub_key, chain_code(), use_info(), origin(), children(), parent_fingerprint());
    }
}

ByteVector HDKey2::key_fingerprint_data() const {
    auto k = wally_ext_key();
    uint8_t bytes[BIP32_KEY_FINGERPRINT_LEN];
    assert(bip32_key_get_fingerprint(&k, &bytes[0], BIP32_KEY_FINGERPRINT_LEN) == WALLY_OK);
    return ByteVector(&bytes[0], &bytes[0] + BIP32_KEY_FINGERPRINT_LEN);
}

uint32_t HDKey2::key_fingerprint() const {
    auto b = key_fingerprint_data();
    return pop_uint32(b);
}

HDKey2 HDKey2::derive(const KeyType& derived_key_type, DerivationStep child_derivation) {
    if(key_type() == KeyType::public_key() && derived_key_type == KeyType::private_key()) {
        throw domain_error("Cannot derive private key from public key.");
    }

    auto is_master = false;
    auto parent_fingerprint = key_fingerprint();
    auto key = wally_ext_key();
    auto child_num = child_derivation.child_num();
    auto flags = derived_key_type.bip32_flag();
    ext_key output;
    if(bip32_key_from_parent(&key, child_num, flags, &output) != WALLY_OK) {
        throw domain_error("Unknown problem deriving HDKey.");
    }

    auto key_data = derived_key_type.is_private() ? data_of(output.priv_key, sizeof(output.priv_key)) : data_of(output.pub_key, sizeof(output.pub_key));
    auto chain_code = data_of(output.chain_code, sizeof(output.chain_code));
    auto use_info = this->use_info();
    auto origin = DerivationPath2({});
    if(auto parent_origin_opt = this->origin()) {
        auto parent_origin = *parent_origin_opt;
        auto steps = parent_origin.steps();
        steps.push_back(child_derivation);
        uint32_t source_fingerprint;
        if(auto parent_source_fingerprint_opt = parent_origin.source_fingerprint()) {
            source_fingerprint = *parent_source_fingerprint_opt;
        } else {
            source_fingerprint = parent_fingerprint;
        }
        uint8_t depth;
        if(auto parent_depth_opt = parent_origin.depth()) {
            depth = *parent_depth_opt + 1;
        } else {
            depth = 1;
        }
        origin = DerivationPath2(steps, source_fingerprint, depth);
    } else {
        origin = DerivationPath2({child_derivation}, parent_fingerprint, 1);
    }

    auto children = nullopt;
    return HDKey2(is_master, derived_key_type, key_data, chain_code, use_info, origin, children, parent_fingerprint);
}

HDKey2 HDKey2::derive(const KeyType& derived_key_type, const DerivationPath2& child_derivation_path) {
    auto key = *this;
    for(auto step: child_derivation_path.steps()) {
        key = key.derive(key_type(), step);
    }
    return key.derive(derived_key_type);
}

void HDKey2::encode_cbor(ByteVector& cbor) const {
    size_t map_size = 0;

    // is_master
    ByteVector is_master_map_entry;
    if(is_master()) {
        map_size += 1;
        encodeInteger(is_master_map_entry, 1);
        encodeBool(is_master_map_entry, true);
    }

    // is_private
    ByteVector is_private_map_entry;
    if(key_type().is_private()) {
        map_size += 1;
        encodeInteger(is_private_map_entry, 2);
        encodeBool(is_private_map_entry, true);
    }

    // key_data
    ByteVector key_data_map_entry;
    map_size += 1;
    encodeInteger(key_data_map_entry, 3);
    encodeBytes(key_data_map_entry, key_data());

    // chain_code
    ByteVector chain_code_map_entry;
    if(auto chain_code_opt = chain_code()) {
        map_size += 1;
        encodeInteger(chain_code_map_entry, 4);
        encodeBytes(chain_code_map_entry, *chain_code_opt);
    }
}

void HDKey2::encode_tagged_cbor(ByteVector &cbor) const {

}
