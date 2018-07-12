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

                        if(strMsg.find("exit") != std::string::npos)
                        {
                            disconnectUser(itMapSockNick->first);
                        }

                        std::cout << "Coloquei na pilha: " << strMsg << std::endl;
                        mMsgQueue.push(strMsg);

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
            Mensagem msg(mMsgQueue.front());

            tratarMensagem(msg);

            mMtx.lock();
            mMsgQueue.pop();
            mMtx.unlock();
        }
        //std::cout << "message Queue" << std::endl;
        boost::this_thread::sleep( boost::posix_time::millisec(delay::max));
    }
}

void ServidorBoost::disconnectUser(asio::ip::tcp::socket *userSock)
{


    std::cout << "desconectando antes do for..." << std::endl;
    std::map<asio::ip::tcp::socket*, std::string*>::iterator itMap = mMapSockNickname.begin();
    for(;itMap != mMapSockNickname.end(); ++itMap)
    {
        if(itMap->first == userSock)
        {
            break;
        }
    }

    std::cout << "desconectando antes do shtdown..." << std::endl;
    userSock->shutdown(asio::ip::tcp::socket::shutdown_both);
    userSock->close();

    std::cout << "desconectando antes do erase..." << std::endl;
    mMapSockNickname.erase(itMap);

    std::cout << "desconectando antes do delete nickname..." << std::endl;
//    delete &itMap->second;  //desalocando nickname
    std::cout << "desconectando antes do delete socket" << std::endl;
//    delete &itMap->first;   //desalocando socket

    std::cout << "desconectado" << std::endl;


    std::cout << "Quantidade de usuários no map: " << mMapSockNickname.size() << std::endl;



}

void ServidorBoost::tratarMensagem(Mensagem &msg)
{

    //nickname validação
    /**
     * @brief The Mensagem class
     * Organiza o corpo e o cabecalho da mensagem
     *
     * Padroes de envio:
     *
     * #origem#destino#:mensagem    //padrão
     *
     * #origem##:                   //confirmação de nickname
     *
     * #$$$$#$c$#:user1;user2;user3 //brodcast para usuário conectado
     *
     * #$$$$#$d$#:user1;user2;user3 //broadcast para usuário desconectado
     *
     * ###:                         //nickname invalido ou qualquer msg invalida
     *
     */
    //mensagem fora da estrutura


    mMtx.lock();
    for(auto itMapSockNick = mMapSockNickname.begin(); itMapSockNick != mMapSockNickname.end(); ++itMapSockNick)
    {
        //validando nickname
        if(!msg.origem().empty() && msg.destino().empty() && msg.mensagem().empty())
        {
            std::cout << "tratar mensagem dentro do if" << std::endl;
            addNickname(itMapSockNick->first, new std::string(msg.origem()));

            break;
        }
    }
    mMtx.unlock();


    std::cout << msg.mensagemEstruturada() << std::endl;
}

void ServidorBoost::sendMsg(asio::ip::tcp::socket* userSock, Mensagem &msg)
{

    asio::write(*userSock, asio::buffer(msg.mensagemEstruturada()));
}

void ServidorBoost::addNickname(asio::ip::tcp::socket* userSock, std::string* nick)
{
    Mensagem msg;


    if(*nickname(userSock) == "")
    {
        std::cout << "Adding nickname" << *nick << std::endl;
        //caso nao exista o nickname na sala
        msg.setCabecalho(*nick, "");
        msg.setCorpo("");
        mMapSockNickname[userSock] = nick;
    }

    sendMsg(userSock, msg);

    if(msg.empty())
    {
        std::cout << "desconectando..." << std::endl;
        disconnectUser(userSock);
    }

}

std::string *ServidorBoost::nickname(asio::ip::tcp::socket *userSock)
{
    std::cout << "procurando nickname" << std::endl;
    auto itMap = mMapSockNickname.begin();
    for(; itMap != mMapSockNickname.end(); ++itMap)
    {
        if(itMap->first == userSock)
            break;
    }

    std::cout << "achei nickname: " << *itMap->second << std::endl;


    return itMap->second;
}

asio::ip::tcp::socket *ServidorBoost::socketOrigem(Mensagem &msg)
{
    mMtx.lock();
    auto itMap = mMapSockNickname.begin();
    for(; itMap != mMapSockNickname.end(); ++itMap)
    {
        if(*(itMap->second) == msg.origem())
            break;
    }
    mMtx.unlock();

    return itMap->first;
}
