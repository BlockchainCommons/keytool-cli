#include "child-index-wildcard.hpp"
#include <bc-ur/bc-ur.hpp>
#include <stdexcept>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

ChildIndexWildcard::ChildIndexWildcard() { }

std::optional<ChildIndexWildcard> ChildIndexWildcard::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
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
    return ChildIndexWildcard();
}

void ChildIndexWildcard::encode_cbor(ByteVector& cbor) const {
    size_t array_size = 0;
    encodeArraySize(cbor, array_size);
}
