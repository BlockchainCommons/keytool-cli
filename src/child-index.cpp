#include "child-index.hpp"
#include <bc-ur/bc-ur.hpp>
#include <stdexcept>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

ChildIndex::ChildIndex(uint32_t value) : _value(value) {
    if(value & 0x80000000) {
        throw domain_error("Invalid child index.");
    }
}

optional<ChildIndex> ChildIndex::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    Tag tag = undefined;
    uint64_t value;
    auto p = pos;
    decodeTagAndValue(p, end, tag, value, cborDecodingFlags);
    if(tag != Major::unsignedInteger) {
        return nullopt;
    }
    if(value >= 0x80000000) {
        throw domain_error("Invalid child index.");
    }
    pos = p;
    return value;
}

void ChildIndex::encode_cbor(ByteVector& cbor) const {
    encodeInteger(cbor, _value);
}
