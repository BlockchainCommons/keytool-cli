#include "chain-type.hpp"

ChainType ChainType::external { "external", 0 };
ChainType ChainType::change { "change", 1 };
ChainType ChainType::identity { "identity", 7 };

std::vector<ChainType> ChainType::_chain_types = {
    external, change, identity
};

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
