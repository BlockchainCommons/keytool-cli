#include "derivation-index-spec.hpp"
#include <bc-ur/bc-ur.hpp>
#include <stdexcept>
#include <sstream>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

DerivationIndexSpec DerivationIndexSpec::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    optional<DerivationIndexSpecValue> value;

    value = DerivationIndex::decode_cbor(pos, end);
    if(value.has_value()) {
        return *value;
    }

    value = DerivationIndexRange::decode_cbor(pos, end);
    if(value.has_value()) {
        return *value;
    }

    value = DerivationIndexWildcard::decode_cbor(pos, end);
    if(value.has_value()) {
        return *value;
    }

    throw domain_error("Invalid DerivationIndexSpec.");
}

void DerivationIndexSpec::encode_cbor(ByteVector& cbor) const {
    if(is_index()) {
        index().encode_cbor(cbor);
    } else if(is_range()) {
        range().encode_cbor(cbor);
    } else if(is_wildcard()) {
        wildcard().encode_cbor(cbor);
    }
}

ostream& operator<< (std::ostream& os, const DerivationIndexSpec& rhs) {
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

string DerivationIndexSpec::to_string() const {
    ostringstream o;
    o << *this;
    return o.str();
}

DerivationIndexSpec DerivationIndexSpec::from_string(const string& s) {
    auto range = DerivationIndexRange::from_string(s);
    if(range) {
        return DerivationIndexSpec(*range);
    }
    auto wildcard = DerivationIndexWildcard::from_string(s);
    if(wildcard) {
        return DerivationIndexSpec(*wildcard);
    }
    auto index = DerivationIndex::from_string(s);
    if(index) {
        return DerivationIndexSpec(*index);
    }
    throw domain_error("Invalid derivation path element.");
}
