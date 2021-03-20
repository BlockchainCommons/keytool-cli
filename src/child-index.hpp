#ifndef KEYTOOL_CHILD_INDEX_HPP
#define KEYTOOL_CHILD_INDEX_HPP

#include "utils.hpp"
#include <stdint.h>
#include <ostream>
#include <optional>

class ChildIndex final {
public:
    ChildIndex(uint32_t value);

    uint32_t value() const { return _value; }

    static std::optional<ChildIndex> decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);
    void encode_cbor(ByteVector& cbor) const;

    inline friend bool operator ==(const ChildIndex& lhs, const ChildIndex& rhs) {
        return lhs.value() == rhs.value();
    }

    inline friend bool operator <(const ChildIndex& lhs, const ChildIndex& rhs) {
        return lhs.value() < rhs.value();
    }

    inline friend std::ostream& operator<< (std::ostream& os, const ChildIndex& rhs) {
        return os << rhs.value();
    }

private:
    uint32_t _value;
};

#endif
