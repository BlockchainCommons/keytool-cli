#ifndef KEYTOOL_REQUEST_HPP
#define KEYTOOL_REQUEST_HPP

#include "uuid.hpp"
#include "utils.hpp"
#include <string>

class Request {
public:
    Request(const std::string& description = "", const UUID& id = UUID());

    const UUID& id() const { return _id; }
    const std::string& description() const { return _description; }

    std::string ur() const;

protected:
    virtual void encode_body(ByteVector& cbor) const = 0;

private:
    UUID _id;
    std::string _description;
};

class SeedRequest: public Request {
public:
    SeedRequest(const ByteVector& digest, const std::string& description, const UUID& id = UUID());

    const ByteVector& digest() const { return _digest; }

protected:
    virtual void encode_body(ByteVector& cbor) const override;

private:
    ByteVector _digest;
};

class KeyRequest: public Request {

};

class PSBTSignatureRequest: public Request {

};

#endif
