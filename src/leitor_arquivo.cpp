#include "leitor_arquivo.h"
#include "utils.h"
#include "fstream"

std::string formatadorInst(std::string str);
Action getAction(std::string linha);

AddressQueue* lerArquivo(std::string path) {
    std::ifstream arquivo(path);

    if(!arquivo) return {};

    std::string linha;
    AddressQueue* queue = createQueue();

    while (std::getline(arquivo, linha))
    {   
        int i = 0;
        Action action = Action::UNDEFINED;
        while (i < linha.size())
        {
            if (linha[i] == ' ') {
                linha = linha.erase(i, 1);
            }
            else if (linha[i] == '#') {
                linha = linha.substr(0, i);
                break;
            }
            else if (getAction(linha) != Action::UNDEFINED) {
                action = getAction(linha);
                linha = linha.erase(i, 1);
            }
            else {
                i++;
            }
        }
        linha = formatadorInst(linha);

        if (!linha.empty()) enqueue(queue, linha, action);   
    }

    arquivo.close();

    return queue;
}


std::string confereBin(std::string bin) {
    if (bin.size() == 34) { 
        bin.erase(0, 2);
    }

    if (bin.find_first_not_of("01") != std::string::npos)
        return "";
    else
        return bin;
}

std::string confereHexa(std::string hex) {
    if (hex.size() == 10) {
        hex.erase(0, 2);
    }

    std::string validos = "0123456789abcdefABCDEF";
    if (hex.find_first_not_of(validos) != std::string::npos)
        return "";
    else 
        return conversorHexBin(hex);    
}

std::string formatadorInst(std::string str) {
    if (str.size() == 8 || str.size() == 10) {
        return confereHexa(str);
    }
    else if (str.size() == 32 || str.size() == 34) {
        return confereBin(str);
    }
    else {
        return "";
    }
}

Action getAction(std::string linha) {
    Action action = Action::UNDEFINED;

    if (linha[0] == 'R')
        action = Action::READ;
    else if (linha[0] == 'W')
        action = Action::WRITE;

    return action;
}