#pragma once

#include "pch.h"

typedef enum Escrita {
    WRITE_BACK,
    WRITE_THROUGH
} Escrita;

/* typedef enum Substituicao {
    LRU

} Substituicao; */

typedef struct  Via {
    bool valid, dirt;
    int timer;
    std::string bloco;
    std::string tag;
} Via;


typedef struct Conjunto {
    Via* vias;

} Conjunto;

typedef struct Cache {
    int capacidade;
    int tamanhoBloco;
    int numeroVias;
    int numeroConjuntos;
    int cacheTimer;
    Escrita escrita;
    Conjunto* conjuntos;
} Cache;


Cache* criarCache(int capacidade, int tamanhoBloco, int numeroVias, Escrita escrita);

void inserirCache(Cache* cache, Node* no);