// Begin ipcclient.h
#ifndef IPCCLIENT_H
#define IPCCLIENT_H

#include <QObject>
#include <QtNetwork/QLocalSocket>
#include <QString>

class IpcClient : public QObject {

    Q_OBJECT

    public:
        IpcClient(const QJsonDocument& inJsonDoc, QObject *parent = nullptr);
        ~IpcClient();

    private slots:
        void onReadyRead();

    private:
        // Загрузка списка клиентов программу. В данном случае из файла
        bool loadClientsFromFile(const QString& fileName);

        QLocalSocket* m_socket;
        const QString m_sockeFullPath = "/tmp/pingfinder/msgd.socket";

        void sendCommand(const QJsonDocument& inJsonDoc);
        void handleCommand(const QJsonObject& obj);
        // Обработка полученной от IPC сервера ошибки обработки запроса
        void handleMessagePushValidationFailed(const QJsonObject& obj);
        // Обработка успешного встаивания заданий в расписание
        void handleMessagePushScheduled(const QJsonObject& obj);
};

#endif
// End ipcclient.h
