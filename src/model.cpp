#include "model.hpp"
#include "model-seed.hpp"

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

void Model::add_node(DataNodeProtocol* n) {
    _all_nodes.push_back(n);
}

void Model::add_derivation(const std::string& d) {
    _derivations.push_back(d);
}

Model::Model() {
    seed = setup_seed(*this);
    seed_name = setup_seed_name(*this);
    seed_note = setup_seed_note(*this);
    seed_ur = setup_seed_ur(*this);
    seed_digest = setup_seed_digest(*this);

    add_derivation("asset");
    asset = new DataNode<Asset>();
    add_node(asset);
    asset->set_info("asset", "ENUM btc|btct", "A cryptocurrency asset.");
    asset->set_to_string([](const Asset& asset) { return asset.symbol(); });
    asset->set_from_string([](const string& symbol) -> Asset { return Asset::find(symbol); });
    asset->set_value(Asset::btc());

    add_derivation("network <- [asset]");
    network = new DataNode<Network>();
    add_node(network);
    network->set_info("network", "ENUM mainnet|testnet", "The network.");
    network->set_to_string([](const Network& network) { return network.name(); });
    network->set_from_string([](const string& name) -> Network { return Network::find(name); });
    network->set_derivation([&]() {
        return asset->value().network();
    });

    add_derivation("master-key <- [network, seed]");
    master_key = new DataNode<HDKey>();
    add_node(master_key);
    master_key->set_info("master-key", "XPRV", "The BIP-32 master HD key.");
    master_key->set_to_string([](const HDKey& key) { return key.to_base58(true); });
    master_key->set_from_string([](const string& prv) -> HDKey { return HDKey(prv, true); });
    master_key->set_derivation([&]() -> optional<HDKey> {
        if(seed->has_value() && network->has_value()) {
            return HDKey(seed->value(), network->value());
        } else {
            return nullopt;
        }
    });

    add_derivation("master-key-fingerprint <- [master-key]");
    master_key_fingerprint = new DataNode<ByteVector>();
    add_node(master_key_fingerprint);
    master_key_fingerprint->set_info("master-key-fingerprint", "HEX", "Fingerprint of the master HD key.");
    master_key_fingerprint->set_to_string([](const ByteVector& bytes) { return data_to_hex(bytes); });
    master_key_fingerprint->set_from_string([](const string& hex) -> ByteVector { return hex_to_data(hex); });
    master_key_fingerprint->set_derivation([&]() -> optional<ByteVector> {
        if(master_key->has_value()) {
            return master_key->value().fingerprint();
        } else {
            return nullopt;
        }
    });

    add_derivation("output-type (default: pkh)");
    output_type = new DataNode<OutputDescriptorType>();
    add_node(output_type);
    output_type->set_info("output-type", "ENUM pkh|wpkh|sh-wpkh", "The output descriptor type.");
    output_type->set_to_string([](const OutputDescriptorType& d) { return d.name(); });
    output_type->set_from_string([](const string& name) -> OutputDescriptorType { return OutputDescriptorType::find(name); });
    output_type->set_value(OutputDescriptorType::wpkh());

    add_derivation("purpose <- [output-type]");
    purpose = new DataNode<uint32_t>();
    add_node(purpose);
    purpose->set_info("purpose", "INDEX", "The purpose field of the BIP-44 derivation path.");
    purpose->set_to_string([](uint32_t n) { return to_string(n); });
    purpose->set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });
    purpose->set_derivation([&]() {
        return output_type->value().purpose();
    });

    add_derivation("coin-type <- asset");
    coin_type = new DataNode<uint32_t>();
    add_node(coin_type);
    coin_type->set_info("coin-type", "INDEX", "The coin type field of the BIP-44 derivation path.");
    coin_type->set_to_string([](uint32_t n) { return to_string(n); });
    coin_type->set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });
    coin_type->set_derivation([&]() {
        return asset->value().coin_type();
    });

    add_derivation("account-index (default: 0)");
    account_index = new DataNode<uint32_t>();
    add_node(account_index);
    account_index->set_info("account-index", "INDEX", "The account field of the BIP-44 derivation path.");
    account_index->set_to_string([](uint32_t n) { return to_string(n); });
    account_index->set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });
    account_index->set_value(0);

    add_derivation("account-derivation-path <- [master-key-fingerprint, purpose, coin-type, account-index]");
    account_derivation_path = new DataNode<DerivationPath>();
    add_node(account_derivation_path);
    account_derivation_path->set_info("account-derivation-path", "BIP32_PATH", "m/purpose'/coin-type'/accont-index'.");
    account_derivation_path->set_to_string([](const DerivationPath& path) { return to_string(path); });
    account_derivation_path->set_from_string([](const string& p) -> DerivationPath { return parse_derivation_path(p); });
    account_derivation_path->set_derivation([&]() {
        DerivationPathElement m = master_key_fingerprint->has_value() ? DerivationPathElement(master_key_fingerprint->value()) : DerivationPathElement();
        return DerivationPath {
            m,
            DerivationPathElement(purpose->value(), true),
            DerivationPathElement(coin_type->value(), true),
            DerivationPathElement(account_index->value(), true)
        };
    });

    add_derivation("account-key <- [master-key, account-derivation-path]");
    account_key = new DataNode<HDKey>();
    add_node(account_key);
    account_key->set_info("account-key", "XPRV", "The BIP-44 account key.");
    account_key->set_to_string([](const HDKey& key) { return key.to_base58(true); });
    account_key->set_from_string([](const string& prv) -> HDKey { return HDKey(prv, true); });
    account_key->set_derivation([&]() -> optional<HDKey> {
        if(master_key->has_value()) {
            return master_key->value().derive(account_derivation_path->value(), true);
        } else {
            return nullopt;
        }
    });

    add_derivation("account-pub-key <- [account-key]");
    account_pub_key = new DataNode<HDKey>();
    add_node(account_pub_key);
    account_pub_key->set_info("account-pub-key", "XPUB", "The BIP-44 account public key.");
    account_pub_key->set_to_string([](const HDKey& key) { return key.to_base58(false); });
    account_pub_key->set_from_string([](const string& pub) -> HDKey { return HDKey(pub, false); });
    account_pub_key->set_derivation([&]() -> optional<HDKey> {
        if(account_key->has_value()) {
            return account_key->value().to_public();
        } else {
            return nullopt;
        }
    });

    add_derivation("chain-type (default: external)");
    chain_type = new DataNode<ChainType>();
    add_node(chain_type);
    chain_type->set_info("chain-type", "ENUM internal|external|identity", "The BIP-44 chain type (change) field.");
    chain_type->set_to_string([](const ChainType& t) { return t.to_string(); });
    chain_type->set_from_string([](const string& t) -> ChainType { return ChainType::find(t); });
    chain_type->set_value(ChainType::external());

    add_derivation("chain-type-int <- [chain-type];");
    chain_type_int = new DataNode<uint32_t>();
    add_node(chain_type_int);
    chain_type_int->set_info("chain-type-int", "INDEX", "The BIP-44 change field integer value.");
    chain_type_int->set_to_string([](uint32_t n) { return to_string(n); });
    chain_type_int->set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });
    chain_type_int->set_derivation([&]() {
        return chain_type->value().index();
    });

    add_derivation("address-index (default: 0)");
    address_index = new DataNode<IndexBound>();
    add_node(address_index);
    address_index->set_info("address-index", "INDEX_BOUND", "The BIP-44 address_index field. '*' is allowed for output descriptors.");
    address_index->set_to_string([](IndexBound n) { return n.to_string(); });
    address_index->set_from_string([](const string& s) -> IndexBound { return parse_index_range(s); });
    address_index->set_value(0);

    add_derivation("address-derivation-path <- [chain-type-int, address-index]");
    address_derivation_path = new DataNode<DerivationPath>();
    add_node(address_derivation_path);
    address_derivation_path->set_info("address-derivation-path", "BIP32_PATH", "The BIP-32 address derivation path, starting from the account-key.");
    address_derivation_path->set_to_string([](const DerivationPath& path) { return to_string(path); });
    address_derivation_path->set_from_string([](const string& p) -> DerivationPath { return parse_derivation_path(p); });
    address_derivation_path->set_derivation([&]() {
        return DerivationPath {
            DerivationPathElement(chain_type_int->value(), false),
            DerivationPathElement(address_index->value(), false)
        };
    });

    add_derivation("full-address-derivation-path <- [account-derivation-path, address-derivation-path]");
    full_address_derivation_path = new DataNode<DerivationPath>();
    add_node(full_address_derivation_path);
    full_address_derivation_path->set_info("full-address-derivation-path", "BIP32_PATH", "The BIP-32 address derivation path, starting from the master-key.");
    full_address_derivation_path->set_to_string([](const DerivationPath& path) { return to_string(path); });
    full_address_derivation_path->set_from_string([](const string& p) -> DerivationPath { return parse_derivation_path(p); });
    full_address_derivation_path->set_derivation([&]() {
        auto path = account_derivation_path->value();
        auto address_path = address_derivation_path->value();
        path.insert(path.end(), address_path.begin(), address_path.end());
        return path;
    });

    add_derivation("address-key <- [master-key, full-address-derivation-path]");
    add_derivation("address-key <- [account-key, address-derivation-path]");
    address_key = new DataNode<HDKey>();
    add_node(address_key);
    address_key->set_info("address-key", "XPRV", "The BIP-32 address HD key.");
    address_key->set_to_string([](const HDKey& key) { return key.to_base58(true); });
    address_key->set_from_string([](const string& prv) -> HDKey { return HDKey(prv, true); });
    address_key->set_derivation([&]() -> optional<HDKey> {
        if(master_key->has_value() && full_address_derivation_path->has_value()) {
            return master_key->value().derive(full_address_derivation_path->value(), true);
        } else if(account_key->has_value() && address_derivation_path->has_value()) {
            return account_key->value().derive(address_derivation_path->value(), true);
        } else {
            return nullopt;
        }
    });

    add_derivation("address-pub-key <- [address-key]");
    add_derivation("address-pub-key <- [account-pub-key, address-derivation-path]");
    address_pub_key = new DataNode<HDKey>();
    add_node(address_pub_key);
    address_pub_key->set_info("address-pub-key", "XPUB", "The BIP-32 address public HD key.");
    address_pub_key->set_to_string([](const HDKey& key) { return key.to_base58(false); });
    address_pub_key->set_from_string([](const string& pub) -> HDKey { return HDKey(pub, false); });
    address_pub_key->set_derivation([&]() -> optional<HDKey> {
        if(address_key->has_value()) {
            return address_key->value().to_public();
        } else if(account_pub_key->has_value() && address_derivation_path->has_value()) {
            return account_pub_key->value().derive(address_derivation_path->value(), false);
        } else {
            return nullopt;
        }
    });

    add_derivation("address-ec-key <- [address-key]");
    add_derivation("address-ec-key <- [address-ec-key-wif, network]");
    address_ec_key = new DataNode<ECPrivateKey>();
    add_node(address_ec_key);
    address_ec_key->set_info("address-ec-key", "ECPRV", "The address EC key.");
    address_ec_key->set_to_string([](const ECPrivateKey& key) { return key.to_hex(); });
    address_ec_key->set_from_string([](const string& k) -> ECPrivateKey { return ECPrivateKey(hex_to_data(k)); });
    address_ec_key->set_derivation([&]() -> optional<ECPrivateKey> {
        if(address_key->has_value()) {
            return Wally::instance.bip32_key_to_ec_private(address_key->value());
        } else if(address_ec_key_wif->has_value()) {
            return Wally::instance.wif_to_ec_key(address_ec_key_wif->value(), network->value());
        } else {
            return nullopt;
        }
    });

    add_derivation("address-ec-key-wif <- [address-ec-key, network]");
    address_ec_key_wif = new DataNode<string>();
    add_node(address_ec_key_wif);
    address_ec_key_wif->set_info("address-ec-key-wif", "WIF", "The address EC key in WIF format.");
    address_ec_key_wif->set_to_string([](const string& s) { return s; });
    address_ec_key_wif->set_from_string([](const string& wif) -> string { return wif; });
    address_ec_key_wif->set_derivation([&]() -> optional<string> {
        if(address_ec_key->has_value()) {
            return Wally::instance.ec_key_to_wif(address_ec_key->value(), network->value());
        } else {
            return nullopt;
        }
    });

    add_derivation("address-pub-ec-key <- [address-ec-key]");
    add_derivation("address-pub-ec-key <- [address-pub-key]");
    address_pub_ec_key = new DataNode<ECCompressedPublicKey>();
    add_node(address_pub_ec_key);
    address_pub_ec_key->set_info("address-pub-ec-key", "ECPUB", "The compressed public EC key.");
    address_pub_ec_key->set_to_string([](const ECCompressedPublicKey& key) { return key.to_hex(); });
    address_pub_ec_key->set_from_string([](const string& k) -> ECCompressedPublicKey { return ECCompressedPublicKey(hex_to_data(k)); });
    address_pub_ec_key->set_derivation([&]() -> optional<ECCompressedPublicKey> {
        if(address_pub_key->has_value()) {
            return Wally::instance.bip32_key_to_ec_public(address_pub_key->value());
        } else if(address_ec_key->has_value()) {
            return address_ec_key->value().to_public();
        } else {
            return nullopt;
        }
    });

    add_derivation("address-pkh <- [address-pub-ec-key, asset]");
    address_pkh = new DataNode<string>();
    add_node(address_pkh);
    address_pkh->set_info("address-pkh", "ADDRESS", "The pay-to-public-key-hash address.");
    address_pkh->set_to_string([](const string& s) { return s; });
    address_pkh->set_from_string([](const string& a) -> string { return a; });
    address_pkh->set_derivation([&]() -> optional<string> {
        if(address_pub_ec_key->has_value()) {
            return Wally::instance.to_address(address_pub_ec_key->value(), asset->value(), false);
        } else {
            return nullopt;
        }
    });

    add_derivation("address-sh <- [address-pub-ec-key, asset]");
    address_sh = new DataNode<string>();
    add_node(address_sh);
    address_sh->set_info("address-sh", "ADDRESS", "The pay-to-script-hash address.");
    address_sh->set_to_string([](const string& s) { return s; });
    address_sh->set_from_string([](const string& a) -> string { return a; });
    address_sh->set_derivation([&]() -> optional<string> {
        if(address_pub_ec_key->has_value()) {
            return Wally::instance.to_address(address_pub_ec_key->value(), asset->value(), true);
        } else {
            return nullopt;
        }
    });

    add_derivation("address-segwit <- [address-pub-key, network]");
    address_segwit = new DataNode<string>();
    add_node(address_segwit);
    address_segwit->set_info("address-segwit", "ADDRESS", "The segwit address.");
    address_segwit->set_to_string([](const string& s) { return s; });
    address_segwit->set_from_string([](const string& a) -> string { return a; });
    address_segwit->set_derivation([&]() -> optional<string> {
        if(address_pub_key->has_value()) {
            return Wally::instance.to_segwit_address(address_pub_key->value(), network->value());
        } else {
            return nullopt;
        }
    });

    add_derivation("output-descriptor <- [output_type, account_derivation_path, address_derivation_path, account_pub_key]");
    output_descriptor = new DataNode<OutputDescriptor>();
    add_node(output_descriptor);
    output_descriptor->set_info("output-descriptor", "OUTPUT_DESCRIPTOR", "A single-signature output descriptor.");
    output_descriptor->set_to_string([](const OutputDescriptor& o) -> string { return o.to_string(); });
    output_descriptor->set_derivation([&]() -> optional<OutputDescriptor> {
        if(output_type->has_value() && account_derivation_path->has_value(), address_derivation_path->has_value() && account_pub_key->has_value()) {
            return OutputDescriptor(output_type->value(), account_derivation_path->value(), address_derivation_path->value(), account_pub_key->value());
        } else {
            return nullopt;
        }
    });

    add_derivation("psbt");
    psbt = new DataNode<PSBT>();
    add_node(psbt);
    psbt->set_info("psbt", "BASE64 | HEX | UR:CRYPTO-PSBT", "A partially signed Bitcoin transaction (PSBT).");
    psbt->set_to_string([](const PSBT& p) { return p.base64(); });
    psbt->set_from_string([](const string& s) -> PSBT { return PSBT(s); });

    add_derivation("psbt-hex <- [psbt]");
    psbt_hex = new DataNode<string>();
    add_node(psbt_hex);
    psbt_hex->set_info("psbt-hex", "HEX", "PSBT in hex format.");
    psbt_hex->set_to_string([](const string& s) { return s; });
    psbt_hex->set_derivation([&]() -> optional<string> {
        if(psbt->has_value()) {
            return psbt->value().hex();
        } else {
            return nullopt;
        }
    });

    add_derivation("psbt-ur <- [psbt]");
    psbt_ur = new DataNode<string>();
    add_node(psbt_ur);
    psbt_ur->set_info("psbt-ur", "UR:CRYPTO-PSBT", "A PSBT in UR format.");
    psbt_ur->set_to_string([](const string& s) { return s; });
    psbt_ur->set_derivation([&]() -> optional<string> {
        if(psbt->has_value()) {
            return psbt->value().ur();
        } else {
            return nullopt;
        }
    });

    add_derivation("psbt-finalized <- [psbt]");
    psbt_finalized = new DataNode<PSBT>();
    add_node(psbt_finalized);
    psbt_finalized->set_info("psbt-finalized", "BASE64 | HEX | UR:CRYPTO-PSBT", "The finalized PSBT.");
    psbt_finalized->set_to_string([](const PSBT& p) { return p.base64(); });
    psbt_finalized->set_from_string([](const string& s) -> PSBT {
        auto psbt = PSBT(s);
        if(!psbt.is_finalized()) {
            throw domain_error("Cannot assign non-finalized PSBT.");
        }
        return psbt;
    });
    psbt_finalized->set_derivation([&]() -> optional<PSBT> {
        if(psbt->has_value()) {
            return psbt->value().finalized();
        } else {
            return nullopt;
        }
    });

    add_derivation("psbt-finalized-hex <- [psbt-finalized]");
    psbt_finalized_hex = new DataNode<string>();
    add_node(psbt_finalized_hex);
    psbt_finalized_hex->set_info("psbt-finalized-hex", "HEX", "Finalized PSBT in hex format.");
    psbt_finalized_hex->set_to_string([](const string& s) { return s; });
    psbt_finalized_hex->set_derivation([&]() -> optional<string> {
        if(psbt_finalized->has_value()) {
            return psbt_finalized->value().hex();
        } else {
            return nullopt;
        }
    });

    add_derivation("psbt-finalized-ur <- [psbt-finalized]");
    psbt_finalized_ur = new DataNode<string>();
    add_node(psbt_finalized_ur);
    psbt_finalized_ur->set_info("psbt-finalized-ur", "UR:CRYPTO-PSBT", "Finalized PSBT in UR format.");
    psbt_finalized_ur->set_to_string([](const string& s) { return s; });
    psbt_finalized_ur->set_derivation([&]() -> optional<string> {
        if(psbt_finalized->has_value()) {
            return psbt_finalized->value().ur();
        } else {
            return nullopt;
        }
    });

    add_derivation("psbt-signed <- [psbt, address-ec-key]");
    psbt_signed = new DataNode<PSBT>();
    add_node(psbt_signed);
    psbt_signed->set_info("psbt-signed", "BASE64 | HEX | UR:CRYPTO-PSBT", "A PBST signed by address-key.");
    psbt_signed->set_to_string([](const PSBT& p) { return p.base64(); });
    psbt_signed->set_derivation([&]() -> optional<PSBT> {
        if(psbt->has_value() && address_ec_key->has_value()) {
            return psbt->value().sign(address_ec_key->value());
        } else {
            return nullopt;
        }
    });

    add_derivation("psbt-signed-hex <- [psbt-signed]");
    psbt_signed_hex = new DataNode<string>();
    add_node(psbt_signed_hex);
    psbt_signed_hex->set_info("psbt-signed-hex", "HEX", "Signed PSBT in hex format.");
    psbt_signed_hex->set_to_string([](const string& s) { return s; });
    psbt_signed_hex->set_derivation([&]() -> optional<string> {
        if(psbt_signed->has_value()) {
            return psbt_signed->value().hex();
        } else {
            return nullopt;
        }
    });

    add_derivation("psbt-signed-ur <- [psbt-signed]");
    psbt_signed_ur = new DataNode<string>();
    add_node(psbt_signed_ur);
    psbt_signed_ur->set_info("psbt-signed-ur", "UR:CRYPTO-PSBT", "Signed PSBT in UR format.");
    psbt_signed_ur->set_to_string([](const string& s) { return s; });
    psbt_signed_ur->set_derivation([&]() -> optional<string> {
        if(psbt_signed->has_value()) {
            return psbt_signed->value().ur();
        } else {
            return nullopt;
        }
    });

    add_derivation("transaction <- [psbt-finalized]");
    transaction = new DataNode<Transaction>();
    add_node(transaction);
    transaction->set_info("transaction", "HEX | UR:CRYPTO-TX", "The raw Bitcoin transaction.");
    transaction->set_to_string([](const Transaction& t) { return t.hex(); });
    transaction->set_from_string([](const string& s) -> Transaction { return Transaction(s); });
    transaction->set_derivation([&]() -> optional<Transaction> {
        if(psbt_finalized->has_value()) {
            return Transaction(psbt_finalized->value());
        } else {
            return nullopt;
        }
    });

    add_derivation("transaction-ur <- [transaction]");
    transaction_ur = new DataNode<string>();
    add_node(transaction_ur);
    transaction_ur->set_info("transaction-ur", "UR:CRYPTO-TX", "The raw Bitcoin transaction in UR format.");
    transaction_ur->set_to_string([](const string& s) { return s; });
    transaction_ur->set_derivation([&]() -> optional<string> {
        if(transaction->has_value()) {
            return transaction->value().ur();
        } else {
            return nullopt;
        }
    });

    int next_tag = -1;
    for(auto node: all_nodes()) {
        node->set_tag(next_tag);
        next_tag -= 1;
    }
}

DataNodeProtocol* Model::find_by_name(const string& node_name) const {
    auto node = std::find_if(all_nodes().begin(), all_nodes().end(), [&](auto node) { return node->name() == node_name; });
    return node == all_nodes().end() ? nullptr : *node;
}

DataNodeProtocol* Model::find_by_tag(int node_tag) const {
    auto node = std::find_if(all_nodes().begin(), all_nodes().end(), [&](auto node) { return node->tag() == node_tag; });
    return node == all_nodes().end() ? nullptr : *node;
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
