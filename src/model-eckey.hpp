#ifndef KEYTOOL_MODEL_ECKEY_HPP
#define KEYTOOL_MODEL_ECKEY_HPP

#include "data-node.hpp"
#include "utils.hpp"
#include "seed.hpp"
#include "asset-2.hpp"
#include "network.hpp"
#include "ec-key.hpp"
#include <string>

class Model;

DataNode<Asset2>* setup_asset(Model& model);
DataNode<Network>* setup_network(Model& model);
DataNode<ECPrivateKey>* setup_address_ec_key(Model& model);
DataNode<std::string>* setup_address_ec_key_wif(Model& model);
DataNode<ECCompressedPublicKey>* setup_address_pub_ec_key(Model& model);
DataNode<std::string>* setup_address_pkh(Model& model);
DataNode<std::string>* setup_address_sh(Model& model);
DataNode<std::string>* setup_address_segwit(Model& model);

#endif
