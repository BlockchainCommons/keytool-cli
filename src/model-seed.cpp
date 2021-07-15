#include "model-seed.hpp"
#include "model.hpp"
#include <optional>
#include <iostream>

using namespace std;

DataNode<ByteVector>* setup_seed_hex(Model& model) {
    auto node = new DataNode<ByteVector>();
    model.add_node(node);
    node->set_info("seed-hex", "HEX", "Random data from which to generate a master key.");
    node->set_to_string([](const ByteVector& data) { return data_to_hex(data); });
    node->set_from_string([](const string& s) -> ByteVector { return hex_to_data(s); });

    model.add_derivation("seed-hex <- [seed]");
    node->set_derivation([&]() -> optional<ByteVector> {
        if(model.seed->has_value()) {
            return model.seed->value().data();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<string>* setup_seed_name(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("seed-name", "TEXT", "The name of the seed.");
    node->set_to_string([](const string& s) { return s; });
    node->set_from_string([](const string& s) -> string { return s; });

    model.add_derivation("seed-name <- [seed]");
    node->set_derivation([&]() -> optional<string> {
        if(model.seed->has_assigned_value()) {
            return model.seed->value().name();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<string>* setup_seed_note(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("seed-note", "TEXT", "An informational note about the seed.");
    node->set_to_string([](const string& s) { return s; });
    node->set_from_string([](const string& s) -> string { return s; });

    model.add_derivation("seed-note <- [seed]");
    node->set_derivation([&]() -> optional<string> {
        if(model.seed->has_assigned_value()) {
            return model.seed->value().note();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<Seed>* setup_seed(Model& model) {
    model.add_derivation("seed <- [seed-hex, seed-name (optional), seed-note (optional)]");
    auto node = new DataNode<Seed>();
    model.add_node(node);
    node->set_info("seed", "SEED", "A seed in UR or HEX format.");
    node->set_to_string([](const Seed& seed) { return seed.ur(); });
    node->set_from_string([](const string& s) -> Seed { return Seed(s); });
    model.add_derivation("seed <- [seed-response]");
    node->set_derivation([&]() -> optional<Seed> {
        if(model.seed_response->has_assigned_value()) {
            return model.seed_response->value().seed_response();
        } else if(model.seed_hex->has_value()) {
            auto data = model.seed_hex->value();
            auto name = model.seed_name->has_value() ? model.seed_name->value() : "";
            auto note = model.seed_note->has_value() ? model.seed_note->value() : "";
            return Seed(data, name, note);
        } else {
            return nullopt;
        }
    });
    return node;
}
