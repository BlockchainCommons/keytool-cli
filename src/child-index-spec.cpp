#include "child-index-spec.hpp"
#include <bc-ur/bc-ur.hpp>
#include <stdexcept>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

ChildIndexSpec::ChildIndexSpec(ChildIndexSpecValue value) : _value(value) { }

ChildIndexSpec ChildIndexSpec::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    optional<ChildIndexSpecValue> value;

    value = ChildIndex::decode_cbor(pos, end);
    if(value.has_value()) {
        return *value;
    }

    value = ChildIndexRange::decode_cbor(pos, end);
    if(value.has_value()) {
        return *value;
    }

    value = ChildIndexWildcard::decode_cbor(pos, end);
    if(value.has_value()) {
        return *value;
    }

    throw domain_error("Invalid ChildIndexSpec.");
}

void ChildIndexSpec::encode_cbor(ByteVector& cbor) const {
    if(is_index()) {
        index().encode_cbor(cbor);
    } else if(is_range()) {
        range().encode_cbor(cbor);
    } else if(is_wildcard()) {
        wildcard().encode_cbor(cbor);
    }
}

ostream& operator<< (std::ostream& os, const ChildIndexSpec& rhs) {
    if(rhs.is_index()) {
        return os << rhs.index();
    } else if(rhs.is_range()) {
        return os << rhs.range();
    } else if(rhs.is_wildcard()) {
        return os << rhs.wildcard();
    } else {
        throw runtime_error("Unknown child index spec.");
    }
}
