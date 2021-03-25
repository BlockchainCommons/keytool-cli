#include "derivation-step.hpp"
#include <bc-ur/bc-ur.hpp>
#include <stdexcept>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

DerivationStep::DerivationStep(DerivationIndexSpec spec, bool is_hardened) : _spec(spec), _is_hardened(is_hardened) { }

DerivationStep DerivationStep::decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end) {
    auto spec = DerivationIndexSpec::decode_cbor(pos, end);
    bool is_hardened;
    decodeBool(pos, end, is_hardened);
    return DerivationStep(spec, is_hardened);
}

void DerivationStep::encode_cbor(ByteVector& cbor) const {
    spec().encode_cbor(cbor);
    encodeBool(cbor, is_hardened());
}

uint32_t DerivationStep::child_num() const {
    if(spec().is_index()) {
        auto value = spec().index().value();
        return is_hardened() ? value | 0x80000000 : value;
    } else {
        throw domain_error("Inspecific child number in derivation path.");
    }
}
