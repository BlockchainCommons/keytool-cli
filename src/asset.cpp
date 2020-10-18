#include "asset.hpp"
#include <exception>
#include <assert.h>

const Asset *Asset::btc = NULL;
const Asset *Asset::btct = NULL;
std::vector<Asset> *Asset::_assets = NULL;

// construct static members at run-time as they depend on objects from another scope
void Asset::init(void) {
    static const Asset _btc = {"Bitcoin", "btc", Network::mainnet, 0, 0x00, 0x05};
    static const Asset _btct = {"Bitcoin Testnet", "btct", Network::testnet, 1, 0x6f, 0xc4};

    btc = &_btc;
    btct = &_btct;

    assert(_assets == NULL);

    _assets = new std::vector<Asset>;
    _assets->push_back(*btc);
    _assets->push_back(*btct);
}

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
