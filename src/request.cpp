#include "request.hpp"
#include <bc-ur/bc-ur.hpp>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

Request::Request(const string& description, const UUID& id)
    : _id(id)
    , _description(description)
{ }

string Request::ur() const {
    size_t map_size = 2;

    // id
    ByteVector id_map_entry;
    encodeInteger(id_map_entry, 1);
    id().encode_cbor(id_map_entry);

    // body
    ByteVector body_map_entry;
    encodeInteger(body_map_entry, 2);
    encode_body(body_map_entry);

    // description
    ByteVector description_map_entry;
    if(!description().empty()) {
        map_size += 1;
        encodeInteger(description_map_entry, 3);
        encodeText(body_map_entry, description());
    }

    ByteVector cbor;
    encodeMapSize(cbor, map_size);
    ::append(cbor, id_map_entry);
    ::append(cbor, body_map_entry);
    ::append(cbor, description_map_entry);

    return UREncoder::encode(UR("crypto-request", cbor));
}

SeedRequest::SeedRequest(const ByteVector& digest, const string& description, const UUID& id)
    : Request(description, id)
    , _digest(digest)
{ }

void SeedRequest::encode_body(ByteVector& cbor) const {
    encodeTagAndValue(cbor, Major::semantic, Tag(500));
    encodeBytes(cbor, digest());
}
