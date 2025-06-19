#include<iostream>
#include <fstream>

using namespace std;

struct nodeTrie{
    bool ehfolha;
    int valor;
    int offsets[26];
};

int main(){
    int tam = 1;
    nodeTrie raiz;
    raiz.ehfolha = false;
    raiz.valor = -1;
    for(int i = 0; i < 26; ++i){
        raiz.offsets[i] = -1;
    }
    fstream arquivo("trie.bin",  ios::in | ios::out | ios::binary);
    arquivo.seekp(0);
    arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));
    arquivo.seekp(0);
    arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));
    arquivo.seekp(sizeof(tam));
    arquivo.write(reinterpret_cast<char*>(&raiz),sizeof(raiz));
    arquivo.close();
}