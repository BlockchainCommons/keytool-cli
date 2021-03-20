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
    decodeTagAndValue(pos, end, tag, array_size, cborDecodingFlags);
    if(tag != Major::array) {
        return nullopt;
    }
    if(array_size != 0) {
        return nullopt;
    }
    return ChildIndexWildcard();
}

void ChildIndexWildcard::encode_cbor(ByteVector& cbor) const {
    size_t array_size = 0;
    encodeArraySize(cbor, array_size);
}
