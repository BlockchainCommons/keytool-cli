#include "seed.hpp"
#include <set>
#include <bc-ur/bc-ur.hpp>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

static const auto cborDecodingFlags = ur::CborLite::Flag::requireMinimalEncoding;

Seed::Seed(const ByteVector& data, const string& name, const string& note)
    : _data(data)
    , _name(name)
    , _note(note)
    { }

static Seed parse_seed(const string& s) {
    try {
        return Seed(hex_to_data(s));
    } catch (...) { }

    try {
        auto ur = URDecoder::decode(s);
        if(ur.type() != "crypto-seed") {
            throw domain_error("Unexpected UR type: " + ur.type() + ". Expected crypto-seed.");
        }
        auto cbor = ur.cbor();
        auto pos = cbor.begin();
        auto end = cbor.end();
        size_t map_len;
        decodeMapSize(pos, end, map_len, cborDecodingFlags);
        set<int> labels;
        auto data = ByteVector();
        auto name = string();
        auto note = string();
        for(auto index = 0; index < map_len; index++) {
            int label;
            decodeInteger(pos, end, label, cborDecodingFlags);
            if(labels.find(label) != labels.end()) {
                throw domain_error("Duplicate label.");
            }
            labels.insert(label);
            switch (label) {
                case 1: // payload
                    decodeBytes(pos, end, data, cborDecodingFlags);
                    break;
                case 2: { // creation-date
                    ur::CborLite::Tag tag;
                    size_t value;
                    decodeTagAndValue(pos, end, tag, value, cborDecodingFlags);
                    if(tag != ur::CborLite::Major::semantic) {
                        throw domain_error("Invalid date.");
                    }
                    switch(value) {
                        case 0: {
                            string date;
                            decodeText(pos, end, date, cborDecodingFlags);
                        }
                            break;
                        case 1: {
                            double date;
                            decodeDoubleFloat(pos, end, date, cborDecodingFlags);
                        }
                            break;
                        case 100: {
                            int date;
                            decodeInteger(pos, end, date, cborDecodingFlags);
                        }
                            break;
                        default:
                            throw domain_error("Invalid date.");
                    }
                }
                    break;
                case 3: // name
                    decodeText(pos, end, name, cborDecodingFlags);
                    break;
                case 4: // note
                    decodeText(pos, end, note, cborDecodingFlags);
                    break;
                default:
                    throw domain_error("Unknown label.");
                    break;
            }
        }
        if(data.empty()) {
            throw domain_error("Seed data may not be empty.");
        }
        return Seed(data, name, note);
    } catch(...) { }

    throw domain_error("Invalid ur:crypto-seed.");
}

Seed::Seed(const Seed& seed)
    : _data(seed.data())
    , _name(seed.name())
    , _note(seed.note())
    { }

Seed::Seed(const string& s)
    : Seed(parse_seed(s))
    { }

string Seed::hex() const {
    return ::data_to_hex(data());
}

string Seed::ur() const {
    size_t map_size = 1;

    // payload
    ByteVector data_map_entry;
    encodeInteger(data_map_entry, 1);
    encodeBytes(data_map_entry, data());

    // creation-date

    // name
    ByteVector name_map_entry;
    if (!name().empty()) {
        map_size += 1;
        encodeInteger(name_map_entry, 3);
        encodeText(name_map_entry, name());
    }

    // note
    ByteVector note_map_entry;
    if (!note().empty()) {
        map_size += 1;
        encodeInteger(name_map_entry, 4);
        encodeText(name_map_entry, note());
    }

    ByteVector cbor;
    encodeMapSize(cbor, map_size);
    ::append(cbor, data_map_entry);
    ::append(cbor, name_map_entry);
    ::append(cbor, note_map_entry);

    return UREncoder::encode(UR("crypto-seed", cbor));
}

ByteVector Seed::digest() const {
    return ::sha256(_data);
}
