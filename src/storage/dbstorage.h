#pragma once
#include "istorage.h"
#include <string>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>

// хранилище для работы с sql
class dbstorage : public istorage {
public:
    // hodt - адрес сервера, dbname - имя базы,
    // user, password - данные для входа, port - порт
    dbstorage(const std::string& host, const std::string& dbname, 
              const std::string& user, const std::string& password, int port = 5432);
    ~dbstorage() override;

    // перезапись всех контактов
    bool save(const std::vector<contact>& contacts) override;

    // загрузка контактов из базы
    std::vector<contact> load() override;

    // добавить контакт
    bool add(const contact& c) override;

    // удалить контакт по id 
    bool remove(int id) override;

    // обновить существующий контакт по id
    bool update(const contact& c) override;

private:
    // соединение с бд
    QSqlDatabase db;

    // создание таблицы при первом запуске
    bool init_db();
};