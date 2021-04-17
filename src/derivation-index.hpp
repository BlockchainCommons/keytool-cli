#ifndef KEYTOOL_DERIVATION_INDEX_HPP
#define KEYTOOL_DERIVATION_INDEX_HPP

#include "utils.hpp"
#include <stdint.h>
#include <ostream>
#include <optional>

class DerivationIndex final {
public:
    // cppcheck-suppress noExplicitConstructor
    DerivationIndex(uint32_t value);

    uint32_t value() const { return _value; }

    static std::optional<DerivationIndex> decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);
    void encode_cbor(ByteVector& cbor) const;

    inline friend bool operator ==(const DerivationIndex& lhs, const DerivationIndex& rhs) {
        return lhs.value() == rhs.value();
    }

    inline friend bool operator <(const DerivationIndex& lhs, const DerivationIndex& rhs) {
        return lhs.value() < rhs.value();
    }

    inline friend std::ostream& operator<< (std::ostream& os, const DerivationIndex& rhs) {
        return os << rhs.value();
    }

    static std::optional<DerivationIndex> from_string(const std::string& s);
    std::string to_string() const;

private:
    uint32_t _value;
};

#endif
