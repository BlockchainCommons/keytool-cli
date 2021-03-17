#ifndef KEYTOOL_REQUEST_HPP
#define KEYTOOL_REQUEST_HPP

#include "uuid.hpp"
#include "utils.hpp"
#include <string>
#include <variant>
#include <optional>

class SeedRequestBody final {
public:
    SeedRequestBody(const ByteVector& digest) : _digest(digest) { }

    const ByteVector& digest() const { return _digest; }

    void encode_cbor(ByteVector& cbor) const;
    static SeedRequestBody decode_cbor(ByteVector::iterator& pos, ByteVector::iterator end);

private:
    ByteVector _digest;
};

class KeyRequestBody final {
public:
    KeyRequestBody() { }

    void encode_cbor(ByteVector& cbor) const;
    static KeyRequestBody decode_cbor(ByteVector::iterator& pos, ByteVector::iterator end);

private:
};

class PSBTSignatureRequestBody final {
public:
    PSBTSignatureRequestBody() { }

    void encode_cbor(ByteVector& cbor) const;
    static PSBTSignatureRequestBody decode_cbor(ByteVector::iterator& pos, ByteVector::iterator end);

private:
};

typedef std::variant<SeedRequestBody, KeyRequestBody, PSBTSignatureRequestBody> RequestBodyVariant;

class Request final {
public:
    Request(RequestBodyVariant body, const std::string& description = "", const UUID& id = UUID());
    Request(const std::string& ur);

    const RequestBodyVariant& body() const { return *_body; }
    const std::string& description() const { return _description; }
    const UUID& id() const { return *_id; }

    std::string ur() const;

private:
    std::optional<UUID> _id;
    std::string _description;
    std::optional<RequestBodyVariant> _body;
};

#endif
