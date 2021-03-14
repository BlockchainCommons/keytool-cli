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
    virtual const std::string& help_type() const = 0;
    virtual const std::string& help_desc() const = 0;

    virtual void set_info(const std::string& name, const std::string& type, const std::string& desc) = 0;
    virtual void set_tag(int tag) = 0;
    virtual std::optional<std::string> to_string() = 0;
    virtual void from_string(const std::string& s) = 0;
    virtual bool has_value() = 0;
};

template <typename T>
class DataNode final: public DataNodeProtocol {
public:
    DataNode() : _tried(false) { }

    virtual const std::string& name() const override { return _name; }
    virtual int tag() const override { return _tag; }
    virtual const std::string& help_type() const override { return _help_type; }
    virtual const std::string& help_desc() const override { return _help_desc; }

    virtual void set_info(const std::string& name, const std::string& type, const std::string& desc) override { _name = name; _help_type = type; _help_desc = desc; }
    virtual void set_tag(int tag) override { _tag = tag; }
    void set_derivation(std::function<std::optional<T>(void)> derivation) { _derivation = derivation; }
    void set_to_string(std::function<std::string(const T&)> t) { _to_string = t; }
    void set_from_string(std::function<T(const std::string& s)> t) { _from_string = t; }

    std::optional<T> optional_value() {
        if(!_tried) {
            _tried = true;
            if(!_value.has_value() && _derivation) {
                _value = _derivation();
            }
        }
        return _value;
    }

    T value() {
        auto t = optional_value();
        if(t.has_value()) {
            return *t;
        } else {
            std::string message = "Cannot derive " + name();
            throw std::domain_error(message);
        }
    }

    void set_value(const T& value) { _value = value; }

    virtual bool has_value() override {
        return optional_value().has_value();
    }

    virtual std::optional<std::string> to_string() override {
        auto t = optional_value();
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
            this->set_value(_from_string(s));
        } catch(std::exception &e) {
            throw std::domain_error("Invalid input for " + name() + ": " + e.what());
        }
    }

private:
    std::string _name;
    std::string _help_type;
    std::string _help_desc;
    int _tag;
    bool _tried;
    std::optional<T> _value;
    std::function<std::string(const T&)> _to_string;
    std::function<T(const std::string&)> _from_string;
    std::function<std::optional<T>(void)> _derivation;
};

#endif // KEYTOOL_DATA_NODE_HPP
