#pragma once
#include <QDialog>
#include <QLabel>
#include <QListWidget>
#include "../models/contact.h"

// создание окна с информацией о контакте
class contactdetails : public QDialog {
    Q_OBJECT // обязательный макрос qt
public:
    // конструктор
    explicit contactdetails(const contact& c, QWidget *parent = nullptr);

private:
    // создание визуальных элементов
    void setup_ui(const contact& c);
};