//
//  utils.cpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#include "utils.hpp"

#include <math.h>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <algorithm>

#include <bc-crypto-base/bc-crypto-base.h>
#include "randombytes.h"

using namespace std;

string byte_to_hex(uint8_t byte) {
    auto hex = "0123456789abcdef";
    string result;
    result.append(1, hex[(byte >> 4) & 0xF]);
    result.append(1, hex[byte & 0xF]);
    return result;
}

string data_to_hex(const ByteVector& in) {
    string result;
    for(auto c: in) {
        result.append(byte_to_hex(c));
    }
    return result;
}

uint8_t hex_digit_to_bin(char hex) {
    if (hex >= '0' && hex <= '9') {
        return hex - '0';
    } else if (hex >= 'A' && hex <= 'F') {
        return hex - 'A' + 10;
    } else if (hex >= 'a' && hex <= 'f') {
        return hex - 'a' + 10;
    } else {
        throw domain_error("Invalid hex digit");
    }
}

ByteVector hex_to_data(const string& hex) {
    ByteVector result;

    auto len = hex.length();
    if(len % 2 != 0) {
        throw domain_error("Hex string must have even number of characters.");
    }
    auto count = len / 2;
    result.reserve(count);
    for(auto i = 0; i < count; i++) {
        auto b1 = hex_digit_to_bin(hex[i * 2]);
        auto b2 = hex_digit_to_bin(hex[i * 2 + 1]);
        result.push_back((b1 << 4) | b2);
    }

    return result;
}

string join(const StringVector &strings, const string &separator) {
    ostringstream result;
    bool first = true;
    for(auto s: strings) {
        if(!first) {
            result << separator;
        }
        result << s;
        first = false;
    }
    return result.str();
}

StringVector split(const string& s, const char& separator) {
	StringVector result;
	string buf;

	for(auto c: s) {
		if(c != separator) {
            buf += c;
        } else if(c == separator && buf.length() > 0) {
            result.push_back(buf);
            buf = "";
        }
	}

	if(buf != "") {
        result.push_back(buf);
    }

	return result;
}

string to_lowercase(const string& s) {
    string out;
    transform(s.begin(), s.end(), back_inserter(out), ::tolower);
    return out;
}

ByteVector random_bytes(size_t len) {
    ByteVector result(len, 0);
    randombytes(&result[0], len);
    return result;
}

ostream& operator<< (ostream& os, const ByteVector& bytes) {
    return os << data_to_hex(bytes);
}

static string get_base64_chars() {
    static string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz"
                                   "0123456789+/";
    return base64_chars;
}

// Base64 codec based on:
// https://github.com/tobiaslocker/base64
// MIT License

string data_to_base64(const ByteVector& data) {
    int counter = 0;
    uint32_t bit_stream = 0;
    const string base64_chars = get_base64_chars();
    string encoded;
    int offset = 0;
    for (auto const& c : data) {
        auto num_val = static_cast<unsigned int>(c);
        offset = 16 - counter % 3 * 8;
        bit_stream += num_val << offset;
        if (offset == 16) {
            encoded += base64_chars.at(bit_stream >> 18 & 0x3f);
        }
        if (offset == 8) {
            encoded += base64_chars.at(bit_stream >> 12 & 0x3f);
        }
        if (offset == 0 && counter != 3) {
            encoded += base64_chars.at(bit_stream >> 6 & 0x3f);
            encoded += base64_chars.at(bit_stream & 0x3f);
            bit_stream = 0;
        }
        counter++;
    }
    if (offset == 16) {
        encoded += base64_chars.at(bit_stream >> 12 & 0x3f);
        encoded += "==";
    }
    if (offset == 8) {
        encoded += base64_chars.at(bit_stream >> 6 & 0x3f);
        encoded += '=';
    }
    return encoded;
}

ByteVector base64_to_data(string const& data) {
    int counter = 0;
    uint32_t bit_stream = 0;
    ByteVector decoded;
    int offset;
    const string base64_chars = get_base64_chars();
    for (auto c : data) {
        auto num_val = base64_chars.find(c);
        if (num_val != string::npos) {
            offset = 18 - counter % 4 * 6;
            bit_stream += num_val << offset;
            if (offset == 12) {
                decoded.push_back(
                    static_cast<uint8_t>(bit_stream >> 16 & 0xff));
            }
            if (offset == 6) {
                decoded.push_back(static_cast<uint8_t>(bit_stream >> 8 & 0xff));
            }
            if (offset == 0 && counter != 4) {
                decoded.push_back(static_cast<uint8_t>(bit_stream & 0xff));
                bit_stream = 0;
            }
        } else if (c != '=') {
            throw domain_error("Invalid base64.");
        }
        counter++;
    }
    return decoded;
}

const ByteVector sha256(const ByteVector &buf) {
    uint8_t digest[SHA256_DIGEST_LENGTH];
    sha256_Raw(buf.data(), buf.size(), digest);
    return ByteVector(digest, digest + SHA256_DIGEST_LENGTH);
}

uint32_t parse_uint32(const string& s) {
    int n;
    try {
        n = stoi(s, nullptr, 0);
    } catch(exception &e) {
        throw domain_error("Expected integer >= 0.");
    }
    if(n < 0) {
        throw domain_error("Expected integer >= 0.");
    }
    return n;
}

void push_uint32(ByteVector& v, uint32_t i) {
    v.push_back( (i >> 24) & 0xFF);
    v.push_back( (i >> 16) & 0xFF);
    v.push_back( (i >>  8) & 0xFF);
    v.push_back( (i >>  0) & 0xFF);
}

ByteVector big_endian_data(uint32_t i) {
    ByteVector result;
    push_uint32(result, i);
    return result;
}

ByteVector data_of(const uint8_t* b, size_t len) {
    return ByteVector(b, b + len);
}

bool is_all_zero(const ByteVector& v) {
    return all_of(v.begin(), v.end(), [](auto b) { return b == 0; });
}
