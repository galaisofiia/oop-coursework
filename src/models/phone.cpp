#include "phone.h"
#include "../services/validator.h"

phone::phone() : valid(false) {}

phone::phone(const std::string& raw_number) {
    setnumber(raw_number);
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

std::string phone::getformatted() const {
    return formatted_number;
}

std::string phone::getraw() const {
    return raw_number;
}

bool phone::isvalid() const {
    return valid;
}