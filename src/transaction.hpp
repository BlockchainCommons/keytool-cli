#ifndef KEYTOOL_TRANSACTION_HPP
#define KEYTOOL_TRANSACTION_HPP

#include "utils.hpp"
#include "psbt.hpp"
#include <memory>

struct wally_tx;

class Transaction final {
public:
    Transaction();
    Transaction(const ByteVector& data);
    Transaction(wally_tx* tx);
    Transaction(const PSBT& psbt);

    ByteVector data() const;

private:
    std::shared_ptr<wally_tx> _tx;
};

#endif // KEYTOOL_TRANSACTION_HPP
