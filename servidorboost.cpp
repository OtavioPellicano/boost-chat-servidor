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

            std::cout << msg.mensagemEstruturada() << std::endl;

            mMtx.lock();
            mMsgQueue.pop();
            mMtx.unlock();
        }
        //std::cout << "message Queue" << std::endl;
    }
}

void ServidorBoost::disconnectUser(asio::ip::tcp::socket *userSock)
{

    std::cout << "desconectado" << std::endl;

    std::map<asio::ip::tcp::socket*, std::string*>::iterator itMap = mMapSockNickname.begin();
    for(;itMap != mMapSockNickname.end(); ++itMap)
    {
        if(itMap->first == userSock)
        {
            break;
        }
    }

    userSock->shutdown(asio::ip::tcp::socket::shutdown_both);
    userSock->close();

    mMapSockNickname.erase(itMap);

    delete &itMap->second;  //desalocando nickname
    delete &itMap->first;   //desalocando socket



    std::cout << "Quantidade de usuários no map: " << mMapSockNickname.size() << std::endl;



}
