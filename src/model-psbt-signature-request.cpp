#include "model-psbt-signature-request.hpp"
#include "model.hpp"
#include <optional>

using namespace std;

DataNode<UUID>* setup_psbt_signature_request_id(Model& model) {
    auto node = new DataNode<UUID>();
    model.add_node(node);
    node->set_info("psbt-signature-request-id", "UUID", "The ID of the request and response.");
    node->set_to_string([](const UUID& uuid) { return uuid.str(); });
    node->set_from_string([](const string& s) -> UUID { return UUID(s); });
    model.add_derivation("psbt-signature-request-id <- [psbt-signature-request]");
    model.add_derivation("psbt-signature-request-id <- [psbt-signature-response]");
    model.add_derivation("psbt-signature-request-id (default: unique)");
    node->set_derivation([&]() -> optional<UUID> {
        if(model.psbt_signature_request->has_assigned_value()) {
            return model.psbt_signature_request->value().id();
        } else if(model.psbt_signature_response->has_assigned_value()) {
            return model.psbt_signature_response->value().id();
        } else {
            return UUID();
        }
    });
    return node;
}

DataNode<string>* setup_psbt_signature_request_description(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("psbt-signature-request-description", "TEXT", "An informational note about the request.");
    node->set_to_string([](const string& s) { return s; });
    node->set_from_string([](const string& s) -> string { return s; });
    model.add_derivation("psbt-signature-request-description <- [psbt-signature-request]");
    model.add_derivation("psbt-signature-request-description (default: empty)");
    node->set_derivation([&]() -> optional<string> {
        if(model.psbt_signature_request->has_assigned_value()) {
            return model.psbt_signature_request->value().description();
        } else {
            return "";
        }
    });
    return node;
}

DataNode<Request>* setup_psbt_signature_request(Model& model) {
    auto node = new DataNode<Request>();
    model.add_node(node);
    node->set_info("psbt-signature-request", "REQUEST", "A request to sign a PSBT.");
    node->set_to_string([](const Request& request) { return request.ur(); });
    node->set_from_string([](const string& s) -> Request { return Request(s); });
    model.add_derivation("psbt-signature-request <- [psbt psbt-signature-request-id psbt-signature-request-description]");
    node->set_derivation([&]() -> optional<Request> {
        if(model.psbt->has_value()) {
            auto body = PSBTSignatureRequestBody(model.psbt->value());
            auto description = model.psbt_signature_request_description->value();
            auto id = model.psbt_signature_request_id->value();
            return Request(body, description, id);
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<Response>* setup_psbt_signature_response(Model& model) {
    auto node = new DataNode<Response>();
    model.add_node(node);
    node->set_info("psbt-signature-response", "RESPONSE", "A response containing the signed PSBT.");
    node->set_to_string([](const Response& response) { return response.ur(); });
    node->set_from_string([](const string& s) -> Response {
        auto response = Response(s);
        if(!response.is_psbt_signature_response()) {
            throw domain_error("Psbt-signature-response can only contain a ur:crypto-response with a PSBT.");
        }
        return response;
    });
    model.add_derivation("psbt-signature-response <- [psbt-signature-request, psbt-signed]");
    node->set_derivation([&]() -> optional<Response> {
        if(model.psbt_signed->has_value()) {
            auto body = model.psbt_signed->value();
            auto id = model.psbt_signature_request->value().id();
            return Response(body, id);
        } else {
            return nullopt;
        }
    });
    return node;
}
