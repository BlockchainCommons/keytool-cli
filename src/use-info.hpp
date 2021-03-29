#ifndef KEYTOOL_USE_INFO_HPP
#define KEYTOOL_USE_INFO_HPP

#include "network.hpp"
#include "asset.hpp"
#include "utils.hpp"

class UseInfo final {
public:
    UseInfo(const Asset& asset = Asset::btc(), const Network& network = Network::mainnet())
        : _asset(asset)
        , _network(network)
    { }

    const Asset& asset() const { return _asset; }
    const Network& network() const { return _network; }

    bool is_default() const;
    uint32_t coin_type() const;

    void encode_cbor(ByteVector& cbor) const;
    void encode_tagged_cbor(ByteVector& cbor) const;
    static UseInfo decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);
    static UseInfo decode_tagged_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);

    uint8_t version_sh() const;
    uint8_t version_pkh() const;

private:
    Asset _asset;
    Network _network;
};

#endif
