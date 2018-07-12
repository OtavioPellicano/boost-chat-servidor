#include "log.h"

Log::Log()
{
    setup();
}

void Log::setup()
{
    mDirLog.mkdir("log");
    mDirLog.cd("log");
    mNomeArqOut =  "log-" + QDate::currentDate().toString("yyyy-MM-dd").toStdString() + ".csv";
}

void Log::salvarLog(const std::string &org, const std::string &dst, const std::string &msg)
{
    salvarArq(gerarQStrLog(org, dst, msg));

}

QDir Log::dirLog() const
{
    return mDirLog;
}

std::string Log::nomeArqOut() const
{
    return mNomeArqOut;
}

string Log::gerarQStrLog(const std::string &org, const std::string &dst, const std::string &msg)
{
    string qstrLog = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toStdString();
    string mensagem;

    if(dst == BROADCAST_CONECTADO)
    {
        mensagem = "conectado";
    }
    else if(dst == BROADCAST_DESCONECTADO)
    {
        mensagem = "desconectado";
    }
    else
    {
        mensagem = msg;
    }

    return qstrLog + ";" + org + ";" + dst + ";" + mensagem;
}

void Log::salvarArq(const std::string &qstrLog)
{
    bool primeiraEscrita;
    ifstream arqExist(dirLog().absoluteFilePath(QString::fromStdString(nomeArqOut())).toStdString());

    if(arqExist.is_open())
    {
        primeiraEscrita = false;
        arqExist.close();
    }
    else
    {
        primeiraEscrita = true;
    }



    ofstream arq(dirLog().absoluteFilePath(QString::fromStdString(nomeArqOut())).toStdString(), ios_base::app);

    if(arq.is_open())
    {
        if(primeiraEscrita)
            arq << "data-hora;origem;destino;mensagem" << endl;

        arq << qstrLog << endl;
        arq.close();
    }
    else
    {

        std::cout << "impossivel abrir o arquivo de log: " << nomeArqOut() << endl;
    }

}
