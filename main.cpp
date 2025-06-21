#include <iostream>
#include <unordered_set>
#include <vector>
#include <fstream>
#include "objlib.hpp"

using namespace std;

//por algum motivo ao final de  thriller não esta sendo gravado
int main(){
    listaRegistroExcluido excreg("excreg.bin");
    listaRegistroExcluido exctr("exctr.bin");
    listaRegistroExcluido exctag("exctag.bin");
    listaRegistroExcluido excinter("excinter.bin");
    admin teste(excreg,"registros.bin",exctr,"trie.bin",excinter,"inter.bin",exctag,"tags.bin","idxs.bin");
    /*
    string artistas[6] = {"Charlie Brown Jr", "Michael Jackson", "Twenty One Pilots", "Yun Li","Eminem","Charlie Brown Jr"};
    string musicas[6] = {"Senhor do Tempo", "Beat It", "Stressed Out","Twist","Till I Collapse","Champanhe E Agua Benta"};
    string albuns[6] = {"Imunidade Musical", "Thriller", "Bluryface","Bons Tempos","The Eminem Show","Tamo Ai Na Atividade"};
    registro r;
    r.generos = 1;
    r.instrumentos = 1;
    r.tags = 1;
    r.ativo = true;
    for(int i = 0; i < 6; ++i){
        for(int j = 0; j < artistas[i].size();++j){
            r.artista[j] = artistas[i][j]; 
        }
        for(int j = artistas[i].size(); j < TAMANHO_STRING; ++j){
            r.artista[j] = '\0';
        }
        for(int j = 0; j < musicas[i].size();++j){
            r.nome[j] = musicas[i][j]; 
        }
        for(int j = musicas[i].size(); j < TAMANHO_STRING; ++j){
            r.nome[j] = '\0';
        }
        for(int j = 0; j < albuns[i].size();++j){
            r.album[j] = albuns[i][j]; 
        }
        for(int j = albuns[i].size(); j < TAMANHO_STRING; ++j){
            r.album[j] = '\0';
        }
        teste.adicionar(r);
        cout << "\nEscrito com sucesso\n";
    }
    */
    // /*
    // navegar trie
    nodeTrie node;
    int tmp;
    fstream arquivo("trie.bin", ios::in|ios::out|ios::binary);
    arquivo.seekg(0);
    arquivo.read(reinterpret_cast<char*>(&node),sizeof(node));
    char resp = 'a';
    while(resp != ' '){
        arquivo.seekg(tmp);
        arquivo.read(reinterpret_cast<char*>(&node),sizeof(node));
        cout << node.ehfolha << endl;
        cout << node.valor << endl;
        for(int i = 0; i < 26; ++i){
            cout << char(i + 'a') << ": ";
            if(node.offsets[i] == -1){
                cout << "vazio\n";
            }else{
                cout << (node.offsets[i] - sizeof(int))/sizeof(nodeTrie) << endl;
            }
        }
        cout << "qual caracter: ";
        cin >> resp;
        tmp = node.offsets[resp-'a'];
    }
    // */
}