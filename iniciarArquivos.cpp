#include<iostream>
#include <fstream>
#define QTD_CARACTER 26

using namespace std;

struct nodeTrie{
    bool ehfolha;
    int valor;
    int offsets[QTD_CARACTER];
};

int main(){
    int tam = 0;
    // excreg.bin
    fstream arquivo("excreg.bin",std::ios::in | std::ios::out | std::ios::binary);
    if(!arquivo) cerr << "Erro ao abrir excreg.bin\n";
    arquivo.seekp(0);
    arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));
    arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));
    arquivo.close();
    // excinter.bin
    arquivo.open("excinter.bin",std::ios::in | std::ios::out | std::ios::binary);
    if(!arquivo) cerr << "Erro ao abrir excinter.bin\n";
    arquivo.seekp(0);
    arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));
    arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));
    arquivo.close();
    //  exctr.bin
    tam = 1;
    arquivo.open("exctr.bin",std::ios::in | std::ios::out | std::ios::binary);
    if(!arquivo) cerr << "Erro ao abrir exctr.bin\n";
    arquivo.seekp(0);
    arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));
    tam = 0;
    arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));
    arquivo.close();
    // trie.bin
    nodeTrie node;
    node.ehfolha = false;
    node.valor = -1;
    for(int i = 0; i < QTD_CARACTER; ++i){
        node.offsets[i] = -1;
    }
    arquivo.open("trie.bin",std::ios::in | std::ios::out | std::ios::binary);
    if(!arquivo) cerr << "Erro ao abrir trie.bin\n";
    arquivo.seekp(0);
    arquivo.write(reinterpret_cast<char*>(&node),sizeof(node));
    arquivo.close();
    // exctag.bin
    tam = 0;
    arquivo.open("exctag.bin",std::ios::in | std::ios::out | std::ios::binary);
    if(!arquivo) cerr << "Erro ao abrir exctag.bin\n";
    arquivo.seekp(0);
    arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));
    arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));
    arquivo.close();
    // idxs.bin
    arquivo.open("idxs.bin",std::ios::in | std::ios::out | std::ios::binary);
    if(!arquivo) cerr << "Erro ao abrir idxs.bin\n";
    arquivo.seekp(0);
    tam = -1;
    for(int i = 0; i < 192;++i){
        arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));
    }
    arquivo.close();
}