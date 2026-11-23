#pragma once
#include <string>
#include <vector>
#include "phone.h"

class contact {
public:
    contact(); // конструктор по умолчанию
    explicit contact(int id);

    static int generateid();

    void setid(int id);
    int getid() const;
    
    // обязательные поля
    void setname(const std::string& name);
    void setsurname(const std::string& surname);
    void setemail(const std::string& email);
    void addphone(const std::string& phone);
    
    // необязательные поля
    void setpatronymic(const std::string& patronymic);
    void setaddress(const std::string& address);
    void setbirthdate(int day, int month, int year);
    
    // геттеры
    std::string getname() const;
    std::string getsurname() const;
    std::string getpatronymic() const;
    std::string getaddress() const;
    std::string getemail() const;
    std::string getbirthdate() const;
    std::vector<phone> getphones() const;
    
    // валидация всех полей контакта
    bool validate() const;
    
    // проверка обязательных полей контакта
    bool hasrequiredfields() const;

private:
    int id;
    std::string name;
    std::string surname;
    std::string patronymic;
    std::string address;
    std::string email;
    int birth_day;
    int birth_month;
    int birth_year;
    std::vector<phone> phones;
};