#include "psbt.hpp"
#include "wally.hpp"
#include "wally-utils.hpp"
#include <bc-ur/bc-ur.hpp>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

static wally_psbt* parse_psbt(const string& s) {
    try {
        return Wally::instance.psbt_from_data(base64_to_data(s));
    } catch(...) { }

    try {
        return Wally::instance.psbt_from_data(hex_to_data(s));
    } catch(...) { }

    try {
        auto ur = URDecoder::decode(s);
        if(ur.type() != "crypto-psbt") {
            throw domain_error("Unexpected UR type: " + ur.type() + ". Expected crypto-psbt.");
        }
        auto cbor = ur.cbor();
        auto i = cbor.begin();
        ByteVector data;
        decodeBytes(i, cbor.end(), data);
        return Wally::instance.psbt_from_data(data);
    } catch(...) { }

    throw domain_error("Invalid PSBT.");
}

PSBT::PSBT()
    { }

PSBT::PSBT(wally_psbt* psbt)
    : _psbt(psbt, [](wally_psbt* psbt) { wally_psbt_free(psbt); })
    { }

PSBT::PSBT(const ByteVector& data)
    : PSBT(Wally::instance.psbt_from_data(data))
    { }

PSBT::PSBT(const string& base64)
    : PSBT(parse_psbt(base64))
    { }

PSBT PSBT::from_cbor(const ByteVector& cbor) {
    auto pos = cbor.begin();
    auto end = cbor.end();
    return decode_cbor(pos, end);
}

PSBT PSBT::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    ByteVector data;
    decodeBytes(pos, end, data, cbor_decoding_flags);
    return PSBT(data);
}

PSBT PSBT::decode_tagged_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    Tag major_tag;
    Tag minor_tag;
    decodeTagAndValue(pos, end, major_tag, minor_tag, cbor_decoding_flags);
    if(major_tag != Major::semantic || minor_tag != 310) {
        throw domain_error("Invalid PSBT.");
    }
    return decode_cbor(pos, end);
}

ByteVector PSBT::cbor() const {
    ByteVector cbor;
    encodeBytes(cbor, data());
    return cbor;
}

ByteVector PSBT::tagged_cbor() const {
    ByteVector c;
    encodeTagAndValue(c, Major::semantic, Tag(310));
    ::append(c, cbor());
    return c;
}

bool PSBT::is_finalized() const {
    return Wally::instance.psbt_is_finalized(_psbt.get());
}

string PSBT::base64() const {
    return data_to_base64(data());
}

string PSBT::ur() const {
    ByteVector cbor;
    encodeBytes(cbor, data());
    return UREncoder::encode(UR("crypto-psbt", cbor));
}

string PSBT::hex() const {
    return ::data_to_hex(data());
}

ByteVector PSBT::data() const {
    return Wally::instance.psbt_to_data(_psbt.get());
}

PSBT PSBT::finalized() const {
    auto result = Wally::instance.psbt_copy(_psbt.get());
    Wally::instance.psbt_finalize(result);
    return PSBT(result);
}

PSBT PSBT::sign(const ECPrivateKey& key) const {
    auto result = Wally::instance.psbt_copy(_psbt.get());
    Wally::instance.psbt_sign(result, key);
    return PSBT(result);
}
