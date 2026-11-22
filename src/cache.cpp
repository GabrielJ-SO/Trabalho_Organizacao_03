#include "cache.h"

#include "utils.h"
#include <format>
#include <cmath>

Cache* criarCache(int capacidade, int tamanhoBloco, int numeroVias, Escrita escrita) {
    Cache* cache = (Cache*)calloc(1, sizeof(Cache));
    
    cache->capacidade      = capacidade;
    cache->tamanhoBloco    = tamanhoBloco;
    cache->numeroVias      = numeroVias;
    cache->escrita         = escrita;
    cache->numeroConjuntos = capacidade/(tamanhoBloco*numeroVias);

    cache->bitsIdx    = std::round(std::log2(cache->numeroConjuntos));
    cache->bitsOffset = std::round(std::log2(tamanhoBloco));
    cache->bitsTag    = 32 - cache->bitsIdx - cache->bitsOffset;

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

std::string reconstruirEndereco(int tag, int idx, int offset, int bitsTag, int bitsIdx, int bitsOffset) {

    std::string strTag    = std::format("{:0{}b}", tag, bitsTag);
    std::string strOffset = std::format("{:0{}b}", offset, bitsOffset);

    std::string strIdx;
    if (bitsIdx == 0)
        strIdx = "";
    else
        strIdx = std::format("{:0{}b}", idx, bitsIdx);

    std::string endereco = conversorBinHex((strTag + strIdx + strOffset), 8, true);

    return endereco;
}

bool conflito(std::vector<std::string> blocosInserido, std::string tag) {
    for (int i = 0; i < blocosInserido.size(); i++) {
        if (blocosInserido[i] == tag)
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
            vias[i].lru     = cache->contadorLRU;
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
            cache->estatisticas.blocosLidos   += 1;

            std::string enderecoHex = conversorBinHex(no->address, 8, true);
            std::string offset      = getOffset(no->address, cache->tamanhoBloco);
            int acesso              = cache->estatisticas.numeroAcessos;

            std::string log;
            if (conflito(cache->conjuntos[idx].blocosInserido, vias[i].rotulo)) {
                cache->estatisticas.conflitos += 1;
                log = std::format("{} {} {} rot={} idx={} off={} FALTA conflito\n", acesso, acao, enderecoHex, vias[i].rotulo, idx, offset);
            }
            else {
                cache->estatisticas.compulsorias += 1;
                cache->conjuntos[idx].blocosInserido.push_back(vias[i].rotulo);
                log = std::format("{} {} {} rot={} idx={} off={} FALTA compulsoria\n", acesso, acao, enderecoHex, vias[i].rotulo, idx, offset);
            }
            
            cache->estatisticas.logAcessos.push_back(log);

            return true;
        }
    }

    return false;
}

bool substituirPorLRU(Cache *cache, Node* no, int idx) {
    std::vector<Via>& vias = cache->conjuntos[idx].vias;

    int lru = 0;
    for (int i = 0; i < cache->numeroVias; i++)
        if (vias[lru].lru < vias[i].lru) lru = i;

    if (vias[lru].dirty && cache->escrita == Escrita::WRITE_BACK) {
        cache->estatisticas.escritasMemoria += 1;
        vias[lru].dirty = 0;
    }
    std::string enderecoExpulso = reconstruirEndereco(std::stoi(vias[lru].rotulo, nullptr, 16), idx, 0, cache->bitsTag, cache->bitsIdx, cache->bitsOffset);

    vias[lru].rotulo  = getRotulo(no->address, cache->capacidade);
    vias[lru].lru     = cache->contadorLRU;
    vias[lru].valid   = true;

    char acao = 'R';
    if (no->action == Action::WRITE && Escrita::WRITE_BACK) {
        vias[lru].dirty = 1;
        acao = 'W';
    }
                
    if (no->action == Action::WRITE && cache->escrita == Escrita::WRITE_THROUGH) {
        cache->estatisticas.escritasMemoria += 1;
        acao = 'W';
    }

    cache->estatisticas.numeroAcessos += 1;
    cache->estatisticas.blocosLidos   += 1;

    std::string enderecoHex = conversorBinHex(no->address, 8, true);
    std::string offset      = getOffset(no->address, cache->tamanhoBloco);
    int acesso              = cache->estatisticas.numeroAcessos;

    std::string log;
    if (conflito(cache->conjuntos[idx].blocosInserido, vias[lru].rotulo)) {
        cache->estatisticas.conflitos += 1;
        log = std::format("{} {} {} rot={} idx={} off={} FALTA conflito expulsa {}\n", acesso, acao, enderecoHex, vias[lru].rotulo, idx, offset, enderecoExpulso);
    }
    else {
        cache->estatisticas.compulsorias += 1;
        cache->conjuntos[idx].blocosInserido.push_back(vias[lru].rotulo);
        log = std::format("{} {} {} rot={} idx={} off={} FALTA compulsoria expulsa {}\n", acesso, acao, enderecoHex, vias[lru].rotulo, idx, offset, enderecoExpulso);
    }

    cache->estatisticas.logAcessos.push_back(log);

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
            vias[i].lru = cache->contadorLRU;

            
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

    cache->contadorLRU += 1;
    if (!acerto(cache, no, idx))
        inserirConjuntoCache(cache, no, idx);


    freeNode(no);
}