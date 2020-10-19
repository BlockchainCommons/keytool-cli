#include "asset.hpp"

Network Network::mainnet() {
    static auto n = new Network("mainnet");
    return *n;
}

Network Network::testnet() {
    static auto n = new Network("testnet");
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

Network Network::find(const std::string& name) {
    for(auto network: networks()) {
        if(network.name() == name) {
            return network;
        }
    }
    throw std::domain_error("Unknown network: " + name);
}
