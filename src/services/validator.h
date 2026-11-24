#pragma once
#include <string>

class validator {
public:
    // проверка имени/фамилии/отчества
    static bool checkname(const std::string& name);
    
    // проверка email
    static bool checkemail(const std::string& email);
    
    // проверка даты рождения
    static bool checkdate(int day, int month, int year);
    
    // форматирование телефона к единому виду +7XXXXXXXXXX
    static std::string formatphone(const std::string& phone);
    
    // проверка формата телефона
    static bool checkphone(const std::string& phone);

private:
    // проверка високосного года
    static bool isleapyear(int year);
    
    // количество дней в месяце
    static int daysinmonth(int month, int year);

    // удаление незначащих пробелов
    static std::string trim(const std::string& s);
};