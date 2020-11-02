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
#include "output-type.hpp"
#include "index-bound.hpp"
#include "output-descriptor.hpp"
#include "psbt.hpp"
#include "transaction.hpp"

class Model final {
public:
    Model();

    // seed
    DataNode<ByteVector> seed;

    // asset (default: btc)
    DataNode<Asset> asset;

    // network <- [asset]
    DataNode<Network> network;

    // master-key <- [network, seed]
    DataNode<HDKey> master_key;

    // master-key-fingerprint <- [master-key]
    DataNode<ByteVector> master_key_fingerprint;

    // output-type (default: wpkh)
    DataNode<OutputDescriptorType> output_type;

    // purpose <- [output-type]
    DataNode<uint32_t> purpose;

    // coin-type <- [asset]
    DataNode<uint32_t> coin_type;

    // account-index (default: 0)
    DataNode<uint32_t> account_index;

    // account-derivation-path <- [master-key-fingerprint, purpose, coin-type, account-index]
    DataNode<DerivationPath> account_derivation_path;

    // account-key <- [master-key, account-derivation-path]
    DataNode<HDKey> account_key;

    // account-pub-key <- [account-key]
    DataNode<HDKey> account_pub_key;

    // chain-type (default: external)
    DataNode<ChainType> chain_type;

    // chain-type-int <- [chain-type];
    DataNode<uint32_t> chain_type_int;

    // address-index (default: 0)
    DataNode<IndexBound> address_index;

    // address-derivation-path <- [chain-type-int, address-index]
    DataNode<DerivationPath> address_derivation_path;

    // full-address-derivation-path <- [account-derivation-path, address-derivation-path]
    DataNode<DerivationPath> full_address_derivation_path;

    // address-key <- [master-key, full-address-derivation-path]
    // address-key <- [account-key, address-derivation-path]
    DataNode<HDKey> address_key;

    // address-pub-key <- [address-key]
    // address-pub-key <- [account-pub-key, address-derivation-path]
    DataNode<HDKey> address_pub_key;

    // address-ec-key <- [address-key]
    // address-ec-key <- [address-ec-key-wif, network]
    DataNode<ECPrivateKey> address_ec_key;

    // address-ec-key-wif <- [address-ec-key, network]
    DataNode<std::string> address_ec_key_wif;

    // address-pub-ec-key <- [address-ec-key]
    // address-pub-ec-key <- [address-pub-key]
    DataNode<ECCompressedPublicKey> address_pub_ec_key;

    // address-pkh <- [address-pub-ec-key, asset]
    DataNode<std::string> address_pkh;

    // address-sh <- [address-pub-ec-key, asset]
    DataNode<std::string> address_sh;

    // address-segwit <- [address-pub-key, network]
    DataNode<std::string> address_segwit;

    // output-descriptor <- [output-type, account-derivation-path, address-derivation-path, account-pub-key]
    DataNode<OutputDescriptor> output_descriptor;

    // psbt
    DataNode<PSBT> psbt;

    // psbt-finalized <- [psbt]
    DataNode<PSBT> psbt_finalized;

    // transaction <- [psbt-finalized]
    DataNode<Transaction> transaction;

    // psbt-signed <- [psbt, address-ec-key]
    DataNode<PSBT> psbt_signed;

    std::vector<DataNodeProtocol*> all_nodes;

    DataNodeProtocol* find_by_name(const std::string& node_name) const;
    DataNodeProtocol* find_by_tag(int node_tag) const;
    bool is_valid_name(const std::string& node_name) const;
    bool can_derive(const std::string& node_name) const;
};

#endif // KEYTOOL_MODEL_HPP
