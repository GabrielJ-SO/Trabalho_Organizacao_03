#include "cache.h"

#include "utils.h"
#include <format>

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


std::string getRotulo(std::string endereco, int capacidade) {
    int rotulo = conversorBinInt(endereco)/capacidade;
    return conversorIntHex(rotulo, 7, true);
}

std::string getOffset(std::string endereco, int tamanhoBloco) {
    return std::to_string(conversorBinInt(endereco)%tamanhoBloco);
}

bool conflito(std::vector<std::string> enderecosInserido, std::string endereco) {
    for (int i = 0; i < enderecosInserido.size(); i++) {
        if (enderecosInserido[i] == endereco)
            return true;
    }

    return false;
}

bool inserirEmViaLivre(Cache *cache, Node* no, int idx) {
    std::vector<Via>& vias = cache->conjuntos[idx].vias;

    for (int i = 0; i < cache->numeroVias; i++) 
    {
        if (vias[i].valid == false) 
        {
            vias[i].rotulo  = getRotulo(no->address, cache->capacidade);
            vias[i].lru     = cache->cacheTimer;
            vias[i].valid   = true;

            char acao = 'R';
            if (no->action == Action::WRITE && Escrita::WRITE_BACK) {
                vias[i].dirty = 1;
                acao = 'W';
            }
                
            if (no->action == Action::WRITE && cache->escrita == Escrita::WRITE_THROUGH) {
                cache->estatisticas.escritasMemoria += 1;
                acao = 'W';
            }

            cache->estatisticas.numeroAcessos += 1;
            cache->estatisticas.compulsorias  += 1;

            std::string enderecoHex = conversorBinHex(no->address, 8, true);
            std::string offset      = getOffset(no->address, cache->tamanhoBloco);
            int acesso              = cache->estatisticas.numeroAcessos;

            std::string log = std::format("{} {} {} rot={} idx={} off={} FALTA compulsoria\n", acesso, acao, enderecoHex, vias[i].rotulo, idx, offset);
            cache->estatisticas.logAcessos.push_back(log);
            cache->conjuntos[idx].enderecosInserido.push_back(no->address);

            return true;
        }
    }

    return false;
}

bool substituirPorLRU(Cache *cache, Node* no, int idx) {
    std::vector<Via>& vias = cache->conjuntos[idx].vias;

    int viaMenosAcessada = 0;
    for (int i = 0; i < cache->numeroVias; i++)
        if (vias[viaMenosAcessada].lru < vias[i].lru) viaMenosAcessada = i;
    
    vias[viaMenosAcessada].rotulo  = getRotulo(no->address, cache->capacidade);
    vias[viaMenosAcessada].lru     = cache->cacheTimer;
    vias[viaMenosAcessada].valid   = true;

    return false;
}

void inserirConjuntoCache(Cache *cache, Node* no, int idx) {
    
    if (inserirEmViaLivre(cache, no, idx) == false)
        substituirPorLRU(cache, no, idx);

}

bool acerto(Cache* cache, Node* no, int idx) {
    std::vector<Via>& vias = cache->conjuntos[idx].vias;

    std::string rotulo = getRotulo(no->address, cache->capacidade);
    
    for (int i = 0; i < vias.size(); i++) 
    {
        if (vias[i].rotulo == rotulo)
        {   
            char acao = 'R';
            if (no->action == Action::WRITE && Escrita::WRITE_BACK) {
                vias[i].dirty = 1;
                acao = 'W';
            }
                

            if (no->action == Action::WRITE && cache->escrita == Escrita::WRITE_THROUGH) {
                cache->estatisticas.escritasMemoria += 1;
                acao = 'W';
            }
                

            cache->estatisticas.acertos += 1;
            cache->estatisticas.numeroAcessos += 1;
            vias[i].lru = cache->cacheTimer;

            
            std::string enderecoHex = conversorBinHex(no->address, 8, true);
            std::string offset      = getOffset(no->address, cache->tamanhoBloco);
        
            int acesso = cache->estatisticas.numeroAcessos;
            std::string log = std::format("{} {} {} rot={} idx={} off={} ACERTO\n", acesso, acao, enderecoHex, rotulo, idx, offset);
            cache->estatisticas.logAcessos.push_back(log);

            return true;
        }
            
    }

    return false;
}

void acessarCache(Cache* cache, Node* no) {
    int enderecoDecimal = conversorBinInt(no->address);

    int idx = (enderecoDecimal/cache->tamanhoBloco) % cache->numeroConjuntos;

    if (!acerto(cache, no, idx))
        inserirConjuntoCache(cache, no, idx);

    cache->cacheTimer += 1;

    freeNode(no);
}