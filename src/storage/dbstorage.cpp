#include "dbstorage.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>

dbstorage::dbstorage(const std::string& host, const std::string& dbname, 
                     const std::string& user, const std::string& password, int port) {

    // новое соединение с базой данных 
    // (QPSQL - имя драйвера, phonebook_conn - имя соединения)
    db = QSqlDatabase::addDatabase("QPSQL", "phonebook_conn");
    db.setHostName(QString::fromStdString(host));
    db.setDatabaseName(QString::fromStdString(dbname));
    db.setUserName(QString::fromStdString(user));
    db.setPassword(QString::fromStdString(password));
    db.setPort(port);

    if (!db.open()) {
        qCritical() << "Ошибка подключения к базе данных:" << db.lastError().text();
    } else {
        init_db();
    }
}

dbstorage::~dbstorage() {
    if (db.isOpen()) {
        db.close();
    }
    QSqlDatabase::removeDatabase("phonebook_conn");
}

bool dbstorage::init_db() {
    // создание объекта для запросов
    QSqlQuery query(db);
    
    // таблица для телефонов
    query.exec("CREATE TABLE IF NOT EXISTS contacts ("
               "id SERIAL PRIMARY KEY, " // авто генерация id 
               "name VARCHAR, " // текстовые строки
               "surname VARCHAR, "
               "patronymic VARCHAR, "
               "address VARCHAR, "
               "email VARCHAR, "
               "birth_date VARCHAR)");
               
    // таблица для телефонов (тк у одного контакта их может быть несколько)
    query.exec("CREATE TABLE IF NOT EXISTS phones ("
               "id SERIAL PRIMARY KEY, "
               // связь с таблицей contacts
               "contact_id INT REFERENCES contacts(id) ON DELETE CASCADE, "
               "phone_number VARCHAR, "
               "phone_type VARCHAR DEFAULT 'домашний')");
               
    return true;
}

std::vector<contact> dbstorage::load() {
    std::vector<contact> contacts;
    // объект для запросов к таблице контактов
    QSqlQuery query(db);
    
    // запрос на выборку всех колонок из таблицы contacts
    if (query.exec("SELECT id, name, surname, patronymic, address, email, birth_date FROM contacts")) {
        // перебор каждой строки результата запроса
        while (query.next()) {
            contact c;
            c.setid(query.value(0).toInt());
            c.setname(query.value(1).toString().toStdString());
            c.setsurname(query.value(2).toString().toStdString());
            c.setpatronymic(query.value(3).toString().toStdString());
            c.setaddress(query.value(4).toString().toStdString());
            c.setemail(query.value(5).toString().toStdString());
            
            std::string bdate = query.value(6).toString().toStdString();
            if (!bdate.empty()) {
                size_t p1 = bdate.find('.');
                size_t p2 = bdate.find('.', p1 + 1);
                if (p1 != std::string::npos && p2 != std::string::npos) {
                    // stoi вырезает подстроку
                    int d = std::stoi(bdate.substr(0, p1));
                    int m = std::stoi(bdate.substr(p1 + 1, p2 - p1 - 1));
                    int y = std::stoi(bdate.substr(p2 + 1));
                    c.setbirthdate(d, m, y);
                }
            }
            
            // объект для поиска теелфонов текущего контакта
            QSqlQuery phone_query(db);
            // подготовка запроса возврата номеров телефона по нужному id
            phone_query.prepare("SELECT phone_number FROM phones WHERE contact_id = :id");
            // подставляем id 
            phone_query.bindValue(":id", c.getid());
            
            // запрос на получение телефонов
            if (phone_query.exec()) {
                while (phone_query.next()) {
                    // добавляем номер телефона в контакт
                    std::string pnumber = phone_query.value(0).toString().toStdString();
                    std::string ptype = phone_query.value(1).toString().toStdString();
                    c.addphone(pnumber, phone::string_to_type(ptype));
                }
            }
            contacts.push_back(c);
        }
    } else {
        qCritical() << "Ошибка загрузки контактов:" << query.lastError().text();
    }
    
    return contacts;
}

bool dbstorage::add(const contact& c) {
    // объект для запросов в contacts
    QSqlQuery query(db);
    // запрос на вставку новой строки в contacts с генерацией id
    query.prepare("INSERT INTO contacts (name, surname, patronymic, address, email, birth_date) "
                  "VALUES (:name, :surname, :patronymic, :address, :email, :birth_date) RETURNING id");
                  
    query.bindValue(":name", QString::fromStdString(c.getname()));
    query.bindValue(":surname", QString::fromStdString(c.getsurname()));
    query.bindValue(":patronymic", QString::fromStdString(c.getpatronymic()));
    query.bindValue(":address", QString::fromStdString(c.getaddress()));
    query.bindValue(":email", QString::fromStdString(c.getemail()));
    query.bindValue(":birth_date", QString::fromStdString(c.getbirthdate()));
    
    // отправляем запрос на вставку
    if (query.exec() && query.next()) {
        // id из новой строки
        int new_id = query.value(0).toInt();
        
        // объект для запросов в phones
        QSqlQuery phone_query(db);
        // подготовка запроса на добавление телефона (и привязку его к contacts)
        phone_query.prepare("INSERT INTO phones (contact_id, phone_number, phone_type) VALUES (:contact_id, :phone_number, :phone_type)");
        
        for (const auto& p : c.getphones()) {
            phone_query.bindValue(":contact_id", new_id);
            phone_query.bindValue(":phone_number", QString::fromStdString(p.getformatted()));
            phone_query.bindValue(":phone_type", QString::fromStdString(phone::type_to_string(p.gettype())));
            phone_query.exec();
        }
        return true;
    }
    
    qCritical() << "Ошибка добавления контакта:" << query.lastError().text();
    return false;
}

bool dbstorage::remove(int id) {
    // объяект запроса для бд
    QSqlQuery query(db);
    //подготовка запроса на удаление строки из contacts
    query.prepare("DELETE FROM contacts WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qCritical() << "Ошибка удаления контакта:" << query.lastError().text();
        return false;
    }
    return true;
}

bool dbstorage::update(const contact& c) {
    QSqlQuery query(db);
    // подготовка запроса на изменение указанных строк для контакта с нужным id
    query.prepare("UPDATE contacts SET name = :name, surname = :surname, patronymic = :patronymic, "
                  "address = :address, email = :email, birth_date = :birth_date WHERE id = :id");
                  
    query.bindValue(":name", QString::fromStdString(c.getname()));
    query.bindValue(":surname", QString::fromStdString(c.getsurname()));
    query.bindValue(":patronymic", QString::fromStdString(c.getpatronymic()));
    query.bindValue(":address", QString::fromStdString(c.getaddress()));
    query.bindValue(":email", QString::fromStdString(c.getemail()));
    query.bindValue(":birth_date", QString::fromStdString(c.getbirthdate()));
    query.bindValue(":id", c.getid());
    
    if (query.exec()) {
        // запрос на удаление старых телефонов этого контакта
        QSqlQuery del_query(db);
        del_query.prepare("DELETE FROM phones WHERE contact_id = :id");
        del_query.bindValue(":id", c.getid());
        del_query.exec();
        
        // запрос на вставку новых телефонов
        QSqlQuery ins_query(db);
        ins_query.prepare("INSERT INTO phones (contact_id, phone_number, phone_type) VALUES (:contact_id, :phone_number, :phone_type)");
        
        // каждый телефон вставляем в базу
        for (const auto& p : c.getphones()) {
            ins_query.bindValue(":contact_id", c.getid());
            ins_query.bindValue(":phone_number", QString::fromStdString(p.getformatted()));
            ins_query.bindValue(":phone_type", QString::fromStdString(phone::type_to_string(p.gettype())));
            ins_query.exec();
        }
        return true;
    }
    
    qCritical() << "Ошибка обновления контакта:" << query.lastError().text();
    return false;
}

bool dbstorage::save(const std::vector<contact>& contacts) {
    QSqlQuery query(db);
    query.exec("DELETE FROM phones");
    query.exec("DELETE FROM contacts");
    
    for (const auto& c : contacts) {
        add(c);
    }
    return true;
}