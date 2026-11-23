// src/main.cpp
#include <iostream>

int main() {
    std::cout << "phonebook application started" << std::endl;
    // здесь будет консольный интерфейс для задачи 1
    return 0;
}

// все тесты
// ./build/phonebook_tests
// ./build/phonebook_tests -s

// только определенные тесты
// ./build/phonebook_tests "[validator]" -s
// ./build/phonebook_tests "[phone]" -s
// ./build/phonebook_tests "[contact]" -s
// ./build/phonebook_tests "[storage]" -s

// список тестов
// ./build/phonebook_tests --list-tests