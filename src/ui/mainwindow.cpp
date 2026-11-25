#include "mainwindow.h"
#include "contactform.h"
#include "contactdetails.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <QLabel>

mainwindow::mainwindow(std::shared_ptr<contactmanager> manager, QWidget *parent)
    : QMainWindow(parent), manager(manager) {
    setup_ui();
    refresh_table();
}

void mainwindow::setup_ui() {
    //основной самый большой виджет (центральный)
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    // вертикальный компоновщик для элементов внутри главного виджета
    QVBoxLayout *layout = new QVBoxLayout(central);

    // горизонтальнй компоновщик для панели поиска
    QHBoxLayout *search_layout = new QHBoxLayout();
    QLabel *search_label = new QLabel("Поиск:", this);
    // поле для ввода текста
    search_input = new QLineEdit(this);
    search_input->setPlaceholderText("введите запрос...");

    // выпадающий список для выбора критерия поиска
    search_type = new QComboBox(this);
    search_type->addItem("по имени");
    search_type->addItem("по email");
    search_type->addItem("по телефону");

    QPushButton *search_btn = new QPushButton("Найти", this);
    QPushButton *reset_btn = new QPushButton("сбросить", this);

    // добавляем все элементы в горизонтальный компоновщик
    search_layout->addWidget(search_label);
    search_layout->addWidget(search_input);
    search_layout->addWidget(search_type);
    search_layout->addWidget(search_btn);
    search_layout->addWidget(reset_btn);

    // выпадающий список для выбора критерия сортировки
    sort_type = new QComboBox(this);
    sort_type->addItem("по имени");
    sort_type->addItem("по email");
    sort_type->addItem("по телефону");
    QPushButton *sort_btn = new QPushButton("сортировать", this);

    search_layout->addWidget(sort_type);
    search_layout->addWidget(sort_btn);

    // встраиваем в главный компоновщик
    layout->addLayout(search_layout);

    table = new QTableWidget(this);
    table->setColumnCount(5); // кол-во колонок в таблице
    table->setHorizontalHeaderLabels({"ID", "Имя", "Фамилия", "Е-mail", "Телефон"});
    // автоматическое выравнивание колонок по ширине окна
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // выделять всю строку, а не одну ячейку
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    // можно выделить тольок одну строку за раз
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    // встраиваем в главный компоновщик
    layout->addWidget(table);

    // компоновщик для кнопок управления
    QHBoxLayout *btn_layout = new QHBoxLayout();
    QPushButton *add_btn = new QPushButton("Добавить", this);
    QPushButton *edit_btn = new QPushButton("Редактировать", this);
    QPushButton *del_btn = new QPushButton("Удалить", this);

    btn_layout->addWidget(add_btn);
    btn_layout->addWidget(edit_btn);
    btn_layout->addWidget(del_btn);
    layout->addLayout(btn_layout);

    // связь сигналов нажатия кнопки с ее слотом
    connect(add_btn, &QPushButton::clicked, this, &mainwindow::add_contact);
    connect(edit_btn, &QPushButton::clicked, this, &mainwindow::edit_contact);
    connect(del_btn, &QPushButton::clicked, this, &mainwindow::delete_contact);
    connect(search_btn, &QPushButton::clicked, this, &mainwindow::search_contacts);
    connect(reset_btn, &QPushButton::clicked, this, &mainwindow::reset_search);
    connect(sort_btn, &QPushButton::clicked, this, &mainwindow::sort_contacts);

    QPushButton *details_btn = new QPushButton("Подробности", this);
    btn_layout->addWidget(details_btn);
    btn_layout->addWidget(add_btn);
    btn_layout->addWidget(edit_btn);
    btn_layout->addWidget(del_btn);

    connect(details_btn, &QPushButton::clicked, this, &mainwindow::show_details);
}

void mainwindow::refresh_table() {
    table->setRowCount(0);
    auto contacts = manager->getallcontacts();
    for (const auto& c : contacts) {
        // текущее число строк
        int row = table->rowCount();
        table->insertRow(row); // вставка новой строки
        table->setItem(row, 0, new QTableWidgetItem(QString::number(c.getid())));
        table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(c.getname())));
        table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(c.getsurname())));
        table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(c.getemail())));

        auto phones = c.getphones();
        QString phone_str;
        if (!phones.empty()) {
            // формирует строку типа "домашний: +7ХХХХХХХХХХ"
            phone_str = QString::fromStdString(phone::type_to_string(phones[0].gettype())) + ": " 
                       + QString::fromStdString(phones[0].getformatted());
        }
        // помещает сформированную строку в phone_str
        table->setItem(row, 4, new QTableWidgetItem(phone_str));
    }
}

void mainwindow::add_contact() {
    contactform form;
    if (form.exec() == QDialog::Accepted) {
        manager->addcontact(form.get_contact());
        refresh_table();
    }
}

void mainwindow::edit_contact() {
    // индекс строки в таблице
    int row = table->currentRow();
    if (row >= 0) {
        // id контакта
        int id = table->item(row, 0)->text().toInt();
        // запрашиваем контакт по id
        auto opt_c = manager->getcontactbyid(id);
        if (opt_c) {
            // создаем форму редактирования
            contactform form(*opt_c);
            // ждем пока кнопка сохранения не будет нажата
            if (form.exec() == QDialog::Accepted) {
                manager->updatecontact(form.get_contact());
                refresh_table();
            }
        }
    }
}

void mainwindow::delete_contact() {
    int row = table->currentRow();
    if (row >= 0) {
        int id = table->item(row, 0)->text().toInt();
        manager->removecontact(id);
        refresh_table();
    }
}

void mainwindow::search_contacts() {
    // текст из поля поиска
    QString query = search_input->text().trimmed();
    if (query.isEmpty()) {
        refresh_table();
        return;
    }
    // индекс выбранного критерия из выпадающего списка
    int search_index = search_type->currentIndex();
    std::vector<contact> results;

    if (search_index == 0) {
        results = manager->searchbyname(query.toStdString());
    } else if (search_index == 1) {
        results = manager->searchbyemail(query.toStdString());
    } else if (search_index == 2) {
        results = manager->searchbyphone(query.toStdString());
    }

    // очищаем таблицу
    table->setRowCount(0);
    // проход по всем найденным контактам
    for (const auto& c : results) {
        // индекс для новой строки
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(QString::number(c.getid())));
        table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(c.getname())));
        table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(c.getsurname())));
        table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(c.getemail())));

        auto phones = c.getphones();
        QString phone_str = phones.empty() ? "" : QString::fromStdString(phones[0].getformatted());
        table->setItem(row, 4, new QTableWidgetItem(phone_str));
    }
}

void mainwindow::reset_search() {
    search_input->clear();
    refresh_table();
}

void mainwindow::sort_contacts() {
    int sort_index = sort_type->currentIndex();

    if (sort_index == 0) {
        manager->sortbyname();
    } else if (sort_index == 1) {
        manager->sortbyemail();
    } else if (sort_index == 2) {
        manager->sortbyphone();
    }

    refresh_table();
}

void mainwindow::show_details() {
    int row = table->currentRow();
    if (row >= 0) {
        int id = table->item(row, 0)->text().toInt();
        auto opt_c = manager->getcontactbyid(id);
        if (opt_c) {
            contactdetails details(*opt_c, this);
            details.exec();
        }
    }
}