cmake_minimum_required(VERSION 3.16)
project(phonebook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# qt настройки
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Sql REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Catch2 3 REQUIRED)

# основная библиотека
file(GLOB_RECURSE LIB_SOURCES
    src/models/*.cpp
    src/services/*.cpp
    src/storage/*.cpp
)

add_library(phonebook_lib ${LIB_SOURCES})
target_include_directories(phonebook_lib PUBLIC src)
target_link_libraries(phonebook_lib PUBLIC nlohmann_json::nlohmann_json Qt5::Core Qt5::Sql Qt5::Widgets)

# основной исполняемый файл
add_executable(phonebook src/main.cpp src/ui/mainwindow.cpp src/ui/contactform.cpp src/ui/contactdetails.cpp)
target_link_libraries(phonebook PRIVATE phonebook_lib)

# тесты
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    file(GLOB_RECURSE TEST_SOURCES tests/*.cpp)
    add_executable(phonebook_tests ${TEST_SOURCES})
    target_link_libraries(phonebook_tests PRIVATE phonebook_lib Catch2::Catch2WithMain)
    target_include_directories(phonebook_tests PRIVATE src)
endif()