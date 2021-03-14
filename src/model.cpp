#include "model.hpp"

#include <algorithm>

using namespace std;

static uint32_t parse_uint32(const string& s) {
    int n;
    try {
        n = stoi(s, nullptr, 0);
    } catch(exception &e) {
        throw domain_error("Expected integer >= 0.");
    }
    if(n < 0) {
        throw domain_error("Expected integer >= 0.");
    }
    return n;
}

Model::Model() {
    // seed
    seed = new DataNode<Seed>();
    seed->set_name("seed");
    seed->set_to_string([](const Seed& seed) { return seed.hex(); });
    seed->set_from_string([](const string& s) -> Seed { return Seed(s); });
    all_nodes.push_back(seed);

    // seed-ur <- [seed]
    seed_ur = new DataNode<string>();
    seed_ur->set_name("seed-ur");
    seed_ur->set_f([&]() -> optional<string> {
        if(seed->has_value()) {
            return seed->value().ur();
        } else {
            return nullopt;
        }
    });
    seed_ur->set_to_string([](const string& s) { return s; });
    all_nodes.push_back(seed_ur);

    // asset
    asset = new DataNode<Asset>();
    all_nodes.push_back(asset);
    asset->set_name("asset");
    asset->set_value(Asset::btc());
    asset->set_to_string([](const Asset& asset) { return asset.symbol(); });
    asset->set_from_string([](const string& symbol) -> Asset { return Asset::find(symbol); });

    // network <- asset
    network = new DataNode<Network>();
    all_nodes.push_back(network);
    network->set_name("network");
    network->set_f([&]() { return asset->value().network(); });
    network->set_to_string([](const Network& network) { return network.name(); });
    network->set_from_string([](const string& name) -> Network { return Network::find(name); });

    // master-key <- [network, seed]
    master_key = new DataNode<HDKey>();
    all_nodes.push_back(master_key);
    master_key->set_name("master-key");
    master_key->set_f([&]() -> optional<HDKey> {
        if(seed->has_value() && network->has_value()) {
            return HDKey(seed->value(), network->value());
        } else {
            return nullopt;
        }
    });
    master_key->set_to_string([](const HDKey& key) { return key.to_base58(true); });
    master_key->set_from_string([](const string& prv) -> HDKey { return HDKey(prv, true); });

    // master-key-fingerprint <- [master-key]
    master_key_fingerprint = new DataNode<ByteVector>();
    all_nodes.push_back(master_key_fingerprint);
    master_key_fingerprint->set_name("master-key-fingerprint");
    master_key_fingerprint->set_f([&]() -> optional<ByteVector> {
        if(master_key->has_value()) {
            return master_key->value().fingerprint();
        } else {
            return nullopt;
        }
    });
    master_key_fingerprint->set_to_string([](const ByteVector& bytes) { return data_to_hex(bytes); });
    master_key_fingerprint->set_from_string([](const string& hex) -> ByteVector { return hex_to_data(hex); });

    // output-type (default: pkh)
    output_type = new DataNode<OutputDescriptorType>();
    all_nodes.push_back(output_type);
    output_type->set_name("output-type");
    output_type->set_value(OutputDescriptorType::wpkh());
    output_type->set_to_string([](const OutputDescriptorType& d) { return d.name(); });
    output_type->set_from_string([](const string& name) -> OutputDescriptorType { return OutputDescriptorType::find(name); });

    // purpose <- [output-type]
    purpose = new DataNode<uint32_t>();
    all_nodes.push_back(purpose);
    purpose->set_name("purpose");
    purpose->set_f([&]() {
        return output_type->value().purpose();
    });
    purpose->set_to_string([](uint32_t n) { return to_string(n); });
    purpose->set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });

    // coin-type <- asset
    coin_type = new DataNode<uint32_t>();
    all_nodes.push_back(coin_type);
    coin_type->set_name("coin-type");
    coin_type->set_f([&]() {
        return asset->value().coin_type();
    });
    coin_type->set_to_string([](uint32_t n) { return to_string(n); });
    coin_type->set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });

    // account-index
    account_index = new DataNode<uint32_t>();
    all_nodes.push_back(account_index);
    account_index->set_name("account-index");
    account_index->set_value(0);
    account_index->set_to_string([](uint32_t n) { return to_string(n); });
    account_index->set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });

    // account-derivation-path <- [master-key-fingerprint, purpose, coin-type, account-index]
    account_derivation_path = new DataNode<DerivationPath>();
    all_nodes.push_back(account_derivation_path);
    account_derivation_path->set_name("account-derivation-path");
    account_derivation_path->set_f([&]() {
        DerivationPathElement m = master_key_fingerprint->has_value() ? DerivationPathElement(master_key_fingerprint->value()) : DerivationPathElement();
        return DerivationPath {
            m,
            DerivationPathElement(purpose->value(), true),
            DerivationPathElement(coin_type->value(), true),
            DerivationPathElement(account_index->value(), true)
        };
    });
    account_derivation_path->set_to_string([](const DerivationPath& path) { return to_string(path); });
    account_derivation_path->set_from_string([](const string& p) -> DerivationPath { return parse_derivation_path(p); });

    // account-key <- [master-key, account-derivation-path]
    account_key = new DataNode<HDKey>();
    all_nodes.push_back(account_key);
    account_key->set_name("account-key");
    account_key->set_f([&]() -> optional<HDKey> {
        if(master_key->has_value()) {
            return master_key->value().derive(account_derivation_path->value(), true);
        } else {
            return nullopt;
        }
    });
    account_key->set_to_string([](const HDKey& key) { return key.to_base58(true); });
    account_key->set_from_string([](const string& prv) -> HDKey { return HDKey(prv, true); });

    // account-pub-key <- [account-key]
    account_pub_key = new DataNode<HDKey>();
    all_nodes.push_back(account_pub_key);
    account_pub_key->set_name("account-pub-key");
    account_pub_key->set_f([&]() -> optional<HDKey> {
        if(account_key->has_value()) {
            return account_key->value().to_public();
        } else {
            return nullopt;
        }
    });
    account_pub_key->set_to_string([](const HDKey& key) { return key.to_base58(false); });
    account_pub_key->set_from_string([](const string& pub) -> HDKey { return HDKey(pub, false); });

    // chain-type
    chain_type = new DataNode<ChainType>();
    all_nodes.push_back(chain_type);
    chain_type->set_name("chain-type");
    chain_type->set_value(ChainType::external());
    chain_type->set_to_string([](const ChainType& t) { return t.to_string(); });
    chain_type->set_from_string([](const string& t) -> ChainType { return ChainType::find(t); });

    // chain-type-int <- [chain-type];
    chain_type_int = new DataNode<uint32_t>();
    all_nodes.push_back(chain_type_int);
    chain_type_int->set_name("chain-type-int");
    chain_type_int->set_f([&]() {
        return chain_type->value().index();
    });
    chain_type_int->set_to_string([](uint32_t n) { return to_string(n); });
    chain_type_int->set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });

    // address-index
    address_index = new DataNode<IndexBound>();
    all_nodes.push_back(address_index);
    address_index->set_name("address-index");
    address_index->set_value(0);
    address_index->set_to_string([](IndexBound n) { return n.to_string(); });
    address_index->set_from_string([](const string& s) -> IndexBound { return parse_index_range(s); });

    // address-derivation-path <- [chain-type-int, address-index]
    address_derivation_path = new DataNode<DerivationPath>();
    all_nodes.push_back(address_derivation_path);
    address_derivation_path->set_name("address-derivation-path");
    address_derivation_path->set_f([&]() {
        return DerivationPath {
            DerivationPathElement(chain_type_int->value(), false),
            DerivationPathElement(address_index->value(), false)
        };
    });
    address_derivation_path->set_to_string([](const DerivationPath& path) { return to_string(path); });
    address_derivation_path->set_from_string([](const string& p) -> DerivationPath { return parse_derivation_path(p); });

    // full-address-derivation-path <- [account-derivation-path, address-derivation-path]
    full_address_derivation_path = new DataNode<DerivationPath>();
    all_nodes.push_back(full_address_derivation_path);
    full_address_derivation_path->set_name("full-address-derivation-path");
    full_address_derivation_path->set_f([&]() {
        auto path = account_derivation_path->value();
        auto address_path = address_derivation_path->value();
        path.insert(path.end(), address_path.begin(), address_path.end());
        return path;
    });
    full_address_derivation_path->set_to_string([](const DerivationPath& path) { return to_string(path); });
    full_address_derivation_path->set_from_string([](const string& p) -> DerivationPath { return parse_derivation_path(p); });

    // address-key <- [master-key, full-address-derivation-path]
    // address-key <- [account-key, address-derivation-path]
    address_key = new DataNode<HDKey>();
    all_nodes.push_back(address_key);
    address_key->set_name("address-key");
    address_key->set_f([&]() -> optional<HDKey> {
        if(master_key->has_value() && full_address_derivation_path->has_value()) {
            return master_key->value().derive(full_address_derivation_path->value(), true);
        } else if(account_key->has_value() && address_derivation_path->has_value()) {
            return account_key->value().derive(address_derivation_path->value(), true);
        } else {
            return nullopt;
        }
    });
    address_key->set_to_string([](const HDKey& key) { return key.to_base58(true); });
    address_key->set_from_string([](const string& prv) -> HDKey { return HDKey(prv, true); });

    // address-pub-key <- [address-key]
    // address-pub-key <- [account-pub-key, address-derivation-path]
    address_pub_key = new DataNode<HDKey>();
    all_nodes.push_back(address_pub_key);
    address_pub_key->set_name("address-pub-key");
    address_pub_key->set_f([&]() -> optional<HDKey> {
        if(address_key->has_value()) {
            return address_key->value().to_public();
        } else if(account_pub_key->has_value() && address_derivation_path->has_value()) {
            return account_pub_key->value().derive(address_derivation_path->value(), false);
        } else {
            return nullopt;
        }
    });
    address_pub_key->set_to_string([](const HDKey& key) { return key.to_base58(false); });
    address_pub_key->set_from_string([](const string& pub) -> HDKey { return HDKey(pub, false); });

    // address-ec-key <- [address-key]
    // address-ec-key <- [address-ec-key-wif, network]
    address_ec_key = new DataNode<ECPrivateKey>();
    all_nodes.push_back(address_ec_key);
    address_ec_key->set_name("address-ec-key");
    address_ec_key->set_f([&]() -> optional<ECPrivateKey> {
        if(address_key->has_value()) {
            return Wally::instance.bip32_key_to_ec_private(address_key->value());
        } else if(address_ec_key_wif->has_value()) {
            return Wally::instance.wif_to_ec_key(address_ec_key_wif->value(), network->value());
        } else {
            return nullopt;
        }
    });
    address_ec_key->set_to_string([](const ECPrivateKey& key) { return key.to_hex(); });
    address_ec_key->set_from_string([](const string& k) -> ECPrivateKey { return ECPrivateKey(hex_to_data(k)); });

    // address-ec-key-wif <- [address-ec-key, network]
    address_ec_key_wif = new DataNode<string>();
    all_nodes.push_back(address_ec_key_wif);
    address_ec_key_wif->set_name("address-ec-key-wif");
    address_ec_key_wif->set_f([&]() -> optional<string> {
        if(address_ec_key->has_value()) {
            return Wally::instance.ec_key_to_wif(address_ec_key->value(), network->value());
        } else {
            return nullopt;
        }
    });
    address_ec_key_wif->set_to_string([](const string& s) { return s; });
    address_ec_key_wif->set_from_string([](const string& wif) -> string { return wif; });

    // address-pub-ec-key <- [address-ec-key]
    // address-pub-ec-key <- [address-pub-key]
    address_pub_ec_key = new DataNode<ECCompressedPublicKey>();
    all_nodes.push_back(address_pub_ec_key);
    address_pub_ec_key->set_name("address-pub-ec-key");
    address_pub_ec_key->set_f([&]() -> optional<ECCompressedPublicKey> {
        if(address_pub_key->has_value()) {
            return Wally::instance.bip32_key_to_ec_public(address_pub_key->value());
        } else if(address_ec_key->has_value()) {
            return address_ec_key->value().to_public();
        } else {
            return nullopt;
        }
    });
    address_pub_ec_key->set_to_string([](const ECCompressedPublicKey& key) { return key.to_hex(); });
    address_pub_ec_key->set_from_string([](const string& k) -> ECCompressedPublicKey { return ECCompressedPublicKey(hex_to_data(k)); });

    // address-pkh <- [address-pub-ec-key, asset]
    address_pkh = new DataNode<string>();
    all_nodes.push_back(address_pkh);
    address_pkh->set_name("address-pkh");
    address_pkh->set_f([&]() -> optional<string> {
        if(address_pub_ec_key->has_value()) {
            return Wally::instance.to_address(address_pub_ec_key->value(), asset->value(), false);
        } else {
            return nullopt;
        }
    });
    address_pkh->set_to_string([](const string& s) { return s; });
    address_pkh->set_from_string([](const string& a) -> string { return a; });

    // address-sh <- [address-pub-ec-key, asset]
    address_sh = new DataNode<string>();
    all_nodes.push_back(address_sh);
    address_sh->set_name("address-sh");
    address_sh->set_f([&]() -> optional<string> {
        if(address_pub_ec_key->has_value()) {
            return Wally::instance.to_address(address_pub_ec_key->value(), asset->value(), true);
        } else {
            return nullopt;
        }
    });
    address_sh->set_to_string([](const string& s) { return s; });
    address_sh->set_from_string([](const string& a) -> string { return a; });

    // address-segwit <- [address-pub-key, network]
    address_segwit = new DataNode<string>();
    all_nodes.push_back(address_segwit);
    address_segwit->set_name("address-segwit");
    address_segwit->set_f([&]() -> optional<string> {
        if(address_pub_key->has_value()) {
            return Wally::instance.to_segwit_address(address_pub_key->value(), network->value());
        } else {
            return nullopt;
        }
    });
    address_segwit->set_to_string([](const string& s) { return s; });
    address_segwit->set_from_string([](const string& a) -> string { return a; });

    // output-descriptor <- [output_type, account_derivation_path, address_derivation_path, account_pub_key]
    output_descriptor = new DataNode<OutputDescriptor>();
    all_nodes.push_back(output_descriptor);
    output_descriptor->set_name("output-descriptor");
    output_descriptor->set_f([&]() -> optional<OutputDescriptor> {
        if(output_type->has_value() && account_derivation_path->has_value(), address_derivation_path->has_value() && account_pub_key->has_value()) {
            return OutputDescriptor(output_type->value(), account_derivation_path->value(), address_derivation_path->value(), account_pub_key->value());
        } else {
            return nullopt;
        }
    });
    output_descriptor->set_to_string([](const OutputDescriptor& o) -> string { return o.to_string(); });

    // psbt
    psbt = new DataNode<PSBT>();
    all_nodes.push_back(psbt);
    psbt->set_name("psbt");
    psbt->set_to_string([](const PSBT& p) { return p.base64(); });
    psbt->set_from_string([](const string& s) -> PSBT { return PSBT(s); });

    // psbt-hex <- [psbt]
    psbt_hex = new DataNode<string>();
    all_nodes.push_back(psbt_hex);
    psbt_hex->set_name("psbt-hex");
    psbt_hex->set_f([&]() -> optional<string> {
        if(psbt->has_value()) {
            return psbt->value().hex();
        } else {
            return nullopt;
        }
    });
    psbt_hex->set_to_string([](const string& s) { return s; });

    // psbt-ur <- [psbt]
    psbt_ur = new DataNode<string>();
    all_nodes.push_back(psbt_ur);
    psbt_ur->set_name("psbt-ur");
    psbt_ur->set_f([&]() -> optional<string> {
        if(psbt->has_value()) {
            return psbt->value().ur();
        } else {
            return nullopt;
        }
    });
    psbt_ur->set_to_string([](const string& s) { return s; });

    // psbt-finalized <- [psbt]
    psbt_finalized = new DataNode<PSBT>();
    all_nodes.push_back(psbt_finalized);
    psbt_finalized->set_name("psbt-finalized");
    psbt_finalized->set_f([&]() -> optional<PSBT> {
        if(psbt->has_value()) {
            return psbt->value().finalized();
        } else {
            return nullopt;
        }
    });
    psbt_finalized->set_to_string([](const PSBT& p) { return p.base64(); });
    psbt_finalized->set_from_string([](const string& s) -> PSBT {
        auto psbt = PSBT(s);
        if(!psbt.is_finalized()) {
            throw domain_error("Cannot assign non-finalized PSBT.");
        }
        return psbt;
    });

    // psbt-finalized-hex <- [psbt-finalized]
    psbt_finalized_hex = new DataNode<string>();
    all_nodes.push_back(psbt_finalized_hex);
    psbt_finalized_hex->set_name("psbt-finalized-hex");
    psbt_finalized_hex->set_f([&]() -> optional<string> {
        if(psbt_finalized->has_value()) {
            return psbt_finalized->value().hex();
        } else {
            return nullopt;
        }
    });
    psbt_finalized_hex->set_to_string([](const string& s) { return s; });

    // psbt-finalized-ur <- [psbt-finalized]
    psbt_finalized_ur = new DataNode<string>();
    all_nodes.push_back(psbt_finalized_ur);
    psbt_finalized_ur->set_name("psbt-finalized-ur");
    psbt_finalized_ur->set_f([&]() -> optional<string> {
        if(psbt_finalized->has_value()) {
            return psbt_finalized->value().ur();
        } else {
            return nullopt;
        }
    });
    psbt_finalized_ur->set_to_string([](const string& s) { return s; });

    // psbt-signed <- [psbt, address-ec-key]
    psbt_signed = new DataNode<PSBT>();
    all_nodes.push_back(psbt_signed);
    psbt_signed->set_name("psbt-signed");
    psbt_signed->set_f([&]() -> optional<PSBT> {
        if(psbt->has_value() && address_ec_key->has_value()) {
            return psbt->value().sign(address_ec_key->value());
        } else {
            return nullopt;
        }
    });
    psbt_signed->set_to_string([](const PSBT& p) { return p.base64(); });

    // psbt-signed-hex <- [psbt-signed]
    psbt_signed_hex = new DataNode<string>();
    all_nodes.push_back(psbt_signed_hex);
    psbt_signed_hex->set_name("psbt-signed-hex");
    psbt_signed_hex->set_f([&]() -> optional<string> {
        if(psbt_signed->has_value()) {
            return psbt_signed->value().hex();
        } else {
            return nullopt;
        }
    });
    psbt_signed_hex->set_to_string([](const string& s) { return s; });

    // psbt-signed-ur <- [psbt-signed]
    psbt_signed_ur = new DataNode<string>();
    all_nodes.push_back(psbt_signed_ur);
    psbt_signed_ur->set_name("psbt-signed-ur");
    psbt_signed_ur->set_f([&]() -> optional<string> {
        if(psbt_signed->has_value()) {
            return psbt_signed->value().ur();
        } else {
            return nullopt;
        }
    });
    psbt_signed_ur->set_to_string([](const string& s) { return s; });

    // transaction <- [psbt-finalized]
    transaction = new DataNode<Transaction>();
    all_nodes.push_back(transaction);
    transaction->set_name("transaction");
    transaction->set_f([&]() -> optional<Transaction> {
        if(psbt_finalized->has_value()) {
            return Transaction(psbt_finalized->value());
        } else {
            return nullopt;
        }
    });
    transaction->set_to_string([](const Transaction& t) { return t.hex(); });
    transaction->set_from_string([](const string& s) -> Transaction { return Transaction(s); });

    // transaction-ur <- [transaction]
    transaction_ur = new DataNode<string>();
    all_nodes.push_back(transaction_ur);
    transaction_ur->set_name("transaction-ur");
    transaction_ur->set_f([&]() -> optional<string> {
        if(transaction->has_value()) {
            return transaction->value().ur();
        } else {
            return nullopt;
        }
    });
    transaction_ur->set_to_string([](const string& s) { return s; });

    int next_tag = -1;
    for(auto node: all_nodes) {
        node->set_tag(next_tag);
        next_tag -= 1;
    }
}

DataNodeProtocol* Model::find_by_name(const string& node_name) const {
    auto node = std::find_if(all_nodes.begin(), all_nodes.end(), [&](auto node) { return node->name() == node_name; });
    return node == all_nodes.end() ? nullptr : *node;
}

DataNodeProtocol* Model::find_by_tag(int node_tag) const {
    auto node = std::find_if(all_nodes.begin(), all_nodes.end(), [&](auto node) { return node->tag() == node_tag; });
    return node == all_nodes.end() ? nullptr : *node;
}

bool Model::is_valid_name(const string& node_name) const {
    return find_by_name(node_name) != nullptr;
}

// cppcheck-suppress unusedFunction
bool Model::can_derive(const string& node_name) const {
    auto node = find_by_name(node_name);
    if(node == nullptr) {
        return false;
    }
    return node->has_value();
}
