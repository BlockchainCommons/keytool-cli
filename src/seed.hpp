#ifndef KEYTOOL_SEED_HPP
#define KEYTOOL_SEED_HPP

#include <string>
#include "utils.hpp"

class Seed final {
public:
    Seed(const ByteVector& data, const std::string& name = "", const std::string& note = "");
    Seed(const std::string& s); // hex or ur:crypto-seed

    std::string hex() const;
    std::string ur() const;

    const ByteVector& data() const { return _data; }
    const std::string& name() const { return _name; }
    const std::string& note() const { return _note; }

private:
    ByteVector _data;
    std::string _name;
    std::string _note;
};

#endif
