#include <catch2/catch_test_macros.hpp>
#include "services/validator.h"

TEST_CASE("validator::checkname validates names correctly", "[validator][name]") {
    SECTION("valid names") {
        INFO("Проверка корректности имен");

        std::string n1 = "ivan";
        CAPTURE(n1);
        REQUIRE(validator::checkname(n1));

        std::string n2 = "Ivan";
        CAPTURE(n2);
        REQUIRE(validator::checkname(n2));

        std::string n3 = "ivan-petrov";
        CAPTURE(n3);
        REQUIRE(validator::checkname(n3));

        std::string n4 = "ivan petrov";
        CAPTURE(n4);
        REQUIRE(validator::checkname(n4));

        std::string n5 = "ivan123";
        CAPTURE(n5);
        REQUIRE(validator::checkname(n5));

        std::string n6 = "  ivan  ";
        CAPTURE(n6);
        REQUIRE(validator::checkname(n6));
    }

    SECTION("invalid names") {
        INFO("Проверка некорректных имен");

        std::string empty = "";
        CAPTURE(empty);
        REQUIRE_FALSE(validator::checkname(empty));

        std::string spaces = "   ";
        CAPTURE(spaces);
        REQUIRE_FALSE(validator::checkname(spaces));

        std::string start_dash = "-ivan";
        CAPTURE(start_dash);
        REQUIRE_FALSE(validator::checkname(start_dash));

        std::string end_dash = "ivan-";
        CAPTURE(end_dash);
        REQUIRE_FALSE(validator::checkname(end_dash));

        std::string start_digit = "123ivan";
        CAPTURE(start_digit);
        REQUIRE_FALSE(validator::checkname(start_digit));

        std::string with_at = "ivan@petrov";
        CAPTURE(with_at);
        REQUIRE_FALSE(validator::checkname(with_at));
    }
}

TEST_CASE("validator::checkemail validates emails correctly", "[validator][email]") {
    SECTION("valid emails") {
        INFO("ПРоверка корректных email");

        std::string e1 = "ivan@example.com";
        CAPTURE(e1);
        REQUIRE(validator::checkemail(e1));

        std::string e2 = "ivan123@ex.org";
        CAPTURE(e2);
        REQUIRE(validator::checkemail(e2));

        std::string e3 = "  ivan@example.com  ";
        CAPTURE(e3);
        REQUIRE(validator::checkemail(e3));

        std::string e4 = "ivan @example.com";
        CAPTURE(e4);
        REQUIRE(validator::checkemail(e4));
    }

    SECTION("invalid emails") {
        INFO("Проверка некорректных email");

        std::string empty = "";
        CAPTURE(empty);
        REQUIRE_FALSE(validator::checkemail(empty));

        std::string no_at = "ivan";
        CAPTURE(no_at);
        REQUIRE_FALSE(validator::checkemail(no_at));

        std::string no_user = "@example.com";
        CAPTURE(no_user);
        REQUIRE_FALSE(validator::checkemail(no_user));

        std::string no_domain = "ivan@";
        CAPTURE(no_domain);
        REQUIRE_FALSE(validator::checkemail(no_domain));

        std::string no_tld = "ivan@example";
        CAPTURE(no_tld);
        REQUIRE_FALSE(validator::checkemail(no_tld));

        std::string cyrillic = "иван@example.com";
        CAPTURE(cyrillic);
        REQUIRE_FALSE(validator::checkemail(cyrillic));
    }
}

TEST_CASE("validator::checkphone validates phones correctly", "[validator][phone]") {
    SECTION("valid phones") {
        INFO("Проверка форматов номера телефона");

        std::string p1 = "+78121234567";
        CAPTURE(p1);
        REQUIRE(validator::checkphone(p1));

        std::string p2 = "88121234567";
        CAPTURE(p2);
        REQUIRE(validator::checkphone(p2));

        std::string p3 = "+7(812)1234567";
        CAPTURE(p3);
        REQUIRE(validator::checkphone(p3));

        std::string p4 = "8(812)1234567";
        CAPTURE(p4);
        REQUIRE(validator::checkphone(p4));

        std::string p5 = "+7(812)123-45-67";
        CAPTURE(p5);
        REQUIRE(validator::checkphone(p5));

        std::string p6 = "8(812)123-45-67";
        CAPTURE(p6);
        REQUIRE(validator::checkphone(p6));
    }

    SECTION("invalid phones") {
        INFO("Проверка некорректных номеров телефона");

        std::string empty = "";
        CAPTURE(empty);
        REQUIRE_FALSE(validator::checkphone(empty));

        std::string too_short = "123";
        CAPTURE(too_short);
        REQUIRE_FALSE(validator::checkphone(too_short));

        std::string wrong_country = "+68121234567";
        CAPTURE(wrong_country);
        REQUIRE_FALSE(validator::checkphone(wrong_country));

        std::string missing_digit = "+7812123456";
        CAPTURE(missing_digit);
        REQUIRE_FALSE(validator::checkphone(missing_digit));
    }
}

TEST_CASE("validator::formatphone formats phones correctly", "[validator][phone]") {
    INFO("Проверка приведения номера телефона к единому формату");

    std::string in1 = "+78121234567";
    std::string out1 = validator::formatphone(in1);
    CAPTURE(in1);
    CAPTURE(out1);
    REQUIRE(out1 == "+78121234567");

    std::string in2 = "88121234567";
    std::string out2 = validator::formatphone(in2);
    CAPTURE(in2);
    CAPTURE(out2);
    REQUIRE(out2 == "+78121234567");

    std::string in3 = "+7(812)123-45-67";
    std::string out3 = validator::formatphone(in3);
    CAPTURE(in3);
    CAPTURE(out3);
    REQUIRE(out3 == "+78121234567");

    std::string in4 = "8(812)123-45-67";
    std::string out4 = validator::formatphone(in4);
    CAPTURE(in4);
    CAPTURE(out4);
    REQUIRE(out4 == "+78121234567");
}

TEST_CASE("validator::checkdate validates dates correctly", "[validator][date]") {
    SECTION("valid dates") {
        INFO("Проверка корректных дат");

        REQUIRE(validator::checkdate(1, 1, 2000));
        REQUIRE(validator::checkdate(31, 12, 2020));
        REQUIRE(validator::checkdate(29, 2, 2020));
        REQUIRE(validator::checkdate(28, 2, 2021));
    }

    SECTION("invalid dates") {
        INFO("Проверка некорректных дат");

        SECTION("День вне диапазона") {
            REQUIRE_FALSE(validator::checkdate(0, 1, 2000));
            REQUIRE_FALSE(validator::checkdate(32, 1, 2000));
        }

        SECTION("Месяц вне диапазона") {
            REQUIRE_FALSE(validator::checkdate(1, 0, 2000));
            REQUIRE_FALSE(validator::checkdate(1, 13, 2000));
        }

        SECTION("Високосный год") {
            INFO("2021 не високосный, 29 февраля нет");
            REQUIRE_FALSE(validator::checkdate(29, 2, 2021));
        }

        SECTION("Дней в месяце меньше введенного") {
            INFO("в апреле 30 дней");
            REQUIRE_FALSE(validator::checkdate(31, 4, 2000));
        }

        SECTION("Год вне диапазона") {
            REQUIRE_FALSE(validator::checkdate(1, 1, 1899));
            REQUIRE_FALSE(validator::checkdate(1, 1, 2101));
        }
    }
}