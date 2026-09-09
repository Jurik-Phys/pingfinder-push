// Begin cmd.h

#ifndef CMD_H
#define CMD_H

#include <QString>
#include <QVector>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "src/dbreader.h"

struct CommandInfo
{
    QString name;
    QString args;
    QString description;
    QString example;
};

const QVector<CommandInfo>& getCommandList();
void printHelp();
void listUsers();

struct CmdInfo {
    QString     msgType;   // hello | notice //
    QString     identType; // id | nicknames //
    QStringList idents;
    QString     message;
};

bool cmdParsing(const QStringList& args, CmdInfo &request, QString& error);
bool cmdValidateAndSetDefault(CmdInfo& request, QString& error);

QJsonDocument cmdToIpcJsonDocument(const CmdInfo& cmdInfo);

#endif
// End cmd.h
