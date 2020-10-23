#ifndef KEYTOOL_output_type_HPP
#define KEYTOOL_output_type_HPP

#include <string>
#include <vector>
#include <ostream>

class OutputDescriptorType final {
public:
    OutputDescriptorType(const std::string& name, uint32_t purpose) : _name(name), _purpose(purpose) { }

    const std::string& name() const { return _name; }
    uint32_t purpose() const { return _purpose; }

    static OutputDescriptorType pkh();
    static OutputDescriptorType wpkh();
    static OutputDescriptorType sh_wpkh();
    static std::vector<OutputDescriptorType> output_types();

    static OutputDescriptorType find(const std::string& name);

private:
    std::string _name;
    uint32_t _purpose;
};

std::ostream& operator<< (std::ostream& os, const OutputDescriptorType& asset);

#endif // KEYTOOL_output_type_HPP
