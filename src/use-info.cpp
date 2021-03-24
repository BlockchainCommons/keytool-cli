#include "use-info.hpp"
#include <bc-ur/bc-ur.hpp>
#include <set>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

bool UseInfo::is_default() const {
    return asset() == Asset2::btc() && network() == Network::mainnet();
}

uint32_t UseInfo::coin_type() const {
    if(asset() == Asset2::btc()) {
        if(network() == Network::mainnet()) {
            return asset().coin_type();
        } else if(network() == Network::testnet()) {
            return 1;
        } else {
            assert(false);
        }
    } else if(asset() == Asset2::eth()) {
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
    if(asset() != Asset2::btc()) {
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

UseInfo UseInfo::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    size_t map_len;
    decodeMapSize(pos, end, map_len, cborDecodingFlags);
    set<int> labels;
    auto asset = Asset2::btc();
    auto network = Network::mainnet();
    for(auto index = 0; index < map_len; index++) {
        int label;
        decodeInteger(pos, end, label, cborDecodingFlags);
        if(labels.find(label) != labels.end()) {
            throw domain_error("Duplicate label.");
        }
        labels.insert(label);
        switch(label) {
        case 1: // asset
            asset = Asset2::decode_cbor(pos, end);
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
