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
#include <algorithm>

#include <bc-crypto-base/bc-crypto-base.h>
#include <wally.hpp>

#include "params.hpp"
#include "config.h"

using namespace std;

Params::~Params() {
}

void Params::validate() {
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

    auto bad_arg = std::find_if(args.begin(), args.end(), [&](auto arg) { return !model.is_valid_name(arg); });
    if(bad_arg != args.end()) {
        throw domain_error("Unknown output requested: " + *bad_arg);
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
                auto& opts = p->opts;
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
    {"seed-ur",                         -2, "HEX", 0, "Random data from which to generate a master key."},
    {"asset",                           -3, "ENUM btc|btct", 0, "A cryptocurrency asset."},
    {"network",                         -4, "ENUM mainnet|testnet", 0, "The network."},
    {"master-key",                      -5, "XPRV", 0, "The BIP-32 master HD key."},
    {"master-key-fingerprint",          -6, "HEX", 0, "Fingerprint of the master HD key."},
    {"output-type",                     -7, "ENUM pkh|wpkh|sh-wpkh", 0, "The output descriptor type."},
    {"purpose",                         -8, "INDEX", 0, "The purpose field of the BIP-44 derivation path."},
    {"coin-type",                       -9, "INDEX", 0, "The coin type field of the BIP-44 derivation path."},
    {"account-index",                   -10, "INDEX", 0, "The account field of the BIP-44 derivation path."},
    {"account-derivation-path",         -11, "BIP32_PATH", 0, "m/purpose'/coin-type'/accont-index'."},
    {"account-key",                     -12, "XPRV", 0, "The BIP-44 account key."},
    {"account-pub-key",                 -13, "XPUB", 0, "The BIP-44 account public key."},
    {"chain-type",                      -14, "ENUM internal|external|identity", 0, "The BIP-44 chain type (change) field."},
    {"chain-type-int",                  -15, "INDEX", 0, "The BIP-44 change field integer value."},
    {"address-index",                   -16, "INDEX_BOUND", 0, "The BIP-44 address_index field. '*' is allowed for output descriptors."},
    {"address-derivation-path",         -17, "BIP32_PATH", 0, "The BIP-32 address derivation path, starting from the account-key."},
    {"full-address-derivation-path",    -18, "BIP32_PATH", 0, "The BIP-32 address derivation path, starting from the master-key."},
    {"address-key",                     -19, "XPRV", 0, "The BIP-32 address HD key."},
    {"address-pub-key",                 -20, "XPUB", 0, "The BIP-32 address public HD key."},
    {"address-ec-key",                  -21, "ECPRV", 0, "The address EC key."},
    {"address-ec-key-wif",              -22, "WIF", 0, "The address EC key in WIF format."},
    {"address-pub-ec-key",              -23, "ECPUB", 0, "The compressed public EC key."},
    {"address-pkh",                     -24, "ADDRESS", 0, "The pay-to-public-key-hash address."},
    {"address-sh",                      -25, "ADDRESS", 0, "The pay-to-script-hash address."},
    {"address-segwit",                  -26, "ADDRESS", 0, "The segwit address."},
    {"output-descriptor",               -27, "OUTPUT_DESCRIPTOR", 0, "A single-signature output descriptor."},
    {"psbt",                            -28, "BASE64 | HEX | UR:CRYPTO-PSBT", 0, "A partially signed Bitcoin transaction (PSBT)."},
    {"psbt-hex",                        -29, "HEX", 0, "PSBT in hex format."},
    {"psbt-ur",                         -30, "UR:CRYPTO-PSBT", 0, "PSBT in UR format."},
    {"psbt-finalized",                  -31, "BASE64 | HEX | UR:CRYPTO-PSBT", 0, "The finalized PSBT."},
    {"psbt-finalized-hex",              -32, "HEX", 0, "Finalized PSBT in hex format."},
    {"psbt-finalized-ur",               -33, "UR:CRYPTO-PSBT", 0, "Finalized PSBT in UR format."},
    {"psbt-signed",                     -34, "BASE64 | HEX | UR:CRYPTO-PSBT", 0, "A PBST signed by address-key."},
    {"psbt-signed-hex",                 -35, "HEX", 0, "Signed PSBT in hex format."},
    {"psbt-signed-ur",                  -36, "UR:CRYPTO-PSBT", 0, "Signed PSBT in UR format."},
    {"transaction",                     -37, "HEX | UR:CRYPTO-TX", 0, "The raw Bitcoin transaction."},
    {"transaction-ur",                  -38, "UR:CRYPTO-TX", 0, "The raw Bitcoin transaction in UR format."},

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
