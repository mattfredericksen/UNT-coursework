//
// Created by mfred on 4/2/2020.
//

#ifndef AUTOCOMPLETE_TRIE_NODE_H
#define AUTOCOMPLETE_TRIE_NODE_H

#include <string>
#define ALPHABET_SIZE 26

using namespace std;

struct TrieNode {
    // pointers to all possible children
    // initialized to nullptr
    TrieNode* children[ALPHABET_SIZE] = {};
    
    // word only gets set if a node is the end of a word
    string* word = nullptr;

    TrieNode() {};
    ~TrieNode() {};
    
    // adds a word to the trie, creating nodes if necessary
    void insert(string word);
    
    // prints all words that share a prefix with the given node
    static void print_children(const TrieNode* node);
    
    // prints all words that start with `prefix`
    void autocomplete(string prefix);
};


#endif //AUTOCOMPLETE_TRIE_NODE_H
