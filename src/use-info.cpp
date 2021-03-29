#include "use-info.hpp"
#include <bc-ur/bc-ur.hpp>
#include <set>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

bool UseInfo::is_default() const {
    return asset() == Asset::btc() && network() == Network::mainnet();
}

uint32_t UseInfo::coin_type() const {
    if(asset() == Asset::btc()) {
        if(network() == Network::mainnet()) {
            return asset().coin_type();
        } else if(network() == Network::testnet()) {
            return 1;
        } else {
            assert(false);
        }
    } else if(asset() == Asset::eth()) {
        if(network() == Network::mainnet()) {
            return asset().coin_type();
        } else if(network() == Network::testnet()) {
            return 1;
        } else {
            assert(false);
        }
    } else {
        assert(false);
    }
}

void UseInfo::encode_cbor(ByteVector& cbor) const {
    size_t map_size = 0;

    // asset
    ByteVector asset_map_entry;
    if(asset() != Asset::btc()) {
        map_size += 1;
        encodeInteger(asset_map_entry, 1);
        asset().encode_cbor(asset_map_entry);
    }

    // network
    ByteVector network_map_entry;
    if(network() != Network::mainnet()) {
        map_size += 1;
        encodeInteger(network_map_entry, 2);
        network().encode_cbor(network_map_entry);
    }

    encodeMapSize(cbor, map_size);
    ::append(cbor, asset_map_entry);
    ::append(cbor, network_map_entry);
}

void UseInfo::encode_tagged_cbor(ByteVector& cbor) const {
    encodeTagAndValue(cbor, Major::semantic, Tag(305));
    encode_cbor(cbor);
}

UseInfo UseInfo::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    size_t map_len;
    decodeMapSize(pos, end, map_len, cbor_decoding_flags);
    set<int> labels;
    auto asset = Asset::btc();
    auto network = Network::mainnet();
    for(auto index = 0; index < map_len; index++) {
        int label;
        decodeInteger(pos, end, label, cbor_decoding_flags);
        if(labels.find(label) != labels.end()) {
            throw domain_error("Duplicate label.");
        }
        labels.insert(label);
        switch(label) {
        case 1: // asset
            asset = Asset::decode_cbor(pos, end);
            break;
        case 2: // network
            network = Network::decode_cbor(pos, end);
            break;
        default:
            throw domain_error("Unknown label.");
        }
    }
    return UseInfo(asset, network);
}

UseInfo UseInfo::decode_tagged_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    Tag major_tag;
    Tag minor_tag;
    decodeTagAndValue(pos, end, major_tag, minor_tag, cbor_decoding_flags);
    if(major_tag != Major::semantic || minor_tag != 305) {
        throw domain_error("Invalid use-info.");
    }
    return decode_cbor(pos, end);
}

uint8_t UseInfo::version_sh() const {
    if(asset() == Asset::btc()) {
        if(network() == Network::mainnet()) {
            return 0x05;
        } else if(network() == Network::testnet()) {
            return 0xc4;
        } else {
            assert(false);
        }
    } else {
        throw domain_error("Unsupported asset.");
    }
}

uint8_t UseInfo::version_pkh() const {
    if(asset() == Asset::btc()) {
        if(network() == Network::mainnet()) {
            return 0x00;
        } else if(network() == Network::testnet()) {
            return 0x6f;
        } else {
            assert(false);
        }
    } else {
        throw domain_error("Unsupported asset.");
    }
}
