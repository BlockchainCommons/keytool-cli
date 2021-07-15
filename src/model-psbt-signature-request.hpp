#ifndef KEYTOOL_MODEL_PSBT_SIGNATURE_REQUEST_HPP
#define KEYTOOL_MODEL_PSBT_SIGNATURE_REQUEST_HPP

#include "data-node.hpp"
#include "utils.hpp"
#include "psbt.hpp"
#include "uuid.hpp"
#include "request.hpp"
#include "response.hpp"
#include <string>

class Model;

DataNode<std::string>* setup_psbt_signature_request_description(Model& model);
DataNode<UUID>* setup_psbt_signature_request_id(Model& model);
DataNode<Request>* setup_psbt_signature_request(Model& model);
DataNode<Response>* setup_psbt_signature_response(Model& model);

#endif
