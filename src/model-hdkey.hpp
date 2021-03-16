#ifndef KEYTOOL_MODEL_HDKEY_HPP
#define KEYTOOL_MODEL_HDKEY_HPP

#include "data-node.hpp"
#include "utils.hpp"
#include "seed.hpp"
#include "hd-key.hpp"
#include "output-type.hpp"
#include "derivation-path.hpp"
#include <string>

class Model;

DataNode<HDKey>* setup_master_key(Model& model);
DataNode<ByteVector>* setup_master_key_fingerprint(Model& model);
DataNode<OutputDescriptorType>* setup_output_type(Model& model);
DataNode<uint32_t>* setup_purpose(Model& model);
DataNode<uint32_t>* setup_coin_type(Model& model);
DataNode<uint32_t>* setup_account_index(Model& model);
DataNode<DerivationPath>* setup_account_derivation_path(Model& model);

#endif
