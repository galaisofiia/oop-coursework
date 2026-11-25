#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <optional>
#include <vector>
#include "../models/contact.h"
#include "../models/phone.h"

// окно для создания/редактирования контакта
class contactform : public QDialog {
    Q_OBJECT // обязательный макрос qt
public:
    // конструктор. std::optional<contact> - если передать существующий
    // контакт - режим редактирования, nullopt - создание нового контакта
    explicit contactform(std::optional<contact> existing_contact = std::nullopt, QWidget *parent = nullptr);
    // возвращает собранный и проверенный контакт
    contact get_contact() const;

// слоты - методы, вызывающиеся автоматически в ответ на сигнал
private slots: 
    // кнопка "сохранить"
    void on_accepted();
    // кнопка "добавить телефон"
    void add_phone_field();

private: // поля ввода для характеристик контакта
    QLineEdit *name_edit;
    QLineEdit *surname_edit;
    QLineEdit *email_edit;
    QLineEdit *patronymic_edit;
    QLineEdit *address_edit;
    QLineEdit *birthdate_edit;

    // контейнер для кнопок 
    QDialogButtonBox *button_box;
    // вертикальный компоновщик
    QVBoxLayout *phones_layout;
    // кнопка "добавить телефон", вызывает add_phone_field
    QPushButton *add_phone_btn;
    // динамический массив пар указателей на поле ввода номера и
    // ссылку на выпадающий список номера телефона
    std::vector<std::pair<QLineEdit*, QComboBox*>> phone_inputs;
    // исходные данные контакта (нужно при редактировании)
    std::optional<contact> current_contact;

    // отрисовка всех элементов окна
    void setup_ui();
};