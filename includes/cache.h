#pragma once

#include "pch.h"

typedef enum Escrita {
    WRITE_BACK,
    WRITE_THROUGH,
    NONE
} Escrita;

typedef struct  Via {
    bool valid, dirty;
    int timer;
    std::string address;
    std::string tag;
    std::string rotulo;
    std::string offset;
} Via;


typedef struct Conjunto {
    std::vector<Via> vias;
} Conjunto;

typedef struct Cache {
    int capacidade;
    int tamanhoBloco;
    int numeroVias;
    int numeroConjuntos;
    int cacheTimer;
    Escrita escrita;
    std::vector<Conjunto> conjuntos;
} Cache;


Cache* criarCache(int capacidade, int tamanhoBloco, int numeroVias, Escrita escrita);

void inserirCache(Cache* cache, Node* no);