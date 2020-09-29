//
//  keytool.cpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#include <iostream>
#include <stdexcept>

#include "params.hpp"

using namespace std;

int main( int argc, char *argv[] ) {
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
