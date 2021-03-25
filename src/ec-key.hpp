#ifndef KEYTOOL_EC_KEY_HPP
#define KEYTOOL_EC_KEY_HPP

#include "utils.hpp"
#include "use-info.hpp"

class ECCompressedPublicKey;
class ECUncompressedPublicKey;

class ECKey {
public:
    explicit ECKey(const ByteVector& data) : _data(data) { }
    virtual ~ECKey() { }

    const ByteVector& data() const { return _data; }
    std::string to_hex() const;

private:
    ByteVector _data;
};

class ECPrivateKey final: public ECKey {
public:
    explicit ECPrivateKey(const ByteVector& data);

    ECCompressedPublicKey to_public() const;
};

class ECPublicKey: public ECKey {
public:
    explicit ECPublicKey(const ByteVector& data) : ECKey(data) { }

    virtual ECCompressedPublicKey compressed() const;
    virtual ECUncompressedPublicKey uncompressed() const;
};

class ECCompressedPublicKey: public ECPublicKey {
public:
    explicit ECCompressedPublicKey(const ByteVector& data);

    virtual ECUncompressedPublicKey uncompressed() const override;

    std::string to_address(uint8_t version) const;
    std::string to_address(const UseInfo& use_info, bool is_sh) const;
};

class ECUncompressedPublicKey: public ECPublicKey {
public:
    explicit ECUncompressedPublicKey(const ByteVector& data);

    virtual ECCompressedPublicKey compressed() const override;
};

#endif // KEYTOOL_EC_KEY_HPP
