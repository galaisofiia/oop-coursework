#pragma once
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include "../models/contact.h"
#include "../storage/istorage.h"

// класс связи интерфейса и хранилища данных
class contactmanager {
public:
    // shared_ptr<istorage> - внедрение зависимости (указатель на хранилище извне)
    explicit contactmanager(std::shared_ptr<istorage> storage);
    
    bool addcontact(const contact& c);
    bool removecontact(int id);
    bool updatecontact(const contact& c);

    std::vector<contact> getallcontacts() const;
    std::optional<contact> getcontactbyid(int id) const;
    // геттер текущего списка конткатов (отсортированного или нет)
    std::vector<contact> getcurrentlist() const;
    
    std::vector<contact> searchbyname(const std::string& name) const;
    std::vector<contact> searchbyemail(const std::string& email) const;
    std::vector<contact> searchbyphone(const std::string& phone) const;
    
    void sortbyname();
    void sortbyemail();
    void sortbyphone();

private:
    std::shared_ptr<istorage> storage;

    // кэш контактов в памяти
    std::vector<contact> currentlist;
    
    void loadfromstorage();
    void savetostorage();
};