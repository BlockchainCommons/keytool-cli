#include "transaction.hpp"
#include "wally-utils.hpp"
#include "wally.hpp"
#include <bc-ur/bc-ur.hpp>

using namespace std;
using namespace ur;

static wally_tx* parse_tx(const string& s) {
    try {
        return Wally::instance.tx_from_data(hex_to_data(s));
    } catch(...) { }

    try {
        auto ur = URDecoder::decode(s);
        if(ur.type() != "crypto-tx") {
            throw domain_error("Unexpected UR type: " + ur.type() + ". Expected crypto-tx.");
        }
        auto cbor = ur.cbor();
        auto i = cbor.begin();
        ByteVector data;
        CborLite::decodeBytes(i, cbor.end(), data);
        return Wally::instance.tx_from_data(data);
    } catch(...) { }

    throw domain_error("Invalid transaction.");
}

Transaction::Transaction()
    { }

Transaction::Transaction(wally_tx* tx)
    : _tx(tx, [](wally_tx* tx) { wally_tx_free(tx); })
    { }

Transaction::Transaction(const ByteVector& data)
    : Transaction(Wally::instance.tx_from_data(data))
    { }

Transaction::Transaction(const std::string& s)
    : Transaction(parse_tx(s))
    { }

Transaction::Transaction(const PSBT& psbt)
    : Transaction(Wally::instance.tx_from_psbt(psbt._psbt.get()))
{ }

ByteVector Transaction::data() const {
    return Wally::instance.tx_to_data(_tx.get());
}

string Transaction::hex() const {
    return ::data_to_hex(data());
}

string Transaction::ur() const {
    ByteVector cbor;
    CborLite::encodeBytes(cbor, data());
    return UREncoder::encode(UR("crypto-tx", cbor));
}
