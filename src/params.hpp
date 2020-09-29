//
//  params.hpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#ifndef KEYTOOL_PARAMS_HPP
#define KEYTOOL_PARAMS_HPP

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <optional>
#include <map>

#include "utils.hpp"
#include "model.hpp"

#define MAX_GROUPS 16
#define MAX_RAW_GROUPS (MAX_GROUPS + 1)

#define MAX_ARGS 256

class Params {
public:
    Params() { }
    ~Params();

    void validate();

    std::map<int, std::string> opts;
    StringVector args;

    Model model;
    std::string output;
    struct argp_state* state;

    static Params* parse( int argc, char *argv[] );

    void process();

    // void read_args_from_stdin();
    // std::string get_one_argument();
    // std::string get_combined_arguments();
    // StringVector get_multiple_arguments();
};

#endif // KEYTOOL_PARAMS_HPP
