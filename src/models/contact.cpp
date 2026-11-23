#include "contact.h"
#include "../services/validator.h"
#include <sstream>

contact::contact() : id(0), birth_day(0), birth_month(0), birth_year(0) {}

contact::contact(int id) : id(id), birth_day(0), birth_month(0), birth_year(0) {}

int contact::generateid() {
    static int next_id = 1;
    return next_id++;
}

void contact::setid(int id) {
    this->id = id;
}

void contact::setname(const std::string& name) {
    this->name = name;
}

void contact::setsurname(const std::string& surname) {
    this->surname = surname;
}

void contact::setemail(const std::string& email) {
    this->email = email;
}

void contact::addphone(const std::string& phone_str) {
    phones.emplace_back(phone_str);
}

void contact::setpatronymic(const std::string& patronymic) {
    this->patronymic = patronymic;
}

void contact::setaddress(const std::string& address) {
    this->address = address;
}

void contact::setbirthdate(int day, int month, int year) {
    birth_day = day;
    birth_month = month;
    birth_year = year;
}

std::string contact::getname() const { return name; }
std::string contact::getsurname() const { return surname; }
std::string contact::getpatronymic() const { return patronymic; }
std::string contact::getaddress() const { return address; }
std::string contact::getemail() const { return email; }
int contact::getid() const { return id; }

std::string contact::getbirthdate() const {
    // создание объекта потока для удобного форматирования текста
    std::ostringstream oss; 
    oss << birth_day << "." << birth_month << "." << birth_year;
    return oss.str();
}

std::vector<phone> contact::getphones() const { return phones; }

bool contact::validate() const {
    if (!validator::checkname(name)) return false;
    if (!validator::checkname(surname)) return false;
    if (!patronymic.empty() && !validator::checkname(patronymic)) return false;
    if (!validator::checkemail(email)) return false;
    
    // auto& - ссылка на оригинал чтобы не создавать копии
    for (const auto& p : phones) {
        if (!p.isvalid()) return false;
    }
    
    if (birth_day != 0 || birth_month != 0 || birth_year != 0) {
        if (!validator::checkdate(birth_day, birth_month, birth_year)) return false;
    }
    
    return true;
}

bool contact::hasrequiredfields() const {
    return !name.empty() && !surname.empty() && !email.empty() && !phones.empty();
}