#ifndef KEYTOOL_MODEL_HDKEY_HPP
#define KEYTOOL_MODEL_HDKEY_HPP

#include "data-node.hpp"
#include "utils.hpp"
#include "seed.hpp"
#include "hd-key.hpp"
#include "output-type.hpp"
#include "derivation-path.hpp"
#include "chain-type.hpp"
#include "index-bound.hpp"
#include <string>

class Model;

DataNode<HDKey>* setup_master_key(Model& model);
DataNode<ByteVector>* setup_master_key_fingerprint(Model& model);
DataNode<OutputDescriptorType>* setup_output_type(Model& model);
DataNode<uint32_t>* setup_purpose(Model& model);
DataNode<uint32_t>* setup_coin_type(Model& model);
DataNode<uint32_t>* setup_account_index(Model& model);
DataNode<DerivationPath>* setup_account_derivation_path(Model& model);
DataNode<HDKey>* setup_account_key(Model& model);
DataNode<HDKey>* setup_account_pub_key(Model& model);
DataNode<ChainType>* setup_chain_type(Model& model);
DataNode<uint32_t>* setup_chain_type_int(Model& model);
DataNode<IndexBound>* setup_address_index(Model& model);
DataNode<DerivationPath>* setup_address_derivation_path(Model& model);
DataNode<DerivationPath>* setup_full_address_derivation_path(Model& model);
DataNode<HDKey>* setup_address_key(Model& model);
DataNode<HDKey>* setup_address_pub_key(Model& model);

#endif
