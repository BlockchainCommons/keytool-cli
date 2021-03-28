#ifndef KEYTOOL_MODEL_SEED_HPP
#define KEYTOOL_MODEL_SEED_HPP

#include "data-node.hpp"
#include "utils.hpp"
#include "seed.hpp"
#include <string>

class Model;

DataNode<ByteVector>* setup_seed_hex(Model& model);
DataNode<std::string>* setup_seed_name(Model& model);
DataNode<std::string>* setup_seed_note(Model& model);
DataNode<Seed>* setup_seed(Model& model);

#endif
