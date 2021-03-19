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

ChildIndexRange ChildIndexRange::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    size_t array_size;
    decodeArraySize(pos, end, array_size, cborDecodingFlags);
    if(array_size != 2) {
        throw domain_error("Invalid child index range.");
    }
    auto low = ChildIndex::decode_cbor(pos, end);
    auto high = ChildIndex::decode_cbor(pos, end);
    return ChildIndexRange(low, high);
}

void ChildIndexRange::encode_cbor(ByteVector& cbor) const {
    size_t array_size = 2;
    encodeArraySize(cbor, array_size);
    low().encode_cbor(cbor);
    high().encode_cbor(cbor);
}
