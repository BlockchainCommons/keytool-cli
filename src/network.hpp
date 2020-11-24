#ifndef KEYTOOL_NETWORK_HPP
#define KEYTOOL_NETWORK_HPP

#include <string>
#include <ostream>

class Network final {
public:
    explicit Network(const std::string& name) : _name(name) { }

    const std::string& name() const { return _name; }

    static Network mainnet();
    static Network testnet();
    static std::vector<Network> networks();
    static Network find(const std::string& name);

private:
    std::string _name;
};

std::ostream& operator<< (std::ostream& os, const Network& network);
bool operator==(const Network& n1, const Network& n2);

#endif // KEYTOOL_NETWORK_HPP
