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
    int         lru;
    std::string rotulo;
} Via;


typedef struct Conjunto {
    std::vector<Via> vias;
    std::vector<std::string> blocosInserido;
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

    std::vector<std::string> logAcessos;
};

typedef struct Cache {
    int         capacidade;
    int         tamanhoBloco;
    int         numeroVias;
    int         numeroConjuntos;
    int         bitsTag;
    int         bitsIdx;
    int         bitsOffset;

    int         contadorLRU;
    Escrita     escrita;
    Estatistica estatisticas;

    std::vector<Conjunto> conjuntos;
} Cache;



Cache* criarCache(int capacidade, int tamanhoBloco, int numeroVias, Escrita escrita);

void acessarCache(Cache* cache, Node* no);