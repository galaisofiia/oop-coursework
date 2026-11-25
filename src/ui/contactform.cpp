#include "contactform.h"
#include "../services/validator.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QStringList>
#include <QLabel>

contactform::contactform(std::optional<contact> existing_contact, QWidget *parent)
    : QDialog(parent), current_contact(existing_contact) {
    setup_ui();
    
    // режим редактирования
    if (current_contact.has_value()) {
        const contact& c = current_contact.value();
        name_edit->setText(QString::fromStdString(c.getname()));
        surname_edit->setText(QString::fromStdString(c.getsurname()));
        email_edit->setText(QString::fromStdString(c.getemail()));
        patronymic_edit->setText(QString::fromStdString(c.getpatronymic()));
        address_edit->setText(QString::fromStdString(c.getaddress()));
        birthdate_edit->setText(QString::fromStdString(c.getbirthdate()));
        
        for (const auto& p : c.getphones()) {
            add_phone_field();
            // ссылка на последний добавленный телефон в списке телефонов
            auto& last_input = phone_inputs.back();
            // вставляем в поле ввода сырой текст номера телефона
            last_input.first->setText(QString::fromStdString(p.getraw()));
            // устанавливаем тип телефона
            last_input.second->setCurrentText(QString::fromStdString(phone::type_to_string(p.gettype())));
        }
    } else { // режим создания нового контакта
        add_phone_field();
    }

    // связь кнопки "сохранить" с accepted
    connect(button_box, &QDialogButtonBox::accepted, this, &contactform::on_accepted);
    // связь сигнала отмены с слотом закрытия окна
    connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void contactform::setup_ui() {
    // Заголовок окна
    setWindowTitle(current_contact.has_value() ? "Редактирование контакта" : "Добавление контакта");
    
    // главный вертикальный компоновщик
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    // компоновщик для пар метка-поле
    QFormLayout *form_layout = new QFormLayout();
    
    // поля ввода
    name_edit = new QLineEdit(this);
    surname_edit = new QLineEdit(this);
    email_edit = new QLineEdit(this);
    patronymic_edit = new QLineEdit(this);
    address_edit = new QLineEdit(this);
    birthdate_edit = new QLineEdit(this);
    // серая подсказка в поле ввода
    birthdate_edit->setPlaceholderText("дд.мм.гггг");

    // добавляем пары метка-поле
    form_layout->addRow("Имя *:", name_edit);
    form_layout->addRow("Фамилия *:", surname_edit);
    form_layout->addRow("Е-mail *:", email_edit);
    form_layout->addRow("Отчество:", patronymic_edit);
    form_layout->addRow("Адрес:", address_edit);
    form_layout->addRow("Дата рождения:", birthdate_edit);

    // встраиваем блок с характеристиками в компоновщик
    main_layout->addLayout(form_layout);

    // вертикальный компоновщик для списка телефонов
    phones_layout = new QVBoxLayout();
    QLabel *phones_label = new QLabel("Телефоны *:", this);
    // добавляем список телефонов в главное окно
    main_layout->addWidget(phones_label);
    main_layout->addLayout(phones_layout);

    add_phone_btn = new QPushButton("Добавить телефон", this);
    main_layout->addWidget(add_phone_btn);
    // связь кнопки со слотом создания нового телефона
    connect(add_phone_btn, &QPushButton::clicked, this, &contactform::add_phone_field);

    // кнопки "сохранить" и "отмена"
    button_box = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    main_layout->addWidget(button_box);
}

void contactform::add_phone_field() {
    // горизонтальный компоновщик для поля ввода и выпаающего списка в один ряд
    QHBoxLayout *phone_row = new QHBoxLayout();
    // поле ввода для номера телефона
    QLineEdit *phone_edit = new QLineEdit(this);
    // серая подсказка
    phone_edit->setPlaceholderText("+78121234567");
    
    // выпадающий список для выбора типа телефона
    QComboBox *type_combo = new QComboBox(this);
    type_combo->addItem("домашний");
    type_combo->addItem("рабочий");
    type_combo->addItem("служебный");
    
    // добавляем в компоновщик (3 и 2 - пропорции кнопок)
    phone_row->addWidget(phone_edit, 3);
    phone_row->addWidget(type_combo, 2);
    
    // вставляем горизонтальный компоновщик 
    // в общий вертикальный компоновщик телефонов
    phones_layout->addLayout(phone_row);
    // сохраняем указатели на виджеты в вкетор для on_accepted
    phone_inputs.push_back({phone_edit, type_combo});
}

// обработка нажатия кнопки сохранения
void contactform::on_accepted() {
    contact c;
    // проверка, редактируем ли мы существующий контакт
    if (current_contact.has_value()) {
        c = current_contact.value(); // копируем данные контакта
        // очищаем список старых телефонов (чтобы не повторялись)
        c.getphones().clear();  
    }
    
    std::string name = name_edit->text().toStdString();
    std::string surname = surname_edit->text().toStdString();
    std::string email = email_edit->text().toStdString();
    std::string patronymic = patronymic_edit->text().toStdString();
    std::string address = address_edit->text().toStdString();

    c.setname(name);
    c.setsurname(surname);
    c.setemail(email);
    c.setpatronymic(patronymic);
    c.setaddress(address);

    // проход по всем полям ввода телефонов
    for (const auto& input : phone_inputs) {
        // считываем номер телефона из текущего поля
        std::string phone_str = input.first->text().toStdString();
        if (!phone_str.empty()) {
            phone_type p_type = phone::string_to_type(input.second->currentText().toStdString());
            c.addphone(phone_str, p_type);
        }
    }

    QString bdate_str = birthdate_edit->text().trimmed();
    if (!bdate_str.isEmpty()) {
        QStringList parts = bdate_str.split('.');
        if (parts.size() == 3) {
            bool ok1, ok2, ok3;
            int d = parts[0].toInt(&ok1);
            int m = parts[1].toInt(&ok2);
            int y = parts[2].toInt(&ok3);
            if (ok1 && ok2 && ok3) {
                c.setbirthdate(d, m, y);
            }
        }
    }

    QStringList errors;
    if (name.empty()) errors << "Имя обязательно!";
    else if (!validator::checkname(name)) errors << "Неверный формат имени.";

    if (surname.empty()) errors << "Фамилия обязательна!";
    else if (!validator::checkname(surname)) errors << "неверный формат фамилии.";

    if (email.empty()) errors << "Е-mail обязателен!";
    else if (!validator::checkemail(email)) errors << "Неверный формат e-mail.";

    if (phone_inputs.empty() || phone_inputs[0].first->text().trimmed().isEmpty()) {
        errors << "Необходимо ввести хотя бы один телефон.";
    } else {
        for (const auto& input : phone_inputs) {
            std::string p = input.first->text().toStdString();
            if (!p.empty() && !validator::checkphone(p)) {
                errors << "Неверный формат телефона: " + QString::fromStdString(p);
            }
        }
    }

    if (!bdate_str.isEmpty()) {
        QStringList parts = bdate_str.split('.');
        if (parts.size() == 3) {
            bool ok1, ok2, ok3;
            int d = parts[0].toInt(&ok1);
            int m = parts[1].toInt(&ok2);
            int y = parts[2].toInt(&ok3);
            if (ok1 && ok2 && ok3) {
                if (!validator::checkdate(d, m, y)) errors << "Неверная дата рождения.";
            } else {
                errors << "Неверный формат даты (дд.мм.гггг).";
            }
        } else {
            errors << "Неверный формат даты (дд.мм.гггг).";
        }
    }

    if (errors.isEmpty()) {
        accept();
    } else {
        QMessageBox::warning(this, "Ошибка валидации: ", errors.join("\n"));
    }
}

contact contactform::get_contact() const {
    contact c;
    if (current_contact.has_value()) {
        c = current_contact.value();
        c.getphones().clear();
    }
    
    c.setname(name_edit->text().toStdString());
    c.setsurname(surname_edit->text().toStdString());
    c.setemail(email_edit->text().toStdString());
    c.setpatronymic(patronymic_edit->text().toStdString());
    c.setaddress(address_edit->text().toStdString());

    // проход по всем полям номеров телефона
    for (const auto& input : phone_inputs) {
        // считываем номер телефона из текущего поля
        std::string phone_str = input.first->text().toStdString();
        if (!phone_str.empty()) {
            phone_type p_type = phone::string_to_type(input.second->currentText().toStdString());
            c.addphone(phone_str, p_type);
        }
    }

    QString bdate_str = birthdate_edit->text().trimmed();
    if (!bdate_str.isEmpty()) {
        QStringList parts = bdate_str.split('.');
        if (parts.size() == 3) {
            bool ok1, ok2, ok3;
            int d = parts[0].toInt(&ok1);
            int m = parts[1].toInt(&ok2);
            int y = parts[2].toInt(&ok3);
            if (ok1 && ok2 && ok3) {
                c.setbirthdate(d, m, y);
            }
        }
    }
    
    return c;
}