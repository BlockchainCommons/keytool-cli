#ifndef KEYTOOL_OUTPUT_DESCRIPTOR_TYPE_HPP
#define KEYTOOL_OUTPUT_DESCRIPTOR_TYPE_HPP

#include <string>
#include <vector>
#include <ostream>

class OutputDescriptorType final {
public:
    OutputDescriptorType(const std::string& name) : _name(name) { }

    const std::string& name() const { return _name; }

    static OutputDescriptorType pkh();
    static OutputDescriptorType wpkh();
    static OutputDescriptorType sh_wpkh();
    static std::vector<OutputDescriptorType> output_descriptor_types();

    static OutputDescriptorType find(const std::string& name);

private:
    std::string _name;
};

std::ostream& operator<< (std::ostream& os, const OutputDescriptorType& asset);

#endif // KEYTOOL_OUTPUT_DESCRIPTOR_TYPE_HPP
