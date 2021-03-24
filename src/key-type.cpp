#include "key-type.hpp"
#include "wally.hpp"

KeyType KeyType::public_key() {
    static auto k = new KeyType("public", BIP32_FLAG_KEY_PUBLIC);
    return *k;
}

KeyType KeyType::private_key() {
    static auto k = new KeyType("private", BIP32_FLAG_KEY_PRIVATE);
    return *k;
}

std::vector<KeyType> KeyType::key_types() {
    static auto a = new std::vector<KeyType>{ public_key(), private_key() };
    return *a;
}

std::ostream& operator<< (std::ostream& os, const KeyType& key_type) {
    return os << key_type.name();
}

bool operator==(const KeyType& k1, const KeyType& k2) {
    return k1.name() == k2.name();
}

KeyType KeyType::find(const std::string& name) {
    for(auto key_type: key_types()) {
        if(key_type.name() == name) {
            return key_type;
        }
    }
    throw std::domain_error("Unknown key_type: " + name);
}

bool KeyType::is_public() const {
    return *this == public_key();
}

bool KeyType::is_private() const {
    return *this == private_key();
}
