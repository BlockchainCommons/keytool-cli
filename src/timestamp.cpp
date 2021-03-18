#include "timestamp.hpp"
#include "date.h"
#include <sstream>
#include <iomanip>
#include <bc-ur/bc-ur.hpp>

using namespace std;
using namespace std::chrono;

static const auto cborDecodingFlags = ur::CborLite::Flag::requireMinimalEncoding;

Timestamp::Timestamp() {
    _time = time_point_cast<milliseconds>(system_clock::now());
}

Timestamp::Timestamp(double secs_since_epoch) {
    std::chrono::milliseconds dur(uint64_t(secs_since_epoch * 1000.0));
    _time = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(dur);
}

Timestamp::Timestamp(const string& iso8601) {
    istringstream in(iso8601);
    in >> date::parse("%FT%TZ", _time);
    if (in.fail()) {
        in.clear();
        in.str(iso8601);
        in >> date::parse("%FT%T%Ez", _time);
    }
    if (in.fail()) {
        in.clear();
        in.str(iso8601);
        in >> date::parse("%F", _time);
    }
    if (in.fail()) {
        throw domain_error("Invalid timestamp.");
    }
}

const std::string Timestamp::iso8601() const {
    ostringstream o;
    o << date::format("%FT%TZ", _time);
    return o.str();
}

int64_t Timestamp::millis_since_epoch() const {
    return _time.time_since_epoch().count();
}

double Timestamp::secs_since_epoch() const {
    return millis_since_epoch() / 1000.0;
}

const void Timestamp::encode_cbor(ByteVector &buf) const {
    ur::CborLite::encodeTagAndValue(buf, ur::CborLite::Major::semantic, ur::CborLite::Tag(1));
    ur::CborLite::encodeDoubleFloat(buf, secs_since_epoch());
}

Timestamp Timestamp::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    ur::CborLite::Tag major_tag;
    ur::CborLite::Tag minor_tag;
    ur::CborLite::decodeTagAndValue(pos, end, major_tag, minor_tag, cborDecodingFlags);
    if(major_tag != ur::CborLite::Major::semantic || minor_tag != 1) {
        throw domain_error("Invalid UUID.");
    }
    double secs_since_epoch;
    ur::CborLite::decodeDoubleFloat(pos, end, secs_since_epoch, cborDecodingFlags);
    return Timestamp(secs_since_epoch);
}
