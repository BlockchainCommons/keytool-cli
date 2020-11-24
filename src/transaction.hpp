#ifndef KEYTOOL_TRANSACTION_HPP
#define KEYTOOL_TRANSACTION_HPP

#include "utils.hpp"
#include "psbt.hpp"
#include <memory>

struct wally_tx;

class Transaction final {
public:
    Transaction();
    explicit Transaction(const ByteVector& data);
    explicit Transaction(const std::string& s);
    explicit Transaction(wally_tx* tx);
    explicit Transaction(const PSBT& psbt);

    ByteVector data() const;
    std::string hex() const;
    std::string ur() const;

private:
    std::shared_ptr<wally_tx> _tx;
};

#endif // KEYTOOL_TRANSACTION_HPP
