#ifndef KEYTOOL_EC_KEY_HPP
#define KEYTOOL_EC_KEY_HPP

#include "utils.hpp"

class ECCompressedPublicKey;
class ECUncompressedPublicKey;

class ECKey {
public:
    ECKey(const ByteVector& data) : _data(data) { }
    virtual ~ECKey() { }

    const ByteVector& data() const { return _data; }
    std::string to_hex() const;

private:
    ByteVector _data;
};

class ECPrivateKey final: public ECKey {
public:
    ECPrivateKey(const ByteVector& data);

    ECCompressedPublicKey to_public() const;
};

class ECPublicKey: public ECKey {
public:
    ECPublicKey(const ByteVector& data) : ECKey(data) { }

    virtual ECCompressedPublicKey compressed() const;
    virtual ECUncompressedPublicKey uncompressed() const;
};

class ECCompressedPublicKey: public ECPublicKey {
public:
    ECCompressedPublicKey(const ByteVector& data);

    virtual ECUncompressedPublicKey uncompressed() const override;
};

class ECUncompressedPublicKey: public ECPublicKey {
public:
    ECUncompressedPublicKey(const ByteVector& data);

    virtual ECCompressedPublicKey compressed() const override;
};

#endif // KEYTOOL_EC_KEY_HPP
