#ifndef KEYTOOL_MODEL_SEED_HPP
#define KEYTOOL_MODEL_SEED_HPP

#include "data-node.hpp"
#include "utils.hpp"
#include "seed.hpp"
#include "uuid.hpp"
#include "request.hpp"
#include "response.hpp"
#include <string>

class Model;

DataNode<ByteVector>* setup_seed_hex(Model& model);
DataNode<std::string>* setup_seed_name(Model& model);
DataNode<std::string>* setup_seed_note(Model& model);
DataNode<Seed>* setup_seed(Model& model);
DataNode<ByteVector>* setup_seed_digest(Model& model);
DataNode<UUID>* setup_seed_request_id(Model& model);
DataNode<std::string>* setup_request_description(Model& model);
DataNode<Request>* setup_seed_request(Model& model);
DataNode<Response>* setup_seed_response(Model& model);

#endif
