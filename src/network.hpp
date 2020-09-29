#ifndef KEYTOOL_NETWORK_HPP
#define KEYTOOL_NETWORK_HPP

#include <string>
#include <ostream>

class Network final {
public:
    Network(const std::string& name) : _name(name) { }

    const std::string& name() const { return _name; }

    static Network mainnet;
    static Network testnet;
    static const std::vector<Network>& networks() { return _networks; }
    static Network find(const std::string& name);

private:
    std::string _name;

    static std::vector<Network> _networks;
};

std::ostream& operator<< (std::ostream& os, const Network& network);
bool operator==(const Network& n1, const Network& n2);

#endif // KEYTOOL_NETWORK_HPP
