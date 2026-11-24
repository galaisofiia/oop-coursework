#include <iostream>
#include <string>
#include <memory>
#include <optional>
#include "models/contact.h"
#include "services/contactmanager.h"
#include "storage/filestorage.h"

void printcontact(const contact& c) {
    std::cout << "id: " << c.getid() << "\n";
    std::cout << "имя: " << c.getname() << "\n";
    std::cout << "фамилия: " << c.getsurname() << "\n";
    std::cout << "отчество: " << c.getpatronymic() << "\n";
    std::cout << "email: " << c.getemail() << "\n";
    std::cout << "адрес: " << c.getaddress() << "\n";
    std::cout << "дата рождения: " << c.getbirthdate() << "\n";
    std::cout << "телефоны:\n";
    for (const auto& p : c.getphones()) {
        std::cout << "  - " << p.getformatted() << "\n";
    }
    std::cout << "\n";
}

std::string getinput(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

int main() {
    auto storage = std::make_shared<filestorage>("contacts.json");
    contactmanager manager(storage);

    while (true) {
        std::cout << "Меню:\n";
        std::cout << "1. Показать все контакты\n";
        std::cout << "2. Добавить контакт\n";
        std::cout << "3. Редактировать контакт\n";
        std::cout << "4. Удалить контакт\n";
        std::cout << "5. Поиск\n";
        std::cout << "6. Сортировка\n";
        std::cout << "7. Выход\n";
        std::cout << "Выбор: ";
        
        std::string choice_str;
        std::getline(std::cin, choice_str);

        int choice = 0;
        try {
            choice = std::stoi(choice_str);
        } catch (const std::invalid_argument&) {
            std::cout << "Неверный ввод. Введите число.\n\n";
            continue;
        } catch (const std::out_of_range&) {
            std::cout << "Число слишком большое!\n\n";
            continue;
        }

        switch (choice) {
            case 1: {
                auto contacts = manager.getallcontacts();
                if (contacts.empty()) {
                    std::cout << "Список пуст\n\n";
                } else {
                    for (const auto& c : contacts) {
                        printcontact(c);
                    }
                }
                break;
            }
            case 2: {
                contact c;
                c.setname(getinput("Имя: "));
                c.setsurname(getinput("Фамилия: "));
                c.setemail(getinput("E-mail: "));
                c.addphone(getinput("Телефон: "));
                c.setpatronymic(getinput("Отчество (необязательно): "));
                c.setaddress(getinput("Адрес (необязательно): "));
                
                std::string bday = getinput("Дата рождения дд.мм.гггг (необязательно): ");
                if (!bday.empty()) {
                    size_t p1 = bday.find('.');
                    size_t p2 = bday.find('.', p1 + 1);
                    if (p1 != std::string::npos && p2 != std::string::npos) {
                        int d = std::stoi(bday.substr(0, p1));
                        int m = std::stoi(bday.substr(p1 + 1, p2 - p1 - 1));
                        int y = std::stoi(bday.substr(p2 + 1));
                        c.setbirthdate(d, m, y);
                    }
                }

                if (manager.addcontact(c)) {
                    std::cout << "Контакт добавлен.\n\n";
                } else {
                    std::cout << "Ошибка валидации. Проверьте данные.\n\n";
                }
                break;
            }
            case 3: {
                int id = 0;
                try {
                    id = std::stoi(getinput("Введите id контакта для редактирования: "));
                } catch (...) {
                    std::cout << "Неверный id!\n\n";
                    continue;
                }

                auto opt_c = manager.getcontactbyid(id);
                if (!opt_c) {
                    std::cout << "Контакт не найден!\n\n";
                    continue;
                }
                
                contact c = *opt_c;
                std::cout << "Оставьте поле пустым, чтобы не менять его\n";
                
                std::string name = getinput("Новое имя [" + c.getname() + "]: ");
                if (!name.empty()) c.setname(name);
                
                std::string surname = getinput("Новая фамилия [" + c.getsurname() + "]: ");
                if (!surname.empty()) c.setsurname(surname);
                
                std::string email = getinput("Новый e-mail [" + c.getemail() + "]: ");
                if (!email.empty()) c.setemail(email);
                
                std::string phone = getinput("Новый телефон [" + (c.getphones().empty() ? "" : c.getphones()[0].getformatted()) + "]: ");
                if (!phone.empty()) {
                    c = contact(c.getid());
                    c.setname(name.empty() ? opt_c->getname() : name);
                    c.setsurname(surname.empty() ? opt_c->getsurname() : surname);
                    c.setemail(email.empty() ? opt_c->getemail() : email);
                    c.addphone(phone);
                    c.setpatronymic(opt_c->getpatronymic());
                    c.setaddress(opt_c->getaddress());
                }

                if (manager.updatecontact(c)) {
                    std::cout << "Контакт обновлен.\n\n";
                } else {
                    std::cout << "Ошибка валидации при обновлении.\n\n";
                }
                break;
            }
            case 4: {
                int id = 0;
                try {
                    id = std::stoi(getinput("Введите id контакта для удаления: "));
                } catch (...) {
                    std::cout << "Неверный id!\n\n";
                    continue;
                }

                if (manager.removecontact(id)) {
                    std::cout << "Контакт удален.\n\n";
                } else {
                    std::cout << "Контакт не найден!\n\n";
                }
                break;
            }
            case 5: {
                std::cout << "1. По имени\n2. По email\n3. По телефону\nВыбор: ";
                std::string search_choice;
                std::getline(std::cin, search_choice);
                std::string query = getinput("Поисковый запрос: ");
                
                std::vector<contact> results;
                if (search_choice == "1") results = manager.searchbyname(query);
                else if (search_choice == "2") results = manager.searchbyemail(query);
                else if (search_choice == "3") results = manager.searchbyphone(query);
                
                if (results.empty()) {
                    std::cout << "Ничего не найдено.\n\n";
                } else {
                    for (const auto& c : results) {
                        printcontact(c);
                    }
                }
                break;
            }
            case 6: {
                std::cout << "1. По фамилии\n2. По email\n3. По телефону\nВыбор: ";
                std::string sort_choice;
                std::getline(std::cin, sort_choice);
                
                if (sort_choice == "1") manager.sortbyname();
                else if (sort_choice == "2") manager.sortbyemail();
                else if (sort_choice == "3") manager.sortbyphone();
                
                std::cout << "Сортировка применена. Используйте пункт 1 для просмотра.\n\n";
                break;
            }
            case 7: {
                std::cout << "Выход из программы...\n";
                return 0;
            }
            default: {
                std::cout << "Неверный выбор. Попробуйте снова.\n\n";
                break;
            }
        }
    }
}