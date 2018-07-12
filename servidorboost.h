#ifndef SERVIDORBOOST_H
#define SERVIDORBOOST_H

#include <iostream>
#include <map>
#include <queue>
#include <functional>
#include <algorithm>
#include "mensagem.h"
#include "log.h"

#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace boost;

typedef std::pair<asio::ip::tcp::socket*, std::string> pairSock_ptrStr;

class ServidorBoost
{


private:
    enum delay{
        min = 100,
        max = 200
    };

    enum tipoConexao{
        CONECTADO,
        DESCONECTADO
    };

public:
    ServidorBoost();

    void incommingConnectionLoop(); //Aguarda novo cliente
    void readyReadLoop();       //Aguarda mensagem no buff
    void messageQueueLoop();    //Fila de mensagem
    void logQueueLoop();        //fila de log

private:
    void disconnectUser(asio::ip::tcp::socket* userSock);
    void sendMsg(asio::ip::tcp::socket *userSock, Mensagem &msg);

    bool addNickname(asio::ip::tcp::socket *userSock, std::string* nick);
    std::string* nickname(asio::ip::tcp::socket *userSock);

    bool validarNickname(const std::string &nick);

    void sendBroadcast(asio::ip::tcp::socket* sock, const std::string &org, const tipoConexao& tipo);

    void redirecionarMensagem(const std::string &org, const std::string &dst, const std::string &msg);

private:

    const std::string BROADCAST_KEY = "$$$";
    const std::string BROADCAST_CONECTADO = "$c$";
    const std::string BROADCAST_DESCONECTADO = "$d$";

    asio::io_service mIos;
    asio::ip::tcp::acceptor *mAcceptor;
    boost::mutex mMtx;

    std::map<asio::ip::tcp::socket*, std::string*> mMapSockNickname;
    std::queue<pairSock_ptrStr> mMsgQueue;

    std::queue<Mensagem> mLogQueue;

    Log mLog;

};

#endif // SERVIDORBOOST_H
