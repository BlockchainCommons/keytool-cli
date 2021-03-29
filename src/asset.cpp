#include "asset.hpp"
#include <exception>
#include <bc-ur/bc-ur.hpp>
#include <limits>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

Asset Asset::btc() {
    static auto a = new Asset("Bitcoin", "btc", 0);
    return *a;
}

Asset Asset::eth() {
    static auto a = new Asset("Ethereum", "eth", 0x3c);
    return *a;
}

std::vector<Asset> Asset::assets() {
    static auto a = new std::vector<Asset>{ btc(), eth() };
    return *a;
}

std::ostream& operator<< (std::ostream& os, const Asset& asset) {
    return os << asset.symbol();
}

bool operator== (const Asset& lhs, const Asset& rhs) {
    return lhs.symbol() == rhs.symbol();
}

bool operator!= (const Asset& lhs, const Asset& rhs) {
    return lhs.symbol() != rhs.symbol();
}

Asset Asset::find(const std::string& symbol) {
    for(auto asset: assets()) {
        if(asset.symbol() == symbol) {
            return asset;
        }
    }
    throw std::domain_error("Unknown asset: " + symbol);
}

void Asset::encode_cbor(ByteVector& cbor) const {
    encodeInteger(cbor, coin_type());
}

Asset Asset::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    uint64_t v;
    decodeUnsigned(pos, end, v, cbor_decoding_flags);
    if(v == btc().coin_type()) {
        return btc();
    } else if(v == eth().coin_type()) {
        return eth();
    } else {
        throw domain_error("Unknown asset.");
    }
}
