#include "phone.h"
#include "../services/validator.h"

phone::phone() : valid(false), type(phone_type::home) {}

phone::phone(const std::string& number, phone_type type) : type(type), valid(false) {
    setnumber(number);
}

void phone::setnumber(const std::string& raw_number) {
    this->raw_number = raw_number;
    valid = validator::checkphone(raw_number);
    if (valid) {
        formatted_number = validator::formatphone(raw_number);
    } else {
        formatted_number.clear();
    }
}

void phone::settype(phone_type type) { this->type = type; }

std::string phone::getformatted() const { return formatted_number; }
std::string phone::getraw() const { return raw_number; }
phone_type phone::gettype() const { return type; }
bool phone::isvalid() const {  return valid; }

std::string phone::type_to_string(phone_type type) {
    switch (type) {
        case phone_type::work: return "рабочий";
        case phone_type::service: return "служебный";
        case phone_type::home:
        default: return "домашний";
    }
}

phone_type phone::string_to_type(const std::string& type_str) {
    if (type_str == "рабочий" || type_str == "work") return phone_type::work;
    if (type_str == "служебный" || type_str == "service") return phone_type::service;
    return phone_type::home;
}