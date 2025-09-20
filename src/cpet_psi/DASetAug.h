#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <cmath>
#include <cstdint>

typedef uint64_t D_INT;
const unsigned int MAXBITLEN = 41;  // lenth of binary string


class Trie;
std::string intToBinary(D_INT x);
std::vector<std::string> genRepString(std::vector<D_INT> &iset, D_INT thres);


/*** Debugging ***/

void print(int i);

void print(std::string s);

void print(std::vector<char> s);

void print(std::vector<std::string> &sv);

void print(std::vector<D_INT> &iv);

/*** Utility ***/

// integer to binary string
std::string intToBinary(D_INT x);

// Trie
class Trie {

private:
    typedef struct Node {
        int next_depth;
        struct Node* parent = nullptr;
        struct Node* child[2];

        // Don't care bit, true -> *
        bool dcb;
    } node;

    inline node* newNode(node* parent, int next_depth) {
        node* nnode = new node;
        nnode->next_depth = next_depth;
        nnode->parent = parent;
        nnode->child[0] = nnode->child[1] = nullptr;
        nnode->dcb = false;
        return nnode;
    }

    inline void deleteNode(node* delete_node) {
        node* current_node;
        for(int idx = 0; idx <= 1; idx++) {
            current_node = delete_node->child[idx];
            if(current_node == nullptr)
                continue;
            
            while(current_node != delete_node) {
                if(current_node->child[0] != nullptr) {
                    current_node = current_node->child[0];
                    current_node->parent->child[0] = nullptr;
                }
                else if(current_node->child[1] != nullptr){
                    current_node = current_node->child[1];
                    current_node->parent->child[1] = nullptr;
                }
                else {
                    node* last_node = current_node;
                    current_node = current_node->parent;
                    delete last_node;
                }
            }
        }
        node* parent_node = delete_node->parent;
        if(parent_node != nullptr) {
            if(parent_node->child[0] == delete_node)
                parent_node->child[0] = nullptr;
            else if(parent_node->child[1] == delete_node)
                parent_node->child[1] = nullptr;
        }
        delete delete_node;
    }

    inline void setDCBTrue(node* parent_node) {
        if(parent_node->child[0] != nullptr)
            deleteNode(parent_node->child[0]);
        if(parent_node->child[1] != nullptr)
            deleteNode(parent_node->child[1]);
        parent_node->child[0] = parent_node->child[1] = nullptr;
        parent_node->dcb = true;
    }

    node* root;

public:
    Trie() {
        root = newNode(nullptr, 0);
    }

    ~Trie() {
        deleteNode(root);
    }

    // insert string in trie
    inline void insert(const std::string &s) {
        node* current_node = root;
        int next_idx = 0;

        while(next_idx < MAXBITLEN) {
            if(current_node->dcb == true)
                break;
            int next_bit = s[next_idx] == '0' ? 0 : 1;
            if(current_node->child[next_bit] == nullptr)
                current_node->child[next_bit] = newNode(current_node, next_idx + 1);
            next_idx++;
            current_node = current_node->child[next_bit];
        }

        setDCBTrue(current_node);
        node* parent_node = current_node->parent;
        while(
            parent_node->child[0] != nullptr && parent_node->child[0]->dcb &&
            parent_node->child[1] != nullptr && parent_node->child[1]->dcb
        ) {
            current_node = parent_node;
            setDCBTrue(current_node);
            parent_node = current_node->parent;
        }
        setDCBTrue(current_node);
    }

    // generate representative string vector
    inline std::vector<std::string> genRepStrVec() {
        std::vector<std::string> oset;
        std::vector<char> cs;
        for(int idx = 0; idx <= 1; idx++) {
            node* current_node = root->child[idx];
            if(current_node == nullptr)
                continue;
            cs.push_back('0' + idx);
            do {
                if(current_node->dcb == true || current_node->next_depth == MAXBITLEN) {
                    std::string s(cs.begin(), cs.end());
                    s.append(MAXBITLEN - current_node->next_depth, '*');
                    oset.push_back(s);
                    node* last_node = current_node;
                    current_node = current_node->parent;
                    deleteNode(last_node);
                    cs.pop_back();
                }
                else if(current_node->child[0] != nullptr) {
                    cs.push_back('0');
                    current_node = current_node->child[0];
                }
                else if(current_node->child[1] != nullptr) {
                    cs.push_back('1');
                    current_node = current_node->child[1];
                }
                else {
                    node* last_node = current_node;
                    current_node = current_node->parent;
                    deleteNode(last_node);
                    cs.pop_back();
                }
            } while(!cs.empty());
        }
        return oset;
    }
};


/*** Generate Representative String ***/

// genRepString for Alice
std::vector<std::string> genRepStringA(D_INT &start, D_INT &end);

// genRepString for Bob
std::vector<std::string> genRepStringB(D_INT &iset, D_INT thres);