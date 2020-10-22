#include "index-bound.hpp"

using namespace std;

string IndexBound::to_string() const {
    switch(type()) {
        case Type::value: {
            return std::to_string(index());
        }
        break;
        case Type::wildcard: {
            return "*";
        }
        break;
    }
}

ostream& operator<< (ostream& os, const IndexBound& i) {
    return os << i.to_string();
}

IndexBound parse_index_range(const string& s) {
    if(s.empty()) {
        throw domain_error("Invalid index.");
    }
    if(s == "*") {
        return IndexBound();
    }
    int index = 0;
    try {
        index = stoi(s);
    } catch(...) {
        throw domain_error("Invalid index.");
    }
    if(index < 0) {
        throw domain_error("Invalid index.");
    }
    return IndexBound(index);
}
