#ifndef KEYTOOL_UUID_HPP
#define KEYTOOL_UUID_HPP

#include <string>
#include "utils.hpp"

class UUID final {
public:
    UUID();
    UUID(const std::string& uuid_str);
    UUID(const ByteVector& data);

    const ByteVector& data() const { return _data; }
    const std::string str() const;

    const void encode_cbor(ByteVector &buf) const;
    static UUID decode_cbor(ByteVector::iterator& pos, ByteVector::iterator end);

    template<typename ostream>
    inline friend ostream &operator<<( ostream &os, const UUID &self ) {
        return os << self.str(), os;
    }

private:
    ByteVector _data;
};

#endif
