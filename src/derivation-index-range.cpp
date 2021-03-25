#include "derivation-index-range.hpp"
#include <bc-ur/bc-ur.hpp>
#include <stdexcept>
#include <sstream>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

DerivationIndexRange::DerivationIndexRange(DerivationIndex low, DerivationIndex high) : _low(low), _high(high) {
    if(high < low) {
        throw domain_error("Invalid child index range.");
    }
}

optional<DerivationIndexRange> DerivationIndexRange::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    auto tag = undefined;
    size_t array_size;
    auto p = pos;
    decodeTagAndValue(p, end, tag, array_size, cbor_decoding_flags);
    if(tag != Major::array) {
        return nullopt;
    }
    if(array_size != 2) {
        return nullopt;
    }
    auto low = DerivationIndex::decode_cbor(p, end);
    if(!low.has_value()) {
        return nullopt;
    }
    auto high = DerivationIndex::decode_cbor(p, end);
    if(!high.has_value()) {
        return nullopt;
    }
    pos = p;
    return DerivationIndexRange(*low, *high);
}

void DerivationIndexRange::encode_cbor(ByteVector& cbor) const {
    size_t array_size = 2;
    encodeArraySize(cbor, array_size);
    low().encode_cbor(cbor);
    high().encode_cbor(cbor);
}

optional<DerivationIndexRange> DerivationIndexRange::from_string(const string& s) {
    istringstream in(s);
    uint32_t low;
    uint32_t high;
    char c;
    in >> low >> c >> high;
    if(in.fail() || c != '-') {
        return nullopt;
    } else {
        return DerivationIndexRange(low, high);
    }
}

string DerivationIndexRange::to_string() const {
    ostringstream o;
    o << *this;
    return o.str();
}
