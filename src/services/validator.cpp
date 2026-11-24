#include "validator.h"
#include <cctype>
#include <chrono>
#include <ctime>
#include <regex>

std::string validator::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

bool validator::checkname(const std::string& name) {
    std::string trimmed = trim(name);
    if (trimmed.empty()) {
        return false;
    }

    // не должен начинаться или заканчиваться дефисом
    if (trimmed.front() == '-' || trimmed.back() == '-') {
        return false;
    }

    // должен начинаться на букву 
    unsigned char first = static_cast<unsigned char>(trimmed.front());
    bool starts_with_letter =
        (first >= 'A' && first <= 'Z') ||
        (first >= 'a' && first <= 'z') ||
        (first >= 128); // поддержка байтов utf-8 

    if (!starts_with_letter) {
        return false;
    }

    // разрешенные символы
    std::regex pattern(R"(^[A-Za-zА-Яа-яЁё0-9\- ]+$)");
    return std::regex_match(trimmed, pattern);
}

bool validator::checkemail(const std::string& email) {
    std::string cleaned = trim(email);
    size_t at_pos = cleaned.find('@');
    if (at_pos == std::string::npos || cleaned.find('@', at_pos + 1) != std::string::npos) {
        return false;
    }

    std::string before_at = trim(cleaned.substr(0, at_pos));
    std::string after_at = trim(cleaned.substr(at_pos + 1));

    if (before_at.empty() || after_at.empty()) {
        return false;
    }

    std::string normalized = before_at + "@" + after_at;

    // имя пользователя и домен — латинские буквы и цифры
    std::regex pattern(R"(^[A-Za-z0-9]+@[A-Za-z0-9]+(\.[A-Za-z0-9]+)+$)");
    return std::regex_match(normalized, pattern);
}

bool validator::checkdate(int day, int month, int year) {
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    if (year < 1900) return false;
    
    int max_days = daysinmonth(month, year);
    if (day > max_days) return false;
    
    // Дата должна быть строго меньше текущей даты
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm{};
#if defined(_WIN32)
    localtime_s(&local_tm, &now_time);
#else
    localtime_r(&now_time, &local_tm);
#endif

    int cur_year = local_tm.tm_year + 1900;
    int cur_month = local_tm.tm_mon + 1;
    int cur_day = local_tm.tm_mday;

    if (year != cur_year) return year < cur_year;
    if (month != cur_month) return month < cur_month;
    return day < cur_day;
}

std::string validator::formatphone(const std::string& phone) {
    // удаляем все кроме цифр и +
    std::string digits;
    for (unsigned char c : phone) {
        if (std::isdigit(c)) { // если цифра - добавляем
            digits.push_back(static_cast<char>(c));
        }
    }
    
    if (digits.size() == 11 && (digits[0] == '7' || digits[0] == '8')) {
        return "+7" + digits.substr(1);
    }
    if (digits.size() == 10) {
        return "+7" + digits;
    }

    return "";
}

bool validator::checkphone(const std::string& phone) {
    // форматы: +78121234567, 88121234567, +7(812)1234567 итд
    // R - raw string чтобы писать ОДИН \ а не два
    // ^ $ - начало и конец строки
    std::regex pattern(R"(^(\+7|8)(\(\d{3}\)|\d{3})(\d{7}|\d{3}-\d{2}-\d{2})$)");
    return std::regex_match(phone, pattern);
}

bool validator::isleapyear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int validator::daysinmonth(int month, int year) {
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isleapyear(year)) return 29;
    return days[month - 1];
}