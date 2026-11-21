#pragma once

#include "pch.h"
#include "address_queue.h"
#include "leitor_arquivo.h"

std::string conversorHexBin(std::string hex);
std::string conversorBinHex(std::string bin);
int conversorBinInt(std::string bin);

void readFile(std::string path);