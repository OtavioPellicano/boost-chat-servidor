#ifndef MENSAGEM_H
#define MENSAGEM_H

#include <string>
#include <algorithm>

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


class Mensagem
{
public:
    Mensagem(const std::string &msg = "");

    void setCabecalho(const std::string& origem, const std::string& destino);

    void setCorpo(const std::string &corpo);

    std::string mensagemEstruturada();

    bool validarEstruturaMensagem(const std::string &msg);

    std::string origem() const;

    std::string destino() const;

    std::string mensagem() const;

    bool empty() const;

private:
    std::string cabecalho() const;
    std::string corpo() const;
    void setMensagem(const std::string &msg);
    void setDestino(const std::string &msg);
    void setOrigem(const std::string &msg);

private:

    std::string mCabecalho = "";
    std::string mCorpo = "";

    std::string mOrigem = "";
    std::string mDestino = "";
    std::string mMensagem = "";

};

#endif // MENSAGEM_H
