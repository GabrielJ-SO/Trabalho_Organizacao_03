#include "utils.h"
#include "cache.h"
#include <fstream>
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
        cout << "\t\t    _________________________" << endl;
        cout << "\t\t    |   Tipo de escrita:    |" << endl;
        cout << "\t\t    |   < 1.Write-back >    |" << endl;
        cout << "\t\t    |  < 2.Write-through >  |" << endl;
        cout << "\t\t    |      < 0.Sair >       |" << endl;
        cout << "\t\t    |_______________________|" << endl;
        cout << "\t\t    |Escrita: ";
        cin >> opcao;

        system("cls");
        switch (opcao)
        {
            case '1':
                return Escrita::WRITE_BACK;
            case '2':
                return Escrita::WRITE_THROUGH;
            case '0':
                break;
        
            default:
                cout << "Digite '1' ou '2' para escolher o tipo de escrita." << endl;
                break;
        }

    } while (opcao != '0');

    return Escrita::NONE;
}

bool validarEntrada(int capacidade, int tamanhoBloco, int numeroVias, Escrita politicaEscrita) {
    if (capacidade < 0 || (tamanhoBloco < 0 || tamanhoBloco < 4) || numeroVias < 0) {
        cout << "< ERRO: Capacidade, tamanho do bloco e numero de vias, devem ser numeros maior que zero. >" << endl;
        return false;
    }
    else if ((capacidade & (capacidade - 1)) != 0 && (tamanhoBloco & (tamanhoBloco - 1)) != 0 && (numeroVias & (numeroVias - 1)) != 0) {
        cout << "< ERRO: Capacidade, tamanho do bloco e numero de vias, deve ser potencias de 2 >" << endl;        
        return false;
    }
        
    else if (capacidade < (tamanhoBloco * numeroVias)) {
        cout << "< ERRO: A capacidade nao pode ser menor do que o tamanho do bloco vezes numero de vias >" << endl;    
        return false;

    }
    else if (politicaEscrita == Escrita::NONE) {
        cout << "< ERRO: Deve ser escolhido uma politica de escrita >" << endl;
        return false; 
    }
    

    return true;
}

int lerInt(std::string rotulo) {
    int numero;
    
    cout << rotulo;
    if (!(cin >> numero)) {
        cin.clear();
        cin.ignore();
    }

    return numero;
}

Cache* iniciarCache() {
    int capacidade, tamanhoBloco, numeroVias;
    
    capacidade   = lerInt("Capacidade da cache (bytes): ");
    tamanhoBloco = lerInt("Tamanho do bloco (bytes): ");
    numeroVias   = lerInt("Numero de vias: ");    

    Escrita politicaEscrita = entradaEscrita();

    if (validarEntrada(capacidade, tamanhoBloco, numeroVias, politicaEscrita))
        return criarCache(capacidade, tamanhoBloco, numeroVias, politicaEscrita);
    else 
        return NULL;
}

bool gerarOutputExperimentos() {
    ofstream output("experimentos.txt");

    if (!output) return false;

    for (int i = 0; i < experimentos.size(); i++)
        output << experimentos[i];

    output.close();

    return true;
}

void gerarEstatisticas(Cache* cache, string path) {
    cout << endl;
    contagemExperimentos += 1;

    calcularTaxaAcertos(cache);
    descargaFinal(cache);
    
    string saida = format("+====================== (Experimento {} - {}) =============================\n|\n", contagemExperimentos, path);
    saida += format("| CONFIGURACAO   capacidade={}   bloco={} B   vias={}   conjuntos={}   escrita={}\n", cache->capacidade, cache->tamanhoBloco, cache->numeroVias, cache->numeroConjuntos, ((cache->escrita == Escrita::WRITE_BACK) ? "WB" : "WT"));
    saida += format("| CAMPOS         rotulo={} bits   indice={} bits   offset={} bits\n", cache->bitsTag, cache->bitsIdx, cache->bitsOffset);
    
    saida += "|\n";
    for (int i = 0; i < cache->estatisticas.logAcessos.size(); i++)
        saida += format("|{}", cache->estatisticas.logAcessos[i]);

    saida += "|\n|\n";

    saida += format("|acessos ............. {}\n", cache->estatisticas.numeroAcessos);
    saida += format("|acertos ............. {}\n", cache->estatisticas.acertos);
    saida += format("|faltas .............. {} (compulsorias {}, conflito {}, capacidade {})\n", cache->estatisticas.faltas, cache->estatisticas.compulsorias, cache->estatisticas.conflitos, cache->estatisticas.capacidade);
    saida += format("|taxa de acerto ...... {}%\n", cache->estatisticas.taxaAcertos);
    saida += format("|blocos lidos da mem.  {} ({} B)\n", cache->estatisticas.blocosLidos, (cache->estatisticas.blocosLidos * cache->tamanhoBloco));
    
    int bytesEscrito = ((cache->escrita == Escrita::WRITE_BACK) ?
        cache->estatisticas.escritasMemoria * cache->tamanhoBloco : cache->estatisticas.escritasMemoria * 4);

    saida += format("|escritas na memoria   {} ({} B)", cache->estatisticas.escritasMemoria, bytesEscrito);

    if (cache->escrita == Escrita::WRITE_BACK)
        saida += format( "[writebacks {} + descarga final {}]", cache->estatisticas.wb, cache->estatisticas.descargaFinal);

    saida += format("\n+=======================================================================================================\n\n");

    experimentos.push_back(saida);
}

bool simularCache() {

    string inputPath;

    cout << "Digite o nome do arquivo (com extensao): ";
    cin >> inputPath;

    AddressQueue* queue = lerArquivo(inputPath);
    Cache* cache = iniciarCache();

    if (queue == nullptr) {
        cout << "< ERRO: Nao foi possivel ler o arquivo >" << endl;
        return false;
    }

    if (cache == NULL) return false;

    int tamanho = queue->size;
    for (int i = 0; i < tamanho; i++)
        acessarCache(cache, dequeue(queue));     

    gerarEstatisticas(cache, inputPath);

    freeQueue(queue);
    liberarCache(cache);
    return true;
}

void menu() {
    char opcao;
    do
    {
        cout << "\t\t+-------------------------------+" << endl;
        cout << "\t\t|              MENU             |" << endl;
        cout << "\t\t|                               |" << endl;
        cout << "\t\t|      < 1.Simular Cache >      |" << endl;
        cout << "\t\t|                               |" << endl;
        cout << "\t\t|       < 2.Gerar Saida >       |" << endl;
        cout << "\t\t|                               |" << endl;
        cout << "\t\t|          < 0.sair >           |" << endl;
        cout << "\t\t|                               |" << endl;
        cout << "\t\t+-------------------------------+" << endl;
        cout << "\t\t|Escolha uma opcao: ";
        cin >> opcao;

        system("cls");
        switch (opcao)
        {
        case '1':
            if(simularCache())
                cout << "\t\t     Simulacao concluida. :)" << endl;
            else
                cout << endl << "\t\t Falha ao realizar simulacao :(" << endl;
            break;
        
        case '2':
            if (gerarOutputExperimentos())
                cout << "\t     Output de experimentos gerado com sucesso!" << endl;
            else
                cout << "\t\t   Falha ao gerar saida." << endl;
        
        case '0': break;
        
        default:
            cout << "Opcao Invalida." << endl;
            break;
        }

        cin.ignore();

    } while (opcao != '0');
    
}