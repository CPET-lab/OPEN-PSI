#include "DASetAug.h"
#include <iostream>
#include <vector>

void print(int i){
    std::cout << i << std::endl;
}

void print(std::string s) {
    std::cout << s << std::endl;
}

void print(std::vector<char> s) {
    for(char c : s)
        std::cout << c;
    std::cout << std::endl;
}

void print(std::vector<std::string> &sv) {
    for(std::string str : sv)
        print(str);
    std::cout << std::endl;
}

void print(std::vector<D_INT> &iv) {
    for(D_INT e : iv)
        print(e);
    std::cout << std::endl;
}

/*** Utility ***/

// integer to binary string
std::string intToBinary(D_INT x) {
    std::string bin(MAXBITLEN, '0');
    for(int i = MAXBITLEN - 1; i >= 0; i--) {
        bin[i] = char('0' + (x & 1ULL));
        x = x >> 1ULL;
    }
    return bin;
}

// genRepString for Alice
std::vector<std::string> genRepStringA(D_INT &start, D_INT &end) {
    Trie trie;

    for(D_INT cnum = start + 1; cnum < end; cnum++) {
        std::string s = intToBinary(cnum);
        trie.insert(s);
    }
    return trie.genRepStrVec();
}

// genRepString for Bob
std::vector<std::string> genRepStringB(D_INT &iset, D_INT thres) {
    std::set<std::string> ts;
    
    std::string s = intToBinary(iset);
    ts.insert(s);
    for(int i = MAXBITLEN; i >= MAXBITLEN - log2(2*thres - 1); i--) {
        s[i] = '*';
        ts.insert(s);
    }
    
    std::vector<std::string> oset(ts.begin(), ts.end());
    return oset;
}


// // input
// std::vector<D_INT> A = {5};  // Alice's input set
// std::vector<D_INT> B = {9};  // Bob's input set
// D_INT threshold = 5;



// int main() {
//     std::vector<std::string> sa = genRepStringA(A, threshold);
//     print("result A: ");
//     print(sa);

//     std::vector<std::string> sb = genRepStringB(B, threshold);
//     print("result B: ");
//     print(sb);
// }