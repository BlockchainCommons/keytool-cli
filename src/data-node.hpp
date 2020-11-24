#ifndef KEYTOOL_DATA_NODE_HPP
#define KEYTOOL_DATA_NODE_HPP

#include <optional>
#include <functional>
#include <string>
#include <sstream>
#include <stdexcept>

class DataNodeProtocol {
public:
    virtual const std::string& name() const = 0;
    virtual int tag() const = 0;
    virtual std::optional<std::string> to_string() = 0;
    virtual void from_string(const std::string& s) = 0;
    virtual bool has_value() = 0;
};

template <typename T>
class DataNode final: public DataNodeProtocol {
public:
    DataNode(const std::string& name, int tag) : _name(name), _tag(tag), _tried(false) { }

    virtual const std::string& name() const override { return _name; }
    virtual int tag() const override { return _tag; }

    void set_f(std::function<std::optional<T>(void)> f) { _f = f; }
    void set_to_string(std::function<std::string(const T&)> t) { _to_string = t; }
    void set_from_string(std::function<T(const std::string& s)> t) { _from_string = t; }

    std::optional<T> value() {
        if(!_tried) {
            _tried = true;
            if(!_value.has_value() && _f) {
                _value = _f();
            }
        }
        return _value;
    }

    virtual bool has_value() override {
        return value().has_value();
    }

    virtual std::optional<std::string> to_string() override {
        auto t = value();
        if(t.has_value()) {
            return _to_string(*t);
        } else {
            return std::nullopt;
        }
    }

    virtual void from_string(const std::string& s) override {
        if(!_from_string) {
            throw std::domain_error("Input not accepted for " + name());
        }
        try {
            *this = _from_string(s);
        } catch(std::exception &e) {
            throw std::domain_error("Invalid input for " + name() + ": " + e.what());
        }
    }

    T operator() () {
        auto t = value();
        if(t.has_value()) {
            return *t;
        } else {
            std::string message = "Cannot derive " + name();
            throw std::domain_error(message);
        }
    }

    void operator= (const T& value) { _value = value; }

private:
    std::string _name;
    int _tag;
    bool _tried;
    std::optional<T> _value;
    std::function<std::optional<T>(void)> _f;
    std::function<std::string(const T&)> _to_string;
    std::function<T(const std::string&)> _from_string;
};

#endif // KEYTOOL_DATA_NODE_HPP
