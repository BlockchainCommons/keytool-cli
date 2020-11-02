#include "transaction.hpp"
#include "wally-utils.hpp"
#include "wally.hpp"

Transaction::Transaction()
    { }

Transaction::Transaction(wally_tx* tx)
    : _tx(tx, [](wally_tx* tx) { wally_tx_free(tx); })
    { }

Transaction::Transaction(const ByteVector& data)
    : Transaction(Wally::instance.tx_from_data(data))
    { }

ByteVector Transaction::data() const {
    return Wally::instance.tx_to_data(_tx.get());
}

Transaction::Transaction(const PSBT& psbt)
    : Transaction(Wally::instance.tx_from_psbt(psbt._psbt.get()))
{

}
