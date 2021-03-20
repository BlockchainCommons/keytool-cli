#include "child-index-range.hpp"
#include <bc-ur/bc-ur.hpp>
#include <stdexcept>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

ChildIndexRange::ChildIndexRange(ChildIndex low, ChildIndex high) : _low(low), _high(high) {
    if(high < low) {
        throw domain_error("Invalid child index range.");
    }
}

optional<ChildIndexRange> ChildIndexRange::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    auto tag = undefined;
    size_t array_size;
    decodeTagAndValue(pos, end, tag, array_size, cborDecodingFlags);
    if(tag != Major::array) {
        return nullopt;
    }
    if(array_size != 2) {
        return nullopt;
    }
    auto low = ChildIndex::decode_cbor(pos, end);
    if(!low.has_value()) {
        return nullopt;
    }
    auto high = ChildIndex::decode_cbor(pos, end);
    if(!high.has_value()) {
        return nullopt;
    }
    return ChildIndexRange(*low, *high);
}

void ChildIndexRange::encode_cbor(ByteVector& cbor) const {
    size_t array_size = 2;
    encodeArraySize(cbor, array_size);
    low().encode_cbor(cbor);
    high().encode_cbor(cbor);
}
