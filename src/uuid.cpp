#include "uuid.hpp"
#include "utils.hpp"

#include <stdexcept>
#include <random>
#include <algorithm>
#include <bc-ur/bc-ur.hpp>

using namespace std;

UUID::UUID() {
    random_device rd;
    uniform_int_distribution<uint8_t> dist(0, 255);

    for(auto i = 0; i < 16; i++) {
        _data.push_back(dist(rd));
    }

    // Identify this UUID as type 4.
    _data[6] = (_data[6] & 0x0F) | 0x40;
    _data[8] = (_data[8] & 0x3F) | 0x80;
}

const string UUID::str() const {
    bool seps[] = { false, false, false, true, false, true, false, true, false, true, false, false, false, false, false, false };
    string result;

    for(int i = 0; i < 16; i++) {
        result.append(byte_to_hex(_data[i]));
        if(seps[i]) {
            result.append("-");
        }
    }
    return result;
}

static void check_uuid(const ByteVector& data) {
    if(data.size() != 16) {
        throw domain_error("UUID must be 16 bytes.");
    }
}

UUID::UUID(const string& uuid_str) {
    string str = uuid_str;
    str.erase(std::remove(str.begin(), str.end(), '-'), str.end());
    auto data = hex_to_data(str);
    check_uuid(data);
    _data = data;
}

UUID::UUID(const ByteVector& data) {
    check_uuid(data);
    _data = data;
}

const void UUID::encode_cbor(ByteVector &buf) const {
    ur::CborLite::encodeTagAndValue(buf, ur::CborLite::Major::semantic, ur::CborLite::Tag(37));
    ur::CborLite::encodeBytes(buf, data());
}

UUID UUID::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    ur::CborLite::Tag major_tag;
    ur::CborLite::Tag minor_tag;
    ur::CborLite::decodeTagAndValue(pos, end, major_tag, minor_tag, cborDecodingFlags);
    if(major_tag != ur::CborLite::Major::semantic || minor_tag != 37) {
        throw domain_error("Invalid UUID.");
    }
    ByteVector data;
    ur::CborLite::decodeBytes(pos, end, data, cborDecodingFlags);
    return UUID(data);
}
