#include <catch2/catch_test_macros.hpp>
#include "storage/dbstorage.h"
#include "models/contact.h"

TEST_CASE("dbstorage works correctly with postgresql", "[dbstorage]") {
    // инициализация хранилища
    dbstorage storage("localhost", "testdb", "testuser", "testpass");

    // очищаем базу перед каждым запуском секции
    // save с пустым вектором просто удаляет всё из таблиц
    storage.save(std::vector<contact>{});

    SECTION("add and load contact") {
        contact c;
        c.setname("ivan");
        c.setsurname("petrov");
        c.setemail("ivan@test.com");
        c.addphone("+78121234567");

        REQUIRE(storage.add(c));

        auto loaded = storage.load();
        REQUIRE(loaded.size() == 1);
        REQUIRE(loaded[0].getname() == "ivan");
    }

    SECTION("remove contact") {
        contact c; // id = 0
        c.setname("anna");
        c.setsurname("sidorova");
        c.setemail("anna@test.com");
        c.addphone("+78129876543");

        storage.add(c); // какой-то другой id в sql
        
        // получаем реальный id, который присвоила база данных
        auto loaded = storage.load();
        REQUIRE(loaded.size() == 1);
        int real_id = loaded[0].getid();

        // удаляем по настоящему id из storage
        REQUIRE(storage.remove(real_id));

        auto loaded_after = storage.load();
        REQUIRE(loaded_after.size() == 0);
    }

    SECTION("update contact") {
        contact c;
        c.setname("boris");
        c.setsurname("ivanov");
        c.setemail("boris@test.com");
        c.addphone("+78121112233");

        storage.add(c);

        // получаем реальный айди из базы
        auto loaded = storage.load();
        REQUIRE(loaded.size() == 1);
        int real_id = loaded[0].getid();
        
        c.setid(real_id); 
        c.setname("boris updated");
        
        // обновляем по настоящему id из storage
        REQUIRE(storage.update(c));

        auto loaded_updated = storage.load();
        REQUIRE(loaded_updated.size() == 1);
        REQUIRE(loaded_updated[0].getname() == "boris updated");
    }
}