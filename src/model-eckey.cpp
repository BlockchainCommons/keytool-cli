#include "model-eckey.hpp"
#include "model.hpp"
#include <optional>

using namespace std;

DataNode<Asset>* setup_asset(Model& model) {
    auto node = new DataNode<Asset>();
    model.add_node(node);
    node->set_info("asset", "ENUM btc|btct", "A cryptocurrency asset.");
    node->set_to_string([](const Asset& asset) { return asset.symbol(); });
    node->set_from_string([](const string& symbol) -> Asset { return Asset::find(symbol); });
    model.add_derivation("asset (default: btc)");
    node->set_value(Asset::btc());
    return node;
}

DataNode<Network>* setup_network(Model& model) {
    auto node = new DataNode<Network>();
    model.add_node(node);
    node->set_info("network", "ENUM mainnet|testnet", "The network.");
    node->set_to_string([](const Network& network) { return network.name(); });
    node->set_from_string([](const string& name) -> Network { return Network::find(name); });
    model.add_derivation("network <- [asset]");
    node->set_derivation([&]() {
        return model.asset->value().network();
    });
    return node;
}
