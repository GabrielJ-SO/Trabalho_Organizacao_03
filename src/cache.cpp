#include "cache.h"

#include "utils.h"
#include "math.h"

void inserirVia(Cache *cache, Node* no, int conjunto) {
    std::vector<Via>& vias = cache->conjuntos[conjunto].vias;

    std::string offset = std::to_string(conversorBinInt(no->address)%cache->tamanhoBloco);
    int rotulo = conversorBinInt(no->address)/cache->capacidade;

    for (int i = 0; i < cache->numeroVias; i++) {
        if (vias[i].valid == false) {

            vias[i].address = conversorBinHex(no->address, 8, true);
            vias[i].offset  =                                offset;
            vias[i].rotulo  =      conversorIntHex(rotulo, 7, true);
            vias[i].timer   =                     cache->cacheTimer;
            vias[i].valid   =                                  true;
            return;
        }
    }

    int viaMenosAcessada = 0;
    for (int i = 0; i < cache->numeroVias; i++)
        if (cache->conjuntos[conjunto].vias[viaMenosAcessada].timer < cache->conjuntos[conjunto].vias[i].timer) viaMenosAcessada = i;
    
    vias[viaMenosAcessada].address = conversorBinHex(no->address, 8, true);
    vias[viaMenosAcessada].offset  =                                offset;
    vias[viaMenosAcessada].rotulo  =      conversorIntHex(rotulo, 7, true);
    vias[viaMenosAcessada].timer   =                     cache->cacheTimer;
    vias[viaMenosAcessada].valid   =                                  true;

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

    inserirVia(cache, no, modulo);
    cache->cacheTimer += 1;

    freeNode(no);
}