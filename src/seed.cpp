#include "seed.hpp"
#include <set>
#include <bc-ur/bc-ur.hpp>

using namespace std;
using namespace ur;
using namespace ur::CborLite;

const auto cborDecodingFlags = ur::CborLite::Flag::requireMinimalEncoding;

Seed::Seed(const ByteVector& data, const std::string& name, const std::string& note)
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
                throw std::runtime_error("Duplicate label.");
            }
            labels.insert(label);
            switch (label) {
                case 1: // payload
                    decodeBytes(pos, end, data, cborDecodingFlags);
                    break;
                case 2: { // creation-date
                    ur::CborLite::Tag tag;
                    size_t value;
                    ur::CborLite::decodeTagAndValue(pos, end, tag, value, cborDecodingFlags);
                    if(tag != ur::CborLite::Major::semantic) {
                        throw std::runtime_error("Invalid date.");
                    }
                    switch(value) {
                        case 0: {
                            std::string date;
                            ur::CborLite::decodeText(pos, end, date, cborDecodingFlags);
                        }
                            break;
                        case 1: {
                            double date;
                            ur::CborLite::decodeDoubleFloat(pos, end, date, cborDecodingFlags);
                        }
                            break;
                        case 100: {
                            int date;
                            ur::CborLite::decodeInteger(pos, end, date, cborDecodingFlags);
                        }
                            break;
                        default:
                            throw std::runtime_error("Invalid date.");
                    }
                }
                    break;
                case 3:
                    break;
                case 4:
                    break;
                default:
                    throw std::runtime_error("Unknown label.");
                    break;
            }
        }
    } catch(...) { }

    throw domain_error("Invalid ur:crypto-seed");
}

Seed::Seed(const std::string& s) {

}
