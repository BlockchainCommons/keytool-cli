#ifndef KEYTOOL_CHILD_INDEX_SPEC_HPP
#define KEYTOOL_CHILD_INDEX_SPEC_HPP

#include "utils.hpp"
#include "child-index.hpp"
#include "child-index-range.hpp"
#include "child-index-wildcard.hpp"
#include <stdint.h>
#include <ostream>
#include <optional>
#include <variant>

typedef std::variant<ChildIndex, ChildIndexRange, ChildIndexWildcard> ChildIndexSpecValue;

class ChildIndexSpec final {
public:
    ChildIndexSpec(ChildIndexSpecValue value);

    ChildIndexSpecValue value() const { return _value; }

    static ChildIndexSpec decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);
    void encode_cbor(ByteVector& cbor) const;

    bool is_index() const { return std::holds_alternative<ChildIndex>(value()); }
    bool is_range() const { return std::holds_alternative<ChildIndexRange>(value()); }
    bool is_wildcard() const { return std::holds_alternative<ChildIndexWildcard>(value()); }

    const ChildIndex index() const { return std::get<ChildIndex>(value()); }
    const ChildIndexRange range() const { return std::get<ChildIndexRange>(value()); }
    const ChildIndexWildcard wildcard() const { return std::get<ChildIndexWildcard>(value()); }

private:
    ChildIndexSpecValue _value;
};

#endif
