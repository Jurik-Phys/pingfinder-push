// Begin cmd.cpp

#include "cmd.h"
#include <QDebug>
#include <QCoreApplication>

static const QVector<CommandInfo> appCommands = {
    {
        "--message-type",
        "{hello, notice}",
        "Type of message to send.\n"
        "      If --message is provided, the message type defaults is "
                                                                 "\"notice\";\n"
                                  "      otherwise, the default is \"hello\"."
    },
    {
        "--ident-type",
        "{id, nickname}",
        "Type of user identifiers. If all --idents are numeric, then "
        "--ident-type\n"
        "      is set to \"id\"; otherwise, it is set to \"nick\"."
    },
    {
        "--idents",
        "<id | nickname | all> [id | nickname | all ...]",
        "List of target user identifiers (IDs or nicknames depending on "
                                    "--ident-type).\n"
              "      Special value 'all' selects all users by Ids or nicknames."
    },
    {
        "--message",
        "<text>",
        "Message text. Required only for --message-type=notice."
    }
};

const QVector<CommandInfo>& getCommandList()
{
    return appCommands;
}

void printHelp(){
    qDebug().noquote();

    qDebug() << "";
    qDebug() << "pingfinder-push - send control commands to pingfinder-msgd";
    qDebug() << "";

    qDebug() << "Usage:";
    qDebug() << "  pingfinder-push <command> [arguments]";
    qDebug() << "";

    qDebug() << "Options:";
    qDebug() << "  -h, --help              Show this help message";
    qDebug() << "  -l, --list-users        Show a list of active users";
    qDebug() << "  -v, --version           Show the version";
    qDebug() << "";

    qDebug() << "Commands:";
    qDebug() << "";

    for (int i = 0; i < getCommandList().size(); ++i){
        qDebug().noquote() << " " << getCommandList().at(i).name
                                                 << getCommandList().at(i).args;
        qDebug().noquote() << "     " << getCommandList().at(i).description;
        qDebug() << "";
    }

    qDebug() << "Notes:";
    qDebug() << "  Arguments are space-separated (no commas)";
    qDebug() << "  This tool communicates with pingfinder-msgd via Unix socket";
    qDebug() << "";

    qDebug() << "Examples:";
    qDebug() << "  ";
    qDebug() << "  Send a \"hello\" message to users by id:";
    qDebug() << "    pingfinder-push --idents 21 62 34";
    qDebug() << "  ";
    qDebug() << "  Send a \"hello\" message to users by nickname:";
    qDebug() << "    pingfinder-push --idents Jumper Pluton Zeus";
    qDebug() << "  ";
    qDebug() << "  Send a notification with message to users by id:";
    qDebug() << "    pingfinder-push --idents 21 62 "
                                       "--message Service maintenance at 22:00";
    qDebug() << "  ";
    qDebug() << "  Send a notification with message to users by nickname:";
    qDebug() << "    pingfinder-push --idents  Jumper Pluton Zeus "
                                       "--message Service maintenance at 22:00";
    qDebug() << "  ";
    qDebug() << "  Send a broadcast message to all users:";
    qDebug() << "    pingfinder-push --message-type notice --ident-type id"
        " --idents all --message Server restart in 5 minutes";
}

bool isKnownCmd(const QString& cmd){
    QStringList appCmdList;
    for (auto it = appCommands.cbegin(); it != appCommands.cend(); ++it){
        appCmdList.push_back((*it).name);
    }

    if (!appCmdList.contains(cmd)){
        QString exeName = QCoreApplication::applicationName();
        qDebug().nospace().noquote() << exeName << ": unrecognized option \""
                                                                 << cmd << "\"";
        qDebug().nospace().noquote() << "Try '"<< exeName
                                            << " --help' for more information.";
        return false;
    }

    return true;
}

bool cmdParsing(const QStringList& args, CmdInfo& request, QString& error){
    bool result = true;

    for (int i = 0; i < args.size(); ++i){

        const QString arg = args[i];

        // --message-type
        if (arg == "--message-type"){
            if (i + 1 >= args.size()){
                error = "--message-type requires a value";
                return false;
            }

            // Следующий аргумент существует, но надо проверить,
            // что это не новая команда

            const QString &next = args[i+1];

            if (next.startsWith("--")){
                error = arg + " requires a value, but got option: " + next;
                return false;
            }
            else {
                request.msgType = next;
                // Пропуск из рассмотрения текущего next
                i++;
            }
        }

        // --ident-type
        if (arg == "--ident-type"){
            if (i + 1 >= args.size()){
                error = "--ident-type requires a value";
                return false;
            }

            // Следующий аргумент существует, но надо проверить, что это такое
            const QString &next = args[i+1];

            if (next.startsWith("--")){
                error = arg + " requires a value, but got option: " + next;
                return false;
            }
            else {
                request.identType = next;
                // Пропуск из рассмотрения текущего next
                i++;
            }
        }

        // --idents
        if (arg == "--idents"){
            i++;

            while (i < args.size() && !args[i].startsWith("--")){
                request.idents.push_back(args[i]);
                i++;
            }
            // Уменьшение индекса для перехвата той самой команды,
            // что стригерила остановку цикла .startsWith("--")
            i--;

            if (request.idents.isEmpty()) {
                error = "--idents requires at least one value";
                return false;
            }
        }

        // --message
        if (arg == "--message"){
            if (i + 1 >= args.size()){
                error = "--ident-type requires a value";
                return false;
            }

            // Следующий аргумент существует, но надо проверить, что это такое
            const QString& next = args[i+1];

            if (next.startsWith("--")){
                error = arg + " requires a value, but got option: " + next;
                return false;
            }
            else {
                request.message.clear();
                while (i - 1 < args.size() && !args[i+1].startsWith("--")){
                    QString tmpArg = args[i + 1];
                    request.message += tmpArg + " ";
                    i++;

                    // Достигнут последний элемент, проверка того, какой элемент
                    // следующий невозможна
                    if (i == args.size() - 1){
                        break;
                    }
                }
                request.message = request.message.trimmed();
            }
        }
    }

    // Нет идентификаторов пользователя, нет корректных данных, ошибка
    if (request.idents.size() == 0){
        error = "User identifiers (nick or numeric Id) are missing or invalid";
        return false;
    }

    return result;
}

bool cmdValidateAndSetDefault(CmdInfo& request, QString& error){
    DbReader dbreader = new DbReader();
    bool result = true;

    // Если msgType не задан, но есть само message, то тип сообщения - "notice"
    if (request.msgType.isEmpty() && !request.message.isEmpty()){
        request.msgType = "notice";
    }

    // Если msgType не задан и нет тела сообщения, то тип сообщения - "hello"
    if (request.msgType.isEmpty() && request.message.isEmpty()){
        request.msgType = "hello";
    }

    // Если msgType "hello" и есть сообщение, то это противоречие - "ошибка"
    if (request.msgType == "hello" && !request.message.isEmpty()){
        error = "The \"hello\" message is not customizable";
        result = false;
    }

    // Если msgType "notice", а сообщение не задано, то это ошибка
    if (request.msgType == "notice" && request.message.isEmpty()){
        error = "Notice message not found";
        result = false;
    }

    // Если identType не задан, то система будет определять его самостоятельно,
    // если всё содержимое из idents - это числа, то тип будет id,
    // иначе nickname. Отдельно стоит слово "all", которое разворачивается
    // либо в список id, либо в список nicknames
    if (request.identType.isEmpty()){
        bool isNumber = true;
        for (int i = 0; i < request.idents.size(); ++i){
            bool ok = true;
            request.idents[i].toInt(&ok);
            if (!ok){
                isNumber = false;
            }
        }

        if  (isNumber){
            request.identType = "id";
        }
        else {
            request.identType = "nick";
        }
    }

    // Обработка "all" при выборе identType "id"
    if (request.identType == "id" && request.idents.join("") == "all" ){
        request.idents.clear();
        request.idents = dbreader.getAllClientId();
        qDebug() << request.idents;
    }

    // Обработка "all" при выборе identType "nick"
    if (request.identType == "nick" && request.idents.join("") == "all" ){
        request.idents.clear();
        request.idents = dbreader.getAllClientNick();
        qDebug() << request.idents;
    }

    // Проверка введённых nickname's на предмет их регистраци в системе
    if (request.identType == "nick"){
        for (int i = 0; i < request.idents.size(); ++i){
            QString nick = request.idents[i];
            if (!dbreader.isExistsClientByNickname(nick)){
                result = false;
                error = "Unknown or disabled user with the nickname \""
                                                                  + nick + "\"";
                break;
            }
        }
    }

    // Проверка вхождения введённого nickname в список известных клиентов
    if (request.identType == "id"){
        for (int i = 0; i < request.idents.size(); ++i){
            QString id = request.idents[i];
            if (!dbreader.isExistsClientById(id)){
                result = false;
                error = "Unknown or disabled user with the id \"" + id + "\"";
                break;
            }
        }
    }

    return result;
}

void listUsers() {
    DbReader dbreader = new DbReader();
    QStringList ids = dbreader.getAllClientId();
    QStringList nicks = dbreader.getAllClientNick();

                qDebug().noquote() << "  -----------------";
                qDebug().noquote() << "      User list";
                qDebug().noquote() << "  -----------------";
                qDebug().noquote() << "     Id    Nickname";
                qDebug().noquote() << "  -----------------";
    for (int i = 0; i < ids.size(); ++i){
        int id = ids[i].toInt();
        qDebug().noquote() << QString("  >  %1").arg(id, 3) << " " << nicks[i];
    }
    qDebug().noquote() << "  -----------------";
    if (ids.size() == 1){
        qDebug().noquote() << "  Total: " << ids.size() << "user";
    }
    else {
        qDebug().noquote() << "  Total: " << ids.size() << "users";
    }
    qDebug().noquote() << "  -----------------";
}

QJsonDocument cmdToIpcJsonDocument(const CmdInfo& cmdInfo){
    QJsonObject ipcJsonObj;
    ipcJsonObj["action"] = "message_push";

    QJsonObject   payloadJson;

    payloadJson["message_type"] = cmdInfo.msgType;
    payloadJson["ident_type"]   = cmdInfo.identType;
    payloadJson["message"]      = cmdInfo.message;
    payloadJson["idents"]       = QJsonArray::fromStringList(cmdInfo.idents);
    // Cлужебный параметр с названием программы, являющейся источником данных
    payloadJson["provided_by"]  = QCoreApplication::applicationName();

    ipcJsonObj["payload"] = payloadJson;

    return QJsonDocument(ipcJsonObj);
}

// End cmd.cpp
