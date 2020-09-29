#ifndef KEYTOOL_DERIVATION_PATH_HPP
#define KEYTOOL_DERIVATION_PATH_HPP

#include <vector>
#include <ostream>
#include <string>

// typedef std::pair<uint32_t, uint32_t> IndexRange;

class DerivationPathElement final {
public:
    DerivationPathElement(uint32_t index, bool is_hardened) : _index(index), _is_hardened(is_hardened) { }

    uint32_t index() const { return _index; }
    bool is_hardened() const { return _is_hardened; }

    std::string to_string() const;

private:
    uint32_t _index;
    bool _is_hardened;
};

std::ostream& operator<< (std::ostream& os, const DerivationPathElement& elem);

typedef std::vector<DerivationPathElement> DerivationPath;
std::string to_string(const DerivationPath& path);

std::ostream& operator<< (std::ostream& os, const DerivationPath& path);

DerivationPath parse_derivation_path(const std::string& path);

#endif // KEYTOOL_DERIVATION_PATH_HPP
