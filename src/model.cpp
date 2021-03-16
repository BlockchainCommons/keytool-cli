#include "model.hpp"
#include "model-eckey.hpp"
#include "model-hdkey.hpp"
#include "model-psbt.hpp"
#include "model-seed.hpp"

#include <algorithm>

using namespace std;

void Model::add_node(DataNodeProtocol* n) {
    _all_nodes.push_back(n);
}

void Model::add_derivation(const std::string& d) {
    _derivations.push_back(d);
}

Model::Model() {
    // seed
    seed = setup_seed(*this);
    seed_name = setup_seed_name(*this);
    seed_note = setup_seed_note(*this);
    seed_ur = setup_seed_ur(*this);
    seed_digest = setup_seed_digest(*this);

    // ec-key
    asset = setup_asset(*this);
    network = setup_network(*this);

    // hd-key
    master_key = setup_master_key(*this);
    master_key_fingerprint = setup_master_key_fingerprint(*this);
    output_type = setup_output_type(*this);
    purpose = setup_purpose(*this);
    coin_type = setup_coin_type(*this);
    account_index = setup_account_index(*this);
    account_derivation_path = setup_account_derivation_path(*this);

    account_key = new DataNode<HDKey>();
    add_node(account_key);
    account_key->set_info("account-key", "XPRV", "The BIP-44 account key.");
    account_key->set_to_string([](const HDKey& key) { return key.to_base58(true); });
    account_key->set_from_string([](const string& prv) -> HDKey { return HDKey(prv, true); });
    add_derivation("account-key <- [master-key, account-derivation-path]");
    account_key->set_derivation([&]() -> optional<HDKey> {
        if(master_key->has_value()) {
            return master_key->value().derive(account_derivation_path->value(), true);
        } else {
            return nullopt;
        }
    });

    account_pub_key = new DataNode<HDKey>();
    add_node(account_pub_key);
    account_pub_key->set_info("account-pub-key", "XPUB", "The BIP-44 account public key.");
    account_pub_key->set_to_string([](const HDKey& key) { return key.to_base58(false); });
    account_pub_key->set_from_string([](const string& pub) -> HDKey { return HDKey(pub, false); });
    add_derivation("account-pub-key <- [account-key]");
    account_pub_key->set_derivation([&]() -> optional<HDKey> {
        if(account_key->has_value()) {
            return account_key->value().to_public();
        } else {
            return nullopt;
        }
    });

    chain_type = new DataNode<ChainType>();
    add_node(chain_type);
    chain_type->set_info("chain-type", "ENUM internal|external|identity", "The BIP-44 chain type (change) field.");
    chain_type->set_to_string([](const ChainType& t) { return t.to_string(); });
    chain_type->set_from_string([](const string& t) -> ChainType { return ChainType::find(t); });
    add_derivation("chain-type (default: external)");
    chain_type->set_value(ChainType::external());

    chain_type_int = new DataNode<uint32_t>();
    add_node(chain_type_int);
    chain_type_int->set_info("chain-type-int", "INDEX", "The BIP-44 change field integer value.");
    chain_type_int->set_to_string([](uint32_t n) { return to_string(n); });
    chain_type_int->set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });
    add_derivation("chain-type-int <- [chain-type];");
    chain_type_int->set_derivation([&]() {
        return chain_type->value().index();
    });

    address_index = new DataNode<IndexBound>();
    add_node(address_index);
    address_index->set_info("address-index", "INDEX_BOUND", "The BIP-44 address_index field. '*' is allowed for output descriptors.");
    address_index->set_to_string([](IndexBound n) { return n.to_string(); });
    address_index->set_from_string([](const string& s) -> IndexBound { return parse_index_range(s); });
    add_derivation("address-index (default: 0)");
    address_index->set_value(0);

    address_derivation_path = new DataNode<DerivationPath>();
    add_node(address_derivation_path);
    address_derivation_path->set_info("address-derivation-path", "BIP32_PATH", "The BIP-32 address derivation path, starting from the account-key.");
    address_derivation_path->set_to_string([](const DerivationPath& path) { return to_string(path); });
    address_derivation_path->set_from_string([](const string& p) -> DerivationPath { return parse_derivation_path(p); });
    add_derivation("address-derivation-path <- [chain-type-int, address-index]");
    address_derivation_path->set_derivation([&]() {
        return DerivationPath {
            DerivationPathElement(chain_type_int->value(), false),
            DerivationPathElement(address_index->value(), false)
        };
    });

    full_address_derivation_path = new DataNode<DerivationPath>();
    add_node(full_address_derivation_path);
    full_address_derivation_path->set_info("full-address-derivation-path", "BIP32_PATH", "The BIP-32 address derivation path, starting from the master-key.");
    full_address_derivation_path->set_to_string([](const DerivationPath& path) { return to_string(path); });
    full_address_derivation_path->set_from_string([](const string& p) -> DerivationPath { return parse_derivation_path(p); });
    add_derivation("full-address-derivation-path <- [account-derivation-path, address-derivation-path]");
    full_address_derivation_path->set_derivation([&]() {
        auto path = account_derivation_path->value();
        auto address_path = address_derivation_path->value();
        path.insert(path.end(), address_path.begin(), address_path.end());
        return path;
    });

    address_key = new DataNode<HDKey>();
    add_node(address_key);
    address_key->set_info("address-key", "XPRV", "The BIP-32 address HD key.");
    address_key->set_to_string([](const HDKey& key) { return key.to_base58(true); });
    address_key->set_from_string([](const string& prv) -> HDKey { return HDKey(prv, true); });
    add_derivation("address-key <- [master-key, full-address-derivation-path]");
    add_derivation("address-key <- [account-key, address-derivation-path]");
    address_key->set_derivation([&]() -> optional<HDKey> {
        if(master_key->has_value() && full_address_derivation_path->has_value()) {
            return master_key->value().derive(full_address_derivation_path->value(), true);
        } else if(account_key->has_value() && address_derivation_path->has_value()) {
            return account_key->value().derive(address_derivation_path->value(), true);
        } else {
            return nullopt;
        }
    });

    address_pub_key = new DataNode<HDKey>();
    add_node(address_pub_key);
    address_pub_key->set_info("address-pub-key", "XPUB", "The BIP-32 address public HD key.");
    address_pub_key->set_to_string([](const HDKey& key) { return key.to_base58(false); });
    address_pub_key->set_from_string([](const string& pub) -> HDKey { return HDKey(pub, false); });
    add_derivation("address-pub-key <- [address-key]");
    add_derivation("address-pub-key <- [account-pub-key, address-derivation-path]");
    address_pub_key->set_derivation([&]() -> optional<HDKey> {
        if(address_key->has_value()) {
            return address_key->value().to_public();
        } else if(account_pub_key->has_value() && address_derivation_path->has_value()) {
            return account_pub_key->value().derive(address_derivation_path->value(), false);
        } else {
            return nullopt;
        }
    });

    address_ec_key = new DataNode<ECPrivateKey>();
    add_node(address_ec_key);
    address_ec_key->set_info("address-ec-key", "ECPRV", "The address EC key.");
    address_ec_key->set_to_string([](const ECPrivateKey& key) { return key.to_hex(); });
    address_ec_key->set_from_string([](const string& k) -> ECPrivateKey { return ECPrivateKey(hex_to_data(k)); });
    add_derivation("address-ec-key <- [address-key]");
    add_derivation("address-ec-key <- [address-ec-key-wif, network]");
    address_ec_key->set_derivation([&]() -> optional<ECPrivateKey> {
        if(address_key->has_value()) {
            return Wally::instance.bip32_key_to_ec_private(address_key->value());
        } else if(address_ec_key_wif->has_value()) {
            return Wally::instance.wif_to_ec_key(address_ec_key_wif->value(), network->value());
        } else {
            return nullopt;
        }
    });

    address_ec_key_wif = new DataNode<string>();
    add_node(address_ec_key_wif);
    address_ec_key_wif->set_info("address-ec-key-wif", "WIF", "The address EC key in WIF format.");
    address_ec_key_wif->set_to_string([](const string& s) { return s; });
    address_ec_key_wif->set_from_string([](const string& wif) -> string { return wif; });
    add_derivation("address-ec-key-wif <- [address-ec-key, network]");
    address_ec_key_wif->set_derivation([&]() -> optional<string> {
        if(address_ec_key->has_value()) {
            return Wally::instance.ec_key_to_wif(address_ec_key->value(), network->value());
        } else {
            return nullopt;
        }
    });

    address_pub_ec_key = new DataNode<ECCompressedPublicKey>();
    add_node(address_pub_ec_key);
    address_pub_ec_key->set_info("address-pub-ec-key", "ECPUB", "The compressed public EC key.");
    address_pub_ec_key->set_to_string([](const ECCompressedPublicKey& key) { return key.to_hex(); });
    address_pub_ec_key->set_from_string([](const string& k) -> ECCompressedPublicKey { return ECCompressedPublicKey(hex_to_data(k)); });
    add_derivation("address-pub-ec-key <- [address-ec-key]");
    add_derivation("address-pub-ec-key <- [address-pub-key]");
    address_pub_ec_key->set_derivation([&]() -> optional<ECCompressedPublicKey> {
        if(address_pub_key->has_value()) {
            return Wally::instance.bip32_key_to_ec_public(address_pub_key->value());
        } else if(address_ec_key->has_value()) {
            return address_ec_key->value().to_public();
        } else {
            return nullopt;
        }
    });

    address_pkh = new DataNode<string>();
    add_node(address_pkh);
    address_pkh->set_info("address-pkh", "ADDRESS", "The pay-to-public-key-hash address.");
    address_pkh->set_to_string([](const string& s) { return s; });
    address_pkh->set_from_string([](const string& a) -> string { return a; });
    add_derivation("address-pkh <- [address-pub-ec-key, asset]");
    address_pkh->set_derivation([&]() -> optional<string> {
        if(address_pub_ec_key->has_value()) {
            return Wally::instance.to_address(address_pub_ec_key->value(), asset->value(), false);
        } else {
            return nullopt;
        }
    });

    address_sh = new DataNode<string>();
    add_node(address_sh);
    address_sh->set_info("address-sh", "ADDRESS", "The pay-to-script-hash address.");
    address_sh->set_to_string([](const string& s) { return s; });
    address_sh->set_from_string([](const string& a) -> string { return a; });
    add_derivation("address-sh <- [address-pub-ec-key, asset]");
    address_sh->set_derivation([&]() -> optional<string> {
        if(address_pub_ec_key->has_value()) {
            return Wally::instance.to_address(address_pub_ec_key->value(), asset->value(), true);
        } else {
            return nullopt;
        }
    });

    address_segwit = new DataNode<string>();
    add_node(address_segwit);
    address_segwit->set_info("address-segwit", "ADDRESS", "The segwit address.");
    address_segwit->set_to_string([](const string& s) { return s; });
    address_segwit->set_from_string([](const string& a) -> string { return a; });
    add_derivation("address-segwit <- [address-pub-key, network]");
    address_segwit->set_derivation([&]() -> optional<string> {
        if(address_pub_key->has_value()) {
            return Wally::instance.to_segwit_address(address_pub_key->value(), network->value());
        } else {
            return nullopt;
        }
    });

    output_descriptor = new DataNode<OutputDescriptor>();
    add_node(output_descriptor);
    output_descriptor->set_info("output-descriptor", "OUTPUT_DESCRIPTOR", "A single-signature output descriptor.");
    output_descriptor->set_to_string([](const OutputDescriptor& o) -> string { return o.to_string(); });
    add_derivation("output-descriptor <- [output_type, account_derivation_path, address_derivation_path, account_pub_key]");
    output_descriptor->set_derivation([&]() -> optional<OutputDescriptor> {
        if(output_type->has_value() && account_derivation_path->has_value(), address_derivation_path->has_value() && account_pub_key->has_value()) {
            return OutputDescriptor(output_type->value(), account_derivation_path->value(), address_derivation_path->value(), account_pub_key->value());
        } else {
            return nullopt;
        }
    });

    psbt = new DataNode<PSBT>();
    add_node(psbt);
    psbt->set_info("psbt", "BASE64 | HEX | UR:CRYPTO-PSBT", "A partially signed Bitcoin transaction (PSBT).");
    psbt->set_to_string([](const PSBT& p) { return p.base64(); });
    psbt->set_from_string([](const string& s) -> PSBT { return PSBT(s); });
    add_derivation("psbt (default: none)");

    psbt_hex = new DataNode<string>();
    add_node(psbt_hex);
    psbt_hex->set_info("psbt-hex", "HEX", "PSBT in hex format.");
    psbt_hex->set_to_string([](const string& s) { return s; });
    add_derivation("psbt-hex <- [psbt]");
    psbt_hex->set_derivation([&]() -> optional<string> {
        if(psbt->has_value()) {
            return psbt->value().hex();
        } else {
            return nullopt;
        }
    });

    psbt_ur = new DataNode<string>();
    add_node(psbt_ur);
    psbt_ur->set_info("psbt-ur", "UR:CRYPTO-PSBT", "A PSBT in UR format.");
    psbt_ur->set_to_string([](const string& s) { return s; });
    add_derivation("psbt-ur <- [psbt]");
    psbt_ur->set_derivation([&]() -> optional<string> {
        if(psbt->has_value()) {
            return psbt->value().ur();
        } else {
            return nullopt;
        }
    });

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
    add_derivation("psbt-finalized <- [psbt]");
    psbt_finalized->set_derivation([&]() -> optional<PSBT> {
        if(psbt->has_value()) {
            return psbt->value().finalized();
        } else {
            return nullopt;
        }
    });

    psbt_finalized_hex = new DataNode<string>();
    add_node(psbt_finalized_hex);
    psbt_finalized_hex->set_info("psbt-finalized-hex", "HEX", "Finalized PSBT in hex format.");
    psbt_finalized_hex->set_to_string([](const string& s) { return s; });
    add_derivation("psbt-finalized-hex <- [psbt-finalized]");
    psbt_finalized_hex->set_derivation([&]() -> optional<string> {
        if(psbt_finalized->has_value()) {
            return psbt_finalized->value().hex();
        } else {
            return nullopt;
        }
    });

    psbt_finalized_ur = new DataNode<string>();
    add_node(psbt_finalized_ur);
    psbt_finalized_ur->set_info("psbt-finalized-ur", "UR:CRYPTO-PSBT", "Finalized PSBT in UR format.");
    psbt_finalized_ur->set_to_string([](const string& s) { return s; });
    add_derivation("psbt-finalized-ur <- [psbt-finalized]");
    psbt_finalized_ur->set_derivation([&]() -> optional<string> {
        if(psbt_finalized->has_value()) {
            return psbt_finalized->value().ur();
        } else {
            return nullopt;
        }
    });

    psbt_signed = new DataNode<PSBT>();
    add_node(psbt_signed);
    psbt_signed->set_info("psbt-signed", "BASE64 | HEX | UR:CRYPTO-PSBT", "A PBST signed by address-key.");
    psbt_signed->set_to_string([](const PSBT& p) { return p.base64(); });
    add_derivation("psbt-signed <- [psbt, address-ec-key]");
    psbt_signed->set_derivation([&]() -> optional<PSBT> {
        if(psbt->has_value() && address_ec_key->has_value()) {
            return psbt->value().sign(address_ec_key->value());
        } else {
            return nullopt;
        }
    });

    psbt_signed_hex = new DataNode<string>();
    add_node(psbt_signed_hex);
    psbt_signed_hex->set_info("psbt-signed-hex", "HEX", "Signed PSBT in hex format.");
    psbt_signed_hex->set_to_string([](const string& s) { return s; });
    add_derivation("psbt-signed-hex <- [psbt-signed]");
    psbt_signed_hex->set_derivation([&]() -> optional<string> {
        if(psbt_signed->has_value()) {
            return psbt_signed->value().hex();
        } else {
            return nullopt;
        }
    });

    psbt_signed_ur = new DataNode<string>();
    add_node(psbt_signed_ur);
    psbt_signed_ur->set_info("psbt-signed-ur", "UR:CRYPTO-PSBT", "Signed PSBT in UR format.");
    psbt_signed_ur->set_to_string([](const string& s) { return s; });
    add_derivation("psbt-signed-ur <- [psbt-signed]");
    psbt_signed_ur->set_derivation([&]() -> optional<string> {
        if(psbt_signed->has_value()) {
            return psbt_signed->value().ur();
        } else {
            return nullopt;
        }
    });

    transaction = new DataNode<Transaction>();
    add_node(transaction);
    transaction->set_info("transaction", "HEX | UR:CRYPTO-TX", "The raw Bitcoin transaction.");
    transaction->set_to_string([](const Transaction& t) { return t.hex(); });
    transaction->set_from_string([](const string& s) -> Transaction { return Transaction(s); });
    add_derivation("transaction <- [psbt-finalized]");
    transaction->set_derivation([&]() -> optional<Transaction> {
        if(psbt_finalized->has_value()) {
            return Transaction(psbt_finalized->value());
        } else {
            return nullopt;
        }
    });

    transaction_ur = new DataNode<string>();
    add_node(transaction_ur);
    transaction_ur->set_info("transaction-ur", "UR:CRYPTO-TX", "The raw Bitcoin transaction in UR format.");
    transaction_ur->set_to_string([](const string& s) { return s; });
    add_derivation("transaction-ur <- [transaction]");
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
