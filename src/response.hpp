#ifndef KEYTOOL_RESPONSE_HPP
#define KEYTOOL_RESPONSE_HPP

#include "uuid.hpp"
#include "seed.hpp"
#include "hd-key.hpp"
#include "utils.hpp"
#include <variant>
#include <optional>

typedef ByteVector PSBTSignatureResponseBody;

typedef std::variant<Seed, HDKey, PSBTSignatureResponseBody> ResponseBodyVariant;

class Response final {
public:
    Response(ResponseBodyVariant body, const UUID& id);
    explicit Response(const std::string& ur);

    const ResponseBodyVariant& body() const { return *_body; }
    const UUID& id() const { return *_id; }

    bool is_seed_response() const { return std::holds_alternative<Seed>(body()); }
    bool is_key_response() const { return std::holds_alternative<HDKey>(body()); }
    bool is_psbt_signature_response() const { return std::holds_alternative<PSBTSignatureResponseBody>(body()); }

    const Seed& seed_response() const { return std::get<Seed>(body()); }
    const HDKey& key_response() const { return std::get<HDKey>(body()); }
    const PSBTSignatureResponseBody& psbt_signature_response() const { return std::get<PSBTSignatureResponseBody>(body()); }

    std::string ur() const;

private:
    std::optional<UUID> _id;
    std::optional<ResponseBodyVariant> _body;
};

#endif
