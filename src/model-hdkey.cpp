#include "model-hdkey.hpp"
#include "model.hpp"
#include <optional>

using namespace std;

DataNode<HDKey>* setup_master_key(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("master-key", "UR:CRYPTO-HDKEY", "The BIP-32 master HD key.");
    node->set_to_string([](const HDKey& key) { return key.ur(); });
    node->set_from_string([](const string& ur) -> HDKey {
        auto k = HDKey::from_ur(ur);
        if(k.key_type() != KeyType::private_key()) {
            throw domain_error("master-key must be a private key.");
        }
        return k;
    });
    model.add_derivation("master-key <- [seed, asset, network]");
    model.add_derivation("master-key <- [master-key-base58]");
    node->set_derivation([&]() -> optional<HDKey> {
        if(model.master_key_base58->has_assigned_value()) {
            return model.master_key_base58->value();
        } else if(model.seed->has_value()) {
            auto seed = model.seed->value();
            auto asset = model.asset->value();
            auto network = model.network->value();
            auto use_info = UseInfo(asset, network);
            return HDKey::from_seed(seed, use_info);
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<uint32_t>* setup_master_key_fingerprint(Model& model) {
    auto node = new DataNode<uint32_t>();
    model.add_node(node);
    node->set_info("master-key-fingerprint", "HEX", "Fingerprint of the master HD key.");
    node->set_to_string([](uint32_t value) { return uint32_to_hex(value); });
    node->set_from_string([](const string& hex) -> uint32_t { return hex_to_uint32(hex); });
    model.add_derivation("master-key-fingerprint <- [master-key]");
    node->set_derivation([&]() -> optional<uint32_t> {
        if(model.master_key->has_value()) {
            return model.master_key->value().key_fingerprint();
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
    node->set_to_string([](const DerivationPath& path) { return path.to_string(); });
    node->set_from_string([](const string& p) -> DerivationPath { return DerivationPath::from_string(p); });
    model.add_derivation("account-derivation-path <- [master-key-fingerprint, purpose, coin-type, account-index]");
    node->set_derivation([&]() {
        auto source_fingerprint = model.master_key_fingerprint->optional_value();
        vector<DerivationStep> steps {
            DerivationStep(model.purpose->value(), true),
            DerivationStep(model.coin_type->value(), true),
            DerivationStep(model.account_index->value(), true)
        };
        return DerivationPath(steps, source_fingerprint);
    });
    return node;
}

DataNode<HDKey>* setup_account_key(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("account-key", "UR:CRYPTO-HDKEY", "The BIP-44 account HD key.");
    node->set_to_string([](const HDKey& key) { return key.ur(); });
    node->set_from_string([](const string& ur) -> HDKey {
        auto k = HDKey::from_ur(ur);
        if(k.key_type() != KeyType::private_key()) {
            throw domain_error("account-key must be a private key.");
        }
        return k;
    });
    model.add_derivation("account-key <- [master-key, account-derivation-path]");
    model.add_derivation("account-key <- [account-key-base58]");
    node->set_derivation([&]() -> optional<HDKey> {
        if(model.account_key_base58->has_assigned_value()) {
            return model.account_key_base58->value();
        } else if(model.master_key->has_value()) {
            auto master_key = model.master_key->value();
            auto path = model.account_derivation_path->value();
            return master_key.derive(KeyType::private_key(), path);
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<HDKey>* setup_account_pub_key(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("account-pub-key", "UR:CRYPTO-HDKEY", "The BIP-44 account public HD key.");
    node->set_to_string([](const HDKey& key) { return key.ur(); });
    node->set_from_string([](const string& ur) -> HDKey { return HDKey::from_ur(ur); });
    model.add_derivation("account-pub-key <- [account-key]");
    model.add_derivation("account-pub-key <- [account-pub-key-base58]");
    node->set_derivation([&]() -> optional<HDKey> {
        if(model.account_pub_key_base58->has_assigned_value()) {
            return model.account_pub_key_base58->value();
        } else if(model.account_key->has_value()) {
            return model.account_key->value().derive(KeyType::public_key());
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

DataNode<DerivationIndexSpec>* setup_address_index(Model& model) {
    auto node = new DataNode<DerivationIndexSpec>();
    model.add_node(node);
    node->set_info("address-index", "INDEX_BOUND", "The BIP-44 address_index field. '*' is allowed for output descriptors.");
    node->set_to_string([](DerivationIndexSpec n) { return n.to_string(); });
    node->set_from_string([](const string& s) -> DerivationIndexSpec { return DerivationIndexSpec::from_string(s); });
    model.add_derivation("address-index (default: 0)");
    node->set_value(0);
    return node;
}

DataNode<DerivationPath>* setup_address_derivation_path(Model& model) {
    auto node = new DataNode<DerivationPath>();
    model.add_node(node);
    node->set_info("address-derivation-path", "BIP32_PATH", "The BIP-32 address derivation path, starting from the account-key.");
    node->set_to_string([](const DerivationPath& path) { return path.to_string(); });
    node->set_from_string([](const string& p) -> DerivationPath { return DerivationPath::from_string(p); });
    model.add_derivation("address-derivation-path <- [chain-type-int, address-index]");
    node->set_derivation([&]() {
        vector<DerivationStep> steps {
            DerivationStep(model.chain_type_int->value(), false),
            DerivationStep(model.address_index->value(), false)
        };
        return DerivationPath(steps);
    });
    return node;
}

DataNode<DerivationPath>* setup_full_address_derivation_path(Model& model) {
    auto node = new DataNode<DerivationPath>();
    model.add_node(node);
    node->set_info("full-address-derivation-path", "BIP32_PATH", "The BIP-32 address derivation path, starting from the master-key.");
    node->set_to_string([](const DerivationPath& path) { return path.to_string(); });
    node->set_from_string([](const string& p) -> DerivationPath { return DerivationPath::from_string(p); });
    model.add_derivation("full-address-derivation-path <- [account-derivation-path, address-derivation-path]");
    node->set_derivation([&]() {
        auto path = model.account_derivation_path->value();
        auto address_path = model.address_derivation_path->value();
        path.append(address_path);
        return path;
    });
    return node;
}

DataNode<HDKey>* setup_address_key(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("address-key", "UR:CRYPTO-HDKEY", "The BIP-32 address HD key.");
    node->set_to_string([](const HDKey& key) { return key.ur(); });
    node->set_from_string([](const string& ur) -> HDKey {
        auto k = HDKey::from_ur(ur);
        if(k.key_type() != KeyType::private_key()) {
            throw domain_error("address-key must be a private key");
        }
        return k;
    });
    model.add_derivation("address-key <- [master-key, full-address-derivation-path]");
    model.add_derivation("address-key <- [account-key, address-derivation-path]");
    model.add_derivation("address-key <- [address-key-base58]");
    model.add_derivation("address-key <- [derived-key]");
    node->set_derivation([&]() -> optional<HDKey> {
        if(model.address_key_base58->has_assigned_value()) {
            return model.address_key_base58->value();
        } else if(model.master_key->has_value() && model.full_address_derivation_path->has_value()) {
            auto master_key = model.master_key->value();
            auto path = model.full_address_derivation_path->value();
            return master_key.derive(KeyType::private_key(), path);
        } else if(model.account_key->has_value() && model.address_derivation_path->has_value()) {
            auto account_key = model.account_key->value();
            auto path = model.address_derivation_path->value();
            return account_key.derive(KeyType::private_key(), path);
        } else if(model.derived_key->has_value()) {
            return model.derived_key->value();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<HDKey>* setup_address_pub_key(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("address-pub-key", "UR:CRYPTO-KEY", "The BIP-32 address public HD key.");
    node->set_to_string([](const HDKey& key) { return key.ur(); });
    node->set_from_string([](const string& ur) -> HDKey { return HDKey::from_ur(ur); });
    model.add_derivation("address-pub-key <- [address-key]");
    model.add_derivation("address-pub-key <- [account-pub-key, address-derivation-path]");
    model.add_derivation("address-pub-key <- [address-pub-key-base58]");
    node->set_derivation([&]() -> optional<HDKey> {
        if(model.address_pub_key_base58->has_assigned_value()) {
            return model.address_pub_key_base58->value();
        } else if(model.address_key->has_value()) {
            return model.address_key->value().derive(KeyType::public_key());
        } else if(model.account_pub_key->has_value() && model.address_derivation_path->has_value()) {
            auto account_pub_key = model.account_pub_key->value();
            auto path = model.address_derivation_path->value();
            return account_pub_key.derive(KeyType::public_key(), path);
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<OutputDescriptor>* setup_output_descriptor(Model& model) {
    auto node = new DataNode<OutputDescriptor>();
    model.add_node(node);
    node->set_info("output-descriptor", "OUTPUT_DESCRIPTOR", "A single-signature output descriptor.");
    node->set_to_string([](const OutputDescriptor& o) -> string { return o.to_string(); });
    model.add_derivation("output-descriptor <- [output-type, account-derivation-path, address-derivation-path, account-pub-key]");
    node->set_derivation([&]() -> optional<OutputDescriptor> {
        if(model.output_type->has_value() && model.account_derivation_path->has_value(), model.address_derivation_path->has_value() && model.account_pub_key->has_value()) {
            return OutputDescriptor(model.output_type->value(), model.account_derivation_path->value(), model.address_derivation_path->value(), model.account_pub_key->value());
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<HDKey>* setup_master_key_base58(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("master-key-base58", "BASE58", "The BIP-32 master HD key.");
    node->set_to_string([](const HDKey& key) { return key.to_base58(); });
    node->set_from_string([](const string& base58) -> HDKey {
        auto k = HDKey::from_base58(base58);
        if(k.key_type() != KeyType::private_key()) {
            throw domain_error("master-key-base58 must be a private key.");
        }
        return k;
    });
    model.add_derivation("master-key-base58 <- [master-key]");
    node->set_derivation([&]() -> optional<HDKey> {
        return model.master_key->optional_value();
    });
    return node;
}

DataNode<HDKey>* setup_account_key_base58(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("account-key-base58", "BASE58", "The BIP-44 account HD key.");
    node->set_to_string([](const HDKey& key) { return key.to_base58(); });
    node->set_from_string([](const string& base58) -> HDKey {
        auto k = HDKey::from_base58(base58);
        if(k.key_type() != KeyType::private_key()) {
            throw domain_error("account-key-base58 must be a private key.");
        }
        return k;
    });
    model.add_derivation("account-key-base58 <- [account-key]");
    node->set_derivation([&]() -> optional<HDKey> {
        return model.account_key->optional_value();
    });
    return node;
}

DataNode<HDKey>* setup_account_pub_key_base58(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("account-pub-key-base58", "BASE58", "The BIP-44 account public HD key.");
    node->set_to_string([](const HDKey& key) { return key.to_base58(); });
    node->set_from_string([](const string& base58) -> HDKey { return HDKey::from_base58(base58); });
    model.add_derivation("account-pub-key-base58 <- [account-pub-key]");
    node->set_derivation([&]() -> optional<HDKey> {
        return model.account_pub_key->optional_value();
    });
    return node;
}

DataNode<HDKey>* setup_address_key_base58(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("address-key-base58", "BASE58", "The BIP-32 address HD key.");
    node->set_to_string([](const HDKey& key) { return key.to_base58(); });
    node->set_from_string([](const string& base58) -> HDKey {
        auto k = HDKey::from_base58(base58);
        if(k.key_type() != KeyType::private_key()) {
            throw domain_error("address-key-base58 must be a private key.");
        }
        return k;
    });
    model.add_derivation("address-key-base58 <- [address-key]");
    node->set_derivation([&]() -> optional<HDKey> {
        return model.address_key->optional_value();
    });
    return node;
}

DataNode<HDKey>* setup_address_pub_key_base58(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("address-pub-key-base58", "BASE58", "The BIP-32 address public HD key.");
    node->set_to_string([](const HDKey& key) { return key.to_base58(); });
    node->set_from_string([](const string& base58) -> HDKey { return HDKey::from_base58(base58); });
    model.add_derivation("address-pub-key-base58 <- [address-pub-key]");
    node->set_derivation([&]() -> optional<HDKey> {
        return model.address_pub_key->optional_value();
    });
    return node;
}
