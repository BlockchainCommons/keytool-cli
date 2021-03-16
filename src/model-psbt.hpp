#ifndef KEYTOOL_MODEL_PSBT_HPP
#define KEYTOOL_MODEL_PSBT_HPP

#include "data-node.hpp"
#include "utils.hpp"
#include "seed.hpp"
#include "psbt.hpp"
#include "transaction.hpp"
#include <string>

class Model;

DataNode<PSBT>* setup_psbt(Model& model);
DataNode<std::string>* setup_psbt_hex(Model& model);
DataNode<std::string>* setup_psbt_ur(Model& model);
DataNode<PSBT>* setup_psbt_finalized(Model& model);
DataNode<std::string>* setup_psbt_finalized_hex(Model& model);
DataNode<std::string>* setup_psbt_finalized_ur(Model& model);
DataNode<PSBT>* setup_psbt_signed(Model& model);
DataNode<std::string>* setup_psbt_signed_hex(Model& model);
DataNode<std::string>* setup_psbt_signed_ur(Model& model);
DataNode<Transaction>* setup_transaction(Model& model);
DataNode<std::string>* setup_transaction_ur(Model& model);

#endif
