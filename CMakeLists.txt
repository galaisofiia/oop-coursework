cmake_minimum_required(VERSION 3.16)
project(phonebook LANGUAGES CXX)

cmake_policy(SET CMP0135 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# основная библиотека (без main)
file(GLOB_RECURSE LIB_SOURCES
    src/models/*.cpp
    src/services/*.cpp
    src/storage/*.cpp
)

add_library(phonebook_lib ${LIB_SOURCES})
target_include_directories(phonebook_lib PUBLIC src)

# основной исполняемый файл
add_executable(phonebook src/main.cpp)
target_link_libraries(phonebook PRIVATE phonebook_lib)

include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

target_link_libraries(phonebook_lib PRIVATE nlohmann_json::nlohmann_json)

# тесты
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    # скачиваем catch2
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
    )
    FetchContent_MakeAvailable(Catch2)
    
    # тестовый исполняемый файл
    file(GLOB_RECURSE TEST_SOURCES tests/*.cpp)
    add_executable(phonebook_tests ${TEST_SOURCES})
    target_link_libraries(phonebook_tests PRIVATE phonebook_lib Catch2::Catch2WithMain)
    target_include_directories(phonebook_tests PRIVATE src)
endif()

# cmake -B build -S . -DBUILD_TESTS=ON
# cmake --build build