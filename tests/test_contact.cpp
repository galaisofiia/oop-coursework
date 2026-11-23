#include <catch2/catch_test_macros.hpp>
#include "models/contact.h"

TEST_CASE("contact class works correctly", "[contact]") {
    SECTION("valid contact with required fields") {
        INFO("Минимальный валидный контакт");
        contact c;
        c.setname("ivan");
        c.setsurname("petrov");
        c.setemail("ivan@example.com");
        c.addphone("+78121234567");

        CAPTURE(c.getname());
        CAPTURE(c.getsurname());
        CAPTURE(c.getemail());
        REQUIRE(c.hasrequiredfields());
        REQUIRE(c.validate());
        REQUIRE(c.getphones().size() == 1);
    }

    SECTION("contact with all fields") {
        INFO("Полный контакт со всеми полями");
        contact c;
        c.setname("ivan");
        c.setsurname("petrov");
        c.setpatronymic("ivanovich");
        c.setaddress("moscow");
        c.setemail("ivan@example.com");
        c.addphone("+78121234567");
        c.addphone("88129876543");
        c.setbirthdate(15, 6, 1990);

        CAPTURE(c.getpatronymic());
        CAPTURE(c.getbirthdate());
        REQUIRE(c.validate());
        REQUIRE(c.getphones().size() == 2);
        REQUIRE(c.getbirthdate() == "15.6.1990");
    }

    SECTION("invalid contact - missing required fields") {
        INFO("Проверка отсутствия обязательных полей");
        contact c;
        c.setname("ivan");

        REQUIRE_FALSE(c.hasrequiredfields());
        REQUIRE_FALSE(c.validate());
    }

    SECTION("invalid contact - invalid email") {
        INFO("Проверка невалидного email");
        contact c;
        c.setname("ivan");
        c.setsurname("petrov");
        c.setemail("invalid-email");
        c.addphone("+78121234567");

        CAPTURE(c.getemail());
        REQUIRE(c.hasrequiredfields());
        REQUIRE_FALSE(c.validate());
    }

    SECTION("invalid contact - invalid phone") {
        INFO("Проверка невалидного телефона");
        contact c;
        c.setname("ivan");
        c.setsurname("petrov");
        c.setemail("ivan@example.com");
        c.addphone("123");

        CAPTURE(c.getphones()[0].getraw());
        REQUIRE(c.hasrequiredfields());
        REQUIRE_FALSE(c.validate());
    }

    SECTION("invalid contact - invalid birth date") {
        INFO("Проверка невалидной даты рождения");
        contact c;
        c.setname("ivan");
        c.setsurname("petrov");
        c.setemail("ivan@example.com");
        c.addphone("+78121234567");
        c.setbirthdate(29, 2, 2021);

        CAPTURE(c.getbirthdate());
        REQUIRE(c.hasrequiredfields());
        REQUIRE_FALSE(c.validate());
    }
}