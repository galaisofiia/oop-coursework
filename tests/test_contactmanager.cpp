#include <catch2/catch_test_macros.hpp>
#include "services/contactmanager.h"
#include "storage/filestorage.h"
#include <filesystem>

TEST_CASE("contactmanager manages contacts correctly", "[manager]") {
    std::string test_file = "test_manager_contacts.json";
    std::filesystem::remove(test_file);
    
    auto storage = std::make_shared<filestorage>(test_file);
    contactmanager manager(storage);
    
    SECTION("add valid contact assigns id") {
        contact c;
        c.setname("ivan");
        c.setsurname("petrov");
        c.setemail("ivan@test.com");
        c.addphone("+78121234567");
        
        REQUIRE(manager.addcontact(c));
        REQUIRE(manager.getallcontacts().size() == 1);
        REQUIRE(manager.getallcontacts()[0].getid() > 0);
    }
    
    SECTION("add contact with duplicate email is allowed") {
        contact c1;
        c1.setid(1);
        c1.setname("ivan");
        c1.setsurname("petrov");
        c1.setemail("same@test.com");
        c1.addphone("+78121234567");
        
        contact c2;
        c2.setid(2);
        c2.setname("anna");
        c2.setsurname("sidorova");
        c2.setemail("same@test.com");
        c2.addphone("+78129876543");
        
        REQUIRE(manager.addcontact(c1));
        REQUIRE(manager.addcontact(c2));
        REQUIRE(manager.getallcontacts().size() == 2);
    }
    
    SECTION("remove contact by id") {
        contact c;
        c.setid(42);
        c.setname("ivan");
        c.setsurname("petrov");
        c.setemail("ivan@test.com");
        c.addphone("+78121234567");
        
        manager.addcontact(c);
        REQUIRE(manager.removecontact(42));
        REQUIRE(manager.getallcontacts().empty());
    }
    
    SECTION("update contact by id") {
        //  создаем 1й контакт в памяти и добавляем в базу
        contact c1;
        c1.setid(10);
        c1.setname("ivan");
        c1.setsurname("petrov");
        c1.setemail("ivan@test.com");
        c1.addphone("+78121234567");
        manager.addcontact(c1);

        // второй контакт - измененный 1й контакт
        contact c2;
        c2.setid(10);
        c2.setname("ivan updated");
        c2.setsurname("petrov");
        c2.setemail("ivan@test.com");
        c2.addphone("+78121234567");

        // обновляем контакт (метод найдет id=10, и заменит в нем данные)
        REQUIRE(manager.updatecontact(c2));
        // весь список контактов для проверки
        auto updated_list = manager.getallcontacts();
        REQUIRE(updated_list.size() == 1);
        REQUIRE(updated_list[0].getname() == "ivan updated");
    }
    
    std::filesystem::remove(test_file);
}