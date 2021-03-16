#include "model-hdkey.hpp"
#include "model.hpp"
#include <optional>

using namespace std;

DataNode<HDKey>* setup_master_key(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("master-key", "XPRV", "The BIP-32 master HD key.");
    node->set_to_string([](const HDKey& key) { return key.to_base58(true); });
    node->set_from_string([](const string& prv) -> HDKey { return HDKey(prv, true); });
    model.add_derivation("master-key <- [network, seed]");
    node->set_derivation([&]() -> optional<HDKey> {
        if(model.seed->has_value() && model.network->has_value()) {
            return HDKey(model.seed->value(), model.network->value());
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<ByteVector>* setup_master_key_fingerprint(Model& model) {
    auto node = new DataNode<ByteVector>();
    model.add_node(node);
    node->set_info("master-key-fingerprint", "HEX", "Fingerprint of the master HD key.");
    node->set_to_string([](const ByteVector& bytes) { return data_to_hex(bytes); });
    node->set_from_string([](const string& hex) -> ByteVector { return hex_to_data(hex); });
    model.add_derivation("master-key-fingerprint <- [master-key]");
    node->set_derivation([&]() -> optional<ByteVector> {
        if(model.master_key->has_value()) {
            return model.master_key->value().fingerprint();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<OutputDescriptorType>* setup_output_type(Model& model) {
    auto node = new DataNode<OutputDescriptorType>();
    model.add_node(node);
    node->set_info("output-type", "ENUM wpkh|pkh|sh-wpkh", "The output descriptor type.");
    node->set_to_string([](const OutputDescriptorType& d) { return d.name(); });
    node->set_from_string([](const string& name) -> OutputDescriptorType { return OutputDescriptorType::find(name); });
    model.add_derivation("output-type (default: wpkh)");
    node->set_value(OutputDescriptorType::wpkh());
    return node;
}

DataNode<uint32_t>* setup_purpose(Model& model) {
    auto node = new DataNode<uint32_t>();
    model.add_node(node);
    node->set_info("purpose", "INDEX", "The purpose field of the BIP-44 derivation path.");
    node->set_to_string([](uint32_t n) { return to_string(n); });
    node->set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });
    model.add_derivation("purpose <- [output-type]");
    node->set_derivation([&]() {
        return model.output_type->value().purpose();
    });
    return node;
}

DataNode<uint32_t>* setup_coin_type(Model& model) {
    auto node = new DataNode<uint32_t>();
    model.add_node(node);
    node->set_info("coin-type", "INDEX", "The coin type field of the BIP-44 derivation path.");
    node->set_to_string([](uint32_t n) { return to_string(n); });
    node->set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });
    model.add_derivation("coin-type <- asset");
    node->set_derivation([&]() {
        return model.asset->value().coin_type();
    });
    return node;
}

DataNode<uint32_t>* setup_account_index(Model& model) {
    auto node = new DataNode<uint32_t>();
    model.add_node(node);
    node->set_info("account-index", "INDEX", "The account field of the BIP-44 derivation path.");
    node->set_to_string([](uint32_t n) { return to_string(n); });
    node->set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });
    model.add_derivation("account-index (default: 0)");
    node->set_value(0);
    return node;
}

DataNode<DerivationPath>* setup_account_derivation_path(Model& model) {
    auto node = new DataNode<DerivationPath>();
    model.add_node(node);
    node->set_info("account-derivation-path", "BIP32_PATH", "m/purpose'/coin-type'/accont-index'.");
    node->set_to_string([](const DerivationPath& path) { return to_string(path); });
    node->set_from_string([](const string& p) -> DerivationPath { return parse_derivation_path(p); });
    model.add_derivation("account-derivation-path <- [master-key-fingerprint, purpose, coin-type, account-index]");
    node->set_derivation([&]() {
        DerivationPathElement m = model.master_key_fingerprint->has_value() ? DerivationPathElement(model.master_key_fingerprint->value()) : DerivationPathElement();
        return DerivationPath {
            m,
            DerivationPathElement(model.purpose->value(), true),
            DerivationPathElement(model.coin_type->value(), true),
            DerivationPathElement(model.account_index->value(), true)
        };
    });
    return node;
}
