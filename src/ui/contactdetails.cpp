#include "contactdetails.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>

contactdetails::contactdetails(const contact& c, QWidget *parent)
    : QDialog(parent) {
    setup_ui(c);
}

void contactdetails::setup_ui(const contact& c) {
    setWindowTitle("Подробности о контакте");
    // главный вертикальный компоновщик,привязан к текущему окну this
    QVBoxLayout *layout = new QVBoxLayout(this);
    // компоновщик в виде таблице (1 колонка - метка, 2 колонка - значение)
    QFormLayout *info_layout = new QFormLayout();
    // информация про контакт
    info_layout->addRow("ID:", new QLabel(QString::number(c.getid()), this));
    info_layout->addRow("Имя:", new QLabel(QString::fromStdString(c.getname()), this));
    info_layout->addRow("Фамилия:", new QLabel(QString::fromStdString(c.getsurname()), this));
    info_layout->addRow("Отчество:", new QLabel(QString::fromStdString(c.getpatronymic()), this));
    info_layout->addRow("Е-mail:", new QLabel(QString::fromStdString(c.getemail()), this));
    info_layout->addRow("Адрес:", new QLabel(QString::fromStdString(c.getaddress()), this));
    info_layout->addRow("Дата рождения:", new QLabel(QString::fromStdString(c.getbirthdate()), this));
    // встраивает блок с информацией в вертикальный компоновщик
    layout->addLayout(info_layout);
    // заголовок для списка телефонов
    QLabel *phone_label = new QLabel("Телефоны:", this);
    layout->addWidget(phone_label);
    
    // виждет списка телефонов
    QListWidget *phone_list = new QListWidget(this);
    for (const auto& p : c.getphones()) {
        // формирование строки типа "домашний: +7ХХХХХХХХХХ"
        QString display_text = QString::fromStdString(phone::type_to_string(p.gettype())) 
                             + ": " + QString::fromStdString(p.getformatted());
        // добавление строки в список
        phone_list->addItem(display_text);
    }
    layout->addWidget(phone_list);

    // кнопка "закрыть окно"
    QPushButton *close_btn = new QPushButton("Закрыть", this);
    // горизонтальный компоновщик для кнопок
    QHBoxLayout *btn_layout = new QHBoxLayout();
    // прижимает кнопку к правому краю
    btn_layout->addStretch();
    // добавляем саму кнопку в горизонтальный компоновщик
    btn_layout->addWidget(close_btn);
    // добавляем горизонтальный компоновщик в конец вертикалного
    layout->addLayout(btn_layout);

    // связь нажатия кнопки с сигналом accept
    connect(close_btn, &QPushButton::clicked, this, &QDialog::accept);
}