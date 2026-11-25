#pragma once
#include <vector>
#include <string>
#include "../models/contact.h"

// абстрактный интерфейс хранилищ данных
class istorage {
public:
    virtual ~istorage() = default; // виртуальный деструктор

    // сохранение всех контактов
    virtual bool save(const std::vector<contact>& contacts) = 0;

    // загрузка всех контактов
    virtual std::vector<contact> load() = 0;

    // добавление одного контакта
    virtual bool add(const contact& c) = 0;

    // удаление контакта по email
    virtual bool remove(int id) = 0;

    // обновление существующего контакта
    virtual bool update(const contact& c) = 0;
};