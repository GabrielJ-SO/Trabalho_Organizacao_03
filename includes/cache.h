#pragma once

#include "pch.h"

typedef enum Escrita {
    WRITE_BACK,
    WRITE_THROUGH,
    NONE
} Escrita;

typedef struct  Via {
    bool        valid;
    bool        dirty;
    int         timer;
    std::string address;
    std::string tag;
    std::string rotulo;
    std::string offset;
} Via;


typedef struct Conjunto {
    std::vector<Via> vias;
} Conjunto;

struct Estatistica {
    int   numeroAcessos   = 0;
    int   acertos         = 0;
    int   faltas          = 0;
    int   blocosLidos     = 0;
    int   escritasMemoria = 0;
    int   descargaFinal   = 0;
    int   compulsorias    = 0;
    int   conflitos       = 0;
    float taxaAcertos     = 0;
};

typedef struct Cache {
    int         capacidade;
    int         tamanhoBloco;
    int         numeroVias;
    int         numeroConjuntos;
    int         cacheTimer;
    Escrita     escrita;
    Estatistica estatisticas;

    std::vector<Conjunto> conjuntos;
} Cache;



Cache* criarCache(int capacidade, int tamanhoBloco, int numeroVias, Escrita escrita);

void inserirCache(Cache* cache, Node* no);