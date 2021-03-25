#ifndef KEYTOOL_KEY_TYPE_HPP
#define KEYTOOL_KEY_TYPE_HPP

#include <string>
#include <ostream>
#include <vector>

class KeyType final {
public:
    KeyType(const std::string& name, uint32_t bip32_flag)
        : _name(name)
        , _bip32_flag(bip32_flag)
    { }

    const std::string& name() const { return _name; }
    uint32_t bip32_flag() const { return _bip32_flag; }

    static KeyType public_key();
    static KeyType private_key();
    static std::vector<KeyType> key_types();
    static KeyType find(const std::string& name);

    bool is_public() const;
    bool is_private() const;

private:
    std::string _name;
    uint32_t _bip32_flag;
};

std::ostream& operator<< (std::ostream& os, const KeyType& key_type);
bool operator==(const KeyType& k1, const KeyType& k2);
bool operator!=(const KeyType& k1, const KeyType& k2);

#endif
