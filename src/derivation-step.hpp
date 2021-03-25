#ifndef KEYTOOL_DERIVATION_STEP_HPP
#define KEYTOOL_DERIVATION_STEP_HPP

#include "derivation-index-spec.hpp"

class DerivationStep final {
public:
    DerivationStep(DerivationIndexSpec spec, bool is_hardened);

    DerivationIndexSpec spec() const { return _spec; }
    bool is_hardened() const { return _is_hardened; }
    uint32_t child_num() const;

    static DerivationStep decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);
    void encode_cbor(ByteVector& cbor) const;

    inline friend std::ostream& operator<< (std::ostream& os, const DerivationStep& rhs) {
        return os << rhs.spec() << (rhs.is_hardened() ? "h" : "");
    }

private:
    DerivationIndexSpec _spec;
    bool _is_hardened;
};

#endif
