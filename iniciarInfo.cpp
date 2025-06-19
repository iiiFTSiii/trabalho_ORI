#include<iostream>
#include <fstream>

using namespace std;

int main(){
    fstream arquivo("info.bin", ios::in | ios::out | ios::binary);
    int x = 0, tam = 0;
    arquivo.write(reinterpret_cast<char*>(&x), sizeof(x));
    arquivo.write(reinterpret_cast<char*>(&tam), sizeof(tam));
}