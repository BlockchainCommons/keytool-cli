//
//  utils.hpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#ifndef KEYTOOL_UTILS_HPP
#define KEYTOOL_UTILS_HPP

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <vector>
#include <ostream>
#include <algorithm>
#include <cctype>
#include <locale>

typedef std::vector<uint8_t> ByteVector;
typedef std::vector<std::string> StringVector;

std::string data_to_hex(const ByteVector& in);
uint8_t hex_digit_to_bin(char hex);
std::string byte_to_hex(uint8_t byte);

ByteVector hex_to_data(const std::string& hex);

std::string join(const StringVector &strings, const std::string &separator);
StringVector split(const std::string& s, const char& separator);

std::string to_lowercase(const std::string& s);

template<typename T>
std::vector<T> take(const std::vector<T> &buf, size_t count) {
    auto first = buf.begin();
    auto c = std::min(buf.size(), count);
    auto last = first + c;
    return std::vector(first, last);
}

template<typename T>
void append(std::vector<T>& target, const std::vector<T>& source) {
    target.insert(target.end(), source.begin(), source.end());
}

template<typename T>
std::vector<T> flatten(const std::vector<std::vector<T>>& source) {
    std::vector<T> result;
    for(auto item: source) {
        append(result, item);
    }
    return result;
}

ByteVector random_bytes(size_t len);

std::ostream& operator<< (std::ostream& os, const ByteVector& bytes);

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}

std::string data_to_base64(const ByteVector& data);
ByteVector base64_to_data(std::string const& data);

ByteVector make_uuid();

const ByteVector sha256(const ByteVector &buf);

uint32_t parse_uint32(const std::string& s);

#endif // KEYTOOL_UTILS_HPP
