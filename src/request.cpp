#include "request.hpp"
#include <bc-ur/bc-ur.hpp>
#include <set>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

void SeedRequestBody::encode_cbor(ByteVector& cbor) const {
    encodeTagAndValue(cbor, Major::semantic, Tag(500));
    encodeBytes(cbor, digest());
}

SeedRequestBody SeedRequestBody::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    auto digest = ByteVector();
    decodeBytes(pos, end, digest, cbor_decoding_flags);
    if(digest.size() != 32) {
        throw domain_error("Invalid seed request.");
    }
    return SeedRequestBody(digest);
}

void KeyRequestBody::encode_cbor(ByteVector& cbor) const {
    encodeTagAndValue(cbor, Major::semantic, Tag(501));

    size_t map_size = 0;

    // is-private
    ByteVector is_private_map_entry;
    map_size += 1;
    encodeInteger(is_private_map_entry, 1);
    bool is_private = key_type() == KeyType::private_key();
    encodeBool(is_private_map_entry, is_private);

    // path
    ByteVector path_map_entry;
    map_size += 1;
    encodeInteger(path_map_entry, 2);
    path().encode_tagged_cbor(path_map_entry);

    // use-info
    ByteVector use_info_map_entry;
    if(!use_info().is_default()) {
        map_size += 1;
        encodeInteger(use_info_map_entry, 3);
        use_info().encode_tagged_cbor(use_info_map_entry);
    }

    ur::CborLite::encodeMapSize(cbor, map_size);
    ::append(cbor, is_private_map_entry);
    ::append(cbor, path_map_entry);
    ::append(cbor, use_info_map_entry);
}

KeyRequestBody KeyRequestBody::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    size_t map_len;
    decodeMapSize(pos, end, map_len, cbor_decoding_flags);
    set<int> labels;
    optional<KeyType> key_type;
    optional<DerivationPath> path;
    UseInfo use_info;
    for(auto index = 0; index < map_len; index++) {
        int label;
        decodeInteger(pos, end, label, cbor_decoding_flags);
        if(labels.find(label) != labels.end()) {
            throw domain_error("Duplicate label.");
        }
        labels.insert(label);
        switch(label) {
            case 1: {
                bool is_private;
                decodeBool(pos, end, is_private, cbor_decoding_flags);
                key_type = is_private ? KeyType::private_key() : KeyType::public_key();
            }
                break;
            case 2: {
                path = DerivationPath::decode_tagged_cbor(pos, end);
            }
                break;
            case 3: {
                use_info = UseInfo::decode_tagged_cbor(pos, end);
            }
                break;
            default:
                throw domain_error("Unknown label.");
        }
    }
    if(!key_type.has_value()) {
        throw domain_error("Key request doesn't contain is-private.");
    }
    if(!path.has_value()) {
        throw domain_error("Key request doesn't contain derivation.");
    }
    return KeyRequestBody(*key_type, *path, use_info);
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
        decodeMapSize(pos, end, map_len, cbor_decoding_flags);
        set<int> labels;
        for(auto index = 0; index < map_len; index++) {
            int label;
            decodeInteger(pos, end, label, cbor_decoding_flags);
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
                    decodeTagAndValue(pos, end, major_tag, minor_tag, cbor_decoding_flags);
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
                    decodeText(pos, end, _description, cbor_decoding_flags);
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
