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
    {"purpose",                         -6, "INDEX", 0, "The purpose field of the BIP-44 derivation path."},
    {"coin-type",                       -7, "INDEX", 0, "The coin type field of the BIP-44 derivation path."},
    {"account-index",                   -8, "INDEX", 0, "The account field of the BIP-44 derivation path."},
    {"account-derivation-path",         -9, "BIP32_PATH", 0, "m/purpose'/coin-type'/accont-index'."},
    {"account-key",                     -10, "XPRV", 0, "The BIP-44 account key."},
    {"account-pub-key",                 -11, "XPUB", 0, "The BIP-44 account public key."},
    {"chain-type",                      -12, "ENUM internal|external|identity", 0, "The BIP-44 chain type (change) field."},
    {"chain-type-int",                  -13, "INDEX", 0, "The BIP-44 change field integer value."},
    {"address-index",                   -14, "INDEX_BOUND", 0, "The BIP-44 address_index field. '*' is allowed for output descriptors."},
    {"partial-address-derivation-path", -15, "BIP32_PATH", 0, "The BIP-32 address derivation path, starting from the account-key."},
    {"full-address-derivation-path",    -16, "BIP32_PATH", 0, "The BIP-32 address derivation path, starting from the master-key."},
    {"address-key",                     -17, "XPRV", 0, "The BIP-32 address HD key."},
    {"address-pub-key",                 -18, "XPUB", 0, "The BIP-32 address public HD key."},
    {"address-ec-key",                  -19, "ECPRV", 0, "The address EC key."},
    {"address-ec-key-wif",              -20, "WIF", 0, "The address EC key in WIF format."},
    {"address-pub-ec-key",              -21, "ECPUB", 0, "The compressed public EC key."},
    {"address-pkh",                     -22, "ADDRESS", 0, "The pay-to-public-key-hash address."},
    {"address-sh",                      -23, "ADDRESS", 0, "The pay-to-script-hash address."},
    {"output-descriptor-type",          -24, "ENUM pkh|wpkh|sh-wpkh", 0, "The output descriptor type."},
    {"output-descriptor",               -25, "OUTPUT_DESCRIPTOR", 0, "A single-signature output descriptor."},

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
