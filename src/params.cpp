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

auto argp_program_version = PACKAGE_VERSION;
const char* argp_program_bug_address = "ChristopherA@BlockchainCommons.com. © 2021 Blockchain Commons";

auto doc = "Derives cryptocurrency keys, addresses, and signatures.";

Params* Params::parse( int argc, char *argv[] ) {
    auto p = new Params();

    auto nodes = p->model.all_nodes();
    auto node_count = nodes.size();
    auto derivations = p->model.derivations();
    auto derivations_count = derivations.size();
    auto options_count = node_count + derivations_count + 2;
    argp_option options[options_count];
    auto option_index = 0;
    for(auto i = 0; i < node_count; i++) {
        auto node = nodes[i];
        options[option_index++] = { node->name().c_str(), node->tag(), node->help_type().c_str(), 0, node->help_desc().c_str() };
    }
    
    options[option_index++] = { nullptr, 0, nullptr, OPTION_DOC | OPTION_NO_USAGE, "DERIVATIONS: " };

    for(auto i = 0; i < derivations_count; i++) {
        auto derivation = derivations[i];
        auto len = derivation.length() + 1;
        char* s = (char*)malloc(len);
        memcpy(s, derivation.c_str(), len);
        options[option_index++] = { nullptr, 0, nullptr, OPTION_DOC | OPTION_NO_USAGE, s };
    }
    options[option_index] = { 0 };

    struct argp argp = { options, parse_opt, "INPUT", doc };

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
