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
    request_id = setup_request_id(*this);
    request_description = setup_request_description(*this);
    seed_request = setup_seed_request(*this);

    // ec-key
    asset = setup_asset(*this);
    network = setup_network(*this);
    address_ec_key = setup_address_ec_key(*this);
    address_ec_key_wif = setup_address_ec_key_wif(*this);
    address_pub_ec_key = setup_address_pub_ec_key(*this);
    address_pkh = setup_address_pkh(*this);
    address_sh = setup_address_sh(*this);
    address_segwit = setup_address_segwit(*this);

    // hd-key
    master_key = setup_master_key(*this);
    master_key_fingerprint = setup_master_key_fingerprint(*this);
    output_type = setup_output_type(*this);
    purpose = setup_purpose(*this);
    coin_type = setup_coin_type(*this);
    account_index = setup_account_index(*this);
    account_derivation_path = setup_account_derivation_path(*this);
    account_key = setup_account_key(*this);
    account_pub_key = setup_account_pub_key(*this);
    chain_type = setup_chain_type(*this);
    chain_type_int = setup_chain_type_int(*this);
    address_index = setup_address_index(*this);
    address_derivation_path = setup_address_derivation_path(*this);
    full_address_derivation_path = setup_full_address_derivation_path(*this);
    address_key = setup_address_key(*this);
    address_pub_key = setup_address_pub_key(*this);
    output_descriptor = setup_output_descriptor(*this);

    // psbt
    psbt = setup_psbt(*this);
    psbt_hex = setup_psbt_hex(*this);
    psbt_ur = setup_psbt_ur(*this);
    psbt_finalized = setup_psbt_finalized(*this);
    psbt_finalized_hex = setup_psbt_finalized_hex(*this);
    psbt_finalized_ur = setup_psbt_finalized_ur(*this);
    psbt_signed = setup_psbt_signed(*this);
    psbt_signed_hex = setup_psbt_signed_hex(*this);
    psbt_signed_ur = setup_psbt_signed_ur(*this);
    transaction = setup_transaction(*this);
    transaction_ur = setup_transaction_ur(*this);

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
