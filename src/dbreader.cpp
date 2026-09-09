// Begin dbreader.cpp

#include "dbreader.h"
#include <QFile>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

DbReader::DbReader(QObject* parent) : QObject(parent){

    // Загрузка списка клиентов, при неудаче выход из программы
    loadClientsData();
}

DbReader::~DbReader(){

}

void DbReader::loadClientsData(){

    QFile clientsFile(m_clientsFile);
    if (!clientsFile.open(QIODevice::ReadOnly)){
        qDebug() << "[EE] Error opening" << m_clientsFile;
        exit(1);
    }

    m_listOfClientId.clear();
    m_listOfClientNickname.clear();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(clientsFile.readAll(), &error);

    if (error.error != QJsonParseError::NoError){
        qDebug() << "[EE] JSON parse error:" << error.errorString();
        exit(1);
    }

    QJsonObject root = doc.object();
    QJsonArray clients = root["clients"].toArray();

    for (int i = 0; i < clients.size(); ++i){
        QJsonObject obj = clients[i].toObject();
        if (obj["enabled"].toBool()){
            QString id = QString::number(obj["id"].toInt());
            QString nickname = obj["nickname"].toString();
            m_listOfClientId.push_back(id);
            m_listOfClientNickname.push_back(nickname);
        }
    }
}

QStringList DbReader::getAllClientId(){
    return m_listOfClientId;
}

QStringList DbReader::getAllClientNick(){
    return m_listOfClientNickname;
}

bool DbReader::isExistsClientById(const QString& clientId){
    return m_listOfClientId.contains(clientId);
}

bool DbReader::isExistsClientByNickname(const QString& clientNickname){
    return m_listOfClientNickname.contains(clientNickname, Qt::CaseInsensitive);
}

// End dbreader.cpp
