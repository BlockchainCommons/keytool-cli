#include "hd-key.hpp"
#include "wally.hpp"
#include "utils.hpp"
#include "wally-utils.hpp"

using namespace std;

HDKey::HDKey(ext_key* key)
    : _key(key, [](ext_key* key) { bip32_key_free(key); })
    { }

HDKey::HDKey(const ByteVector& entropy, const Network& network)
    : HDKey(Wally::instance.bip32_key_from_entropy(entropy, network))
    { }

HDKey::HDKey(const std::string &base58)
    : HDKey(Wally::instance.bip32_key_from_base58(base58))
    { }

HDKey::HDKey(const std::string &base58, bool require_private)
    : HDKey(Wally::instance.bip32_key_from_base58(base58))
{
    if(require_private && !this->is_private()) {
        throw domain_error("Expected private HD key.");
    }
    if(!require_private && this->is_private()) {
        throw domain_error("Expected public HD key.");
    }
}

bool HDKey::is_private() const {
    return _key->priv_key[0] == BIP32_FLAG_KEY_PRIVATE;
}

HDKey HDKey::derive(uint32_t index, bool is_hardened, bool is_private) const {
    return Wally::instance.bip32_key_from_parent(*this, index, is_hardened, is_private);
}

HDKey HDKey::derive(DerivationPath derivation_path, bool is_private) const {
    auto result = *this;
    for(auto elem: derivation_path) {
        result = result.derive(elem.index(), elem.is_hardened(), is_private);
    }
    return result;
}

HDKey HDKey::to_public() const {
    return Wally::instance.bip32_key_to_public(*this);
}

string HDKey::to_base58(bool is_private) const {
    if(!this->is_private() && is_private) {
        throw Wally::UnsupportedDerivation();
    }
    uint32_t flags = is_private ? BIP32_FLAG_KEY_PRIVATE : BIP32_FLAG_KEY_PUBLIC;
    char* output = nullptr;
    assert(bip32_key_to_base58(_key.get(), flags, &output) == WALLY_OK);
    auto result = string(output);
    wally_free_string(output);
    return result;
}

string HDKey::to_base58() const {
    return to_base58(this->is_private());
}

ostream& operator<< (ostream& os, const HDKey& key) {
    if(key.is_private()) {
        return os << key.to_base58(true);
    } else {
        return os << key.to_base58(false);
    }
}
