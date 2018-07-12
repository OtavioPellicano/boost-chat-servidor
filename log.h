#ifndef LOG_H
#define LOG_H


#include <QDir>
#include <QDateTime>
#include <fstream>
#include <iostream>

using std::ifstream;
using std::ofstream;
using std::endl;
using std::ios_base;
using std::string;

class Log
{

private:

public:
    explicit Log();

    void salvarLog(const std::string &org, const std::string &dst, const std::string &msg);


private:
    void run();

    void setup();

    std::string nomeArqOut() const;
    QDir dirLog() const;

    std::string gerarQStrLog(const std::string &org, const std::string &dst, const std::string &msg);
    void salvarArq(const std::string &qstrLog);

private:

    QDir mDirLog;
    std::string mNomeArqOut;

    const std::string BROADCAST_CONECTADO = "$c$";      //broadcast: #$$$$#$c$#:user1;user2;user3
    const std::string BROADCAST_DESCONECTADO = "$d$";   //broadcast: #$$$$#$d$#:user1;user2;user3

};

#endif // LOG_H
