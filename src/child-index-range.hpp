#ifndef KEYTOOL_CHILD_INDEX_RANGE_HPP
#define KEYTOOL_CHILD_INDEX_RANGE_HPP

#include "utils.hpp"
#include "child-index.hpp"
#include <stdint.h>
#include <ostream>
#include <optional>

class ChildIndexRange final {
public:
    ChildIndexRange(ChildIndex low, ChildIndex high);

    ChildIndex low() const { return _low; }
    ChildIndex high() const { return _high; }

    static std::optional<ChildIndexRange> decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);
    void encode_cbor(ByteVector& cbor) const;

    inline friend std::ostream& operator<< (std::ostream& os, const ChildIndexRange& rhs) {
        return os << rhs.low() << "-" << rhs.high();
    }

private:
    ChildIndex _low;
    ChildIndex _high;
};

#endif
