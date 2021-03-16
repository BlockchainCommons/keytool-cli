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

DataNode<HDKey>* setup_account_key(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("account-key", "XPRV", "The BIP-44 account key.");
    node->set_to_string([](const HDKey& key) { return key.to_base58(true); });
    node->set_from_string([](const string& prv) -> HDKey { return HDKey(prv, true); });
    model.add_derivation("account-key <- [master-key, account-derivation-path]");
    node->set_derivation([&]() -> optional<HDKey> {
        if(model.master_key->has_value()) {
            return model.master_key->value().derive(model.account_derivation_path->value(), true);
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<HDKey>* setup_account_pub_key(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("account-pub-key", "XPUB", "The BIP-44 account public key.");
    node->set_to_string([](const HDKey& key) { return key.to_base58(false); });
    node->set_from_string([](const string& pub) -> HDKey { return HDKey(pub, false); });
    model.add_derivation("account-pub-key <- [account-key]");
    node->set_derivation([&]() -> optional<HDKey> {
        if(model.account_key->has_value()) {
            return model.account_key->value().to_public();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<ChainType>* setup_chain_type(Model& model) {
    auto node = new DataNode<ChainType>();
    model.add_node(node);
    node->set_info("chain-type", "ENUM internal|external|identity", "The BIP-44 chain type (change) field.");
    node->set_to_string([](const ChainType& t) { return t.to_string(); });
    node->set_from_string([](const string& t) -> ChainType { return ChainType::find(t); });
    model.add_derivation("chain-type (default: external)");
    node->set_value(ChainType::external());
    return node;
}

DataNode<uint32_t>* setup_chain_type_int(Model& model) {
    auto node = new DataNode<uint32_t>();
    model.add_node(node);
    node->set_info("chain-type-int", "INDEX", "The BIP-44 change field integer value.");
    node->set_to_string([](uint32_t n) { return to_string(n); });
    node->set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });
    model.add_derivation("chain-type-int <- [chain-type];");
    node->set_derivation([&]() {
        return model.chain_type->value().index();
    });
    return node;
}

DataNode<IndexBound>* setup_address_index(Model& model) {
    auto node = new DataNode<IndexBound>();
    model.add_node(node);
    node->set_info("address-index", "INDEX_BOUND", "The BIP-44 address_index field. '*' is allowed for output descriptors.");
    node->set_to_string([](IndexBound n) { return n.to_string(); });
    node->set_from_string([](const string& s) -> IndexBound { return parse_index_range(s); });
    model.add_derivation("address-index (default: 0)");
    node->set_value(0);
    return node;
}

DataNode<DerivationPath>* setup_address_derivation_path(Model& model) {
    auto node = new DataNode<DerivationPath>();
    model.add_node(node);
    node->set_info("address-derivation-path", "BIP32_PATH", "The BIP-32 address derivation path, starting from the account-key.");
    node->set_to_string([](const DerivationPath& path) { return to_string(path); });
    node->set_from_string([](const string& p) -> DerivationPath { return parse_derivation_path(p); });
    model.add_derivation("address-derivation-path <- [chain-type-int, address-index]");
    node->set_derivation([&]() {
        return DerivationPath {
            DerivationPathElement(model.chain_type_int->value(), false),
            DerivationPathElement(model.address_index->value(), false)
        };
    });
    return node;
}

DataNode<DerivationPath>* setup_full_address_derivation_path(Model& model) {
    auto node = new DataNode<DerivationPath>();
    model.add_node(node);
    node->set_info("full-address-derivation-path", "BIP32_PATH", "The BIP-32 address derivation path, starting from the master-key.");
    node->set_to_string([](const DerivationPath& path) { return to_string(path); });
    node->set_from_string([](const string& p) -> DerivationPath { return parse_derivation_path(p); });
    model.add_derivation("full-address-derivation-path <- [account-derivation-path, address-derivation-path]");
    node->set_derivation([&]() {
        auto path = model.account_derivation_path->value();
        auto address_path = model.address_derivation_path->value();
        path.insert(path.end(), address_path.begin(), address_path.end());
        return path;
    });
    return node;
}

DataNode<HDKey>* setup_address_key(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("address-key", "XPRV", "The BIP-32 address HD key.");
    node->set_to_string([](const HDKey& key) { return key.to_base58(true); });
    node->set_from_string([](const string& prv) -> HDKey { return HDKey(prv, true); });
    model.add_derivation("address-key <- [master-key, full-address-derivation-path]");
    model.add_derivation("address-key <- [account-key, address-derivation-path]");
    node->set_derivation([&]() -> optional<HDKey> {
        if(model.master_key->has_value() && model.full_address_derivation_path->has_value()) {
            return model.master_key->value().derive(model.full_address_derivation_path->value(), true);
        } else if(model.account_key->has_value() && model.address_derivation_path->has_value()) {
            return model.account_key->value().derive(model.address_derivation_path->value(), true);
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<HDKey>* setup_address_pub_key(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("address-pub-key", "XPUB", "The BIP-32 address public HD key.");
    node->set_to_string([](const HDKey& key) { return key.to_base58(false); });
    node->set_from_string([](const string& pub) -> HDKey { return HDKey(pub, false); });
    model.add_derivation("address-pub-key <- [address-key]");
    model.add_derivation("address-pub-key <- [account-pub-key, address-derivation-path]");
    node->set_derivation([&]() -> optional<HDKey> {
        if(model.address_key->has_value()) {
            return model.address_key->value().to_public();
        } else if(model.account_pub_key->has_value() && model.address_derivation_path->has_value()) {
            return model.account_pub_key->value().derive(model.address_derivation_path->value(), false);
        } else {
            return nullopt;
        }
    });
    return node;
}
