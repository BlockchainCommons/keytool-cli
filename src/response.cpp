#include "response.hpp"
#include <bc-ur/bc-ur.hpp>
#include <set>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

static const auto cborDecodingFlags = ur::CborLite::Flag::requireMinimalEncoding;

Response::Response(ResponseBodyVariant body, const UUID& id)
    : _body(body)
    , _id(id)
{ }

Response::Response(const string& s) {
    try {
        auto ur = URDecoder::decode(s);
        if(ur.type() != "crypto-response") {
            throw domain_error("Unexpected UR type: " + ur.type() + ". Expected crypto-response.");
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
                case 2: {
                    Tag major_tag;
                    Tag minor_tag;
                    decodeTagAndValue(pos, end, major_tag, minor_tag, cborDecodingFlags);
                    if(major_tag != Major::semantic) {
                        throw domain_error("Invalid response.");
                    }
                    if(minor_tag == 300) {
                        _body = Seed::decode_cbor(pos, end);
                    } else if(minor_tag == 303) {
                        // TODO
                        _body = HDKey::decode_cbor(pos, end);
                        throw runtime_error("Unimplemented.");
                    } else if(minor_tag == 310) {
                        // TODO
                        // _body = PSBT::decode_cbor(pos, end);
                        throw runtime_error("Unimplemented.");
                    } else {
                        throw domain_error("Unknown response.");
                    }
                } break;
            }
        }
    } catch(...) {
        throw domain_error("Invalid ur:crypto-response.");
    }
}

string Response::ur() const {
    size_t map_size = 2;

    // id
    ByteVector id_map_entry;
    encodeInteger(id_map_entry, 1);

    // body
    ByteVector body_map_entry;
    encodeInteger(body_map_entry, 2);
    auto body = *_body;
    if(is_seed_response()) {
        ::append(body_map_entry, seed_response().tagged_cbor());
    } else if(is_key_response()) {
        // TODO
        // ::append(body_map_entry, key_response().tagged_cbor());
    } else if(is_psbt_signature_response()) {
        // TODO
        // ::append(body_map_entry, psbt_signature_response().tagged_cbor());
    } else {
        throw runtime_error("Unknown response.");
    }

    ByteVector cbor;
    encodeMapSize(cbor, map_size);
    ::append(cbor, id_map_entry);
    ::append(cbor, body_map_entry);

    return UREncoder::encode(UR("crypto-response", cbor));
}
