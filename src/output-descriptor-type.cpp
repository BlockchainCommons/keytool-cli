#include "output-descriptor-type.hpp"
#include <exception>

OutputDescriptorType OutputDescriptorType::pkh() {
    static auto a = new OutputDescriptorType("pkh");
    return *a;
}

OutputDescriptorType OutputDescriptorType::wpkh() {
    static auto a = new OutputDescriptorType("wpkh");
    return *a;
}

OutputDescriptorType OutputDescriptorType::sh_wpkh() {
    static auto a = new OutputDescriptorType("sh-wpkh");
    return *a;
}

std::vector<OutputDescriptorType> OutputDescriptorType::output_descriptor_types() {
    static auto a = new std::vector<OutputDescriptorType>{ pkh(), wpkh(), sh_wpkh() };
    return *a;
}

std::ostream& operator<< (std::ostream& os, const OutputDescriptorType& d) {
    return os << d.name();
}

OutputDescriptorType OutputDescriptorType::find(const std::string& name) {
    for(auto t: output_descriptor_types()) {
        if(t.name() == name) {
            return t;
        }
    }
    throw std::domain_error("Unknown output-descriptor-type: " + name);
}
