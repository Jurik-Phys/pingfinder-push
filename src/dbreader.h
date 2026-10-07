// Begin dbreader.h

#ifndef DBREADER_H
#define DBREADER_H

#include <QObject>
#include <QCoreApplication>
#include <QtNetwork/QLocalSocket>

class DbReader : public QObject{

    Q_OBJECT

    public:
        DbReader(QObject* parent = nullptr);
        ~DbReader();

        QStringList getAllClientId();
        QStringList getAllClientNick();

        bool isExistsClientById(const QString& clientId);
        bool isExistsClientByNickname(const QString& clientNickname);

    private:
        // В DbReader используется отдельный запрос к сокету для получения
        // общего списка клиентов и их идентификаторов. Включение данной логики
        // в основнй IpcServer видится не желательным т.к., приводит к смешению
        // ответственноси и необоснованному раздуванию класса IpcServer
        QLocalSocket* m_socket;
        const QString m_sockeFullPath = "/tmp/pingfinder/msgd.socket";

        // Запрос списка клиентов у IPC сервера (pingfinder-msgd)
        QJsonDocument buildClientsListRequest();
        QJsonDocument ipcRequestClientsList();

        void initClientsData(const QJsonDocument& doc);

        QStringList m_listOfClientId;
        QStringList m_listOfClientNickname;
};

#endif
// End dbreader.h
