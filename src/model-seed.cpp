#include "model-seed.hpp"
#include "model.hpp"
#include <optional>
#include <iostream>

using namespace std;

DataNode<ByteVector>* setup_seed(Model& model) {
    auto node = new DataNode<ByteVector>();
    model.add_node(node);
    node->set_info("seed", "HEX", "Random data from which to generate a master key.");
    node->set_to_string([](const ByteVector& data) { return data_to_hex(data); });
    node->set_from_string([](const string& s) -> ByteVector { return hex_to_data(s); });

    model.add_derivation("seed <- [seed-ur]");
    node->set_derivation([&]() -> optional<ByteVector> {
        if(model.seed_ur->has_assigned_value()) {
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
        if(model.seed_ur->has_assigned_value()) {
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
        if(model.seed_ur->has_assigned_value()) {
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
        if(model.seed_response->has_assigned_value()) {
            return model.seed_response->value().seed_response();
        } else if(model.seed->has_value()) {
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
    model.add_derivation("seed-digest <- [seed-request]");
    node->set_derivation([&]() -> optional<ByteVector> {
        if(model.seed_request->has_assigned_value()) {
            return model.seed_request->value().seed_request().digest();
        } else if(model.seed->has_value()) {
            return sha256(model.seed->value());
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<UUID>* setup_request_id(Model& model) {
    auto node = new DataNode<UUID>();
    model.add_node(node);
    node->set_info("request-id", "UUID (default: unique)", "The ID of the request and response.");
    node->set_to_string([](const UUID& uuid) { return uuid.str(); });
    node->set_from_string([](const string& s) -> UUID { return UUID(s); });
    model.add_derivation("request-id <- [seed-request] (default: unique)");
    node->set_derivation([&]() -> optional<UUID> {
        if(model.seed_request->has_assigned_value()) {
            return model.seed_request->value().id();
        } else if(model.seed_response->has_assigned_value()) {
            return model.seed_response->value().id();
        } else {
            return UUID();
        }
    });
    return node;
}

DataNode<string>* setup_request_description(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("request-description", "TEXT", "An informational note about the request.");
    node->set_to_string([](const string& s) { return s; });
    node->set_from_string([](const string& s) -> string { return s; });
    model.add_derivation("request-description <- [seed-request] (default: empty)");
    node->set_derivation([&]() -> optional<string> {
        if(model.seed_request->has_value()) {
            return model.seed_request->value().description();
        } else {
            return "";
        }
    });
    return node;
}

DataNode<Request>* setup_seed_request(Model& model) {
    auto node = new DataNode<Request>();
    model.add_node(node);
    node->set_info("seed-request", "UR:CRYPTO-REQUEST", "A request for a seed with the given digest.");
    node->set_to_string([](const Request& request) { return request.ur(); });
    node->set_from_string([](const string& s) -> Request { return Request(s); });
    model.add_derivation("seed-request <- [seed-digest request-id request-description]");
    node->set_derivation([&]() -> optional<Request> {
        if(model.seed_digest->has_value()) {
            auto body = SeedRequestBody(model.seed_digest->value());
            auto description = model.request_description->value();
            auto id = model.request_id->value();
            return Request(body, description, id);
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<Response>* setup_seed_response(Model& model) {
    auto node = new DataNode<Response>();
    model.add_node(node);
    node->set_info("seed-response", "UR:CRYPTO-RESPONSE", "A response containing the requested seed.");
    node->set_to_string([](const Response& response) { return response.ur(); });
    node->set_from_string([](const string& s) -> Response { return Response(s); });
    model.add_derivation("seed-response <- [request-id, seed-ur]");
    node->set_derivation([&]() -> optional<Response> {
        if(model.seed_ur->has_value()) {
            auto body = model.seed_ur->value();
            auto id = model.request_id->value();
            return Response(body, id);
        } else {
            return nullopt;
        }
    });
    return node;
}
