#ifndef SERVIDORBOOST_H
#define SERVIDORBOOST_H

#include <iostream>
#include <map>
#include <queue>
#include <functional>
#include <algorithm>
#include "mensagem.h"

#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace boost;

class ServidorBoost
{
public:
    enum delay{
        min = 100,
        max = 200
    };
public:
    ServidorBoost();

    void incommingConnectionLoop(); //Aguarda novo cliente
    void readyReadLoop();       //Aguarda mensagem no buff
    void messageQueueLoop();    //Fila de mensagem

private:
    void disconnectUser(asio::ip::tcp::socket* userSock);
    void tratarMensagem(Mensagem &msg);
    void sendMsg(asio::ip::tcp::socket *userSock, Mensagem &msg);

    void addNickname(asio::ip::tcp::socket *userSock, std::string* nick);
    std::string* nickname(asio::ip::tcp::socket *userSock);

    asio::ip::tcp::socket* socketOrigem(Mensagem &msg);


private:
    const std::string BROADCAST_KEY = "$$$";
    const std::string BROADCAST_CONECTADO = "$c$";
    const std::string BROADCAST_DESCONECTADO = "$d$";

    asio::io_service mIos;
    asio::ip::tcp::acceptor *mAcceptor;
    boost::mutex mMtx;

    std::map<asio::ip::tcp::socket*, std::string*> mMapSockNickname;
    std::queue<std::string> mMsgQueue;

//    const unsigned int MESSAGE_SIZE = 1024;
};

#endif // SERVIDORBOOST_H
