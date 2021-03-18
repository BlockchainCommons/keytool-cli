#ifndef KEYTOOL_TIMESTAMP_HPP
#define KEYTOOL_TIMESTAMP_HPP

#include <string>
#include <chrono>
#include "utils.hpp"

class Timestamp final {
public:
    Timestamp();
    Timestamp(double secs_since_epoch);
    Timestamp(const std::string& iso8601);

    const std::chrono::system_clock::time_point time() const { return _time; }
    const std::string iso8601() const;

    int64_t millis_since_epoch() const;
    double secs_since_epoch() const;

    const void encode_cbor(ByteVector &buf) const;
    static Timestamp decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);

    template<typename ostream>
    inline friend ostream &operator<<( ostream &os, const Timestamp &self ) {
        return os << self.iso8601(), os;
    }

    inline friend bool operator ==(const Timestamp& lhs, const Timestamp& rhs) {
        return lhs.time() == rhs.time();
    }

private:
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> _time;
};

#endif
