#include "model.hpp"

using namespace std;

static uint32_t parse_uint32(const std::string& s) {
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

Model::Model()
    : seed("seed", -1)
    , asset("asset", -2)
    , network("network", -3)
    , master_key("master-key", -4)
    , purpose("purpose", -5)
    , coin_type("coin-type", -6)
    , account_index("account-index", -7)
    , account_derivation_path("account-derivation-path", -8)
    , account_key("account-key", -9)
    , account_pub_key("account-pub-key", -10)
    , chain_type("chain-type", -11)
    , chain_type_int("chain-type-int", -12)
    , address_index("address-index", -13)
    , address_derivation_path("address-derivation-path", -14)
    , address_key("address-key", -15)
    , address_pub_key("address-pub-key", -16)
    , address_ec_key("address-ec-key", -17)
    , address_ec_key_wif("address-ec-key-wif", -18)
    , address_pub_ec_key("address-pub-ec-key", -19)
    , address_pkh("address-pkh", -20)
    , address_sh("address-sh", -21)
{
    // seed
    seed.set_to_string([](const ByteVector& bytes) { return data_to_hex(bytes); });
    seed.set_from_string([](const string& hex) -> ByteVector { return hex_to_data(hex); });
    all_nodes.push_back(&seed);

    // asset
    asset = Asset::btc();
    asset.set_to_string([](const Asset& asset) { return asset.symbol(); });
    asset.set_from_string([](const string& symbol) -> Asset { return Asset::find(symbol); });
    all_nodes.push_back(&asset);

    // network <- asset
    network.set_f([&]() { return asset().network(); });
    network.set_to_string([](const Network& network) { return network.name(); });
    network.set_from_string([](const string& name) -> Network { return Network::find(name); });
    all_nodes.push_back(&network);

    // master_key <- [network, seed]
    master_key.set_f([&]() -> optional<HDKey> {
        if(seed.has_value() && network.has_value()) {
            return HDKey(seed(), network());
        } else {
            return nullopt;
        }
    });
    master_key.set_to_string([](const HDKey& key) { return key.to_base58(true); });
    master_key.set_from_string([](const string& prv) -> HDKey { return HDKey(prv, true); });
    all_nodes.push_back(&master_key);

    // purpose
    purpose = 44;
    purpose.set_to_string([](uint32_t n) { return to_string(n); });
    purpose.set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });
    all_nodes.push_back(&purpose);

    // coin_type <- asset
    coin_type.set_f([&]() {
        return asset().coin_type();
    });
    coin_type.set_to_string([](uint32_t n) { return to_string(n); });
    coin_type.set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });
    all_nodes.push_back(&coin_type);

    // account_index
    account_index = 0;
    account_index.set_to_string([](uint32_t n) { return to_string(n); });
    account_index.set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });
    all_nodes.push_back(&account_index);

    // account_derivation_path <- [purpose, coin_type, account_index]
    account_derivation_path.set_f([&]() {
        return DerivationPath {
            DerivationPathElement(purpose(), true),
            DerivationPathElement(coin_type(), true),
            DerivationPathElement(account_index(), true)
        };
    });
    account_derivation_path.set_to_string([](const DerivationPath& path) { return to_string(path); });
    account_derivation_path.set_from_string([](const string& p) -> DerivationPath { return parse_derivation_path(p); });
    all_nodes.push_back(&account_derivation_path);

    // account_key <- [master_key, account_derivation_path]
    account_key.set_f([&]() -> optional<HDKey> {
        if(master_key.has_value()) {
            return master_key().derive(account_derivation_path(), true);
        } else {
            return nullopt;
        }
    });
    account_key.set_to_string([](const HDKey& key) { return key.to_base58(true); });
    account_key.set_from_string([](const string& prv) -> HDKey { return HDKey(prv, true); });
    all_nodes.push_back(&account_key);

    // account_pub_key <- [account_key]
    account_pub_key.set_f([&]() -> optional<HDKey> {
        if(account_key.has_value()) {
            return account_key().to_public();
        } else {
            return nullopt;
        }
    });
    account_pub_key.set_to_string([](const HDKey& key) { return key.to_base58(false); });
    account_pub_key.set_from_string([](const string& pub) -> HDKey { return HDKey(pub, false); });
    all_nodes.push_back(&account_pub_key);

    // chain_type
    chain_type = ChainType::external();
    chain_type.set_to_string([](const ChainType& t) { return t.to_string(); });
    chain_type.set_from_string([](const string& t) -> ChainType { return ChainType::find(t); });
    all_nodes.push_back(&chain_type);

    // chain_type_int <- [chain_type];
    chain_type_int.set_f([&]() {
        return chain_type().index();
    });
    chain_type_int.set_to_string([](uint32_t n) { return to_string(n); });
    chain_type_int.set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });
    all_nodes.push_back(&chain_type_int);

    // address_index
    address_index = 0;
    address_index.set_to_string([](uint32_t n) { return to_string(n); });
    address_index.set_from_string([](const string& n) -> uint32_t { return parse_uint32(n); });
    all_nodes.push_back(&address_index);

    // address_derivation_path <- [account_derivation_path, chain_type_int, address_index]
    address_derivation_path.set_f([&]() {
        auto path = account_derivation_path();
        path.push_back(DerivationPathElement(chain_type_int(), false));
        path.push_back(DerivationPathElement(address_index(), false));
        return path;
    });
    address_derivation_path.set_to_string([](const DerivationPath& path) { return to_string(path); });
    address_derivation_path.set_from_string([](const string& p) -> DerivationPath { return parse_derivation_path(p); });
    all_nodes.push_back(&address_derivation_path);

    // address_key <- [master_key, address_derivation_path]
    // address_key <- [account_key, chain_type_int, address_index]
    address_key.set_f([&]() -> optional<HDKey> {
        if(master_key.has_value() && address_derivation_path.has_value()) {
            return master_key().derive(address_derivation_path(), true);
        } else if(account_key.has_value() && chain_type_int.has_value() && address_index.has_value()) {
            DerivationPath path = { DerivationPathElement(chain_type_int(), false), DerivationPathElement(address_index(), false) };
            return account_key().derive(path, true);
        } else {
            return nullopt;
        }
    });
    address_key.set_to_string([](const HDKey& key) { return key.to_base58(true); });
    address_key.set_from_string([](const string& prv) -> HDKey { return HDKey(prv, true); });
    all_nodes.push_back(&address_key);

    // address_pub_key <- [address_key]
    // address_pub_key <- [account_pub_key, chain_type_int, address_index]
    address_pub_key.set_f([&]() -> optional<HDKey> {
        if(address_key.has_value()) {
            return address_key().to_public();
        } else if(account_pub_key.has_value() && chain_type_int.has_value() && address_index.has_value()) {
            DerivationPath path = { DerivationPathElement(chain_type_int(), false), DerivationPathElement(address_index(), false) };
            return account_pub_key().derive(path, false);
        } else {
            return nullopt;
        }
    });
    address_pub_key.set_to_string([](const HDKey& key) { return key.to_base58(false); });
    address_pub_key.set_from_string([](const string& pub) -> HDKey { return HDKey(pub, false); });
    all_nodes.push_back(&address_pub_key);

    // address_ec_key <- [address_key]
    // address_ec_key <- [address_ec_key_wif]
    address_ec_key.set_f([&]() -> optional<ECPrivateKey> {
        if(address_key.has_value()) {
            return Wally::instance.bip32_key_to_ec_private(address_key());
        } else if(address_ec_key_wif.has_value()) {
            return Wally::instance.wif_to_ec_key(address_ec_key_wif(), network());
        } else {
            return nullopt;
        }
    });
    address_ec_key.set_to_string([](const ECPrivateKey& key) { return key.to_hex(); });
    address_ec_key.set_from_string([](const string& k) -> ECPrivateKey { return ECPrivateKey(hex_to_data(k)); });
    all_nodes.push_back(&address_ec_key);

    // address_ec_key_wif <- [address_ec_key, network]
    address_ec_key_wif.set_f([&]() -> optional<string> {
        if(address_ec_key.has_value()) {
            return Wally::instance.ec_key_to_wif(address_ec_key(), network());
        } else {
            return nullopt;
        }
    });
    address_ec_key_wif.set_to_string([](const string& s) { return s; });
    address_ec_key_wif.set_from_string([](const string& wif) -> std::string { return wif; });
    all_nodes.push_back(&address_ec_key_wif);

    // address_pub_ec_key <- [address_ec_key]
    // address_pub_ec_key <- [address_pub_key]
    address_pub_ec_key.set_f([&]() -> optional<ECCompressedPublicKey> {
        if(address_pub_key.has_value()) {
            return Wally::instance.bip32_key_to_ec_public(address_pub_key());
        } else if(address_ec_key.has_value()) {
            return address_ec_key().to_public();
        } else {
            return nullopt;
        }
    });
    address_pub_ec_key.set_to_string([](const ECCompressedPublicKey& key) { return key.to_hex(); });
    address_pub_ec_key.set_from_string([](const string& k) -> ECCompressedPublicKey { return ECCompressedPublicKey(hex_to_data(k)); });
    all_nodes.push_back(&address_pub_ec_key);

    // address_pkh <- [address_pub_ec_key, asset]
    address_pkh.set_f([&]() -> optional<string> {
        if(address_pub_ec_key.has_value()) {
            return Wally::instance.to_address(address_pub_ec_key(), asset(), false);
        } else {
            return nullopt;
        }
    });
    address_pkh.set_to_string([](const string& s) { return s; });
    address_pkh.set_from_string([](const string& a) -> std::string { return a; });
    all_nodes.push_back(&address_pkh);

    // address_sh <- [address_pub_ec_key, asset]
    address_sh.set_f([&]() -> optional<string> {
        if(address_pub_ec_key.has_value()) {
            return Wally::instance.to_address(address_pub_ec_key(), asset(), true);
        } else {
            return nullopt;
        }
    });
    address_sh.set_to_string([](const string& s) { return s; });
    address_sh.set_from_string([](const string& a) -> std::string { return a; });
    all_nodes.push_back(&address_sh);
}

DataNodeProtocol* Model::find_by_name(const string& node_name) const {
    for(auto node: all_nodes) {
        if(node->name() == node_name) {
            return node;
        }
    }
    return nullptr;
}

DataNodeProtocol* Model::find_by_tag(int node_tag) const {
    for(auto node: all_nodes) {
        if(node->tag() == node_tag) {
            return node;
        }
    }
    return nullptr;
}

bool Model::is_valid_name(const string& node_name) const {
    return find_by_name(node_name) != nullptr;
}

bool Model::can_derive(const string& node_name) const {
    auto node = find_by_name(node_name);
    if(node == nullptr) {
        return false;
    }
    return node->has_value();
}
