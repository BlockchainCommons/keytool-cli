#ifndef KEYTOOL_MODEL_HPP
#define KEYTOOL_MODEL_HPP

#include "data-node.hpp"
#include "asset.hpp"
#include "hd-key.hpp"
#include "ec-key.hpp"
#include "utils.hpp"
#include "wally-utils.hpp"
#include "derivation-path.hpp"
#include "chain-type.hpp"
#include "output-descriptor-type.hpp"
#include "index-bound.hpp"
#include "output-descriptor.hpp"

class Model final {
public:
    Model();

    // seed
    DataNode<ByteVector> seed;

    // asset (default: btc)
    DataNode<Asset> asset;

    // network <- [asset]
    DataNode<Network> network;

    // master_key <- [network, seed]
    DataNode<HDKey> master_key;

    // master_key_fingerprint <- [master_key]
    DataNode<ByteVector> master_key_fingerprint;

    // purpose (default: 44 per BIP-44)
    DataNode<uint32_t> purpose;

    // coin_type <- [asset]
    DataNode<uint32_t> coin_type;

    // account_index (default: 0)
    DataNode<uint32_t> account_index;

    // account_derivation_path <- [master_key_fingerprint, purpose, coin_type, account_index]
    DataNode<DerivationPath> account_derivation_path;

    // account_key <- [master_key, account_derivation_path]
    DataNode<HDKey> account_key;

    // account_pub_key <- [account_key]
    DataNode<HDKey> account_pub_key;

    // chain_type (default: external)
    DataNode<ChainType> chain_type;

    // chain_type_int <- [chain_type];
    DataNode<uint32_t> chain_type_int;

    // address_index (default: 0)
    DataNode<IndexBound> address_index;

    // partial_address_derivation_path <- [chain_type_int, address_index]
    DataNode<DerivationPath> partial_address_derivation_path;

    // full_address_derivation_path <- [account_derivation_path, partial_address_derivation_path]
    DataNode<DerivationPath> full_address_derivation_path;

    // address_key <- [master_key, full_address_derivation_path]
    // address_key <- [account_key, partial_address_derivation_path]
    DataNode<HDKey> address_key;

    // address_pub_key <- [address_key]
    // address_pub_key <- [account_pub_key, partial_address_derivation_path]
    DataNode<HDKey> address_pub_key;

    // address_ec_key <- [address_key]
    // address_ec_key <- [address_ec_key_wif]
    DataNode<ECPrivateKey> address_ec_key;

    // address_ec_key_wif <- [address_ec_key, network]
    DataNode<std::string> address_ec_key_wif;

    // address_pub_ec_key <- [address_ec_key]
    // address_pub_ec_key <- [address_pub_key]
    DataNode<ECCompressedPublicKey> address_pub_ec_key;

    // address_pkh <- [address_pub_ec_key, asset]
    DataNode<std::string> address_pkh;

    // address_sh <- [address_pub_ec_key, asset]
    DataNode<std::string> address_sh;

    // output_descriptor_type
    DataNode<OutputDescriptorType> output_descriptor_type;

    // output_descriptor <- [master_key_fingerprint, partial_address_derivation_path, output_descriptor_type, account_pub_key]
    DataNode<OutputDescriptor> output_descriptor;

    std::vector<DataNodeProtocol*> all_nodes;

    DataNodeProtocol* find_by_name(const std::string& node_name) const;
    DataNodeProtocol* find_by_tag(int node_tag) const;
    bool is_valid_name(const std::string& node_name) const;
    bool can_derive(const std::string& node_name) const;
};

#endif // KEYTOOL_MODEL_HPP
