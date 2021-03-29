#ifndef KEYTOOL_ASSET_2_HPP
#define KEYTOOL_ASSET_2_HPP

#include <string>
#include <vector>
#include <ostream>
#include "network.hpp"
#include "utils.hpp"

class Asset final {
public:
    Asset(const std::string& name, const std::string& symbol, size_t coin_type)
        : _name(name)
        , _symbol(symbol)
        , _coin_type(coin_type)
        { }

    const std::string& name() const { return _name; }
    const std::string& symbol() const { return _symbol; }
    size_t coin_type() const { return _coin_type; }

    static Asset btc();
    static Asset eth();
    static std::vector<Asset> assets();

    static Asset find(const std::string& symbol);

    void encode_cbor(ByteVector& cbor) const;
    static Asset decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);

private:
    std::string _name;
    std::string _symbol;
    size_t _coin_type;
};

std::ostream& operator<< (std::ostream& os, const Asset& asset);
bool operator== (const Asset& lhs, const Asset& rhs);
bool operator!= (const Asset& lhs, const Asset& rhs);

#endif
