#include <iostream>
#include "trie_node.h"
using namespace std;

void TrieNode::insert(string word) {
    // move down the trie, creating any necessary nodes
    TrieNode* pos = this;
    for (int i = 0; i < word.size(); i++) {
        // we don't handle uppercase letters - wasteful
        int char_index = tolower(word[i]) - 'a';
        if (!pos->children[char_index]) {
            pos->children[char_index] = new TrieNode();
        }
        pos = pos->children[char_index];
    }
    // since this node ends a valid word, set its string
    pos->word = new string(word);
}

void TrieNode::print_children(const TrieNode *node) {
    // if `word` isn't null, then this node ends a valid word
    if (node->word) {
        cout << *(node->word) << endl;
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            print_children(node->children[i]);
        }
    }
}

void TrieNode::autocomplete(string prefix) {
    // move down the trie to the end of the prefix
    // then print all words after that point
    TrieNode* pos = this;
    for (int i = 0; i < prefix.size(); i++) {
        int char_index = tolower(prefix[i]) - 'a';
        if (pos->children[char_index]) {
            pos = pos->children[char_index];
        } else {
            return;
        }
    }
    print_children(pos);
}
