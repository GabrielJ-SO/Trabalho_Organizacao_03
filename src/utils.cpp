#include "utils.h"
#include "cache.h"
#include <format>

using namespace std;

string conversorHexBin(string hex) {

    string bin = "";

    for (char c : hex) {
        switch (toupper(c)) 
        {
            case '0': bin += "0000"; break; case '1': bin += "0001"; break;
            case '2': bin += "0010"; break; case '3': bin += "0011"; break;
            case '4': bin += "0100"; break; case '5': bin += "0101"; break;
            case '6': bin += "0110"; break; case '7': bin += "0111"; break;
            case '8': bin += "1000"; break; case '9': bin += "1001"; break;
            case 'A': bin += "1010"; break; case 'B': bin += "1011"; break;
            case 'C': bin += "1100"; break; case 'D': bin += "1101"; break;
            case 'E': bin += "1110"; break; case 'F': bin += "1111"; break;
            default: bin += "";
        }
    }

    return bin;
}

string conversorBinHex(string bin, int preencher, bool prefixo) {
    int decimal = conversorBinInt(bin);
    if (prefixo) {
        preencher += 2;
        return format("{:#0{}X}", decimal, preencher);
    } 
    else {
        return format("{:0{}X}", decimal, preencher);
    }
        
}

std::string conversorIntHex(int numero, int preencher, bool prefixo) {
    if (prefixo) {
        preencher += 2;
        return format("{:#0{}X}", numero, preencher);
    }
    else {
        return format("{:0{}X}", numero, preencher);
    }
        
}

int conversorBinInt(string bin) {
    return stoi(bin, nullptr, 2);
}


Escrita entradaEscrita() {
    char opcao;

    do {
        cout << "Tipo de escrita:\n < 1-Write-back >\n < 2-Write-through >\n < 0-Sair >\n  Escrita: ";
        cin >> opcao;

        switch (opcao)
        {
            case '1':
                return Escrita::WRITE_BACK;
            case '2':
                return Escrita::WRITE_THROUGH;
        
            default:
                cout << "Digite '1' ou '2' para escolher o tipo de escrita.";
                break;
        }

    } while (opcao != 0);

    return Escrita::NONE;
}

bool validarEntrada(int capacidade, int tamanhoBloco, int numeroVias, Escrita politicaEscrita) {
    if (capacidade < 0 || (tamanhoBloco < 0 || tamanhoBloco < 4) || numeroVias < 0)
        return false;
    else if ((capacidade & (capacidade - 1)) != 0 && (tamanhoBloco & (tamanhoBloco - 1)) != 0 && (numeroVias & (numeroVias - 1)) != 0)
        return false;
    else if (capacidade < (tamanhoBloco * numeroVias))
        return false;
    else if (politicaEscrita == Escrita::NONE)
        return false;
    

    return true;
}

Cache* iniciarCache() {
    int capacidade, tamanhoBloco, numeroVias;
    
    cout << "Capacidade da cache (bytes): ";    
    cin >> capacidade;
    cout << "Tamanho do bloco (bytes): ";
    cin >> tamanhoBloco;
    cout << "Numero de vias: ";
    cin >> numeroVias;

    Escrita politicaEscrita = entradaEscrita();

    if (validarEntrada(capacidade, tamanhoBloco, numeroVias, politicaEscrita))
        return criarCache(capacidade, tamanhoBloco, numeroVias, politicaEscrita);
    else 
        return NULL;
}

bool simularCache() {

    string inputPath;

    cout << "Digite o nome do arquivo (com extensao)." << endl;
    cin >> inputPath;

    AddressQueue* queue = lerArquivo(inputPath);
    Cache* cache = iniciarCache();

    if (cache == NULL) return false;

    int tamanho = queue->size;

    for (int i = 0; i < tamanho; i++)
        acessarCache(cache, dequeue(queue));     
    

    for(int i = 0; i < cache->numeroConjuntos; i++) {
        cout << "Conjunto " << i;
        for(int j = 0; j < cache->numeroVias; j++) {
            cout << " | Via = " << j << ": " << " rot = " << cache->conjuntos[i].vias[j].rotulo << " - idx = " << i;
        }
        cout << endl;
    }

    cout << endl << endl;

    cout << " CONFIGURACAO   capacidade=" << cache->capacidade << " B " << " bloco=" << cache->tamanhoBloco << " B " << " vias=" << cache->numeroVias << " conjuntos=" << cache->numeroConjuntos << " escrita=" << ((cache->escrita == Escrita::WRITE_BACK) ? "WB" : "WT") << endl;
    cout << " CAMPOS         rotulo=" << cache->bitsTag << " bits " << "  indice=" << cache->bitsIdx << " bits " << "  offset=" << cache->bitsOffset << " bits " << endl;
    cout << endl;

    for (int i = 0; i < cache->estatisticas.logAcessos.size(); i++) {
        cout << cache->estatisticas.logAcessos[i] << endl;
    }

    return true;

}

void menu() {
    char opcao;
    do
    {
        cout << "\t+---------------------+" << endl;
        cout << "\t|         Menu        |" << endl;
        cout << "\t| < 1.Simular Cache > |" << endl;
        cout << "\t|     < 0.sair >      |" << endl;
        cout << "\t+---------------------+" << endl;
        cout << "Escolha uma opcao: ";
        cin >> opcao;

        switch (opcao)
        {
        case '1':
            if(simularCache())
                cout << "Simulacao concluida." << endl;
            else
                cout << "Erro ao realizar simulacao" << endl;
            break;
        
        case '0': break;
        
        default:
            cout << "Opcao Invalida.";
            break;
        }

    } while (opcao != '0');
    
}