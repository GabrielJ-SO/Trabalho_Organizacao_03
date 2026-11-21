#include "cache.h"

#include "utils.h"

void _criarVias(Cache *cache, int numeroVias, int numeroConjunto) {
    
    for (int i = 0; i < numeroConjunto; i++) {
        cache->conjuntos[i].vias = (Via*)calloc(numeroVias, sizeof(Via)); 
    }
}   

void _inserirVia(Via* via, Node* no, int numeroVias, int tempoAcesso) {
    for (int i = 0; i < numeroVias; i++) {
        if (via[i].valid == false) {
            via->bloco = no->address;
            via->valid = true;
            return;
        }
    }

    int viaMenosAcessada = 0;
    for (int i = 0; i < numeroVias; i++)
        if (via[viaMenosAcessada].timer < via[i].timer) viaMenosAcessada = i;
            
    via[viaMenosAcessada].bloco = no->address;
    via[viaMenosAcessada].valid = true;
    via[viaMenosAcessada].timer = tempoAcesso;
}

Cache* criarCache(int capacidade, int tamanhoBloco, int numeroVias, Escrita escrita) {
    Cache* cache = (Cache*)calloc(1, sizeof(Cache));
    
    cache->capacidade      = capacidade;
    cache->tamanhoBloco    = tamanhoBloco;
    cache->numeroVias      = numeroVias;
    cache->escrita         = escrita;
    cache->numeroConjuntos = capacidade/(tamanhoBloco*numeroVias);

    cache->conjuntos = (Conjunto*)calloc(cache->numeroConjuntos, sizeof(Conjunto));
    _criarVias(cache, numeroVias, cache->numeroConjuntos);

    return cache;
}

void inserirCache(Cache* cache, Node* no) {
    int enderecoDecimal = stoi(no->address, nullptr, 2);
    int idx = cache->numeroConjuntos + 1;

    int modulo = enderecoDecimal % idx;
    std::cout << "Endereco em decimal = " << enderecoDecimal << "|  Modulo = " << modulo << std::endl;

    Via* via = cache->conjuntos[modulo].vias;

    _inserirVia(via, no, cache->numeroVias, cache->cacheTimer);

    freeNode(no);
}