#include <catch2/catch_test_macros.hpp>
#include "storage/filestorage.h"
#include "models/contact.h"
#include <filesystem>

TEST_CASE("filestorage saves and loads contacts", "[storage]") {
    // файл, на котором будем проверять тесты
    std::string test_file = "test_contacts.json";
    // удаляем файл прошлых тестов, если остался
    std::filesystem::remove(test_file);
    
    filestorage storage(test_file);
    
    SECTION("add and load single contact") {
        contact c(contact::generateid());
        c.setname("ivan");
        c.setsurname("petrov");
        c.setemail("ivan@test.com");
        c.addphone("+78121234567");
        
        REQUIRE(storage.add(c));
        
        auto loaded = storage.load();
        REQUIRE(loaded.size() == 1);
        REQUIRE(loaded[0].getid() == c.getid());
        REQUIRE(loaded[0].getemail() == "ivan@test.com");
    }
    
    SECTION("remove contact by id") {
        contact c1(contact::generateid());
        c1.setname("ivan");
        c1.setsurname("petrov");
        c1.setemail("ivan@test.com");
        c1.addphone("+78121234567");
        
        contact c2(contact::generateid());
        c2.setname("anna");
        c2.setsurname("sidorova");
        c2.setemail("anna@test.com");
        c2.addphone("+78129876543");
        
        storage.add(c1);
        storage.add(c2);
        
        int id_to_remove = c1.getid();
        REQUIRE(storage.remove(id_to_remove));
        
        auto loaded = storage.load();
        REQUIRE(loaded.size() == 1);
        REQUIRE(loaded[0].getid() == c2.getid());
    }
    
    std::filesystem::remove(test_file);
}