// #ifndef KEYTOOL_INDEX_RANGE_HPP
// #define KEYTOOL_INDEX_RANGE_HPP

// #include <stdint.h>
// #include <string>
// #include <ostream>

// class IndexBound final {
// public:
//     enum Type {
//         value,
//         wildcard
//     };

//     IndexBound() : _type(Type::wildcard) { }
//     // cppcheck-suppress noExplicitConstructor
//     IndexBound(uint32_t index) : _type(Type::value), _index(index) { }

//     Type type() const { return _type; }
//     uint32_t index() const { return _index; }

//     std::string to_string() const;

// private:
//     Type _type;
//     uint32_t _index = 0;
// };

// std::ostream& operator<< (std::ostream& os, const IndexBound& i);

// IndexBound parse_index_range(const std::string& s);

// #endif // KEYTOOL_INDEX_RANGE_HPP
