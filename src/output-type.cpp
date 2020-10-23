#include "output-type.hpp"
#include <exception>

OutputDescriptorType OutputDescriptorType::pkh() {
    static auto a = new OutputDescriptorType("pkh", 44);
    return *a;
}

OutputDescriptorType OutputDescriptorType::wpkh() {
    static auto a = new OutputDescriptorType("wpkh", 84);
    return *a;
}

OutputDescriptorType OutputDescriptorType::sh_wpkh() {
    static auto a = new OutputDescriptorType("sh-wpkh", 49);
    return *a;
}

std::vector<OutputDescriptorType> OutputDescriptorType::output_types() {
    static auto a = new std::vector<OutputDescriptorType>{ pkh(), wpkh(), sh_wpkh() };
    return *a;
}

std::ostream& operator<< (std::ostream& os, const OutputDescriptorType& d) {
    return os << d.name();
}

OutputDescriptorType OutputDescriptorType::find(const std::string& name) {
    for(auto t: output_types()) {
        if(t.name() == name) {
            return t;
        }
    }
    throw std::domain_error("Unknown output-type: " + name);
}
