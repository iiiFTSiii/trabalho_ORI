#include <iostream>
#include <unordered_set>
#include <vector>
#include <fstream>
#include "objlib.hpp"

using namespace std;

listaRegistroExcluido excreg("excreg.bin");
listaRegistroExcluido exctr("exctr.bin");
listaRegistroExcluido exctag("exctag.bin");
listaRegistroExcluido excinter("excinter.bin");
admin teste(excreg,"registros.bin",exctr,"trie.bin",excinter,"inter.bin",exctag,"tags.bin","idxs.bin");

void rotinaBuscar(){
    registro r;
    r.nome[0] = '\0';
    r.artista[0] = '\0';
    r.album[0] = '\0';
    r.generos = 0;
    r.instrumentos = 0;
    r.tags = 0;
    string s;
    int n;
    int resp = 0;
    bool input = false;
    while(resp != 5){
        cout << "Deseja buscar pelo que?\n1. string\n2. generos\n3. intrumentos\n4. tags\n5. Basta isso\n\n";
        cin >> resp;
        cin.ignore();
        if(resp == 1){
            input = true;
            cout << "Insira a string: ";
            getline(cin,s);
            for(int i = 0; i < s.size() && i < TAMANHO_STRING; ++i) r.nome[i] = s[i];
            for(int i = s.size(); i < TAMANHO_STRING; ++i) r.nome[i] = '\0';
            r.nome[TAMANHO_STRING-1] = '\0';
        }else if(resp == 2){
            input = true;
            cout << "Insira os generos: ";
            cin >> n;
            r.generos = n;
        }else if(resp == 3){
            input = true;
            cout << "Insira os instrumentos: ";
            cin >> n;
            r.instrumentos = n;
        }else if(resp == 4){
            input = true;
            cout << "Insira as tags: ";
            cin >> n;
            r.tags = n;
        }else if(resp != 5){
            cout << "Resposta invalida tente denovo\n\n";
        }
    }

    if(!input){
        cout << "Busca Invalida!\n";
        return;
    }
    
    vector<int> result = teste.busca(r);
    if(result.size() == 0){
        cout << "Nenhum Resultado!\n";
        return;
    }
    for(int i = 0; i < result.size(); ++i){
        r = teste.bd.get_registro(result[i]);
        cout << "Musica: " << r.nome << "\nArtista: " << r.artista << "\nAlbum: " << r.album << endl; 
    }
    
}

void rotinaInserir(){
    registro r;
    int n;
    string s;
    cout << "Digite o nome da musica: ";
    getline(cin,s);
    for(int i = 0; i < s.size() && i < TAMANHO_STRING; ++i) r.nome[i] = s[i];
    for(int i = s.size(); i < TAMANHO_STRING; ++i) r.nome[i] = '\0';
    r.nome[TAMANHO_STRING-1] = '\0';
    cout << "Digite o nome do artista: ";
    getline(cin,s);
    for(int i = 0; i < s.size() && i < TAMANHO_STRING; ++i) r.artista[i] = s[i];
    for(int i = s.size(); i < TAMANHO_STRING; ++i) r.artista[i] = '\0';
    r.artista[TAMANHO_STRING-1] = '\0';
    cout << "Digite o nome do album: ";
    getline(cin,s);
    for(int i = 0; i < s.size() && i < TAMANHO_STRING; ++i) r.album[i] = s[i];
    for(int i = s.size(); i < TAMANHO_STRING; ++i) r.album[i] = '\0';
    r.album[TAMANHO_STRING-1] = '\0';
    cout << "Digite os generos da música: ";
    cin >> n;
    r.generos = n;
    cout << "Digite os instrumentos da música: ";
    cin >> n;
    r.instrumentos = n;
    cout << "Digite as tags da música: ";
    cin >> n;
    r.tags = n;
    teste.adicionar(r);
}

void rotinaExcluir(){
    int n;
    cout << "Qual o id da musica que voce quer excluir: ";
    cin >> n;
    teste.excluir(n);
}

int main(){
    int resp = 0;
    while(resp != 4){
        cout << "O que deseja fazer?\n1. Buscar Musica\n2. Inserir Musica\n3. Excluir musica\n4. Sair\n\n";
        cin >> resp;
        cin.ignore();
        if(resp == 1){
            rotinaBuscar();
        }else if(resp == 2){
            rotinaInserir();
        }else if(resp == 3){
            rotinaExcluir();
        }else if(resp != 4){
            cout << "Resposta invalida tente denovo\n\n";
        }
    }
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
    /*
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
    */
}