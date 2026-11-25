#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <memory>
#include "../services/contactmanager.h"

// главное окно qt
class mainwindow : public QMainWindow {
    Q_OBJECT // обязательный макрос qt
public:
    explicit mainwindow(std::shared_ptr<contactmanager> manager, QWidget *parent = nullptr);

private slots:
    // обновление данных в таблице
    void refresh_table();

    //добавление контакта
    void add_contact();

    // редактирование контакта
    void edit_contact();

    // удаление контакта
    void delete_contact();

    // фильтрация контактов по введеному текстовому запросу
    void search_contacts();

    // очистка поля поиска и возврат исходного списка контакта
    void reset_search();

    // сортировка контактов по определенному критерию
    void sort_contacts();

    // окно с информацией о контакте
    void show_details();

private:
    // умный указатель на класс связи интерфейса и хранилища
    std::shared_ptr<contactmanager> manager;

    // виджет таблицы для отображения списка контактов
    QTableWidget *table;

    // поле ввода для поиска
    QLineEdit *search_input;

    // выпадающий список для выбора поля поиска (имя, емейл или телефон)
    QComboBox *search_type;

    // выпадающий список для выбора поля сортировки
    QComboBox *sort_type;

    // создание и компоновка всех визульных элементов окна
    void setup_ui();
};