#include "model-seed.hpp"
#include "model.hpp"
#include <optional>

using namespace std;

DataNode<ByteVector>* setup_seed(Model& model) {
    auto node = new DataNode<ByteVector>();
    model.add_node(node);
    node->set_info("seed", "HEX", "Random data from which to generate a master key.");
    node->set_to_string([](const ByteVector& data) { return data_to_hex(data); });
    node->set_from_string([](const string& s) -> ByteVector { return hex_to_data(s); });

    model.add_derivation("seed <- [seed-ur]");
    node->set_derivation([&]() -> optional<ByteVector> {
        if(model.seed_ur->has_value()) {
            return model.seed_ur->value().data();
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

    model.add_derivation("seed-name <- [seed-ur]");
    node->set_derivation([&]() -> optional<string> {
        if(model.seed_ur->has_value()) {
            return model.seed_ur->value().name();
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

    model.add_derivation("seed-note <- [seed-ur]");
    node->set_derivation([&]() -> optional<string> {
        if(model.seed_ur->has_value()) {
            return model.seed_ur->value().note();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<Seed>* setup_seed_ur(Model& model) {
    model.add_derivation("seed-ur <- [seed, seed-name (optional), seed-note (optional)]");
    auto node = new DataNode<Seed>();
    model.add_node(node);
    node->set_info("seed-ur", "UR:CRYPTO-SEED", "A seed in UR format.");
    node->set_to_string([](const Seed& seed) { return seed.ur(); });
    node->set_from_string([](const string& s) -> Seed { return Seed(s); });
    node->set_derivation([&]() -> optional<Seed> {
        if(model.seed->has_value()) {
            auto data = model.seed->value();
            auto name = model.seed_name->has_value() ? model.seed_name->value() : "";
            auto note = model.seed_note->has_value() ? model.seed_note->value() : "";
            return Seed(data, name, note);
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<ByteVector>* setup_seed_digest(Model& model) {
    auto node = new DataNode<ByteVector>();
    model.add_node(node);
    node->set_info("seed-digest", "HEX (16 bytes)", "The SHA-256 digest of a seed.");
    node->set_to_string([](const ByteVector& data) { return data_to_hex(data); });
    node->set_from_string([](const string& s) -> ByteVector {
        auto data = hex_to_data(s);
        if(data.size() != 32) {
            throw domain_error("Digest must be exactly 16 bytes.");
        }
        return data;
    });
    model.add_derivation("seed-digest <- [seed]");
    node->set_derivation([&]() -> optional<ByteVector> {
        if(model.seed->has_value()) {
            return sha256(model.seed->value());
        } else {
            return nullopt;
        }
    });
    return node;
}
