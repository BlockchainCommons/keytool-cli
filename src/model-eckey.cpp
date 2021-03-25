#include "model-eckey.hpp"
#include "model.hpp"
#include <optional>

using namespace std;

DataNode<Asset2>* setup_asset(Model& model) {
    auto node = new DataNode<Asset2>();
    model.add_node(node);
    node->set_info("asset", "ENUM btc|btct", "A cryptocurrency asset.");
    node->set_to_string([](const Asset2& asset) { return asset.symbol(); });
    node->set_from_string([](const string& symbol) -> Asset2 { return Asset2::find(symbol); });
    model.add_derivation("asset (default: btc)");
    node->set_value(Asset2::btc());
    return node;
}

DataNode<Network>* setup_network(Model& model) {
    auto node = new DataNode<Network>();
    model.add_node(node);
    node->set_info("network", "ENUM mainnet|testnet", "The network.");
    node->set_to_string([](const Network& network) { return network.name(); });
    node->set_from_string([](const string& name) -> Network { return Network::find(name); });
    model.add_derivation("network (default: mainnet)");
    node->set_value(Network::mainnet());
    return node;
}

DataNode<ECPrivateKey>* setup_address_ec_key(Model& model) {
    auto node = new DataNode<ECPrivateKey>();
    model.add_node(node);
    node->set_info("address-ec-key", "ECPRV", "The address EC key.");
    node->set_to_string([](const ECPrivateKey& key) { return key.to_hex(); });
    node->set_from_string([](const string& k) -> ECPrivateKey { return ECPrivateKey(hex_to_data(k)); });
    model.add_derivation("address-ec-key <- [address-key]");
    model.add_derivation("address-ec-key <- [address-ec-key-wif, network]");
    node->set_derivation([&]() -> optional<ECPrivateKey> {
        if(model.address_key->has_value()) {
            return model.address_key->value().to_ec_private();
        } else if(model.address_ec_key_wif->has_value()) {
            return Wally::instance.wif_to_ec_key(model.address_ec_key_wif->value(), model.network->value());
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<string>* setup_address_ec_key_wif(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("address-ec-key-wif", "WIF", "The address EC key in WIF format.");
    node->set_to_string([](const string& s) { return s; });
    node->set_from_string([](const string& wif) -> string { return wif; });
    model.add_derivation("address-ec-key-wif <- [address-ec-key, network]");
    node->set_derivation([&]() -> optional<string> {
        if(model.address_ec_key->has_value()) {
            return Wally::instance.ec_key_to_wif(model.address_ec_key->value(), model.network->value());
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<ECCompressedPublicKey>* setup_address_pub_ec_key(Model& model) {
    auto node = new DataNode<ECCompressedPublicKey>();
    model.add_node(node);
    node->set_info("address-pub-ec-key", "ECPUB", "The compressed public EC key.");
    node->set_to_string([](const ECCompressedPublicKey& key) { return key.to_hex(); });
    node->set_from_string([](const string& k) -> ECCompressedPublicKey { return ECCompressedPublicKey(hex_to_data(k)); });
    model.add_derivation("address-pub-ec-key <- [address-ec-key]");
    model.add_derivation("address-pub-ec-key <- [address-pub-key]");
    node->set_derivation([&]() -> optional<ECCompressedPublicKey> {
        if(model.address_pub_key->has_value()) {
            return model.address_pub_key->value().to_ec_public();
        } else if(model.address_ec_key->has_value()) {
            return model.address_ec_key->value().to_public();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<string>* setup_address_pkh(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("address-pkh", "ADDRESS", "The pay-to-public-key-hash address.");
    node->set_to_string([](const string& s) { return s; });
    node->set_from_string([](const string& a) -> string { return a; });
    model.add_derivation("address-pkh <- [address-pub-ec-key, asset, network]");
    node->set_derivation([&]() -> optional<string> {
        if(model.address_pub_ec_key->has_value()) {
            UseInfo use_info(model.asset->value(), model.network->value());
            return model.address_pub_ec_key->value().to_address(use_info, false);
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<string>* setup_address_sh(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("address-sh", "ADDRESS", "The pay-to-script-hash address.");
    node->set_to_string([](const string& s) { return s; });
    node->set_from_string([](const string& a) -> string { return a; });
    model.add_derivation("address-sh <- [address-pub-ec-key, asset, network]");
    node->set_derivation([&]() -> optional<string> {
        if(model.address_pub_ec_key->has_value()) {
            UseInfo use_info(model.asset->value(), model.network->value());
            return model.address_pub_ec_key->value().to_address(use_info, true);
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<string>* setup_address_segwit(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("address-segwit", "ADDRESS", "The segwit address.");
    node->set_to_string([](const string& s) { return s; });
    node->set_from_string([](const string& a) -> string { return a; });
    model.add_derivation("address-segwit <- [address-pub-key]");
    node->set_derivation([&]() -> optional<string> {
        if(model.address_pub_key->has_value()) {
            return model.address_pub_key->value().to_segwit_address();
        } else {
            return nullopt;
        }
    });
    return node;
}
