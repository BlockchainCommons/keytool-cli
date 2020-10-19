#include "chain-type.hpp"

ChainType ChainType::external() {
    static auto t = new ChainType("external", 0);
    return *t;
}

ChainType ChainType::change() {
    static auto t = new ChainType("change", 1);
    return *t;
}

ChainType ChainType::identity() {
    static auto t = new ChainType("identity", 7);
    return *t;
}

std::vector<ChainType> ChainType::chain_types() {
    static auto a = new std::vector<ChainType>{ external(), change(), identity() };
    return *a;
}

std::string ChainType::to_string() const {
    if(_name.has_value()) {
        return *_name;
    } else {
        return std::to_string(index());
    }
}

bool operator==(const ChainType& a, const ChainType& b) {
    return a.name() == b.name();
}

std::ostream& operator<< (std::ostream& os, const ChainType& network) {
    return os << network.to_string();
}

ChainType ChainType::find(const std::string& name) {
    for(auto chain_type: chain_types()) {
        if(chain_type.name() == name) {
            return chain_type;
        }
    }
    throw std::domain_error("Unknown chain type: " + name);
}
