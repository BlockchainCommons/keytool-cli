#include "model-seed-request.hpp"
#include "model.hpp"
#include <optional>

using namespace std;

DataNode<ByteVector>* setup_seed_digest(Model& model) {
    auto node = new DataNode<ByteVector>();
    model.add_node(node);
    node->set_info("seed-digest", "HEX", "The 16-byte SHA-256 digest of a seed.");
    node->set_to_string([](const ByteVector& data) { return data_to_hex(data); });
    node->set_from_string([](const string& s) -> ByteVector {
        auto data = hex_to_data(s);
        if(data.size() != 32) {
            throw domain_error("Digest must be exactly 16 bytes.");
        }
        return data;
    });
    model.add_derivation("seed-digest <- [seed-request]");
    model.add_derivation("seed-digest <- [seed]");
    node->set_derivation([&]() -> optional<ByteVector> {
        if(model.seed_request->has_assigned_value()) {
            return model.seed_request->value().seed_request().digest();
        // cppcheck-suppress internalAstError
        } else if(model.seed->has_value()) {
            return sha256(model.seed->value().data());
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<UUID>* setup_seed_request_id(Model& model) {
    auto node = new DataNode<UUID>();
    model.add_node(node);
    node->set_info("seed-request-id", "UUID", "The ID of the request and response.");
    node->set_to_string([](const UUID& uuid) { return uuid.str(); });
    node->set_from_string([](const string& s) -> UUID { return UUID(s); });
    model.add_derivation("seed-request-id <- [seed-request]");
    model.add_derivation("seed-request-id (default: unique)");
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

DataNode<string>* setup_seed_request_description(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("seed-request-description", "TEXT", "An informational note about the request.");
    node->set_to_string([](const string& s) { return s; });
    node->set_from_string([](const string& s) -> string { return s; });
    model.add_derivation("seed-request-description <- [seed-request]");
    model.add_derivation("seed-request-description (default: empty)");
    node->set_derivation([&]() -> optional<string> {
        if(model.seed_request->has_assigned_value()) {
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
    node->set_info("seed-request", "REQUEST", "A request for a seed with the given digest.");
    node->set_to_string([](const Request& request) { return request.ur(); });
    node->set_from_string([](const string& s) -> Request { return Request(s); });
    model.add_derivation("seed-request <- [seed-digest seed-request-id seed-request-description]");
    node->set_derivation([&]() -> optional<Request> {
        if(model.seed_digest->has_value()) {
            auto body = SeedRequestBody(model.seed_digest->value());
            auto description = model.seed_request_description->value();
            auto id = model.seed_request_id->value();
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
    node->set_info("seed-response", "RESPONSE", "A response containing the requested seed.");
    node->set_to_string([](const Response& response) { return response.ur(); });
    node->set_from_string([](const string& s) -> Response {
        auto response = Response(s);
        if(!response.is_seed_response()) {
            throw domain_error("Seed-response can only contain a ur:crypto-response with a seed.");
        }
        return response;
    });
    model.add_derivation("seed-response <- [seed-request, seed]");
    node->set_derivation([&]() -> optional<Response> {
        if(model.seed->has_value()) {
            auto body = model.seed->value();
            auto id = model.seed_request->value().id();
            if(body.digest() != model.seed_request->value().seed_request().digest()) {
                throw domain_error("Digest of seed does not match digest in request.");
            }
            return Response(body, id);
        } else {
            return nullopt;
        }
    });
    return node;
}
