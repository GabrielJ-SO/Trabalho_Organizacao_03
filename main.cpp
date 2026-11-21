#include "pch.h"

#include "utils.h"

using namespace std;

int main() 
{
    string inputPath;

    cout << "Digite o nome do arquivo (com extensao)." << endl;
    cin >> inputPath;

    readFile(inputPath);

    return 0;
}