#ifndef KEYTOOL_DERIVATION_INDEX_SPEC_HPP
#define KEYTOOL_DERIVATION_INDEX_SPEC_HPP

#include "utils.hpp"
#include "derivation-index.hpp"
#include "derivation-index-range.hpp"
#include "derivation-index-wildcard.hpp"
#include <stdint.h>
#include <ostream>
#include <optional>
#include <variant>

typedef std::variant<DerivationIndex, DerivationIndexRange, DerivationIndexWildcard> DerivationIndexSpecValue;

class DerivationIndexSpec final {
public:
    // cppcheck-suppress noExplicitConstructor
    DerivationIndexSpec(DerivationIndexSpecValue value) : _value(value) { }
    // cppcheck-suppress noExplicitConstructor
    DerivationIndexSpec(uint32_t index) : _value(DerivationIndex(index)) { }

    DerivationIndexSpecValue value() const { return _value; }

    static DerivationIndexSpec decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);
    void encode_cbor(ByteVector& cbor) const;

    bool is_index() const { return std::holds_alternative<DerivationIndex>(value()); }
    bool is_range() const { return std::holds_alternative<DerivationIndexRange>(value()); }
    bool is_wildcard() const { return std::holds_alternative<DerivationIndexWildcard>(value()); }

    const DerivationIndex index() const { return std::get<DerivationIndex>(value()); }
    const DerivationIndexRange range() const { return std::get<DerivationIndexRange>(value()); }
    const DerivationIndexWildcard wildcard() const { return std::get<DerivationIndexWildcard>(value()); }

    std::string to_string() const;
    static DerivationIndexSpec from_string(const std::string& s);

private:
    DerivationIndexSpecValue _value;
};

std::ostream& operator<< (std::ostream& os, const DerivationIndexSpec& rhs);

#endif
