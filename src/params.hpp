//
//  params.hpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <optional>
#include <bc-slip39/bc-slip39.h>
#include <bc-ur/bc-ur.hpp>

#include "utils.hpp"

#define MAX_GROUPS 16
#define MAX_RAW_GROUPS (MAX_GROUPS + 1)

#define MAX_ARGS 256

class RawParams {
public:
    std::string seed;

    string_vector args;
};

class Params {
public:
    Params() { }
    ~Params();

    void validate();

    RawParams raw;
    string_vector input;
    std::string output;
    struct argp_state* state;

    static Params* parse( int argc, char *argv[] );
    void read_args_from_stdin();

    std::string get_one_argument();
    std::string get_combined_arguments();
    string_vector get_multiple_arguments();

private:
    group_descriptor parse_group_spec(const std::string &string);
};
