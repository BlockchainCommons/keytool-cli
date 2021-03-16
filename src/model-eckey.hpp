#ifndef KEYTOOL_MODEL_ECKEY_HPP
#define KEYTOOL_MODEL_ECKEY_HPP

#include "data-node.hpp"
#include "utils.hpp"
#include "seed.hpp"
#include "asset.hpp"
#include "network.hpp"
#include <string>

class Model;

DataNode<Asset>* setup_asset(Model& model);
DataNode<Network>* setup_network(Model& model);

#endif
