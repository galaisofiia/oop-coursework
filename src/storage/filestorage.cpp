#include "filestorage.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <algorithm>

using json = nlohmann::json;

filestorage::filestorage(const std::string& filepath) : filepath(filepath) {}

bool filestorage::save(const std::vector<contact>& contacts) {
    json j; // создаем json-объект, где потом будут контакты
    // проход по всем контактам
    for (const auto& c : contacts) {
        json contact_json; // один контакт json 
        contact_json["id"] = c.getid();
        contact_json["name"] = c.getname();
        contact_json["surname"] = c.getsurname();
        contact_json["patronymic"] = c.getpatronymic();
        contact_json["address"] = c.getaddress();
        contact_json["email"] = c.getemail();
        contact_json["birthdate"] = c.getbirthdate();
        
        // массив номеров для контакта
        json phones_json = json::array();
        for (const auto& p : c.getphones()) {
            phones_json.push_back(p.getformatted());
        }
        contact_json["phones"] = phones_json;
        
        j.push_back(contact_json);
    }
    
    // выходной файловый поток out, открывающий filepath
    std::ofstream out(filepath);
    if (!out.is_open()) {
        return false;
    }
    // делаем json строкой текста с отступом в 4 проблема (для красоты)
    out << j.dump(4);
    return true;
}

std::vector<contact> filestorage::load() {
    // вектор для восстановленных из файла контактов
    std::vector<contact> contacts;
    //открытие filepath для чтения
    std::ifstream in(filepath);
    if (!in.is_open()) { return contacts; }
    
    try {
        json j;
        in >> j; // считываем текст из файла
        // перебираем контакы
        for (const auto& item : j) {
            //создаем контакт объектом и передаем ему данные из json
            contact c(item.value("id", 0));
            c.setname(item.value("name", ""));
            c.setsurname(item.value("surname", ""));
            c.setpatronymic(item.value("patronymic", ""));
            c.setaddress(item.value("address", ""));
            c.setemail(item.value("email", ""));
            
            std::string bdate = item.value("birthdate", "");
            if (!bdate.empty() && bdate != "0.0.0") {
                size_t p1 = bdate.find('.');
                size_t p2 = (p1 != std::string::npos) ? bdate.find('.', p1 + 1) : std::string::npos;
                if (p1 != std::string::npos && p2 != std::string::npos) {
                    int d = std::stoi(bdate.substr(0, p1));
                    int m = std::stoi(bdate.substr(p1 + 1, p2 - p1 - 1));
                    int y = std::stoi(bdate.substr(p2 + 1));
                    c.setbirthdate(d, m, y);
                }
            }
            
            if (item.contains("phones") && item["phones"].is_array()) {
                for (const auto& p : item["phones"]) {
                    c.addphone(p.get<std::string>());
                }
            }
            contacts.push_back(c);
        }
    } catch (const std::exception& e) {
        std::cerr << "error loading json: " << e.what() << std::endl;
    }
    
    return contacts;
}

bool filestorage::add(const contact& c) {
    auto contacts = load(); // загрузка текущих контактов
    contacts.push_back(c);
    return save(contacts);
}

bool filestorage::remove(int id) {
    auto contacts = load();
    // если id совпадает - сдвигаем в конец
    // [id] - список захвата (чтобы id был виден внутри лямбда функции)
    // (const contact& c) - параметр, передаваемый в функцию
    auto it = std::remove_if(contacts.begin(), contacts.end(), 
        [id](const contact& c) { return c.getid() == id; });
    
    // если найден хоть один контакт с таким айди - удаляем
    if (it != contacts.end()) {
        contacts.erase(it, contacts.end());
        return save(contacts);
    }
    return false;
}

bool filestorage::update(const contact& c) {
    auto contacts = load();
    for (auto& item : contacts) {
        // если нашли контакт с тем же id X
        if (item.getid() == c.getid()) {
            item = c; // обновляем
            return save(contacts);
        }
    }
    return false;
}