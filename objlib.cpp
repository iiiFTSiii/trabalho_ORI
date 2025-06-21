#include "objlib.hpp"
#include <iostream>
#include <unordered_set>
#include <string>
#include <vector>
#include <fstream>
//ok
listaRegistroExcluido::listaRegistroExcluido(std::string s){
    arquivo.open(s, std::ios::in | std::ios::out | std::ios::binary);
    if(!arquivo) std::cerr << "Erro ao abrir "+s+".bin\n";
}
//ok
int listaRegistroExcluido::get_ultimo_id(){
    int result = -1;
    arquivo.seekg(0);
    arquivo.read(reinterpret_cast<char*>(&result), sizeof(int));
    return result;
}
//ok
void listaRegistroExcluido::set_ultimo_id(int id){
    arquivo.seekp(0);
    arquivo.write(reinterpret_cast<char*>(&id),sizeof(id));
    return;
}
//ok
int listaRegistroExcluido::get_tam_lista_excl(){
    int result = -1;
    arquivo.seekg(sizeof(int));
    arquivo.read(reinterpret_cast<char*>(&result), sizeof(int));
    return result;
}
//ok
void listaRegistroExcluido::set_tam_lista_excl(int tam){
    arquivo.seekp(sizeof(int));
    arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));
    return;
}
//ok
bool listaRegistroExcluido::conferir_id(int id){
    int aux = get_ultimo_id();
    if(id >= 0 && id < aux){
        return true;
    }
    return false;
}
//ok
int listaRegistroExcluido::get_id_excluido(){
    int tam = get_tam_lista_excl(), id = -1;
    if(tam < 1) return id;
    
    arquivo.seekg((1+tam)*sizeof(int));
    arquivo.read(reinterpret_cast<char*>(&id),sizeof(id));
    return id;
}
//ok
int listaRegistroExcluido::get_id(){
    int tam = get_tam_lista_excl(), id = -1;
    if(tam == 0){ 
        id = get_ultimo_id();
        set_ultimo_id(id+1);
    }else{
        id = get_id_excluido();
        set_tam_lista_excl(tam-1);
    }

    return id;
}
//ok
void listaRegistroExcluido::adicionar_excluido(int id){
    if(conferir_id(id)){
        int tam = get_tam_lista_excl();
        arquivo.seekp((2+tam)*sizeof(int));
        arquivo.write(reinterpret_cast<char*>(&id),sizeof(id));
        set_tam_lista_excl(tam+1);
    }
}
//ok
void listaRegistroExcluido::ler_lista_excluidos(){
    int tam = get_tam_lista_excl(), id;
    for(int i = 0; i < tam; ++i){
        arquivo.seekg((2+i)*sizeof(int));
        arquivo.read(reinterpret_cast<char*>(&id),sizeof(int));
        std::cout << id << std::endl;
    }
    
    return;
}
//ok
listaRegistroExcluido::~listaRegistroExcluido(){
    if (arquivo.is_open()) {
        arquivo.close();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ok
bancoRegistro::bancoRegistro(listaRegistroExcluido& lista, std::string s) : lista_ref(lista){
    arquivo.open(s,std::ios::in | std::ios::out | std::ios::binary);
    if(!arquivo) std::cerr << "Erro ao abrir "+s+".bin\n";
}
//ok
registro bancoRegistro::get_registro(int id){
    registro r;
    if(!lista_ref.conferir_id(id)){
        std::cerr << "Erro! id inexistente\n";
        r.ativo = false;
        return r;
    }
    arquivo.seekg(id*sizeof(r));
    arquivo.read(reinterpret_cast<char*>(&r),sizeof(r));
    return r;
}
//ok
int bancoRegistro::set_registro(registro &r){
    r.ID = lista_ref.get_id();

    arquivo.seekp(r.ID * sizeof(registro));
    arquivo.write(reinterpret_cast<char*>(&r), sizeof(registro));

    return r.ID;
}
//ok
void bancoRegistro::excluir_registro(int id){
    if(!lista_ref.conferir_id(id)) return;
    registro r = get_registro(id);
    if(!r.ativo) return;
    
    r.ativo = false;
    arquivo.seekp(id*sizeof(registro));
    arquivo.write(reinterpret_cast<char*>(&r), sizeof(registro));

    lista_ref.adicionar_excluido(id);
    return;
}
//ok
bancoRegistro::~bancoRegistro(){
    if (arquivo.is_open()) {
        arquivo.close();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ok
trie::trie( std::string s){
    arquivo.open(s,std::ios::in | std::ios::out | std::ios::binary);
    if(!arquivo) std::cerr << "Erro ao abrir "+s+".bin\n";
}
//ok
int trie::get_tamanho(){
    int result = -1;
    arquivo.seekg(0);
    arquivo.read(reinterpret_cast<char*>(&result),sizeof(result));
    return result;
}
//ok
void trie::set_tamanho(int tam){
    arquivo.seekp(0);
    arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));
    return;
}
//ok
nodeTrie trie::get_node(int offset){
    nodeTrie result;
    arquivo.seekg(offset);
    arquivo.read(reinterpret_cast<char*>(&result),sizeof(result));
    return result;
}
//ok
void trie::set_node( nodeTrie node,int offset){
    arquivo.seekp(offset);
    arquivo.write(reinterpret_cast<char*>(&node),sizeof(node));
    return;
}

std::vector<int> trie::buscar_trieR(int offset){
    std::vector<int> result;
    nodeTrie node = get_node(offset);

    if(node.ehfolha) result.push_back(node.valor); 

    for(int i = 0; i < QTD_CARACTER; ++i){
        std::vector<int> tmp;
        if(node.offsets[i] != -1){ 
            tmp = buscar_trieR(node.offsets[i]);
            result.insert(result.end(),tmp.begin(),tmp.end());
        }
    }

    return result;
}

int trie::busca_trie_exata(const char s[TAMANHO_STRING]){
    int i = 0, offset = sizeof(int);
    nodeTrie node = get_node(offset);
    while(s[i] != '\0'){
        if(s[i] == ' '){
            i++;
            continue;
        }

        int idx = (s[i] |' ') - 'a';
        if(node.offsets[idx] == -1) return -1;
        
        offset = node.offsets[idx];
        node = get_node(offset);
        i++;
    }
    
    if(node.ehfolha){
        return node.valor;
    }else{
        return -1;
    }
}

std::vector<int> trie::buscar_trie(const char s[TAMANHO_STRING]){
    int i = 0, offset = sizeof(int);
    std::vector<int> result;
    nodeTrie node = get_node(offset);
    while(s[i] != '\0'){
        if(s[i] == ' '){
            i++;
            continue;
        }

        int idx = (s[i] |' ') - 'a';
        if(node.offsets[idx] == -1) return result;
        
        offset = node.offsets[idx];
        node   = get_node(offset);
        i++;
    }
    result = buscar_trieR(offset);

    return result;
}

void trie::inserir_trie(const char s[TAMANHO_STRING], int id){
    int offset = sizeof(int), i = 0, tam;
    nodeTrie node = get_node(offset);
    while(true){
        if(s[i] == ' '){
            i++;
            continue;
        }
        if(s[i] == '\0'){
            node.ehfolha = true;
            node.valor = id;
            set_node(node, offset);
            return;
        }

        int idx = (s[i] |' ') - 'a';
        if(node.offsets[idx] == -1){
            nodeTrie novonode;
            int novooffset;
            novonode.ehfolha = false;
            novonode.valor = -1;
            for(int j = 0; j < 26;++j){
                novonode.offsets[j] = -1;
            }
            tam = get_tamanho();
            novooffset = sizeof(int) + (tam*sizeof(novonode));
            node.offsets[idx] = novooffset;
            set_node(novonode,novooffset);
            set_node(node,offset);
            set_tamanho(tam+1);
            node = novonode;
            offset = novooffset;
        }else{
            offset = node.offsets[idx];
            node = get_node(offset);
        }
        i++;
    }

    return;
}

trie::~trie(){
    if (arquivo.is_open()) {
        arquivo.close();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
listas::listas(std:: string s){
    arquivo.open(s,std::ios::in | std::ios::out | std::ios::binary);
    if(!arquivo) std::cerr << "Erro ao abrir "+s+".bin\n";
}

int listas::get_tamanho_lista(){
    int tam = -1;
    arquivo.seekg(0);
    arquivo.read(reinterpret_cast<char*>(&tam),sizeof(tam));
    return tam;
}

void listas::set_tamanho_lista(int tam){
    arquivo.seekp(0);
    arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));
    return;
}

nodeLista listas::get_node_lista(int offset){
    nodeLista node;
    node.valor = -1;
    arquivo.seekg(offset);
    arquivo.read(reinterpret_cast<char*>(&node),sizeof(node));
    return node;
}

void listas::set_node_lista(nodeLista node, int offset){
    arquivo.seekp(offset);
    arquivo.write(reinterpret_cast<char*>(&node),sizeof(node));
    return;
}

std::vector<int> listas::get_lista(int offset){
    std::vector<int> result;
    int ptr = offset;
    nodeLista node;
    while(ptr != -1){
        node = get_node_lista(ptr);
        ptr = node.nxt;
        result.push_back(node.valor);
    }

    return result;
}

int listas::inserir_lista(int id){
    int tam = get_tamanho_lista();
    nodeLista node;
    node.nxt = -1;
    node.valor = id;
    int offset = sizeof(int)+tam*sizeof(node);
    set_node_lista(node,offset);
    set_tamanho_lista(tam+1);

    return offset;
}

void listas::inserir_elemento_lista(int offset, int id){
    int ptr = offset;
    nodeLista node;
    while(true){
        node = get_node_lista(ptr);
        if(node.nxt != -1){
            ptr = node.nxt;
        }else{
            break;
        }
    }
    nodeLista novo;
    int tam = get_tamanho_lista();
    int novooffset = sizeof(int) + tam*sizeof(novo);
    novo.valor = id;
    novo.nxt = -1;
    node.nxt = novooffset;

    set_node_lista(novo,novooffset);
    set_node_lista(node,ptr);
    set_tamanho_lista(tam+1);
    return;
}

listas::~listas(){
    if (arquivo.is_open()) {
        arquivo.close();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
indices::indices(std::string s){
    arquivo.open(s,std::ios::in | std::ios::out | std::ios::binary);
    if(!arquivo) std::cerr << "Erro ao abrir "+s+".bin\n";
}

int indices::get_idx(int offset){
    int result = -1;
    arquivo.seekg(offset);
    arquivo.read(reinterpret_cast<char*>(&result),sizeof(result));
    return result;
}

void indices::set_idx(int offset, int valor){
    arquivo.seekp(offset);
    arquivo.write(reinterpret_cast<char*>(&valor),sizeof(valor));
    return;
}

indices::~indices(){
    if (arquivo.is_open()) {
        arquivo.close();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
admin::admin(listaRegistroExcluido &q,std::string r, std::string s, std::string t, std::string u, std::string v):bd(q,r), tr(s),lt(t),tags(u),idxs(v){

}

std::vector<int> admin::busca(const registro &r){
    std::vector<int> srch;
    bool pesquisa = false;
    if(r.nome[0] != '\0'){
        srch = tr.buscar_trie(r.nome);
        pesquisa = true;
    }else if(r.artista[0] != '\0'){
        srch = tr.buscar_trie(r.artista);
        pesquisa = true;
    }else if(r.album[0] != '\0'){
        srch = tr.buscar_trie(r.album);
        pesquisa = true;
    }
    std::unordered_set<int> resultados;
    for(int i = 0; i < srch.size(); ++i){
        std::vector<int> tmp = lt.get_lista(srch[i]);
        for( int j : tmp){
            resultados.insert(j);
        }
    }
    if(pesquisa && resultados.empty()){
        std::vector<int> resp;
        return resp;
    }
    int count = 0;
    long long copia = r.generos;
    while(copia != 0){
        if(copia & 1){
            int idx = idxs.get_idx(count*sizeof(int));
            if(idx == -1){
                std::vector<int> resp;
                return resp;
            }else{
                std::vector<int> tmp = tags.get_lista(idx);
                if(pesquisa){
                    std::vector<int> aux;
                    for(int j : tmp){
                        if(resultados.find(j) != resultados.end()) aux.push_back(j);
                    }
                    if (aux.empty()) return aux;
                    resultados.clear();
                    for(int j : aux){
                        resultados.insert(j);
                    }
                }else{
                    pesquisa = true;
                    for(int j : tmp){
                        resultados.insert(j);
                    }
                }
            }
        }
        copia = copia >> 1;
        count++;
    }
    count = 64;
    copia = r.instrumentos;
    while(copia != 0){
        if(copia & 1){
            int idx = idxs.get_idx(count*sizeof(int));
            if(idx == -1){
                std::vector<int> resp;
                return resp;
            }else{
                std::vector<int> tmp = tags.get_lista(idx);
                if(pesquisa){
                    std::vector<int> aux;
                    for(int j : tmp){
                        if(resultados.find(j) != resultados.end()) aux.push_back(j);
                    }
                    if (aux.empty()) return aux;
                    resultados.clear();
                    for(int j : aux){
                        resultados.insert(j);
                    }
                }else{
                    pesquisa = true;
                    for(int j : tmp){
                        resultados.insert(j);
                    }
                }
            }
        }
        copia = copia >> 1;
        count++;
    }
    count = 128;
    copia = r.tags;
    while(copia != 0){
        if(copia & 1){
            int idx = idxs.get_idx(count*sizeof(int));
            if(idx == -1){
                std::vector<int> resp;
                return resp;
            }else{
                std::vector<int> tmp = tags.get_lista(idx);
                if(pesquisa){
                    std::vector<int> aux;
                    for(int j : tmp){
                        if(resultados.find(j) != resultados.end()) aux.push_back(j);
                    }
                    if (aux.empty()) return aux;
                    resultados.clear();
                    for(int j : aux){
                        resultados.insert(j);
                    }
                }else{
                    pesquisa = true;
                    for(int j : tmp){
                        resultados.insert(j);
                    }
                }
            }
        }
        copia = copia >> 1;
        count++;
    }
    std::vector<int> resposta(resultados.begin(),resultados.end());
    return resposta;
}

void admin::adicionar(registro &r){
    int id = bd.set_registro(r), ok = tr.busca_trie_exata(r.nome);
    if(ok == -1){
        int offset = lt.inserir_lista(id);
        tr.inserir_trie(r.nome,offset);
    }else{
        lt.inserir_elemento_lista(ok,id);
    }
    ok = tr.busca_trie_exata(r.artista);
    if(ok == -1){
        int offset = lt.inserir_lista(id);
        tr.inserir_trie(r.artista,offset);
    }else{
        lt.inserir_elemento_lista(ok,id);
    }
    ok = tr.busca_trie_exata(r.album);
    if(ok == -1){
        int offset = lt.inserir_lista(id);
        tr.inserir_trie(r.album,offset);
    }else{
        lt.inserir_elemento_lista(ok,id);
    }
    /*
    int count = 0; 
    long long copia = r.generos;
    while(copia != 0){
        if(copia & 1){
            int idx = idxs.get_idx(count*sizeof(int));
            if(idx == -1){
                int offset = tags.inserir_lista(id);
                idxs.set_idx(count*sizeof(int),offset);
            }else{
                tags.inserir_elemento_lista(id,idx);
            }
        }
        copia = copia >> 1;
        count++;
    }
    count = 64;
    copia = r.instrumentos;
    while(copia != 0){
        if(copia & 1){
            int idx = idxs.get_idx(count*sizeof(int));
            if(idx == -1){
                int offset = tags.inserir_lista(id);
                idxs.set_idx(count*sizeof(int),offset);
            }else{
                tags.inserir_elemento_lista(id,idx);
            }
        }
        copia = copia >> 1;
        count++;
    }
    count = 128, copia = r.tags;
    while(copia != 0){
        if(copia & 1){
            int idx = idxs.get_idx(count*sizeof(int));
            if(idx == -1){
                int offset = tags.inserir_lista(id);
                idxs.set_idx(count*sizeof(int),offset);
            }else{
                tags.inserir_elemento_lista(id,idx);
            }
        }
        copia = copia >> 1;
        count++;
    }
    */
}
