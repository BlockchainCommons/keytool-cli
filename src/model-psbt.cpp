#include "model-psbt.hpp"
#include "model.hpp"
#include <optional>

using namespace std;

DataNode<PSBT>* setup_psbt(Model& model) {
    auto node = new DataNode<PSBT>();
    model.add_node(node);
    node->set_info("psbt", "BASE64 | HEX | UR:CRYPTO-PSBT", "A partially signed Bitcoin transaction (PSBT).");
    node->set_to_string([](const PSBT& p) { return p.base64(); });
    node->set_from_string([](const string& s) -> PSBT { return PSBT(s); });
    model.add_derivation("psbt (default: none)");
    return node;
}

DataNode<string>* setup_psbt_hex(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("psbt-hex", "HEX", "PSBT in hex format.");
    node->set_to_string([](const string& s) { return s; });
    model.add_derivation("psbt-hex <- [psbt]");
    node->set_derivation([&]() -> optional<string> {
        if(model.psbt->has_value()) {
            return model.psbt->value().hex();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<string>* setup_psbt_ur(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("psbt-ur", "UR:CRYPTO-PSBT", "A PSBT in UR format.");
    node->set_to_string([](const string& s) { return s; });
    model.add_derivation("psbt-ur <- [psbt]");
    node->set_derivation([&]() -> optional<string> {
        if(model.psbt->has_value()) {
            return model.psbt->value().ur();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<PSBT>* setup_psbt_finalized(Model& model) {
    auto node = new DataNode<PSBT>();
    model.add_node(node);
    node->set_info("psbt-finalized", "BASE64 | HEX | UR:CRYPTO-PSBT", "The finalized PSBT.");
    node->set_to_string([](const PSBT& p) { return p.base64(); });
    node->set_from_string([](const string& s) -> PSBT {
        auto psbt = PSBT(s);
        if(!psbt.is_finalized()) {
            throw domain_error("Cannot assign non-finalized PSBT.");
        }
        return psbt;
    });
    model.add_derivation("psbt-finalized <- [psbt]");
    node->set_derivation([&]() -> optional<PSBT> {
        if(model.psbt->has_value()) {
            return model.psbt->value().finalized();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<string>* setup_psbt_finalized_hex(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("psbt-finalized-hex", "HEX", "Finalized PSBT in hex format.");
    node->set_to_string([](const string& s) { return s; });
    model.add_derivation("psbt-finalized-hex <- [psbt-finalized]");
    node->set_derivation([&]() -> optional<string> {
        if(model.psbt_finalized->has_value()) {
            return model.psbt_finalized->value().hex();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<string>* setup_psbt_finalized_ur(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("psbt-finalized-ur", "UR:CRYPTO-PSBT", "Finalized PSBT in UR format.");
    node->set_to_string([](const string& s) { return s; });
    model.add_derivation("psbt-finalized-ur <- [psbt-finalized]");
    node->set_derivation([&]() -> optional<string> {
        if(model.psbt_finalized->has_value()) {
            return model.psbt_finalized->value().ur();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<PSBT>* setup_psbt_signed(Model& model) {
    auto node = new DataNode<PSBT>();
    model.add_node(node);
    node->set_info("psbt-signed", "BASE64 | HEX | UR:CRYPTO-PSBT", "A PBST signed by address-key.");
    node->set_to_string([](const PSBT& p) { return p.base64(); });
    model.add_derivation("psbt-signed <- [psbt, address-ec-key]");
    node->set_derivation([&]() -> optional<PSBT> {
        if(model.psbt->has_value() && model.address_ec_key->has_value()) {
            return model.psbt->value().sign(model.address_ec_key->value());
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<string>* setup_psbt_signed_hex(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("psbt-signed-hex", "HEX", "Signed PSBT in hex format.");
    node->set_to_string([](const string& s) { return s; });
    model.add_derivation("psbt-signed-hex <- [psbt-signed]");
    node->set_derivation([&]() -> optional<string> {
        if(model.psbt_signed->has_value()) {
            return model.psbt_signed->value().hex();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<string>* setup_psbt_signed_ur(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("psbt-signed-ur", "UR:CRYPTO-PSBT", "Signed PSBT in UR format.");
    node->set_to_string([](const string& s) { return s; });
    model.add_derivation("psbt-signed-ur <- [psbt-signed]");
    node->set_derivation([&]() -> optional<string> {
        if(model.psbt_signed->has_value()) {
            return model.psbt_signed->value().ur();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<Transaction>* setup_transaction(Model& model) {
    auto node = new DataNode<Transaction>();
    model.add_node(node);
    node->set_info("transaction", "HEX | UR:CRYPTO-TX", "The raw Bitcoin transaction.");
    node->set_to_string([](const Transaction& t) { return t.hex(); });
    node->set_from_string([](const string& s) -> Transaction { return Transaction(s); });
    model.add_derivation("transaction <- [psbt-finalized]");
    node->set_derivation([&]() -> optional<Transaction> {
        if(model.psbt_finalized->has_value()) {
            return Transaction(model.psbt_finalized->value());
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<string>* setup_transaction_ur(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("transaction-ur", "UR:CRYPTO-TX", "The raw Bitcoin transaction in UR format.");
    node->set_to_string([](const string& s) { return s; });
    model.add_derivation("transaction-ur <- [transaction]");
    node->set_derivation([&]() -> optional<string> {
        if(model.transaction->has_value()) {
            return model.transaction->value().ur();
        } else {
            return nullopt;
        }
    });
    return node;
}
