#include <iostream>
#include <fstream>
#include <string>
using namespace std;


// NumMap represents which sections
// of the LCD should be "activated."
const char
NumMap[] = {119,  // 0 = 01110111
            36,   // 1 = 00100100
            93,   // 2 = 01011101
            109,  // 3 = 01101101
            46,   // 4 = 00101110
            107,  // 5 = 01101011
            123,  // 6 = 01111011
            37,   // 7 = 00100101
            127,  // 8 = 01111111
            47};  // 9 = 00101111

void print_number(string number, int display_size) {
    if (display_size < 1) return;

    // create an array of bytes; each byte
    // corresponds to a digit in `number`
    char display_bits[number.length()];
    for (int i = 0; i < number.length(); i++) {
        if (!isdigit(number[i])) {
            cout << number << " is not a number!\n";
            return;
        }
        // convert character to number
        display_bits[i] = NumMap[number[i] - '0'];
    }

    string line;
    for (int line_number = 0; line_number < 5; line_number++) {
        line.clear();
        // odd lines are vertically expanded
        if (line_number % 2) {
            for (int i = 0; i < number.length(); i++) {
                line += (display_bits[i] & 1 ? "|" : " ")
                         + string(display_size, ' ');
                display_bits[i] >>= 1;
                line += display_bits[i] & 1 ? "| " : "  ";
                display_bits[i] >>= 1;
            }
            for (int j = 0; j < display_size; j++) {
                cout << line << endl;
            }
        }
        // even lines are horizontally expanded
        else {
            for (int i = 0; i < number.length(); i++) {
                line += " " + string(display_size, display_bits[i] & 1 ? '-' : ' ') + "  ";
                display_bits[i] >>= 1;
            }
            cout << line << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    int display_size;  // first arg in input file
    string number;  // second arg in input file
    ifstream input_file;

    if (argc != 2) {
        cout << "Error: provide a single file as an argument\n";
        return 1;
    }

    input_file.open(argv[1]);
    if (!input_file.is_open()) {
        cout << "Error: unable to open file: " << argv[1] << endl;
        return 1;
    }

    while (input_file >> display_size >> number) {
        if (display_size == 0 && number == "0") {
            break;
        }
        print_number(number, display_size);
        cout << endl;
    }
    return 0;
}
