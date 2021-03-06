#include "servidorboost.h"

ServidorBoost::ServidorBoost()
{
    mAcceptor = new asio::ip::tcp::acceptor(mIos, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), 1312));

}

void ServidorBoost::incommingConnectionLoop()
{
    std::cout << "Conecte a porta 1312" << std::endl;
    for(;;)
    {
        asio::ip::tcp::socket* userSock(new asio::ip::tcp::socket(mIos));

        mAcceptor->accept(*userSock);

        std::cout << "Novo usuário conectado. Validando nickname..." << std::endl;

        mMtx.lock();
        mMapSockNickname[userSock] = new std::string("");
        mMtx.unlock();

        std::cout << "Quantidade de usuários no map: " << mMapSockNickname.size() << std::endl;

    }

}

void ServidorBoost::readyReadLoop()
{
    for(;;)
    {
        if(!mMapSockNickname.empty())
        {
            mMtx.lock();

            for(auto itMapSockNick = mMapSockNickname.begin(); itMapSockNick != mMapSockNickname.end(); ++itMapSockNick)
            {

                    if(itMapSockNick->first->available())   //verificando se existe informação no buffer do socket
                    {
                        asio::streambuf buf;

                        asio::read_until(*itMapSockNick->first, buf, '\n');

                        std::string strMsg;
                        std::istream inputStream(&buf);
                        std::getline(inputStream, strMsg);

                        mMsgQueue.push(pairSock_ptrStr(itMapSockNick->first, strMsg));

                    }
            }

            mMtx.unlock();
        }
    }
}

void ServidorBoost::messageQueueLoop()
{
    for(;;)
    {
        if(!mMsgQueue.empty())
        {
            auto nextSockMsg = mMsgQueue.front();

            Mensagem msg(nextSockMsg.second);

            mMtx.lock();
            for(auto itSockUser = mMapSockNickname.begin(); itSockUser != mMapSockNickname.end(); ++itSockUser)
            {
                //validando e add nickname
                if(!msg.origem().empty() && msg.destino().empty() && msg.mensagem().empty())
                {
                    if(addNickname(nextSockMsg.first, new std::string(msg.origem())))
                    {
                        //enviando broadcast para os outros usuários
                        sendBroadcast(nextSockMsg.first, msg.origem(), CONECTADO);

                    }
                    break;
                }

                //redirecionando mensagem
                if(!msg.origem().empty() && !msg.destino().empty() && !msg.mensagem().empty())
                {
                    redirecionarMensagem(msg.origem(), msg.destino(), msg.mensagem());
                    break;
                }

                //desconectando usuário
                if(msg.origem().empty() && msg.destino().empty() && msg.mensagem().empty())
                {
                    disconnectUser(nextSockMsg.first);
                    break;
                }

            }
            mMtx.unlock();

            mMtx.lock();
            mMsgQueue.pop();
            mMtx.unlock();
        }
        boost::this_thread::sleep( boost::posix_time::millisec(delay::max));
    }
}

void ServidorBoost::logQueueLoop()
{
    for(;;)
    {
        if(!mLogQueue.empty())
        {
            auto msg = mLogQueue.front();

            mLog.salvarLog(msg.origem(), msg.destino(), msg.mensagem());

            mLogQueue.pop();    //não preciso dar lock aqui, essa fila só será utilizada por uma thread
        }

        //Para caso haja multiplos salvamentos
        boost::this_thread::sleep( boost::posix_time::millisec(delay::max));
    }
}

void ServidorBoost::disconnectUser(asio::ip::tcp::socket *userSock)
{

    std::map<asio::ip::tcp::socket*, std::string*>::iterator itMap = mMapSockNickname.begin();
    for(;itMap != mMapSockNickname.end(); ++itMap)
    {
        if(itMap->first == userSock)
        {
            break;
        }
    }

    sendBroadcast(userSock, *nickname(userSock), DESCONECTADO);
    userSock->shutdown(asio::ip::tcp::socket::shutdown_both);
    userSock->close();

    mMapSockNickname.erase(itMap);

    std::cout << "Quantidade de usuários no map: " << mMapSockNickname.size() << std::endl;

}


void ServidorBoost::sendMsg(asio::ip::tcp::socket* userSock, Mensagem &msg)
{

    asio::write(*userSock, asio::buffer(msg.mensagemEstruturada()));
    boost::this_thread::sleep(boost::posix_time::millisec(delay::min));
}

bool ServidorBoost::addNickname(asio::ip::tcp::socket* userSock, std::string* nick)
{
    Mensagem msg;
    bool result;

    if(validarNickname(*nick))
    {
        //caso nao exista o nickname na sala
        msg.setCabecalho(*nick, "");
        msg.setCorpo("");
        mMapSockNickname[userSock] = nick;
        result = true;
    }

    sendMsg(userSock, msg);

    if(msg.empty())
    {
        result = false;
        disconnectUser(userSock);
    }

    return result;

}

std::string *ServidorBoost::nickname(asio::ip::tcp::socket *userSock)
{
    auto itMap = mMapSockNickname.begin();
    for(; itMap != mMapSockNickname.end(); ++itMap)
    {
        if(itMap->first == userSock)
            break;
    }

    return itMap->second;
}


bool ServidorBoost::validarNickname(const std::string &nick)
{

    for(auto itMap = mMapSockNickname.begin(); itMap != mMapSockNickname.end(); ++itMap)
    {
        if(*(itMap->second) == nick)
            return false;
    }

    return true;
}

void ServidorBoost::sendBroadcast(asio::ip::tcp::socket *sock, const std::string& org, const ServidorBoost::tipoConexao &tipo)
{
    using std::string;
    Mensagem msg;

    if(tipo == CONECTADO)
    {
        msg.setCabecalho(BROADCAST_KEY, BROADCAST_CONECTADO);
    }
    else
    {
        msg.setCabecalho(BROADCAST_KEY, BROADCAST_DESCONECTADO);
    }

    //carregando mensagens
    string strMsg;
    for(auto itSockNick = mMapSockNickname.begin(); itSockNick != mMapSockNickname.end(); ++itSockNick)
    {
        if(itSockNick->first != sock)
        {
            strMsg += *itSockNick->second + ";";
        }
    }
    strMsg += org;
    msg.setCorpo(strMsg);

    //Enviando broadcast
    for(auto itSockNick = mMapSockNickname.begin(); itSockNick != mMapSockNickname.end(); ++itSockNick)
    {
        sendMsg(itSockNick->first, msg);
    }

    mLogQueue.push(msg);

}

void ServidorBoost::redirecionarMensagem(const std::string &org, const std::string &dst, const std::string &msg)
{
    asio::ip::tcp::socket* socketOrigem;
    asio::ip::tcp::socket* socketDestino;

    for(auto itSockNick = mMapSockNickname.begin(); itSockNick != mMapSockNickname.end(); ++itSockNick)
    {
        if(*itSockNick->second == org)
        {
            socketOrigem = itSockNick->first;
        }
        if(*itSockNick->second == dst)
        {
            socketDestino = itSockNick->first;
        }
    }

    Mensagem newMsg;
    newMsg.setCabecalho(org, dst);
    newMsg.setCorpo(msg);

    sendMsg(socketOrigem, newMsg);
    sendMsg(socketDestino, newMsg);

    mLogQueue.push(newMsg);

}
