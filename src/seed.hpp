#ifndef KEYTOOL_SEED_HPP
#define KEYTOOL_SEED_HPP

#include <string>
#include "utils.hpp"

class Seed final {
public:
    explicit Seed(const ByteVector& data, const std::string& name = "", const std::string& note = "");
    Seed(const Seed& seed);
    explicit Seed(const std::string& s); // hex or ur:crypto-seed
    static Seed from_cbor(const ByteVector& cbor);
    static Seed decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);
    static Seed decode_tagged_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);

    std::string hex() const;
    std::string ur() const;
    ByteVector cbor() const;
    ByteVector tagged_cbor() const;

    const ByteVector& data() const { return _data; }
    const std::string& name() const { return _name; }
    const std::string& note() const { return _note; }

    ByteVector digest() const;

private:
    ByteVector _data;
    std::string _name;
    std::string _note;
};

#endif
