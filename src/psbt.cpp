#include "psbt.hpp"
#include "wally.hpp"
#include "wally-utils.hpp"

PSBT::PSBT()
    { }

PSBT::PSBT(wally_psbt* psbt)
    : _psbt(psbt, [](wally_psbt* psbt) { wally_psbt_free(psbt); })
    { }

PSBT::PSBT(const ByteVector& data)
    : PSBT(Wally::instance.psbt_from_data(data))
    { }

PSBT::PSBT(const std::string& base64)
    : PSBT(Wally::instance.psbt_from_base64(base64))
    { }

bool PSBT::is_finalized() const {
    return Wally::instance.psbt_is_finalized(_psbt.get());
}

std::string PSBT::base64() const {
    return Wally::instance.psbt_to_base64(_psbt.get());
}

ByteVector PSBT::data() const {
    return Wally::instance.psbt_to_data(_psbt.get());
}

PSBT PSBT::finalized() const {
    auto result = Wally::instance.psbt_copy(_psbt.get());
    Wally::instance.psbt_finalize(result);
    return PSBT(result);
}

PSBT PSBT::sign(const ECPrivateKey& key) const {
    auto result = Wally::instance.psbt_copy(_psbt.get());
    Wally::instance.psbt_sign(result, key);
    return PSBT(result);
}
