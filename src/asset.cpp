#include "asset.hpp"
#include <exception>

Network Network::mainnet {"mainnet"};
Network Network::testnet {"testnet"};
std::vector<Network> Network::_networks = {
    mainnet, testnet
};
Asset Asset::btc = {"Bitcoin", "btc", Network::mainnet, 0, 0x00, 0x05};
Asset Asset::btct = {"Bitcoin Testnet", "btct", Network::testnet, 1, 0x6f, 0xc4};

std::vector<Asset> Asset::_assets = {
    btc, btct
};

std::ostream& operator<< (std::ostream& os, const Asset& asset) {
    return os << asset.symbol();
}

Asset Asset::find(const std::string& symbol) {
    for(auto asset: assets()) {
        if(asset.symbol() == symbol) {
            return asset;
        }
    }
    throw std::domain_error("Unknown asset: " + symbol);
}
