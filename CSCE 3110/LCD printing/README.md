### Description
This was a small extra credit project completed in April 2020. It prints numbers in an "LCD" style, based on instructions in the input file.

### Usage
1. Download display.cpp and run `gcc -o display display.cpp`.
2. Run `./display <input file>`  
  
Use an input with any number of lines in the following format:  
`<number size> <number>`  
Input file should be terminated with a `0 0` line.


### Example
input.txt:
```
3 12
0 0
```
program output:
```
      ---
   |     |
   |     |
   |     |
      ---
   | |
   | |
   | |
      ---
```
