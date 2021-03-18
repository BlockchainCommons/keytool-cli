#include "request.hpp"
#include <bc-ur/bc-ur.hpp>
#include <set>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

static const auto cborDecodingFlags = ur::CborLite::Flag::requireMinimalEncoding;

void SeedRequestBody::encode_cbor(ByteVector& cbor) const {
    encodeTagAndValue(cbor, Major::semantic, Tag(500));
    encodeBytes(cbor, digest());
}

SeedRequestBody SeedRequestBody::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    auto digest = ByteVector();
    decodeBytes(pos, end, digest, cborDecodingFlags);
    if(digest.size() != 32) {
        throw domain_error("Invalid seed request.");
    }
    return SeedRequestBody(digest);
}

void KeyRequestBody::encode_cbor(ByteVector& cbor) const {
    // TODO
    throw runtime_error("Unimplemented.");
}

KeyRequestBody KeyRequestBody::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    // TODO
    throw runtime_error("Unimplemented.");
}

void PSBTSignatureRequestBody::encode_cbor(ByteVector& cbor) const {
    // TODO
    throw runtime_error("Unimplemented.");
}

PSBTSignatureRequestBody PSBTSignatureRequestBody::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    // TODO
    throw runtime_error("Unimplemented.");
}

Request::Request(RequestBodyVariant body, const string& description, const UUID& id)
    : _body(body)
    , _description(description)
    , _id(id)
{ }

Request::Request(const string& s) {
    try {
        auto ur = URDecoder::decode(s);
        if(ur.type() != "crypto-request") {
            throw domain_error("Unexpected UR type: " + ur.type() + ". Expected crypto-request.");
        }
        auto& cbor = ur.cbor();
        auto pos = cbor.begin();
        auto end = cbor.end();
        size_t map_len;
        decodeMapSize(pos, end, map_len, cborDecodingFlags);
        set<int> labels;
        for(auto index = 0; index < map_len; index++) {
            int label;
            decodeInteger(pos, end, label, cborDecodingFlags);
            if(labels.find(label) != labels.end()) {
                throw domain_error("Duplicate label.");
            }
            labels.insert(label);
            switch (label) {
                case 1: // id
                    _id = UUID::decode_cbor(pos, end);
                    break;
                case 2: { // body
                    Tag major_tag;
                    Tag minor_tag;
                    decodeTagAndValue(pos, end, major_tag, minor_tag, cborDecodingFlags);
                    if(major_tag != Major::semantic) {
                        throw domain_error("Invalid request.");
                    }
                    if(minor_tag == 500) {
                        _body = SeedRequestBody::decode_cbor(pos, end);
                    } else if(minor_tag == 501) {
                        _body = KeyRequestBody::decode_cbor(pos, end);
                    } else if(minor_tag == 502) {
                        _body = PSBTSignatureRequestBody::decode_cbor(pos, end);
                    } else {
                        throw domain_error("Unknown request.");
                    }
                } break;
                case 3: // description
                    decodeText(pos, end, _description, cborDecodingFlags);
                    break;
                default:
                    throw domain_error("Unknown label.");
                    break;
            }
        }
        if(!(_id.has_value() && _body.has_value())) {
            throw domain_error("Invalid request.");
        }
    } catch(...) {
        throw domain_error("Invalid ur:crypto-request.");
    }
}

string Request::ur() const {
    size_t map_size = 2;

    // id
    ByteVector id_map_entry;
    encodeInteger(id_map_entry, 1);
    id().encode_cbor(id_map_entry);

    // body
    ByteVector body_map_entry;
    encodeInteger(body_map_entry, 2);
    auto body = *_body;
    if(is_seed_request()) {
        seed_request().encode_cbor(body_map_entry);
    } else if(is_key_request()) {
        key_request().encode_cbor(body_map_entry);
    } else if(is_psbt_signature_request()) {
        psbt_signature_request().encode_cbor(body_map_entry);
    } else {
        throw runtime_error("Unknown request.");
    }

    // description
    ByteVector description_map_entry;
    if(!description().empty()) {
        map_size += 1;
        encodeInteger(description_map_entry, 3);
        encodeText(description_map_entry, description());
    }

    ByteVector cbor;
    encodeMapSize(cbor, map_size);
    ::append(cbor, id_map_entry);
    ::append(cbor, body_map_entry);
    ::append(cbor, description_map_entry);

    return UREncoder::encode(UR("crypto-request", cbor));
}
