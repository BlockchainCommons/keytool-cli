#ifndef KEYTOOL_NETWORK_HPP
#define KEYTOOL_NETWORK_HPP

#include <string>
#include <ostream>
#include "utils.hpp"

class Network final {
public:
    Network(const std::string& name, uint32_t value, uint32_t bip32_private_version, uint32_t bip32_public_version)
        : _name(name)
        , _value(value)
        , _bip32_private_version(bip32_private_version)
        , _bip32_public_version(bip32_public_version)
    { }

    const std::string& name() const { return _name; }
    uint32_t value() const { return _value; }
    uint32_t bip32_private_version() const { return _bip32_private_version; }
    uint32_t bip32_public_version() const { return _bip32_public_version; }

    static Network mainnet();
    static Network testnet();
    static std::vector<Network> networks();
    static Network find(const std::string& name);

    void encode_cbor(ByteVector& cbor) const;
    static Network decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);

private:
    std::string _name;
    uint32_t _value;
    uint32_t _bip32_private_version;
    uint32_t _bip32_public_version;
};

std::ostream& operator<< (std::ostream& os, const Network& network);
bool operator==(const Network& n1, const Network& n2);
bool operator!=(const Network& n1, const Network& n2);

#endif // KEYTOOL_NETWORK_HPP
