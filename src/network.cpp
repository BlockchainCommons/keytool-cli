#include "asset.hpp"

Network Network::mainnet {"mainnet"};
Network Network::testnet {"testnet"};

std::vector<Network> Network::_networks = {
    mainnet, testnet
};

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
