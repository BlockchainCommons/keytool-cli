#include "ec-key.hpp"
#include "wally.hpp"
#include "wally-utils.hpp"

#include <stdexcept>

std::string ECKey::to_hex() const {
    return data_to_hex(data());
}

ECPrivateKey::ECPrivateKey(const ByteVector& data) : ECKey(data) {
    if(data.size() != EC_PRIVATE_KEY_LEN) {
        throw std::domain_error("Incorrect private key length.");
    }
}

// cppcheck-suppress unusedFunction
ECCompressedPublicKey ECPublicKey::compressed() const { return ECCompressedPublicKey(data()); }
// cppcheck-suppress unusedFunction
ECUncompressedPublicKey ECPublicKey::uncompressed() const { return ECUncompressedPublicKey(data()); }

ECCompressedPublicKey ECPrivateKey::to_public() const {
    return Wally::instance.ec_key_to_public(*this);
}

ECCompressedPublicKey::ECCompressedPublicKey(const ByteVector& data) : ECPublicKey(data) {
    if(data.size() != EC_PUBLIC_KEY_LEN) {
        throw std::domain_error("Incorrect public key length.");
    }
}

std::string ECCompressedPublicKey::to_address(uint8_t version) const {
    auto d = data();
    auto hashed = Wally::instance.hash160(d);
    hashed.insert(hashed.begin(), version);
    return Wally::instance.to_base58(hashed, true);
}

std::string ECCompressedPublicKey::to_address(const UseInfo& use_info, bool is_sh) const {
    auto version = is_sh ? use_info.version_sh() : use_info.version_pkh();
    return to_address(version);
}

ECUncompressedPublicKey::ECUncompressedPublicKey(const ByteVector& data) : ECPublicKey(data) {
    if(data.size() != EC_PUBLIC_KEY_UNCOMPRESSED_LEN) {
        throw std::domain_error("Incorrect uncompressed public key length.");
    }
}

ECUncompressedPublicKey ECCompressedPublicKey::uncompressed() const {
    return Wally::instance.ec_key_uncompress(*this);
}

ECCompressedPublicKey ECUncompressedPublicKey::compressed() const {
    return Wally::instance.ec_key_compress(*this);
}
