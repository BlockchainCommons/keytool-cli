#ifndef KEYTOOL_DERIVATION_INDEX_RANGE_HPP
#define KEYTOOL_DERIVATION_INDEX_RANGE_HPP

#include "utils.hpp"
#include "derivation-index.hpp"
#include <stdint.h>
#include <ostream>
#include <optional>

class DerivationIndexRange final {
public:
    DerivationIndexRange(DerivationIndex low, DerivationIndex high);

    DerivationIndex low() const { return _low; }
    DerivationIndex high() const { return _high; }

    static std::optional<DerivationIndexRange> decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);
    void encode_cbor(ByteVector& cbor) const;

    inline friend std::ostream& operator<< (std::ostream& os, const DerivationIndexRange& rhs) {
        return os << rhs.low() << "-" << rhs.high();
    }

    static std::optional<DerivationIndexRange> from_string(const std::string& s);
    std::string to_string() const;

private:
    DerivationIndex _low;
    DerivationIndex _high;
};

#endif
