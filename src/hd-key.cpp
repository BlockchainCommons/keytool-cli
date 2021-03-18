#include "hd-key.hpp"
#include "wally.hpp"
#include "utils.hpp"
#include "wally-utils.hpp"

using namespace std;

HDKey::HDKey(ext_key* key)
    : _key(key, [](ext_key* key) { bip32_key_free(key); })
    { }

HDKey::HDKey(const ByteVector& seed, const Network& network)
    : HDKey(Wally::instance.bip32_key_from_entropy(seed, network))
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

ByteVector HDKey::fingerprint() const {
    return Wally::instance.bip32_key_get_fingerprint(*this);
}

HDKey HDKey::derive(uint32_t index, bool is_hardened, bool is_private) const {
    return Wally::instance.bip32_key_from_parent(*this, index, is_hardened, is_private);
}

HDKey HDKey::derive(DerivationPath derivation_path, bool is_private) const {
    auto result = *this;
    for(auto elem: derivation_path) {
        switch(elem.type()) {
            case DerivationPathElement::Type::master:
                // ignore
            break;
            case DerivationPathElement::Type::indexed:
                auto index_range = elem.index();
                switch(index_range.type()) {
                    case IndexBound::Type::value: {
                        result = result.derive(index_range.index(), elem.is_hardened(), is_private);
                    }
                    break;
                    case IndexBound::Type::wildcard: {
                        throw domain_error("Cannot derive a single key from a wildcard path element.");
                    }
                    break;
                }
            break;
        }
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

HDKey HDKey::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    // TODO
    throw runtime_error("Unimplemented.");
}

HDKey HDKey::decode_tagged_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    // TODO
    throw runtime_error("Unimplemented.");
}

string HDKey::ur() const {
    // TODO
    throw runtime_error("Unimplemented.");
}

ByteVector HDKey::cbor() const {
    // TODO
    throw runtime_error("Unimplemented.");
}

ByteVector HDKey::tagged_cbor() const {
    // TODO
    throw runtime_error("Unimplemented.");
}
