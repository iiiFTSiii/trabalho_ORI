#include <iostream>
#include <unordered_set>
#include <vector>
#include <fstream>

using namespace std;

#define TAMANHO_STRING 100

struct registro{
    int ID;
    char nome[TAMANHO_STRING];
    char artista[TAMANHO_STRING];
    char album[TAMANHO_STRING];
    long long generos, instrumentos, tags;
    bool ativo; 
};

struct nodeTrie{
    bool ehfolha;
    int valor;
    int offsets[26];
};

struct nodeLista{
    int valor;
    int nxt;
};

int ultimo_id(){
    int result;
    fstream arquivo("info.bin", ios::in | ios::out | ios::binary);
    if (!arquivo) {
        cerr << "Erro ao abrir info.bin\n";
        return -1;
    }
    arquivo.read(reinterpret_cast<char*>(&result), sizeof(int));
    arquivo.close();
    return result;
}

int tam_lista_excl(){
    int result;
    fstream arquivo("info.bin", ios::in | ios::out | ios::binary);
    if (!arquivo) {
        cerr << "Erro ao abrir info.bin\n";
        return -1;
    }
    arquivo.seekg(sizeof(int));
    arquivo.read(reinterpret_cast<char*>(&result), sizeof(int));
    arquivo.close();
    return result;
}

int escreverRegistro(registro &r) {
    int tamex = tam_lista_excl(), ult_id = ultimo_id();
    if(tamex == -1 || ult_id == -1){
        cerr << "Erro ao abrir info.bin\n";
        return -1;
    }
    fstream arquivoi("info.bin", ios::in | ios::out | ios::binary);
    if (!arquivoi) {
        cerr << "Erro ao abrir info.bin\n";
        return -1;
    }

    if (tamex == 0) {
        r.ID = ult_id;
        ult_id++;

        arquivoi.seekp(0);
        arquivoi.write(reinterpret_cast<char*>(&ult_id), sizeof(int));
        arquivoi.close();

        fstream arquivo("dados.bin", ios::in | ios::out | ios::binary);
        if (!arquivo) {
            cerr << "Erro ao abrir dados.bin\n";
            return -1;
        }

        arquivo.seekp(r.ID * sizeof(registro));
        arquivo.write(reinterpret_cast<char*>(&r), sizeof(registro));
        arquivo.close();
    } else {
        arquivoi.seekg(2 * sizeof(int) + (tamex - 1) * sizeof(int));
        arquivoi.read(reinterpret_cast<char*>(&ult_id), sizeof(int));
        r.ID = ult_id;

        tamex--;
        arquivoi.seekp(sizeof(int));
        arquivoi.write(reinterpret_cast<char*>(&tamex), sizeof(int));
        arquivoi.close();

        fstream arquivo("dados.bin", ios::in | ios::out | ios::binary);
        if (!arquivo) {
            cerr << "Erro ao abrir dados.bin\n";
            return -1;
        }

        arquivo.seekp(r.ID * sizeof(registro));
        arquivo.write(reinterpret_cast<char*>(&r), sizeof(registro));
        arquivo.close();
    }

    return r.ID;
}

registro lerRegistro(int id){
    registro r;
    int ult_id = ultimo_id();
    if(ult_id < id || id < 0){
        cerr << "Erro! id inexistente\n";
        r.ativo = false;
        return r;
    }
    fstream arquivo("dados.bin", ios::in | ios::out | ios::binary);
    if (!arquivo) {
        cerr << "Erro ao abrir dados.bin\n";
        r.ativo = false;
        return r;
    }
    arquivo.seekg(id*sizeof(r));
    arquivo.read(reinterpret_cast<char*>(&r),sizeof(r));
    arquivo.close();
    return r;
}

void excluirRegistro(int id){
    registro r = lerRegistro(id);
    if(!r.ativo){
        return;
    }
    r.ativo = false;
    fstream arquivod("dados.bin",ios::in | ios::out | ios::binary);
    if (!arquivod) {
        cerr << "Erro ao abrir dados.bin\n";
        return;
    }
    arquivod.seekp(id*sizeof(registro));
    arquivod.write(reinterpret_cast<char*>(&r), sizeof(registro));
    arquivod.close();
    fstream arquivo("info.bin", ios::in | ios::out | ios::binary);
    if (!arquivo) {
        cerr << "Erro ao abrir infos.bin\n";
        return;
    }
    int tam_lista = tam_lista_excl();
    arquivo.seekp((2+tam_lista)*sizeof(int));
    arquivo.write(reinterpret_cast<char*>(&id),sizeof(int));
    arquivo.seekp(sizeof(int));
    tam_lista++;
    arquivo.write(reinterpret_cast<char*>(&tam_lista),sizeof(int));
    arquivo.close();
    return;
}

int tamanhoTrie(){
    int result;
    fstream arquivo("trie.bin", ios::in | ios::out | ios::binary);
    if (!arquivo) {
        cerr << "Erro ao abrir trie.bin\n";
        return -1;
    }
    arquivo.seekg(0);
    arquivo.read(reinterpret_cast<char*>(&result), sizeof(int));
    return result;
}

void inserirTrie(const char s[TAMANHO_STRING], int id){
    int offset = sizeof(int), i = 0, tam;
    nodeTrie node;
    fstream arvore("trie.bin", ios::in | ios::out | ios::binary);
    arvore.seekg(offset);
    arvore.read(reinterpret_cast<char*>(&node),sizeof(node));
    while(true){
        if(s[i] == ' '){
            i++;
            continue;
        }
        if(s[i] == '\0'){
            node.ehfolha = true;
            node.valor = id;
            arvore.seekp(offset);
            arvore.write(reinterpret_cast<char*>(&node),sizeof(node));
            arvore.close();
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
            arvore.seekg(0);
            arvore.read(reinterpret_cast<char*>(&tam), sizeof(int));
            novooffset = sizeof(int) + (tam*sizeof(novonode));
            node.offsets[idx] = novooffset;
            arvore.seekp(novooffset);
            arvore.write(reinterpret_cast<char*>(&novonode), sizeof(novonode));
            arvore.seekp(offset);
            arvore.write(reinterpret_cast<char*>(&node),sizeof(node));
            tam++;
            arvore.seekp(0);
            arvore.write(reinterpret_cast<char*>(&tam),sizeof(tam));
            node = novonode;
            offset = novooffset;
        }else{
            offset = node.offsets[idx];
            arvore.seekg(offset);
            arvore.read(reinterpret_cast<char*>(&node),sizeof(node));
        }
        i++;
    }

    return;
}

unordered_set<int> buscarTrieR(int offset, fstream &arvore){
    unordered_set<int> result;
    nodeTrie node;
    arvore.seekg(offset);
    arvore.read(reinterpret_cast<char*>(&node),sizeof(node));

    if(node.ehfolha){
        result.insert(node.valor);   
    }

    for(int i = 0; i < 26; ++i){
        unordered_set<int> tmp;
        if(node.offsets[i] != -1){
            tmp = buscarTrieR(node.offsets[i], arvore);
        }

        result.insert(tmp.begin(),tmp.end());
    }

    return result;
}

int buscaTrieExata(const char s[TAMANHO_STRING]){
    int i = 0, offset = sizeof(int);
    fstream arvore("trie.bin", ios::in | ios::out | ios::binary);
    nodeTrie node;
    arvore.seekg(offset);
    arvore.read(reinterpret_cast<char*>(&node),sizeof(node));
    while(s[i] != '\0'){
        if(s[i] == ' '){
            i++;
            continue;
        }

        if(node.offsets[(s[i] |' ') - 'a'] == -1){
            return -1;
        }

        offset = node.offsets[(s[i] |' ') - 'a'];
        arvore.seekg(offset);
        arvore.read(reinterpret_cast<char*>(&node),sizeof(node));

        i++;
    }

    arvore.close();
    
    if(node.ehfolha){
        return node.valor;
    }else{
        return -1;
    }
    
}

unordered_set<int> buscarTrie(const char s[TAMANHO_STRING]){
    int i = 0, offset = sizeof(int);
    unordered_set<int> result;
    fstream arvore("trie.bin", ios::in | ios::out | ios::binary);
    nodeTrie node;
    arvore.seekg(offset);
    arvore.read(reinterpret_cast<char*>(&node),sizeof(node));
    while(s[i] != '\0'){
        if(s[i] == ' '){
            i++;
            continue;
        }

        if(node.offsets[(s[i] |' ') - 'a'] == -1){
            return result;
        }

        offset = node.offsets[(s[i] |' ') - 'a'];
        arvore.seekg(offset);
        arvore.read(reinterpret_cast<char*>(&node),sizeof(node));

        i++;
    }

    result = buscarTrieR(offset, arvore);
    arvore.close();

    return result;
}

void lerListaExcluidos(){
    int tam = tam_lista_excl(), id;
    fstream arquivo("info.bin", ios::in | ios::out | ios::binary);
    if (!arquivo) {
        std::cerr << "Erro ao abrir info.bin\n";
        return;
    }
    cout << tam << endl;
    for(int i = 0; i < tam; ++i){
        arquivo.seekg(2*sizeof(int));
        arquivo.read(reinterpret_cast<char*>(&id),sizeof(int));
        cout << id << endl;
    }
    
    
    arquivo.close();
}

nodeTrie lerNode(int i){
    nodeTrie result;
    fstream arquivo("trie.bin", ios::in|ios::out|ios::binary);
    arquivo.seekg(sizeof(int)+ i*sizeof(nodeTrie));
    arquivo.read(reinterpret_cast<char*>(&result),sizeof(result));
    arquivo.close();
    return result;
}

int tamanhoLista(){
    int tam;
    fstream arquivo("listas.bin", ios::in|ios::out|ios::binary);
    arquivo.seekg(0);
    arquivo.read(reinterpret_cast<char*>(&tam),sizeof(tam));
    arquivo.close();
    return tam;
}

int inserirLista(int id){
    int tam = tamanhoLista();
    nodeLista node;
    node.nxt = -1;
    node.valor = id;
    int offset = sizeof(int)+tam*sizeof(node);
    fstream arquivo("listas.bin", ios::in|ios::out|ios::binary);
    tam++;
    if(!arquivo){
        cerr << "Erro ao abrir listas.bin\n";
        return -1;
    }
    arquivo.seekp(offset);
    arquivo.write(reinterpret_cast<char*>(&node),sizeof(node));
    arquivo.seekp(0);
    arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));

    arquivo.close();

    return offset;
}

void inserirElementoLista(int id, int offset){
    int ptr = offset;
    fstream arquivo("listas.bin", ios::in|ios::out|ios::binary);
    if(!arquivo){
        cerr << "Erro ao abrir listas.bin\n";
        return;
    }
    nodeLista node;
    while(true){
        arquivo.seekg(ptr);
        arquivo.read(reinterpret_cast<char*>(&node),sizeof(node));
        if(node.nxt != -1){
            ptr = node.nxt;
        }else{
            break;
        }
    }
    nodeLista novo;
    int tam = tamanhoLista(), novooffset = sizeof(int) + tam*sizeof(novo);
    novo.valor = id;
    novo.nxt = -1;
    arquivo.seekp(novooffset);
    arquivo.write(reinterpret_cast<char*>(&novo),sizeof(novo));
    node.nxt = novooffset;
    arquivo.seekp(ptr);
    arquivo.write(reinterpret_cast<char*>(&node),sizeof(node));
    tam++;
    arquivo.seekp(0);
    arquivo.write(reinterpret_cast<char*>(&tam),sizeof(tam));

    arquivo.close();
    return;
}

vector<int> lerLista(int offset){
    vector<int> result;
    fstream arquivo("listas.bin", ios::in|ios::out|ios::binary);
    if(!arquivo){
        cerr << "Erro ao abrir listas.bin\n";
        return result;
    }
    int ptr = offset;
    nodeLista node;
    while(ptr != -1){
        arquivo.seekg(ptr);
        arquivo.read(reinterpret_cast<char*>(&node),sizeof(node));
        ptr = node.nxt;
        result.push_back(node.valor);
    }

    arquivo.close();
    return result;
}

void adicionar(registro &r){
    int x = escreverRegistro(r), ok = buscaTrieExata(r.nome);
    if(ok == -1){
        int offset = inserirLista(x);
        inserirTrie(r.nome,offset);
    }else{
        inserirElementoLista(x,ok);
    }
    ok = buscaTrieExata(r.artista);
    if(ok == -1){
        int offset = inserirLista(x);
        inserirTrie(r.artista,offset);
    }else{
        inserirElementoLista(x,ok);
    }
    ok = buscaTrieExata(r.album);
    if(ok == -1){
        int offset = inserirLista(x);
        inserirTrie(r.album,offset);
    }else{
        inserirElementoLista(x,ok);
    }
}

int main(){
    /* 
    // Escrever registros
    string artistas[6] = {"Charlie Brown Jr", "Michael Jackson", "Twenty One Pilots", "Yun Li","Eminem","Charlie Brown Jr"};
    string musicas[6] = {"Senhor do Tempo", "Beat It", "Stressed Out","Twist","Till I Collapse","Champanhe E Agua Benta"};
    string albuns[6] = {"Imunidade Musical", "Thriller", "Bluryface","Bons Tempos","The Eminem Show","Tamo Ai Na Atividade"};
    registro r;
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
        adicionar(r);
        cout << "\nEscrito com sucesso\n";
    }
    */
    /*
    // Ler dados.bit
    int ult_id = ultimo_id();
    cout << ult_id;
    registro reg;
    for(int i = 0; i < ult_id; ++i){
        reg = lerRegistro(i);
        if (!reg.ativo) {
            continue;
        }
        cout <<"\nMusica: "<<reg.nome<<"\nArtista: "<<reg.artista<<"\nAlbum: "<<reg.album;
    }

    cout << endl;
    */
    // /*
    // Busca na Trie
    cout << tamanhoLista() << endl;
    string s;
    cout << "O que voce procura? : ";
    getline(cin, s);
    char convert[TAMANHO_STRING];
    for(int i = 0; i < s.size(); ++i){
        convert[i] = s[i];
    }
    for(int i = s.size(); i < TAMANHO_STRING; ++i){
        convert[i] = '\0';
    }
    registro reg;
    unordered_set<int> resultados = buscarTrie(convert);
    // ch devolve champanhe e agua benta duas vezes, uma por CHarlieBrownjr e outra por CHampanhe e agua benta (implementar depois não pesquisar o mesmo ip duas vezes)
    for( int resultado : resultados){
        cout <<"Resultado: " << resultado << endl;
        vector<int> tmp = lerLista(resultado);
        for(int i = 0; i < tmp.size(); ++i){
            reg = lerRegistro(tmp[i]);
            cout <<"\nMusica: "<<reg.nome<<"\nArtista: "<<reg.artista<<"\nAlbum: "<<reg.album;
        }
        cout << endl;
    }
    // */
    /*
    // navegar trie
    nodeTrie node;
    int tmp;
    fstream arquivo("trie.bin", ios::in|ios::out|ios::binary);
    arquivo.seekg(sizeof(int));
    arquivo.read(reinterpret_cast<char*>(&node),sizeof(node));
    char resp = 'a';
    while(resp != ' '){
        cout << "qual caracter: ";
        cin >> resp;
        tmp = node.offsets[resp-'a'];
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
    }
    */

    /*
    cout << tamanhoTrie() << endl;
    nodeTrie node = lerNode(0);
    cout << node.ehfolha << endl << node.valor << endl;
    char c = 'a';
    for(int i = 0; i < 26; ++i){
        if(node.offsets[i] == -1){
            cout << char(c + i) << ": vazio\n";
        }else{
            cout << char(c + i) << ": " << (node.offsets[i] - sizeof(int))/sizeof(nodeTrie) << endl;
        }
    }
    */ 
}