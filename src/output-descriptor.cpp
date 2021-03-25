#include "output-descriptor.hpp"

#include <sstream>

using namespace std;

string OutputDescriptor::to_string() const {
    ostringstream content;
    content << "[" << account_path() << "]";
    content << account_pub_key().to_base58();
    content << "/" << address_path();

    ostringstream result;
    string start = "(";
    string end = ")";

    auto name = type().name();
    if(name == OutputDescriptorType::pkh().name()) {
        start = "pkh(";
    } else if(name == OutputDescriptorType::wpkh().name()) {
        start = "wpkh(";
    } else if(name == OutputDescriptorType::sh_wpkh().name()) {
        start = "sh(wpkh(";
        end = "))";
    }
    result << start << content.str() << end;

    return result.str();
}
