#ifndef KEYTOOL_MODEL_SEED_HPP
#define KEYTOOL_MODEL_SEED_HPP

#include "data-node.hpp"
#include "utils.hpp"
#include "seed.hpp"
#include "uuid.hpp"
#include <string>

class Model;

DataNode<ByteVector>* setup_seed(Model& model);
DataNode<std::string>* setup_seed_name(Model& model);
DataNode<std::string>* setup_seed_note(Model& model);
DataNode<Seed>* setup_seed_ur(Model& model);
DataNode<ByteVector>* setup_seed_digest(Model& model);
DataNode<UUID>* setup_request_id(Model& model);

#endif
