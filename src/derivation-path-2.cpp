#include "derivation-path-2.hpp"
#include "utils.hpp"
#include <set>
#include <bc-ur/bc-ur.hpp>
#include <limits>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

DerivationPath2::DerivationPath2(
    const vector<DerivationStep>& steps,
    optional<uint32_t> source_fingerprint,
    optional<uint8_t> depth
)
    : _steps(steps)
    , _source_fingerprint(source_fingerprint)
    , _depth(depth)
{ }

DerivationPath2::DerivationPath2(uint32_t source_fingerprint)
    : _steps{}
    , _source_fingerprint(source_fingerprint)
    , _depth(nullopt)
{ }

uint8_t DerivationPath2::effective_depth() const {
    if(depth().has_value()) {
        return *depth();
    } else {
        return steps().size();
    }
}

DerivationPath2 DerivationPath2::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    size_t map_len;
    decodeMapSize(pos, end, map_len, cbor_decoding_flags);
    set<int> labels;
    vector<DerivationStep> steps;
    optional<uint32_t> source_fingerprint;
    optional<uint8_t> depth;
    for(auto index = 0; index < map_len; index++) {
        int label;
        decodeInteger(pos, end, label, cbor_decoding_flags);
        if(labels.find(label) != labels.end()) {
            throw domain_error("Duplicate label.");
        }
        labels.insert(label);
        switch(label) {
            case 1: { // steps
                size_t array_size;
                decodeArraySize(pos, end, array_size, cbor_decoding_flags);
                // each step takes 2 array entries
                if(array_size % 2 != 0) {
                    throw domain_error("Invalid DerivationPath components.");
                }
                auto steps_count = array_size / 2;
                for(auto i = 0; i < steps_count; i++) {
                    steps.push_back(DerivationStep::decode_cbor(pos, end));
                }
            }
                break;
            case 2: { // source_fingerprint
                uint64_t f;
                decodeUnsigned(pos, end, f, cbor_decoding_flags);
                if(f > numeric_limits<uint32_t>::max()) {
                    throw domain_error("Invalid source fingerprint.");
                }
                source_fingerprint = f;
            }
                break;
            case 3: { // depth
                uint64_t d;
                decodeUnsigned(pos, end, d, cbor_decoding_flags);
                if(d > numeric_limits<uint8_t>::max()) {
                    throw domain_error("Invalid depth.");
                }
                depth = d;
            }
                break;
            default:
                throw domain_error("Unknown label.");
        }
    }
    return DerivationPath2(steps, source_fingerprint, depth);
}

DerivationPath2 DerivationPath2::decode_tagged_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    Tag major_tag;
    Tag minor_tag;
    decodeTagAndValue(pos, end, major_tag, minor_tag, cbor_decoding_flags);
    if(major_tag != Major::semantic || minor_tag != 304) {
        throw domain_error("Invalid derivation path.");
    }
    return decode_cbor(pos, end);
}

void DerivationPath2::encode_cbor(ByteVector& cbor) const {
    size_t map_size = 1;

    // steps
    ByteVector steps_map_entry;
    encodeInteger(steps_map_entry, 1);
    encodeArraySize(steps_map_entry, steps().size() * 2); // each step takes 2 array entries
    for(auto step: steps()) {
        step.encode_cbor(steps_map_entry);
    }

    // source_fingerprint
    ByteVector source_fingerprint_map_entry;
    if(source_fingerprint().has_value()) {
        map_size += 1;
        encodeInteger(source_fingerprint_map_entry, 2);
        encodeInteger(source_fingerprint_map_entry, *source_fingerprint());
    }

    // depth
    ByteVector depth_map_entry;
    if(depth().has_value() && *depth() != steps().size()) {
        map_size += 1;
        encodeInteger(depth_map_entry, 3);
        encodeInteger(depth_map_entry, *depth());
    }

    encodeMapSize(cbor, map_size);
    ::append(cbor, steps_map_entry);
    ::append(cbor, source_fingerprint_map_entry);
    ::append(cbor, depth_map_entry);
}

void DerivationPath2::encode_tagged_cbor(ByteVector& cbor) const {
    encodeTagAndValue(cbor, Major::semantic, Tag(304));
    encode_cbor(cbor);
}

ostream& operator<< (ostream& os, const DerivationPath2& rhs) {
    StringVector result;

    if(rhs.source_fingerprint().has_value()) {
        ostringstream o;
        o << hex << setw(8) << setfill('0') << *rhs.source_fingerprint();
        result.push_back(o.str());
    }

    for(auto step: rhs.steps()) {
        ostringstream o;
        o << step;
        result.push_back(o.str());
    }

    return os << ::join(result, "/");
}

string DerivationPath2::to_string() const {
    ostringstream o;
    o << *this;
    return o.str();
}

DerivationPath2 DerivationPath2::from_string(const string& path) {
    auto elems = ::split(::to_lowercase(path), '/');
    if(elems.empty()) {
        throw domain_error("Invalid derivation path.");
    }
    optional<uint32_t> source_fingerprint = nullopt;
    if(elems.front() == "m") {
        elems.erase(elems.begin());
    } else if(elems.front().length() == 8) {
        source_fingerprint = hex_to_uint32(elems.front());
        elems.erase(elems.begin());
    }
    DerivationPath2 result;
    transform(elems.begin(), elems.end(), back_inserter(result._steps), [&](auto elem) {
        return parse_elem(elem);
    });
    result._source_fingerprint = source_fingerprint;
    return result;
}

DerivationStep DerivationPath2::parse_elem(const std::string& elem) {
    string e = elem;
    if(e.empty()) {
        throw domain_error("Invalid derivation path element.");
    }
    bool is_hardened = false;
    if(e.back() == 'h') {
        is_hardened = true;
        e.pop_back();
    }
    if(e.empty()) {
        throw domain_error("Invalid derivation path element.");
    }
    auto spec = DerivationIndexSpec::from_string(elem);
    return DerivationStep(spec, is_hardened);
}

void DerivationPath2::append(const DerivationPath2& other) {
    _steps.insert(_steps.end(), other._steps.begin(), other._steps.end());
}
