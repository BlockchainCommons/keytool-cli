#include "asset.hpp"
#include "wally.hpp"
#include <bc-ur/bc-ur.hpp>
#include <limits>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

Network Network::mainnet() {
    static auto n = new Network("mainnet", 0, BIP32_VER_MAIN_PRIVATE, BIP32_VER_MAIN_PUBLIC);
    return *n;
}

Network Network::testnet() {
    static auto n = new Network("testnet", 1, BIP32_VER_TEST_PRIVATE, BIP32_VER_TEST_PUBLIC);
    return *n;
}

std::vector<Network> Network::networks() {
    static auto a = new std::vector<Network>{ mainnet(), testnet() };
    return *a;
}

std::ostream& operator<< (std::ostream& os, const Network& network) {
    return os << network.name();
}

bool operator==(const Network& n1, const Network& n2) {
    return n1.name() == n2.name();
}

bool operator!=(const Network& n1, const Network& n2) {
    return n1.name() != n2.name();
}

Network Network::find(const std::string& name) {
    for(auto network: networks()) {
        if(network.name() == name) {
            return network;
        }
    }
    throw std::domain_error("Unknown network: " + name);
}

void Network::encode_cbor(ByteVector& cbor) const {
    encodeInteger(cbor, value());
}

Network Network::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    uint64_t v;
    decodeUnsigned(pos, end, v, cborDecodingFlags);
    if(v == mainnet().value()) {
        return mainnet();
    } else if(v == testnet().value()) {
        return testnet();
    } else {
        throw domain_error("Unknown network.");
    }
}
