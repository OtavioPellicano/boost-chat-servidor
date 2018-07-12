#include "mensagem.h"

Mensagem::Mensagem(const std::string &msg)
{
    if(validarEstruturaMensagem(msg))
    {
        setOrigem(msg);
        setDestino(msg);
        setMensagem(msg);
        setCabecalho(origem(), destino());
        setCorpo(mensagem());
    }
    else
    {
        setCabecalho("","");
        setCorpo("");
    }
}

std::string Mensagem::cabecalho() const
{
    return mCabecalho;
}

void Mensagem::setCabecalho(const std::string &origem, const std::string &destino)
{
    mCabecalho = "#" + origem + "#" + destino + "#:";
    mOrigem = origem;
    mDestino = destino;
}

std::string Mensagem::corpo() const
{
    return mCorpo;
}

void Mensagem::setCorpo(const std::string &corpo)
{
    mCorpo = corpo;
    mMensagem = corpo;
}

std::string Mensagem::mensagemEstruturada()
{
    return cabecalho() + corpo() + "\n";
}

bool Mensagem::validarEstruturaMensagem(const std::string &msg)
{
    std::string str = msg;

    if(!(std::count(str.begin(), str.end(),'#') >= 3 && std::count(str.begin(), str.end(), ':') >= 1))
    {
        return false;
    }

    std::string::iterator itFirstQuadrado = std::find(str.begin(), str.end(), '#');
    std::string::iterator itSecondQuadrado = std::find(itFirstQuadrado + 1, str.end(), '#');
    std::string::iterator itThirdQuadrado = std::find(itSecondQuadrado + 1, str.end(), '#');
    std::string::iterator itPonto = std::find(str.begin(), str.end(), ':');

    if(itFirstQuadrado == str.begin())
        if(std::distance(itFirstQuadrado, itSecondQuadrado) > 1)    //deve haver pelo menos 1 letra na origem
            if(itThirdQuadrado + 1 == itPonto)                      //o ultimo # deve seguir de :
                return true;

    return false;
}

std::string Mensagem::origem() const
{
    return mOrigem;
}

void Mensagem::setOrigem(const std::string &msg)
{
    using std::string;
    string str = msg;

    string::iterator itFirstQuadrado = std::find(str.begin(), str.end(), '#');
    string::iterator itSecondQuadrado = std::find(itFirstQuadrado + 1, str.end(), '#');

    mOrigem = string(itFirstQuadrado + 1, itSecondQuadrado);

}

std::string Mensagem::destino() const
{
    return mDestino;
}

void Mensagem::setDestino(const std::string &msg)
{
    using std::string;

    string str = msg;

    string::iterator itSecondQuadrado = std::find(str.begin() + 1, str.end(), '#');
    string::iterator itThirdQuadrado = std::find(itSecondQuadrado + 1, str.end(), '#');

    mDestino = string(itSecondQuadrado + 1, itThirdQuadrado);
}

std::string Mensagem::mensagem() const
{
    return mMensagem;
}

void Mensagem::setMensagem(const std::string &msg)
{
    using std::string;
    string str = msg;

    str = string(str.begin(),std::remove(str.begin(), str.end(), '\r'));
    str = string(str.begin(),std::remove(str.begin(), str.end(), '\n'));

    string::iterator itFirst = std::find(str.begin(), str.end(), ':');

    mMensagem = string(itFirst + 1, str.end());
}

bool Mensagem::empty() const
{

    if(origem().empty() && destino().empty() && mensagem().empty())
        return true;

    return false;
}
