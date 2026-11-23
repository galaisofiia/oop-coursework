#pragma once
#include <string>

class phone {
public:
    phone(); // конструктор по умолчанию

    // конструктор с параметром, 
    explicit phone(const std::string& raw_number);
    
    // сеттер, приводит строку к верному формату и заполняет приватные поля
    void setnumber(const std::string& raw_number);

    // возвращает отформатированный номер
    std::string getformatted() const;

    // возвращает исходную строку, которую ввел пользователь
    std::string getraw() const;

    // возвращает true, если номер прошел проверку регулярным выражением
    bool isvalid() const;

private:
    std::string raw_number; // исходная строка пользователя
    std::string formatted_number; // отформатированная строка
    bool valid; // валиден ли номер
};