// #include "asset.hpp"
// #include <exception>

// Asset Asset::btc() {
//     static auto a = new Asset("Bitcoin", "btc", Network::mainnet(), 0, 0x00, 0x05);
//     return *a;
// }

// Asset Asset::btct() {
//     static auto a = new Asset("Bitcoin Testnet", "btct", Network::testnet(), 1, 0x6f, 0xc4);
//     return *a;
// }

// std::vector<Asset> Asset::assets() {
//     static auto a = new std::vector<Asset>{ btc(), btct() };
//     return *a;
// }

// std::ostream& operator<< (std::ostream& os, const Asset& asset) {
//     return os << asset.symbol();
// }

// Asset Asset::find(const std::string& symbol) {
//     for(auto asset: assets()) {
//         if(asset.symbol() == symbol) {
//             return asset;
//         }
//     }
//     throw std::domain_error("Unknown asset: " + symbol);
// }
