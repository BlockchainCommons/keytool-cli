//
//  params.cpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#include <memory.h>
#include <argp.h>
#include <assert.h>
#include <iostream>
#include <stdexcept>

#include <bc-crypto-base/bc-crypto-base.h>
#include <wally.hpp>

#include "params.hpp"
#include "config.h"

using namespace std;

Params::~Params() {
}

void Params::validate() {
    std::string name;

    for(auto i = opts.begin(); i != opts.end(); i++) {
        auto tag = i->first;
        auto value = i->second;

        auto node = model.find_by_tag(tag);
        assert(node != nullptr);
        node->from_string(value);
    }

    if(args.empty()) {
        throw domain_error("No outputs requested.");
    }

    for(auto arg: args) {
        if(!model.is_valid_name(arg)) {
            throw domain_error("Unknown output requested: " + arg);
        }
    }
}

static std::string read_arg_from_stdin() {
    string line;
    if(getline(cin, line)) {
        trim(line);
        return line;
    } else {
        throw domain_error("Input expected from stdin.");
    }
}

static int parse_opt(int key, char* arg, struct argp_state* state) {
    try {
        auto p = static_cast<Params*>(state->input);
        p->state = state;
        auto& opts = p->opts;
        auto& args = p->args;

        switch (key) {
        case ARGP_KEY_INIT:
            break;
        case ARGP_KEY_ARG:
            args.push_back(arg);
            break;
        case ARGP_KEY_END:
            p->validate();
            break;
        default:
            if(key < 0) {
                string s = arg ? arg : "";
                if(s.empty()) {
                    opts[key] = read_arg_from_stdin();
                } else {
                    opts[key] = s;
                }
            }
            break;
        }
    } catch(exception &e) {
        argp_error(state, "%s", e.what());
    }
    return 0;
}

struct argp_option options[] = {
    {"seed",                            -1, "HEX", 0, "Random data from which to generate a master key."},
    {"asset",                           -2, "ENUM btc|btct", 0, "A cryptocurrency asset."},
    {"network",                         -3, "ENUM mainnet|testnet", 0, "The network."},
    {"master-key",                      -4, "XPRV", 0, "The BIP-32 master HD key."},
    {"master-key-fingerprint",          -5, "HEX", 0, "Fingerprint of the master HD key."},
    {"output-type",                     -6, "ENUM pkh|wpkh|sh-wpkh", 0, "The output descriptor type."},
    {"purpose",                         -7, "INDEX", 0, "The purpose field of the BIP-44 derivation path."},
    {"coin-type",                       -8, "INDEX", 0, "The coin type field of the BIP-44 derivation path."},
    {"account-index",                   -9, "INDEX", 0, "The account field of the BIP-44 derivation path."},
    {"account-derivation-path",         -10, "BIP32_PATH", 0, "m/purpose'/coin-type'/accont-index'."},
    {"account-key",                     -11, "XPRV", 0, "The BIP-44 account key."},
    {"account-pub-key",                 -12, "XPUB", 0, "The BIP-44 account public key."},
    {"chain-type",                      -13, "ENUM internal|external|identity", 0, "The BIP-44 chain type (change) field."},
    {"chain-type-int",                  -14, "INDEX", 0, "The BIP-44 change field integer value."},
    {"address-index",                   -15, "INDEX_BOUND", 0, "The BIP-44 address_index field. '*' is allowed for output descriptors."},
    {"address-derivation-path",         -16, "BIP32_PATH", 0, "The BIP-32 address derivation path, starting from the account-key."},
    {"full-address-derivation-path",    -17, "BIP32_PATH", 0, "The BIP-32 address derivation path, starting from the master-key."},
    {"address-key",                     -18, "XPRV", 0, "The BIP-32 address HD key."},
    {"address-pub-key",                 -19, "XPUB", 0, "The BIP-32 address public HD key."},
    {"address-ec-key",                  -20, "ECPRV", 0, "The address EC key."},
    {"address-ec-key-wif",              -21, "WIF", 0, "The address EC key in WIF format."},
    {"address-pub-ec-key",              -22, "ECPUB", 0, "The compressed public EC key."},
    {"address-pkh",                     -23, "ADDRESS", 0, "The pay-to-public-key-hash address."},
    {"address-sh",                      -24, "ADDRESS", 0, "The pay-to-script-hash address."},
    {"address-segwit",                  -25, "ADDRESS", 0, "The segwit address."},
    {"output-descriptor",               -26, "OUTPUT_DESCRIPTOR", 0, "A single-signature output descriptor."},
    {"psbt",                            -27, "BASE64 | HEX | UR:CRYPTO-PSBT", 0, "A partially signed Bitcoin transaction (PSBT)."},
    {"psbt-hex",                        -28, "HEX", 0, "PSBT in hex format."},
    {"psbt-ur",                         -29, "UR:CRYPTO-PSBT", 0, "PSBT in UR format."},
    {"psbt-finalized",                  -30, "BASE64 | HEX | UR:CRYPTO-PSBT", 0, "The finalized PSBT."},
    {"psbt-finalized-hex",              -31, "HEX", 0, "Finalized PSBT in hex format."},
    {"psbt-finalized-ur",               -32, "UR:CRYPTO-PSBT", 0, "Finalized PSBT in UR format."},
    {"psbt-signed",                     -33, "BASE64 | HEX | UR:CRYPTO-PSBT", 0, "A PBST signed by address-key."},
    {"psbt-signed-hex",                 -34, "HEX", 0, "Signed PSBT in hex format."},
    {"psbt-signed-ur",                  -35, "UR:CRYPTO-PSBT", 0, "Signed PSBT in UR format."},
    {"transaction",                     -36, "HEX | UR:CRYPTO-TX", 0, "The raw Bitcoin transaction."},
    {"transaction-ur",                  -37, "UR:CRYPTO-TX", 0, "The raw Bitcoin transaction in UR format."},

    { 0 }
};

auto argp_program_version = PACKAGE_VERSION;
const char* argp_program_bug_address = "ChristopherA@BlockchainCommons.com. © 2020 Blockchain Commons";

auto doc = "Derives cryptocurrency keys and addresses.";
struct argp argp = { options, parse_opt, "INPUT", doc };

Params* Params::parse( int argc, char *argv[] ) {
    auto p = new Params();
    argp_parse( &argp, argc, argv, 0, 0, p );
    return p;
}

void Params::process() {
    StringVector outputs;

    for(auto node_name: args) {
        auto node = model.find_by_name(node_name);
        assert(node != nullptr);
        auto value = node->to_string();
        if(value.has_value()) {
            outputs.push_back(*value);
        } else {
            throw domain_error("Cannot derive " + node_name + " from provided inputs.");
        }
    }

    output = join(outputs, "\n");
}
