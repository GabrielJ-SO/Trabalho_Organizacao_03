#include "cache.h"

#include "utils.h"
#include "math.h"

void inserirVia(Cache *cache, Node* no, int conjunto) {
    
    for (int i = 0; i < cache->numeroVias; i++) {
        if (cache->conjuntos[conjunto].vias[i].valid == false) {
            cache->conjuntos[conjunto].vias[i].address = no->address;
            cache->conjuntos[conjunto].vias[i].valid = true;
            cache->conjuntos[conjunto].vias[i].timer = cache->cacheTimer;
            return;
        }
    }

    int viaMenosAcessada = 0;
    for (int i = 0; i < cache->numeroVias; i++)
        if (cache->conjuntos[conjunto].vias[viaMenosAcessada].timer < cache->conjuntos[conjunto].vias[i].timer) viaMenosAcessada = i;
            
    cache->conjuntos[conjunto].vias[viaMenosAcessada].address = no->address;
    cache->conjuntos[conjunto].vias[viaMenosAcessada].valid = true;
    cache->conjuntos[conjunto].vias[viaMenosAcessada].timer = cache->cacheTimer;

}

Cache* criarCache(int capacidade, int tamanhoBloco, int numeroVias, Escrita escrita) {
    Cache* cache = (Cache*)calloc(1, sizeof(Cache));
    
    cache->capacidade      = capacidade;
    cache->tamanhoBloco    = tamanhoBloco;
    cache->numeroVias      = numeroVias;
    cache->escrita         = escrita;
    cache->numeroConjuntos = capacidade/(tamanhoBloco*numeroVias);

    cache->conjuntos.resize(cache->numeroConjuntos);

    for (int i = 0; i < cache->numeroConjuntos; i++) 
        cache->conjuntos[i].vias.resize(numeroVias); 

    return cache;
}

void inserirCache(Cache* cache, Node* no) {
    int enderecoDecimal = conversorBinInt(no->address);
    int idx = cache->numeroConjuntos;

    int modulo = (enderecoDecimal/cache->tamanhoBloco) % idx;
    std::cout << "Endereco em decimal = " << enderecoDecimal << "|  Modulo = " << modulo << std::endl;

    inserirVia(cache, no, modulo);
    cache->cacheTimer += 1;

    freeNode(no);
}