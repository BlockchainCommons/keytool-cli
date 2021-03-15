#include "timestamp.hpp"
#include "date.h"
#include <sstream>
#include <iomanip>

using namespace std;
using namespace std::chrono;

Timestamp::Timestamp() {
    _time = time_point_cast<milliseconds>(system_clock::now());
}

// Timestamp::Timestamp(double secs_since_epoch) {

// }

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
