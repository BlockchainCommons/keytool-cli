#ifndef KEYTOOL_CHAIN_TYPE_HPP
#define KEYTOOL_CHAIN_TYPE_HPP

#include <optional>
#include <string>
#include <vector>

class ChainType final {
public:
    ChainType(const std::string& name, uint32_t index) : _name(name), _index(index) { }
    ChainType(uint32_t index) : _index(index) { }

    const std::optional<std::string>& name() const { return _name; }
    uint32_t index() const { return _index; }

    std::string to_string() const;

    static ChainType external();
    static ChainType change();
    static ChainType identity();
    static std::vector<ChainType> chain_types();
    static ChainType find(const std::string& name);

private:
    std::optional<std::string> _name;
    uint32_t _index;
};

bool operator==(const ChainType& a, const ChainType& b);

std::ostream& operator<< (std::ostream& os, const ChainType& network);

#endif // KEYTOOL_CHAIN_TYPE_HPP
