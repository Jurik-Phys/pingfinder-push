// Begin ipcclient.cpp

#include "ipcclient.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

IpcClient::IpcClient(const QJsonDocument& inJsonDoc, QObject *parent)
                                                              : QObject(parent){

    m_socket = new QLocalSocket();
    m_socket->connectToServer(m_sockeFullPath);

    if (!m_socket->waitForConnected(3000)){
        qDebug() << "     - - - ";
        qDebug() << "[EE] Unable to connect" << m_socket->errorString();
        exit(1);
    }

    this->sendCommand(inJsonDoc);

    QObject::connect(m_socket, &QLocalSocket::readyRead,
                                                 this, &IpcClient::onReadyRead);

}

IpcClient::~IpcClient(){
    delete(m_socket);
}

void IpcClient::sendCommand(const QJsonDocument& inJsonDoc){
    QByteArray data = inJsonDoc.toJson(QJsonDocument::Compact);
    data.append('\n');

    if (m_socket->write(data) == - 1){
        qDebug() << "[EE] Write failed:" << m_socket->errorString();
        exit(1);
    }

    if (!m_socket->waitForBytesWritten(3000)){
        qDebug() << "[EE] Write timeout:" << m_socket->errorString();
        exit(1);
    }

}

void IpcClient::onReadyRead(){
    QLocalSocket * socket = qobject_cast<QLocalSocket*>(sender());

    if (!socket){
        return;
    }

    QByteArray buffer = socket->readAll();

    // Символ новой строки '\n' означает окончание полученной комманды
    // Собственно, ниже разбор получаемых данных, каждая строка преобразуется
    // в QJsonObject, которая уже обрабатывается в handleCommand(obj);

    while (true){

        // Индекс символа переноса строки
        int idx = buffer.indexOf('\n');

        // Выход, если больше нет переносов строки
        if (idx < 0){
            break;
        }

        // Выделение строки c json'ом
        QByteArray line = buffer.left(idx).trimmed();

        // Удаление из буфера использованных данных
        buffer.remove(0, idx + 1);

        // Пришёл пустой json
        if (line.isEmpty()){
            continue;
        }

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(line, &error);

        if (error.error != QJsonParseError::NoError){
            qDebug() << "[EE] JSON parse error:" << error.errorString();
            continue;
        }

        QJsonObject obj = doc.object();

        // Обработка полученного json'а
        handleCommand(obj);

        // Disconnect from server
        socket->disconnectFromServer();
        if (socket->state() != QLocalSocket::UnconnectedState){
            socket->waitForDisconnected(1000);
        }

        // На данном этапе, запрос на pingfinder-msgd отправлен, с него получен
        // ответ, который через handleCommand() выведен пользователю.
        // Программу можно закрывать, своё дело она сделала.
        qDebug() << "[II] Done";
        exit(0);
    }
}

void IpcClient::handleCommand(const QJsonObject& obj){

    if (obj["action"] == "message_push_validation_failed"){
        handleMessagePushValidationFailed(obj["payload"].toObject());
    }

    if (obj["action"] == "message_push_scheduled"){
        handleMessagePushScheduled(obj["payload"].toObject());
    }

}

void IpcClient::handleMessagePushValidationFailed(const QJsonObject& obj){

    QString failureType   = obj["failure_type"].toString();
    QString failureReport = obj["failure_report"].toString();

    qDebug() <<"     - - -";

    qDebug() << "[EE] Operation rejected. Error response from "
                                                          "\"pingfinder-msgd\"";
    qDebug() << "[EE] Details:";
    qDebug() << "     > failure type:       " << failureType;
    qDebug() << "     > failure description:" << failureReport;

    if (obj["clients"].toArray().size() > 0){

        qDebug() << "     > affected clients:";
        qDebug().noquote() << "       ----------------";
        qDebug().noquote() << "         Id    Nickname";
        qDebug().noquote() << "       ----------------";

        QJsonArray clientsArray = obj["clients"].toArray();
        for (int i = 0; i < clientsArray.size(); ++i){
            QJsonObject client = clientsArray[i].toObject();
            QString id = client["id"].toString();
            QString nickname = client["nickname"].toString();
            qDebug().noquote() << QString("         %1").arg(id, 3)
                                                             << " " << nickname;
        }
        qDebug().noquote() << "       ----------------";
    }
    if (failureType == "message_time_not_allowed"){
        // При неверной попытке исправления во входящих параметрах не помогут
        qDebug() <<"     - - -";
        qDebug() << "[EE] Please try again during the allowed sending "
                                                                  "time window";
    }
    else {
        qDebug() << "[EE] Please correct the command-line arguments "
                                                                "and try again";
    }
}

void IpcClient::handleMessagePushScheduled(const QJsonObject& obj){

    QString resultType   = obj["result_type"].toString();
    QString resultReport = obj["result_report"].toString();

    qDebug() << "     - - -";
    qDebug() << "[OK] True response from \"pingfinder-msgd\"";
    qDebug() << "[II] [II] The following tasks were added to the schedule "
                                                      "or already exist in it:";

    QJsonArray tasksArray = obj["tasks"].toArray();

    for (int i = 0; i < tasksArray.size(); ++i){
        QString taskNumber = QString::number(i + 1);
        QJsonObject task = tasksArray[i].toObject();
        qDebug().noquote()  << "[II] Task №" + taskNumber;
        qDebug().nospace()  << "     > client id:      "
                                  << " \"" << task["client_id"].toInt() << "\"";
        QString nick = task["client_nickname"].toString();
        nick[0] = nick[0].toUpper();
        qDebug() << "     > client nickname:" << nick;
        qDebug() << "     > execute time:   "
                                             << task["execute_time"].toString();
        qDebug() << "     > uuid:           " << task["uuid"].toString();
    }
}

// End ipcclient.cpp
