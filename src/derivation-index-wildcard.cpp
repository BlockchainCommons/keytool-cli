#include "derivation-index-wildcard.hpp"
#include <bc-ur/bc-ur.hpp>
#include <stdexcept>
#include <sstream>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

DerivationIndexWildcard::DerivationIndexWildcard() { }

std::optional<DerivationIndexWildcard> DerivationIndexWildcard::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    auto tag = undefined;
    size_t array_size;
    auto p = pos;
    decodeTagAndValue(p, end, tag, array_size, cbor_decoding_flags);
    if(tag != Major::array) {
        return nullopt;
    }
    if(array_size != 0) {
        return nullopt;
    }
    pos = p;
    return DerivationIndexWildcard();
}

void DerivationIndexWildcard::encode_cbor(ByteVector& cbor) const {
    size_t array_size = 0;
    encodeArraySize(cbor, array_size);
}

optional<DerivationIndexWildcard> DerivationIndexWildcard::from_string(const string& s) {
    if(s == "*") {
        return DerivationIndexWildcard();
    } else {
        return nullopt;
    }
}

string DerivationIndexWildcard::to_string() const {
    ostringstream o;
    o << *this;
    return o.str();
}
