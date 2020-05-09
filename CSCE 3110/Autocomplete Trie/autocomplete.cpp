#include <iostream>
#include <fstream>
#include "trie_node.h"

using namespace std;

int main(int argc, char* argv[]) {
    // make sure we have a single file name argument
    if (argc != 2) {
        cout << "Provide an input file as an argument.\n";
        return -1;
    }

    // make sure we can read the file
    ifstream file(argv[1]);
    if (file.fail()) {
        cout << "Unable to access \"" << argv[1] << "\". Exiting.\n";
        return -1;
    }

    // load each word from the file into our trie
    cout << "Loading dictionary. Standby...\n";
    string word;
    TrieNode trie;
    while (getline(file, word)) {
        trie.insert(word);
    }
    cout << "Dictionary loaded!\n"
         << "Start typing a word and hit enter ('quit!' to end): ";
    cin >> word;

    // loop with input from user, printing autocomplete possibilities
    while (word != "quit!") {
        cout << "Possible completions:\n";
        trie.autocomplete(word);

        cout << "\nStart typing a word and hit enter ('quit!' to end): ";
        cin >> word;
    }
}
