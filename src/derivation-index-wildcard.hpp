#ifndef KEYTOOL_DERIVATION_INDEX_WILDCARD_HPP
#define KEYTOOL_DERIVATION_INDEX_WILDCARD_HPP

#include "utils.hpp"
#include <stdint.h>
#include <ostream>
#include <optional>

class DerivationIndexWildcard final {
public:
    DerivationIndexWildcard();

    static std::optional<DerivationIndexWildcard> decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);
    void encode_cbor(ByteVector& cbor) const;

    inline friend std::ostream& operator<< (std::ostream& os, const DerivationIndexWildcard& rhs) {
        return os << "*";
    }

    static std::optional<DerivationIndexWildcard> from_string(const std::string& s);
    std::string to_string() const;
};

#endif
