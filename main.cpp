#include <QCoreApplication>
#include "src/ipcclient.h"
#include "src/appInfo.h"
#include "cmd.h"


int main(int argc, char** argv){

    QCoreApplication app(argc, argv);

    QStringList args = QCoreApplication::arguments();

    DbReader dbreader;

    // Help to display & exit
    if (args.contains("-h") || args.contains("--help") || args.size() == 1)
    {
        printHelp();
        return 0;
    }

    // Print application version & exit
    if (args.contains("-v") || args.contains("--version"))
    {
        AppInfo::print();
        return 0;
    }

    // List users to display & exit
    if (args.size() == 2){
        if (args.contains("--list-users") || args.contains("-l")){
            listUsers(dbreader);
            exit(1);
        }
    }

    CmdInfo cmdInfo;
    QString error;
    if (!cmdParsing(args, cmdInfo, error)){
        qDebug().noquote() << "[EE]" << error;
        qDebug().noquote() << "[II] Run with -h or --help to display usage "
                                                       "and available commands";
        exit(1);
    };

    if (!cmdValidateAndSetDefault(cmdInfo, error, dbreader)){
        qDebug().noquote() << "[EE]" << error;
        if (!error.contains("Unknown or disabled user")){
            qDebug().noquote() << "[II] Run with -h or --help to display usage "
                                                       "and available commands";
        }
        exit(1);
    };

    // Обязательное поле (вводится либо пользователем, либо вычисляется
    // по наличию сообщения (--message)
    qDebug() << "[II] Input data details:";
    qDebug() << "     > message-type:  "<< cmdInfo.msgType;
    // Поле обязательное для notice сообщений, welcome использует свой формат
    if (!cmdInfo.message.isEmpty()){
        qDebug() << "     > message:       "<< cmdInfo.message;
    }
    // Обязательное поле (вводится пользователем, либо вычисляется по типу
    // введённых идентификаторов (числа для id)
    qDebug() << "     > ident-type:    "<< cmdInfo.identType;
    qDebug() << "     > idents:        "<< cmdInfo.idents.join(" ");

    // Блок подтверждения ввода //
    QTextStream in(stdin);
    QTextStream out(stdout);

    out << "\n[II] Confirm execution? [y/N]: " << Qt::flush;

    QString answer = in.readLine().trimmed().toLower();

    if (answer != "y" && answer != "yes"){

        out << "     - - -\n";
        out << "[II] Operation cancelled by the user\n";
        out.flush();
        exit(0);
    }

    QJsonDocument doc = cmdToIpcJsonDocument(cmdInfo);

    IpcClient* ipcClient = new IpcClient(doc, &app);

    return app.exec();
}


