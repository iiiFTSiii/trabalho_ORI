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
trie::trie( listaRegistroExcluido &lista, std::string s) : ls(lista){
    arquivo.open(s,std::ios::in | std::ios::out | std::ios::binary);
    if(!arquivo) std::cerr << "Erro ao abrir "+s+".bin\n";
}
//ok
int trie::get_tamanho(){
    return ls.get_ultimo_id();
}
//ok
void trie::set_tamanho(int tam){
    ls.set_ultimo_id(tam);
}

nodeTrie trie::get_node(int offset){
    nodeTrie result;
    arquivo.seekg(offset);
    arquivo.read(reinterpret_cast<char*>(&result),sizeof(result));
    return result;
}

void trie::set_node( nodeTrie node,int offset){
    arquivo.seekp(offset);
    arquivo.write(reinterpret_cast<char*>(&node),sizeof(node));
    return;
}

void trie::set_valor(const char s[TAMANHO_STRING],int valor){
    int i = 0, offset = 0;
    nodeTrie node = get_node(offset);
    while(s[i] != '\0'){
        if(s[i] == ' '){
            i++;
            continue;
        }

        int idx = (s[i] |' ') - 'a';
        if(node.offsets[idx] == -1) return;
        
        offset = node.offsets[idx];
        node = get_node(offset);
        i++;
    }

    node.valor = valor;
    set_node(node,offset);
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
    int i = 0, offset = 0;
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
    int i = 0, offset = 0;
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
    int offset = 0, i = 0, tam;
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
            bool reusado = (ls.get_tam_lista_excl() > 0);
            int novooffset = ls.get_id();
            novonode.ehfolha = false;
            novonode.valor = -1;
            for(int j = 0; j < 26;++j){
                novonode.offsets[j] = -1;
            }
            if(!reusado){
                novooffset *= sizeof(novonode);
            }
            node.offsets[idx] = novooffset;
            set_node(novonode,novooffset);
            set_node(node,offset);
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

bool trie::excluir_ramo(const char s[TAMANHO_STRING], int offset, int i ){
    nodeTrie node = get_node(offset);
    if(s[i] == '\0'){
        node.ehfolha = false;
        node.valor = -1;
        set_node(node,offset);
        for(int i = 0; i < QTD_CARACTER; ++i){
            if(node.offsets[i] != -1) return false;
        }
        return true;
    } 

    if(s[i] == ' '){
        return excluir_ramo(s,offset,i+1);
    }

    int idx = (s[i] | ' ') - 'a';
    if(node.offsets[idx] == -1) return false;
    if(excluir_ramo(s,node.offsets[idx],i+1)){
        ls.adicionar_excluido(node.offsets[idx]);
        node.offsets[idx] = -1;
        for(int i = 0; i < QTD_CARACTER; ++i){
            if(node.offsets[i] != -1) return false;
        }
        return true;
    }else{
        return false;
    }
}

trie::~trie(){
    if (arquivo.is_open()) {
        arquivo.close();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
listas::listas( listaRegistroExcluido &lista, std:: string s) : ls(lista){
    arquivo.open(s,std::ios::in | std::ios::out | std::ios::binary);
    if(!arquivo) std::cerr << "Erro ao abrir "+s+".bin\n";
}

int listas::get_tamanho_lista(){
    return ls.get_ultimo_id();
}

void listas::set_tamanho_lista(int tam){
    ls.set_ultimo_id(tam);
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

// -1 precisa apagar o ramo da trie
// -2 não precisa fazer nada
// numero positivo precisa trocar o offset na estrutura de dados
int listas::excluir_node_lista(int offset, int id){
    nodeLista node = get_node_lista(offset);
    if( node.valor == id ){
        ls.adicionar_excluido(offset);
        if(node.nxt == -1){
            return -1;
        }else{
            return node.nxt;
        }
    }
    int ptr = node.nxt, pat = offset;
    while(ptr != -1){
        node = get_node_lista(ptr);
        if(node.valor == id){
            ls.adicionar_excluido(ptr);
            nodeLista aux = get_node_lista(pat);
            aux.nxt = node.nxt;
            set_node_lista(aux,pat);
            return -2;
        }else{
            pat = ptr;
            ptr = node.nxt;
        }
    }

    return -2;
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
    bool reuso = (ls.get_tam_lista_excl() > 0);
    int tam = ls.get_id();
    nodeLista node;
    node.nxt = -1;
    node.valor = id;
    int offset;
    if(reuso){
        offset = tam;
    }else{
        offset = tam*sizeof(node);
    }
    set_node_lista(node,offset);

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
    bool reuso = (ls.get_tam_lista_excl() > 0);
    int tam = ls.get_id();
    int novooffset;
    if(reuso){
        novooffset = tam;
    }else{
        novooffset = tam*sizeof(node);
    }
    novo.valor = id;
    novo.nxt = -1;
    node.nxt = novooffset;
    set_node_lista(novo,novooffset);
    set_node_lista(node,ptr);
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
admin::admin(listaRegistroExcluido &l, std::string r, listaRegistroExcluido &m, std::string s, listaRegistroExcluido &n, std::string t,listaRegistroExcluido &o, std::string u, std::string v):bd(l,r), tr(m,s),lt(n,t),tags(o,u),idxs(v){

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
    // /*
    int count = 0; 
    long long copia = r.generos;
    while(copia != 0){
        if(copia & 1){
            int idx = idxs.get_idx(count*sizeof(int));
            if(idx == -1){
                int offset = tags.inserir_lista(id);
                idxs.set_idx(count*sizeof(int),offset);
            }else{
                tags.inserir_elemento_lista(idx,id);
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
                tags.inserir_elemento_lista(idx,id);
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
                tags.inserir_elemento_lista(idx,id);
            }
        }
        copia = copia >> 1;
        count++;
    }
    // */
}

void admin::excluir(int id){
    registro r = bd.get_registro(id);
    if(!r.ativo) return;
    bd.excluir_registro(id);
    int offset = tr.busca_trie_exata(r.nome);
    int x = lt.excluir_node_lista(offset,id);
    if( x > 0){
        tr.set_valor(r.nome,x);
    }else if(x == -1){
        tr.excluir_ramo(r.nome,0,0);
    }
    offset = tr.busca_trie_exata(r.artista);
    x = lt.excluir_node_lista(offset,id);
    if( x > 0){
        tr.set_valor(r.artista,x);
    }else if(x == -1){
        tr.excluir_ramo(r.artista,0,0);
    }
    offset = tr.busca_trie_exata(r.artista);
    x = lt.excluir_node_lista(offset,id);
    if( x > 0){
        tr.set_valor(r.artista,x);
    }else if(x == -1){
        tr.excluir_ramo(r.artista,0,0);
    }
    //falta remover as tags
    int count = 0; 
    long long copia = r.generos;
    while(copia != 0){
        if(copia & 1){
            int idx = idxs.get_idx(count*sizeof(int));
            if(idx == -1) {
                std::cerr << "Esse genero não foi registrada\n";
            }else{
                int y = tags.excluir_node_lista(idx,id);
                if(y == -1 || y > 0) idxs.set_idx(count*sizeof(int),y);
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
            if(idx == -1) {
                std::cerr << "Esse instrumento não foi registrada\n";
            }else{
                int y = tags.excluir_node_lista(idx,id);
                if(y == -1 || y > 0) idxs.set_idx(count*sizeof(int),y);
            }
        }
        copia = copia >> 1;
        count++;
    }
    count = 128, copia = r.tags;
    while(copia != 0){
        if(copia & 1){
            int idx = idxs.get_idx(count*sizeof(int));
            if(idx == -1) {
                std::cerr << "Essa tag não foi registrada\n";
            }else{
                int y = tags.excluir_node_lista(idx,id);
                if(y == -1 || y > 0) idxs.set_idx(count*sizeof(int),y);
            }
        }
        copia = copia >> 1;
        count++;
    }
}
