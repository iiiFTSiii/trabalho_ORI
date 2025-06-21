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
    //info
    fstream info("info.bin", ios::in | ios::out | ios::binary);
    int tam = 0, x= 0;
    info.write(reinterpret_cast<char*>(&x), sizeof(x));
    info.write(reinterpret_cast<char*>(&tam), sizeof(tam));
    info.close();
    // /*
    //trie
    tam = 1;
    nodeTrie raiz;
    raiz.ehfolha = false;
    raiz.valor = -1;
    for(int i = 0; i < QTD_CARACTER; ++i){
        raiz.offsets[i] = -1;
    }
    fstream arquivo("trie.bin",  ios::in | ios::out | ios::binary);
    arquivo.seekp(0);
    arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));
    arquivo.seekp(sizeof(tam));
    arquivo.write(reinterpret_cast<char*>(&raiz),sizeof(raiz));
    arquivo.close();
    // */
    // /*
    //listas
    fstream lista("listas.bin", ios::in | ios::out | ios::binary);
    x = 0;
    lista.write(reinterpret_cast<char*>(&x), sizeof(x));
    lista.close();
    // */
    // /*
    //idxtags
    fstream idxs("idxtags.bin", ios::in|ios::out|ios::binary);
    x = -1;
    idxs.seekp(0);
    for(int i = 0; i < 192; ++i){
        idxs.write(reinterpret_cast<char*>(&x),sizeof(x));
    }
    idxs.close();
    // */
    // /*
    //tags
    fstream tags("tags.bin", ios::in|ios::out|ios::binary);
    tags.seekp(0);
    tags.write(reinterpret_cast<char*>(&tam),sizeof(tam));
    tags.close();
    // */
}