#ifndef KEYTOOL_CHILD_INDEX_WILDCARD_HPP
#define KEYTOOL_CHILD_INDEX_WILDCARD_HPP

#include "utils.hpp"
#include <stdint.h>
#include <ostream>
#include <optional>

class ChildIndexWildcard final {
public:
    ChildIndexWildcard();

    static std::optional<ChildIndexWildcard> decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);
    void encode_cbor(ByteVector& cbor) const;

    inline friend std::ostream& operator<< (std::ostream& os, const ChildIndexWildcard& rhs) {
        return os << "*";
    }
};

#endif
