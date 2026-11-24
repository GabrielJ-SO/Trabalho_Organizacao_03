#pragma once

#include "pch.h"
#include "address_queue.h"
#include "leitor_arquivo.h"

std::string conversorHexBin(std::string hex);
std::string conversorBinHex(std::string bin, int preencher = 0, bool prefixo = false);
std::string conversorIntHex(int numero, int preencher = 0, bool prefixo = false);
int conversorBinInt(std::string bin);

static int contagemExperimentos = 0;
inline std::vector<std::string> experimentos;
 
void menu();