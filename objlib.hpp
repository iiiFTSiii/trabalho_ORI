#include <iostream>
#include <unordered_set>
#include <string>
#include <vector>
#include <fstream>

#define TAMANHO_STRING 100
#define QTD_CARACTER 26

struct registro{
    int ID;
    char nome[TAMANHO_STRING];
    char artista[TAMANHO_STRING];
    char album[TAMANHO_STRING];
    long long generos, instrumentos, tags;
    bool ativo = true; 
};

struct nodeTrie{
    bool ehfolha;
    int valor;
    int offsets[QTD_CARACTER];
};

struct nodeLista{
    int valor;
    int nxt;
};

class listaRegistroExcluido{
    public:
        listaRegistroExcluido(std::string s);
        int get_ultimo_id();
        void set_ultimo_id(int id);
        int get_tam_lista_excl();
        void set_tam_lista_excl(int tam);
        bool conferir_id(int id);
        int get_id_excluido();
        void ler_lista_excluidos();
        int get_id();
        void adicionar_excluido(int id);
        ~listaRegistroExcluido();
        std::fstream arquivo;
};

class bancoRegistro{
    public:
        bancoRegistro(listaRegistroExcluido& lista, std::string s);
        registro get_registro(int id);
        int set_registro(registro &r);
        void excluir_registro(int id);
        ~bancoRegistro();
        std::fstream arquivo;
        listaRegistroExcluido& lista_ref;
};

class trie{
    public:
        trie(listaRegistroExcluido& lista, std::string s);
        int get_tamanho();
        void set_tamanho(int tam);
        nodeTrie get_node(int offset);
        void set_node(nodeTrie node,int offset);
        void set_valor(const char s[TAMANHO_STRING], int valor);
        int busca_trie_exata(const char s[TAMANHO_STRING]);
        std::vector<int> buscar_trie(const char s[TAMANHO_STRING]);
        void inserir_trie(const char s[TAMANHO_STRING], int id);
        bool excluir_ramo(const char s[TAMANHO_STRING], int offset, int i );
        ~trie();
        std::fstream arquivo;
        std::vector<int> buscar_trieR(int offset);
        listaRegistroExcluido &ls;
};

// substituir parametro de string por outro
class listas{
    public:
        listas( listaRegistroExcluido& lista, std::string s);
        int get_tamanho_lista();
        void set_tamanho_lista(int tam);
        nodeLista get_node_lista(int offset);
        void set_node_lista(nodeLista node, int offset);
        int excluir_node_lista(int offset, int id);
        std::vector<int> get_lista(int offset);
        int inserir_lista(int id);
        void inserir_elemento_lista(int id, int offset);
        ~listas();
        std::fstream arquivo;  
        listaRegistroExcluido &ls;      
};

class indices{
    public:
        indices(std::string s);
        int get_idx(int offset);
        void set_idx(int offset, int valor);
        ~indices();
        std::fstream arquivo;
};

class admin{
    public:
        admin(listaRegistroExcluido &l,std::string r,listaRegistroExcluido &m,std::string s,listaRegistroExcluido &n,std::string t,listaRegistroExcluido &o,std::string u,std::string v);
        std::vector<int> busca(const registro &r);
        void adicionar(registro &r);
        void excluir(int id);
        bancoRegistro bd;
        trie tr;
        listas lt;
        listas tags;
        indices idxs;
};