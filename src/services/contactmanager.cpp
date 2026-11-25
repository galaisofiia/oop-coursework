#include "contactmanager.h"
#include <algorithm>

contactmanager::contactmanager(std::shared_ptr<istorage> storage) 
    : storage(storage) {
    loadfromstorage();
}

bool contactmanager::addcontact(const contact& c) {
    if (!c.hasrequiredfields() || !c.validate()) {
        return false;
    }
    
    contact new_contact = c;
    if (new_contact.getid() == 0) {
        new_contact.setid(contact::generateid());
    }
    
    currentlist.push_back(new_contact);
    savetostorage();
    return true;
}

bool contactmanager::removecontact(int id) {
    // если id совпадает - сдвигаем в конец
    // [id] - список захвата (чтобы id был виден внутри лямбда функции)
    // (const contact& c) - параметр, передаваемый в функцию
    auto it = std::remove_if(currentlist.begin(), currentlist.end(),
        [id](const contact& c) { return c.getid() == id; });
    
    if (it != currentlist.end()) {
        currentlist.erase(it, currentlist.end());
        savetostorage();
        return true;
    }
    return false;
}

bool contactmanager::updatecontact(const contact& c) {
    if (!c.validate()) { return false; }
    
    for (auto& existing : currentlist) {
        if (existing.getid() == c.getid()) {
            existing = c;
            savetostorage();
            return true;
        }
    }
    return false;
}

std::vector<contact> contactmanager::getallcontacts() const { return currentlist; }
std::vector<contact> contactmanager::getcurrentlist() const { return currentlist; }

std::optional<contact> contactmanager::getcontactbyid(int id) const {
    for (const auto& c : currentlist) {
        if (c.getid() == id) {
            return c;
        }
    }
    return std::nullopt;
}

std::vector<contact> contactmanager::searchbyname(const std::string& name) const {
    // пустой вектор для результатов
    std::vector<contact> result;

    // делаем копию поискового запроса, чтобы не менять оригинал
    std::string lowername = name;

    // поиск без учета регистра
    std::transform(lowername.begin(), lowername.end(), lowername.begin(), ::tolower);
    
    for (const auto& c : currentlist) {
        std::string cname = c.getname();
        // преревод в нижний регистр
        std::transform(cname.begin(), cname.end(), cname.begin(), ::tolower);
        std::string csurname = c.getsurname();
        std::transform(csurname.begin(), csurname.end(), csurname.begin(), ::tolower);
        
        if (cname.find(lowername) != std::string::npos || 
            csurname.find(lowername) != std::string::npos) {
            result.push_back(c);
        }
    }
    return result;
}

std::vector<contact> contactmanager::searchbyemail(const std::string& email) const {
    std::vector<contact> result;
    std::string loweremail = email;
    std::transform(loweremail.begin(), loweremail.end(), loweremail.begin(), ::tolower);
    
    for (const auto& c : currentlist) {
        std::string cemail = c.getemail();
        std::transform(cemail.begin(), cemail.end(), cemail.begin(), ::tolower);
        
        if (cemail.find(loweremail) != std::string::npos) {
            result.push_back(c);
        }
    }
    return result;
}

std::vector<contact> contactmanager::searchbyphone(const std::string& phone) const {
    std::vector<contact> result;
    
    for (const auto& c : currentlist) {
        for (const auto& p : c.getphones()) {
            if (p.getformatted().find(phone) != std::string::npos) {
                result.push_back(c);
                break;
            }
        }
    }
    return result;
}

void contactmanager::sortbyname() {
    std::sort(currentlist.begin(), currentlist.end(),
        [](const contact& a, const contact& b) {
            return a.getsurname() < b.getsurname();
        });
}

void contactmanager::sortbyemail() {
    std::sort(currentlist.begin(), currentlist.end(),
        [](const contact& a, const contact& b) {
            return a.getemail() < b.getemail();
        });
}

void contactmanager::sortbyphone() {
    std::sort(currentlist.begin(), currentlist.end(),
    // [] - лямбда не захватывает никакие переменные извне
    // const contact& a, const contact& b - принимаемые параметры
        [](const contact& a, const contact& b) {
            auto phonesa = a.getphones();
            auto phonesb = b.getphones();
            if (phonesa.empty() && phonesb.empty()) return false;
            if (phonesa.empty()) return true;
            if (phonesb.empty()) return false;
            return phonesa[0].getformatted() < phonesb[0].getformatted();
        });
}

void contactmanager::loadfromstorage() { currentlist = storage->load(); }
void contactmanager::savetostorage() { storage->save(currentlist); }