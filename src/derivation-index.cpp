#include "derivation-index.hpp"
#include <bc-ur/bc-ur.hpp>
#include <stdexcept>
#include <sstream>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

DerivationIndex::DerivationIndex(uint32_t value) : _value(value) {
    if(value & 0x80000000) {
        throw domain_error("Invalid child index.");
    }
}

optional<DerivationIndex> DerivationIndex::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    Tag tag = undefined;
    uint64_t value;
    auto p = pos;
    decodeTagAndValue(p, end, tag, value, cbor_decoding_flags);
    if(tag != Major::unsignedInteger) {
        return nullopt;
    }
    if(value >= 0x80000000) {
        throw domain_error("Invalid child index.");
    }
    pos = p;
    return value;
}

void DerivationIndex::encode_cbor(ByteVector& cbor) const {
    encodeInteger(cbor, _value);
}

optional<DerivationIndex> DerivationIndex::from_string(const string& s) {
    istringstream in(s);
    uint32_t value;
    in >> value;
    if(in.fail()) {
        return nullopt;
    } else {
        return value;
    }
}

string DerivationIndex::to_string() const {
    ostringstream o;
    o << *this;
    return o.str();
}
