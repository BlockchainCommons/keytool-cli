// #ifndef KEYTOOL_HD_KEY_HPP
// #define KEYTOOL_HD_KEY_HPP

// #include <assert.h>
// #include <string>
// #include <exception>
// #include <memory>
// #include <ostream>
// #include "utils.hpp"
// #include "network.hpp"
// #include "derivation-path.hpp"
// #include "seed.hpp"

// struct ext_key;

// class HDKey final {
// public:
//     HDKey(const ByteVector& seed, const Network& network);
//     explicit HDKey(ext_key* key);
//     explicit HDKey(const std::string &base58);
//     HDKey(const std::string &base58, bool require_private);
//     static HDKey decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);
//     static HDKey decode_tagged_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);

//     bool is_private() const;

//     ByteVector fingerprint() const;

//     HDKey derive(uint32_t index, bool is_hardened, bool is_private) const;
//     HDKey derive(DerivationPath derivation_path, bool is_private) const;

//     HDKey to_public() const;

//     std::string to_base58(bool is_private) const;
//     std::string to_base58() const;

//     std::string ur() const;
//     ByteVector cbor() const;
//     ByteVector tagged_cbor() const;

//     friend class Wally;

// private:
//     std::shared_ptr<ext_key> _key;
// };

// std::ostream& operator<< (std::ostream& os, const HDKey& key);

// #endif // KEYTOOL_HD_KEY_HPP
