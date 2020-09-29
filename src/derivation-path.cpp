#include "derivation-path.hpp"

#include <algorithm>
#include <exception>
#include "utils.hpp"

using namespace std;

string DerivationPathElement::to_string() const {
    string h = is_hardened() ? "h" : "";
    auto s = std::to_string(index());
    return s + h;
}

ostream& operator<< (ostream& os, const DerivationPathElement& elem) {
    return os << elem.to_string();
}

string to_string(const DerivationPath& path) {
    StringVector elems;
    elems.push_back("m");
    transform(path.begin(), path.end(), back_inserter(elems), [](const DerivationPathElement& elem) { return elem.to_string(); });
    return join(elems, "/");
}

ostream& operator<< (ostream& os, const DerivationPath& path) {
    return os << to_string(path);
}

static DerivationPathElement parse_elem(const string& s) {
    string s2 = s;
    if(s2.empty()) {
        throw domain_error("Invalid derivation path element.");
    }
    bool is_hardened = false;
    if(s2.back() == 'h') {
        is_hardened = true;
        s2.pop_back();
    }
    if(s2.empty()) {
        throw domain_error("Invalid derivation path element.");
    }
    int index = 0;
    try {
        index = stoi(s2);
    } catch(...) {
        throw domain_error("Invalid derivation path element.");
    }
    if(index < 0) {
        throw domain_error("Invalid derivation path element.");
    }
    return DerivationPathElement(index, is_hardened);
}

DerivationPath parse_derivation_path(const string& path) {
    auto elems = split(to_lowercase(path), '/');
    if(elems.size() < 2) {
        throw domain_error("Invalid derivation path.");
    } else if(elems.front() != "m") {
        throw domain_error("Invalid derivation path.");
    }
    elems.erase(elems.begin());
    DerivationPath result;
    for(auto elem: elems) {
        result.push_back(parse_elem(elem));
    }
    return result;
}
