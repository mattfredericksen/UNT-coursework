### Description
This was a small extra credit project completed in April 2020. It implements a word autocompletion algorithm using a [Trie](https://en.wikipedia.org/wiki/Trie) data structure. Each node of the trie contains a character "key", and leaf nodes contain whole strings. This allows rapidly retrieving a list of words from a given prefix.

My implementation is not very efficient, and is just meant to demonstrate general understanding of the data structure. If I wanted to make it more efficient, I would first create a function for retrieving a word base on a node's ancestors, rather than storing the complete string in the lead node. I would then remove the large number of NULL pointers (which exist because each node has pointers to 26 child nodes, even when those pointers are unnecessary). 

### Usage
1. Download all project files
2. Compile with Makefile
3. Run ./autocomplete dictionary.txt

### Example
```
$ ./autocomplete dictionary.txt
Loading dictionary. Standby...
Dictionary loaded!
Start typing a word and hit enter ('quit!' to end): test
Possible completions:
test
testament
testamentary
testate
testbed
testes
testicle
testicular
testify
testimonial
testimony
testy

Start typing a word and hit enter ('quit!' to end): quit!
$ 
```
