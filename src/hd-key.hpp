#ifndef KEYTOOL_HD_KEY_HPP
#define KEYTOOL_HD_KEY_HPP

#include <assert.h>
#include <string>
#include <exception>
#include <memory>
#include <ostream>
#include "utils.hpp"
#include "network.hpp"
#include "derivation-path.hpp"

struct ext_key;

class HDKey final {
public:
    HDKey(const ByteVector& entropy, const Network& network);
    HDKey(ext_key* key);
    HDKey(const std::string &base58);
    HDKey(const std::string &base58, bool require_private);

    bool is_private() const;

    ByteVector fingerprint() const;

    HDKey derive(uint32_t index, bool is_hardened, bool is_private) const;
    HDKey derive(DerivationPath derivation_path, bool is_private) const;

    HDKey to_public() const;

    std::string to_base58(bool is_private) const;
    std::string to_base58() const;

    friend class Wally;

private:
    std::shared_ptr<ext_key> _key;
};

std::ostream& operator<< (std::ostream& os, const HDKey& key);

#endif // KEYTOOL_HD_KEY_HPP
