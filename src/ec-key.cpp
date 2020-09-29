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

ECCompressedPublicKey ECPublicKey::compressed() const { return data(); }
ECUncompressedPublicKey ECPublicKey::uncompressed() const { return data(); }

ECCompressedPublicKey ECPrivateKey::to_public() const {
    return Wally::instance.ec_key_to_public(*this);
}

ECCompressedPublicKey::ECCompressedPublicKey(const ByteVector& data) : ECPublicKey(data) {
    if(data.size() != EC_PUBLIC_KEY_LEN) {
        throw std::domain_error("Incorrect public key length.");
    }
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
