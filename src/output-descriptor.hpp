#ifndef KEYTOOL_OUTPUT_DESCRIPTOR_HPP
#define KEYTOOL_OUTPUT_DESCRIPTOR_HPP

#include "output-type.hpp"
#include "derivation-path.hpp"
#include "hd-key.hpp"

class OutputDescriptor final {
public:
    OutputDescriptor(
        const OutputDescriptorType& type,
        const DerivationPath& account_path,
        const DerivationPath& address_path,
        const HDKey& account_pub_key
        )
            : _type(type)
            , _account_path(account_path)
            , _address_path(address_path)
            , _account_pub_key(account_pub_key)
        { }

    const OutputDescriptorType& type() const { return _type; }
    const DerivationPath& account_path() const { return _account_path; }
    const DerivationPath& address_path() const { return _address_path; }
    const HDKey& account_pub_key() const { return _account_pub_key; }

    std::string to_string() const;

private:
    OutputDescriptorType _type;
    DerivationPath _account_path;
    DerivationPath _address_path;
    HDKey _account_pub_key;
};

#endif // KEYTOOL_OUTPUT_DESCRIPTOR_HPP
