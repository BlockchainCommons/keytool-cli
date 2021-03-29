#ifndef KEYTOOL_MODEL_HDKEY_HPP
#define KEYTOOL_MODEL_HDKEY_HPP

#include "data-node.hpp"
#include "utils.hpp"
#include "seed.hpp"
#include "hd-key-2.hpp"
#include "output-type.hpp"
#include "chain-type.hpp"
#include "derivation-index-spec.hpp"
#include "output-descriptor.hpp"
#include <string>

class Model;

DataNode<HDKey2>* setup_master_key(Model& model);
DataNode<uint32_t>* setup_master_key_fingerprint(Model& model);
DataNode<OutputDescriptorType>* setup_output_type(Model& model);
DataNode<uint32_t>* setup_purpose(Model& model);
DataNode<uint32_t>* setup_coin_type(Model& model);
DataNode<uint32_t>* setup_account_index(Model& model);
DataNode<DerivationPath>* setup_account_derivation_path(Model& model);
DataNode<HDKey2>* setup_account_key(Model& model);
DataNode<HDKey2>* setup_account_pub_key(Model& model);
DataNode<ChainType>* setup_chain_type(Model& model);
DataNode<uint32_t>* setup_chain_type_int(Model& model);
DataNode<DerivationIndexSpec>* setup_address_index(Model& model);
DataNode<DerivationPath>* setup_address_derivation_path(Model& model);
DataNode<DerivationPath>* setup_full_address_derivation_path(Model& model);
DataNode<HDKey2>* setup_address_key(Model& model);
DataNode<HDKey2>* setup_address_pub_key(Model& model);
DataNode<OutputDescriptor>* setup_output_descriptor(Model& model);

DataNode<HDKey2>* setup_master_key_base58(Model& model);
DataNode<HDKey2>* setup_account_key_base58(Model& model);
DataNode<HDKey2>* setup_account_pub_key_base58(Model& model);
DataNode<HDKey2>* setup_address_key_base58(Model& model);
DataNode<HDKey2>* setup_address_pub_key_base58(Model& model);

#endif
