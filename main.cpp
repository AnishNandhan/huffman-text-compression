#include <iostream>
#include <fstream>
#include <map>
#include <vector>

using namespace std;

fstream open_file(char* filename) {
    fstream myfile;
    myfile.open(filename, ios::in);

    return myfile;
}

class Heap {

};

class Node {
    char c;
    int freq;
    struct Node* left;
    struct Node* right;

    Node(char c, int freq, struct Node* left = NULL, struct Node* right = NULL) {
        this->c = c;
        this->freq = freq;
        this->left = left;
        this->right = right;
    }

    bool lt(const Node& other) {
        return this->freq < other.freq;
    }
};

class HashTable {
    private:
        vector<int> table;

    public:
        HashTable() {
            for(int i = 0; i < 100; i++) {
                table.push_back(0);
            }
        }

        void insert(char c) {
            int ascii = (int)c;
            ascii = ascii - 32;
            table[ascii]++;
        }

        int getFreq(char c) {
            int ascii = (int)c;
            ascii = ascii - 32;

            return table[ascii];
        }
};



int main(int argc, char** argv) {
    fstream textfile = open_file("test.txt");
    HashTable freq_table;

    char ch;

    while(1) {
        textfile >> ch;
        if(textfile.eof()) {
            break;
        }
        freq_table.insert(ch);
    }

    for(int i = 32; i < 128; i++) {
        char c = (char)i;
        if (freq_table.getFreq(c) != 0) {
            cout << c << " : " << freq_table.getFreq(c) << "\n";
        }         
    }

    textfile.close();
}
