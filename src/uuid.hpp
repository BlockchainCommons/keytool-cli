#ifndef KEYTOOL_UUID_HPP
#define KEYTOOL_UUID_HPP

#include <string>
#include "utils.hpp"

class UUID final {
public:
    UUID();
    explicit UUID(const std::string& uuid_str);
    explicit UUID(const ByteVector& data);

    const ByteVector& data() const { return _data; }
    const std::string str() const;

    const void encode_cbor(ByteVector &buf) const;
    static UUID decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);

    template<typename ostream>
    inline friend ostream &operator<<( ostream &os, const UUID &self ) {
        return os << self.str(), os;
    }

private:
    ByteVector _data;
};

#endif
