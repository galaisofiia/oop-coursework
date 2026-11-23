#pragma once
#include "istorage.h"
#include <string>

class filestorage : public istorage {
public:
    explicit filestorage(const std::string& filepath);
    
    // сохранение всех контактов
    bool save(const std::vector<contact>& contacts) override;
    
    // загрузка всех контактов
    std::vector<contact> load() override;
    
    // добавление одного контакта
    bool add(const contact& c) override;
    
    // удаление контакта по email
    bool remove(int id) override;
    
    // обновление существующего контакта
    bool update(const contact& c) override;

private:
    std::string filepath; // путь к файлу внутри себя
};