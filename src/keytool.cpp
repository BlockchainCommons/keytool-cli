//
//  keytool.cpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#include <iostream>
#include <stdexcept>
#include <iomanip>

#include "params.hpp"
#include "timestamp.hpp"

using namespace std;

int main( int argc, char *argv[] ) {
    auto t = Timestamp("2014-11-12T19:12:14.505Z");
    cout << t << endl;
    auto t2 = Timestamp("2014-11-12T19:12:14.505-07:00");
    cout << t2 << endl;
    auto t3 = Timestamp("2014-11-12");
    cout << t3 << endl;
    auto t4 = Timestamp();
    cout << t4 << endl;
    auto millis = t4.millis_since_epoch();
    cout << millis << endl;
    auto secs = t4.secs_since_epoch();
    cout << setprecision(15) << secs << endl;
    auto t5 = Timestamp(secs);
    cout << t5 << endl;
    ByteVector cbor;
    t5.encode_cbor(cbor);
    cout << data_to_hex(cbor) << endl;
    auto pos = cbor.begin();
    auto t6 = Timestamp::decode_cbor(pos, cbor.end());
    cout << t6 << endl;

//    cout << t2 << endl;

    // auto t = Timestamp();
    // auto local = t.iso8601_local();
    // cout << local << endl;
    // auto t2 = Timestamp(local);
    // cout << t2.iso8601_local() << endl;
    // cout << (t.iso8601_local() == t2.iso8601_local() ? "true" : "false") << endl;

    // auto utc = t.iso8601();
    // cout << utc << endl;


    try {
        auto p = Params::parse(argc, argv);
        p->process();

        if(p->output.empty()) {
            throw runtime_error("An internal error occurred.");
        }

        cout << p->output << endl;

        delete p;
        return 0;

    } catch(exception &e) {
        cerr << argv[0] << ": " << e.what() << endl;
        exit(1);
    }
}
