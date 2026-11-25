#pragma once
#include <string>

enum class phone_type { // типы телефонов
    work, // рабочий
    home, // домашний
    service // служебный
};

class phone {
public:
    phone(); // конструктор по умолчанию

    // конструктор с параметром, 
    phone(const std::string& number, phone_type type = phone_type::home);
    
    // сеттер, приводит строку к верному формату и заполняет приватные поля
    void setnumber(const std::string& raw_number);

    // устанавливает типа телефона
    void settype(phone_type type);

    // возвращает отформатированный номер
    std::string getformatted() const;

    // возвращает исходную строку, которую ввел пользователь
    std::string getraw() const;

    // возвращает типа телефона
    phone_type gettype() const;

    // возвращает true, если номер прошел проверку регулярным выражением
    bool isvalid() const;

    // конвертация типа телефона в строку
    static std::string type_to_string(phone_type type);

    // конвертация строки в тип телефона
    static phone_type string_to_type(const std::string& type_str);

private:
    std::string raw_number; // исходная строка пользователя
    std::string formatted_number; // отформатированная строка
    bool valid; // валиден ли номер
    phone_type type; // тип телефона 
};