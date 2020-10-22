#ifndef KEYTOOL_DERIVATION_PATH_HPP
#define KEYTOOL_DERIVATION_PATH_HPP

#include <vector>
#include <ostream>
#include <string>
#include "utils.hpp"
#include "index-bound.hpp"

class DerivationPathElement final {
public:
    enum Type {
        master,
        indexed
    };

    DerivationPathElement(IndexBound index, bool is_hardened) : _type(Type::indexed), _index(index), _is_hardened(is_hardened) { }
    DerivationPathElement(bool is_hardened) : _type(Type::indexed), _is_hardened(is_hardened) { }
    DerivationPathElement(const ByteVector& fingerprint) : _type(Type::master), _fingerprint(fingerprint) { }
    DerivationPathElement() : _type(Type::master) { }

    Type type() const { return _type; }
    ByteVector fingerprint() const { return _fingerprint; }
    IndexBound index() const { return _index; }
    bool is_hardened() const { return _is_hardened; }

    std::string to_string() const;

private:
    Type _type;
    ByteVector _fingerprint;
    IndexBound _index;
    bool _is_hardened;
};

std::ostream& operator<< (std::ostream& os, const DerivationPathElement& elem);

typedef std::vector<DerivationPathElement> DerivationPath;
std::string to_string(const DerivationPath& path);

std::ostream& operator<< (std::ostream& os, const DerivationPath& path);

DerivationPath parse_derivation_path(const std::string& path);

#endif // KEYTOOL_DERIVATION_PATH_HPP
