#include "model-psbt.hpp"
#include "model.hpp"
#include <optional>

using namespace std;

DataNode<PSBT>* setup_psbt(Model& model) {
    auto node = new DataNode<PSBT>();
    model.add_node(node);
    node->set_info("psbt", "PSBT", "A partially signed Bitcoin transaction (PSBT).");
    node->set_to_string([](const PSBT& p) { return p.ur(); });
    node->set_from_string([](const string& s) -> PSBT { return PSBT(s); });
    return node;
}

DataNode<PSBT>* setup_psbt_hex(Model& model) {
    auto node = new DataNode<PSBT>();
    model.add_node(node);
    node->set_info("psbt-hex", "HEX", "PSBT in hex format.");
    node->set_to_string([](const PSBT& p) { return p.hex(); });
    model.add_derivation("psbt-hex <- [psbt]");
    node->set_derivation([&]() -> optional<PSBT> {
        if(model.psbt->has_value()) {
            return model.psbt->value();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<PSBT>* setup_psbt_base64(Model& model) {
    auto node = new DataNode<PSBT>();
    model.add_node(node);
    node->set_info("psbt-base64", "BASE64", "A PSBT in Base64 format.");
    node->set_to_string([](const PSBT& p) { return p.base64(); });
    model.add_derivation("psbt-base64 <- [psbt]");
    node->set_derivation([&]() -> optional<PSBT> {
        if(model.psbt->has_value()) {
            return model.psbt->value();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<PSBT>* setup_psbt_finalized(Model& model) {
    auto node = new DataNode<PSBT>();
    model.add_node(node);
    node->set_info("psbt-finalized", "PSBT", "The finalized PSBT.");
    node->set_to_string([](const PSBT& p) { return p.ur(); });
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

DataNode<PSBT>* setup_psbt_finalized_hex(Model& model) {
    auto node = new DataNode<PSBT>();
    model.add_node(node);
    node->set_info("psbt-finalized-hex", "HEX", "Finalized PSBT in hex format.");
    node->set_to_string([](const PSBT& p) { return p.hex(); });
    model.add_derivation("psbt-finalized-hex <- [psbt-finalized]");
    node->set_derivation([&]() -> optional<PSBT> {
        if(model.psbt_finalized->has_value()) {
            return model.psbt_finalized->value();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<PSBT>* setup_psbt_finalized_base64(Model& model) {
    auto node = new DataNode<PSBT>();
    model.add_node(node);
    node->set_info("psbt-finalized-base64", "BASE64", "Finalized PSBT in Base64 format.");
    node->set_to_string([](const PSBT& p) { return p.base64(); });
    model.add_derivation("psbt-finalized-base64 <- [psbt-finalized]");
    node->set_derivation([&]() -> optional<PSBT> {
        if(model.psbt_finalized->has_value()) {
            return model.psbt_finalized->value();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<PSBT>* setup_psbt_signed(Model& model) {
    auto node = new DataNode<PSBT>();
    model.add_node(node);
    node->set_info("psbt-signed", "PSBT", "A PBST signed by address-key.");
    node->set_to_string([](const PSBT& p) { return p.ur(); });
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

DataNode<PSBT>* setup_psbt_signed_hex(Model& model) {
    auto node = new DataNode<PSBT>();
    model.add_node(node);
    node->set_info("psbt-signed-hex", "HEX", "Signed PSBT in hex format.");
    node->set_to_string([](const PSBT& p) { return p.hex(); });
    model.add_derivation("psbt-signed-hex <- [psbt-signed]");
    node->set_derivation([&]() -> optional<PSBT> {
        if(model.psbt_signed->has_value()) {
            return model.psbt_signed->value();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<PSBT>* setup_psbt_signed_base64(Model& model) {
    auto node = new DataNode<PSBT>();
    model.add_node(node);
    node->set_info("psbt-signed-base64", "BASE64", "Signed PSBT in Base64 format.");
    node->set_to_string([](const PSBT& p) { return p.base64(); });
    model.add_derivation("psbt-signed-base64 <- [psbt-signed]");
    node->set_derivation([&]() -> optional<PSBT> {
        if(model.psbt_signed->has_value()) {
            return model.psbt_signed->value();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<Transaction>* setup_transaction(Model& model) {
    auto node = new DataNode<Transaction>();
    model.add_node(node);
    node->set_info("transaction", "TRANSACTION", "The raw Bitcoin transaction.");
    node->set_to_string([](const Transaction& t) { return t.ur(); });
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

DataNode<Transaction>* setup_transaction_hex(Model& model) {
    auto node = new DataNode<Transaction>();
    model.add_node(node);
    node->set_info("transaction-hex", "HEX", "The raw Bitcoin transaction in Hex format.");
    node->set_to_string([](const Transaction& t) { return t.hex(); });
    model.add_derivation("transaction-hex <- [transaction]");
    node->set_derivation([&]() -> optional<Transaction> {
        if(model.transaction->has_value()) {
            return model.transaction->value();
        } else {
            return nullopt;
        }
    });
    return node;
}
