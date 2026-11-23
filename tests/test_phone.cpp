#include <catch2/catch_test_macros.hpp>
#include "models/phone.h"

TEST_CASE("phone class works correctly", "[phone]") {
    SECTION("valid phone") {
        INFO("Создаем валидный телефон");
        phone p("+78121234567");
        CAPTURE(p.getraw());
        CAPTURE(p.getformatted());
        REQUIRE(p.isvalid());
        REQUIRE(p.getformatted() == "+78121234567");
        REQUIRE(p.getraw() == "+78121234567");
    }

    SECTION("phone with formatting") {
        INFO("Телефон со скобками и дефисами");
        phone p("8(812)123-45-67");
        CAPTURE(p.getraw());
        CAPTURE(p.getformatted());
        REQUIRE(p.isvalid());
        REQUIRE(p.getformatted() == "+78121234567");
    }

    SECTION("invalid phone") {
        INFO("Некорректный номер");
        phone p("123");
        CAPTURE(p.getraw());
        REQUIRE_FALSE(p.isvalid());
        REQUIRE(p.getformatted().empty());
    }

    SECTION("setnumber changes phone") {
        INFO("Проверяем смену номера");
        phone p;
        REQUIRE_FALSE(p.isvalid());

        p.setnumber("+78121234567");
        CAPTURE(p.getformatted());
        REQUIRE(p.isvalid());
        REQUIRE(p.getformatted() == "+78121234567");
    }
}