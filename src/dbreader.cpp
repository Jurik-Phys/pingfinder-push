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
    QJsonDocument clientsListJsonDoc  = ipcRequestClientsList();
    initClientsData(clientsListJsonDoc);
}

DbReader::~DbReader(){

}

void DbReader::initClientsData(const QJsonDocument& doc){

    QJsonObject root = doc.object();
    QJsonObject payloadJson = root["payload"].toObject();
    QJsonArray clients = payloadJson["clients"].toArray();

    if (root["action"] == "clients_list_created"){
        for (int i = 0; i < clients.size(); ++i){
            QJsonObject clientObj = clients[i].toObject();

            QString id = QString::number(clientObj["id"].toInt());
            QString nickname = clientObj["nickname"].toString();
            m_listOfClientId.push_back(id);
            m_listOfClientNickname.push_back(nickname);
        }
    }
    else {
        qDebug() << "[II] Failed to retrieve the client list. Exiting.";
        exit(1);
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

QJsonDocument DbReader::buildClientsListRequest(){
    QJsonObject ipcJsonObj;
    ipcJsonObj["action"] = "clients_request";

    QJsonObject payloadJson;
    payloadJson["provided_by"] = QCoreApplication::applicationName();

    ipcJsonObj["payload"] = payloadJson;

    return QJsonDocument(ipcJsonObj);
}

QJsonDocument DbReader::ipcRequestClientsList(){
    // Синхронное получение списка клиентов при запуске программы

    // Запрос //
    QLocalSocket socket;

    socket.connectToServer(m_sockeFullPath);

    if (!socket.waitForConnected(1000)){
        qDebug() << "     - - - ";
        qDebug() << "[EE] Unable to connect:" << socket.errorString();
        exit(1);
    }

    QByteArray reqst = buildClientsListRequest().toJson(QJsonDocument::Compact);
    reqst.append('\n');

    if (socket.write(reqst) == -1){
        qDebug() << "[II] Write failed:" << socket.errorString();
        exit(1);
    }

    // Ответ //
    QByteArray buffer;
    while (!buffer.contains('\n')){
        if (!socket.waitForReadyRead(1000)){
            qDebug() << "[EE] Receive clients list error:"
                                                        << socket.errorString();
            exit(1);
        }

        buffer += socket.readAll();
    }

    // Выделение строки c json'ом
    QByteArray line = buffer.left(buffer.indexOf('\n')).trimmed();

    // Очистка буфера
    buffer.remove(0, buffer.indexOf('\n') + 1);

    // Clients List //
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(line, &error);

    if (error.error != QJsonParseError::NoError){
        qDebug() << "[EE] JSON parse error:" << error.errorString();
        exit(1);
    }

    return doc;
}
// End dbreader.cpp
