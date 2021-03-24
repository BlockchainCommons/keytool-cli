#include "asset-2.hpp"
#include <exception>
#include <bc-ur/bc-ur.hpp>
#include <limits>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

Asset2 Asset2::btc() {
    static auto a = new Asset2("Bitcoin", "btc", 0);
    return *a;
}

Asset2 Asset2::eth() {
    static auto a = new Asset2("Ethereum", "eth", 0x3c);
    return *a;
}

std::vector<Asset2> Asset2::assets() {
    static auto a = new std::vector<Asset2>{ btc(), eth() };
    return *a;
}

std::ostream& operator<< (std::ostream& os, const Asset2& asset) {
    return os << asset.symbol();
}

bool operator== (const Asset2& lhs, const Asset2& rhs) {
    return lhs.symbol() == rhs.symbol();
}

bool operator!= (const Asset2& lhs, const Asset2& rhs) {
    return lhs.symbol() != rhs.symbol();
}

Asset2 Asset2::find(const std::string& symbol) {
    for(auto asset: assets()) {
        if(asset.symbol() == symbol) {
            return asset;
        }
    }
    throw std::domain_error("Unknown asset: " + symbol);
}

void Asset2::encode_cbor(ByteVector& cbor) const {
    encodeInteger(cbor, coin_type());
}

Asset2 Asset2::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    uint64_t v;
    decodeUnsigned(pos, end, v, cborDecodingFlags);
    if(v == btc().coin_type()) {
        return btc();
    } else if(v == eth().coin_type()) {
        return eth();
    } else {
        throw domain_error("Unknown asset.");
    }
}
