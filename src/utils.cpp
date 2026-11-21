#include "utils.h"
#include "cache.h"

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

int conversorBinInt(string bin) {
    return stoi(bin, nullptr, 2);
}


void readFile(string path) {
    
    AddressQueue* queue = lerArquivo(path);

    Cache* cache = criarCache(128, 16, 1, Escrita::WRITE_BACK);

    int tamanho = queue->size;
    cout << "tamanho: " << tamanho << endl;

    for (int i = 0; i < tamanho; i++)
        inserirCache(cache, dequeue(queue));     
    

    for(int i = 0; i < cache->numeroConjuntos; i++) {
    cout << "Conjunto " << i;
        for(int j = 0; j< cache->numeroVias; j++) {
            cout << " | Via: " << j << " Address: " << cache->conjuntos[i].vias[j].address;
        }
        cout << endl;
    }
    

}