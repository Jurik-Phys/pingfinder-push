// Begin dbreader.h

#ifndef DBREADER_H
#define DBREADER_H

#include <QObject>

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
        QString m_clientsFile = "/etc/pingfinder/msgd/clients.json";
        QString m_clientStatusFile="/var/lib/pingfinder/msgd/clientStatus.json";

        void loadClientsData();

        QStringList m_listOfClientId;
        QStringList m_listOfClientNickname;
};

#endif
// End dbreader.h
