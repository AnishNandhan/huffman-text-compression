#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <queue>
#include <string>

using namespace std;

fstream open_file(char* filename) {
    fstream myfile;
    myfile.open(filename, ios::in);

    return myfile;
}

class Node {
    public:
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

        bool operator>(const Node& other) {
            return this->freq > other.freq;
        }
};

struct compareNode {
    bool operator()(Node* n1, Node* n2) {
        return n1->freq > n2->freq;
    }
};

class HashTable {
    private:
        vector<int> table;

    public:
        HashTable() {
            for(int i = 0; i < 97; i++) {
                table.push_back(0);
            }
        }

        void insert(char c) {
            if(c == '\n') {
                table[96]++;
                return;
            }
            int ascii = (int)c;
            ascii = ascii - 32;
            table[ascii]++;
        }

        int getFreq(char c) {
            if(c == '\n') {
                return table[96];
            }
            int ascii = (int)c;
            ascii = ascii - 32;

            return table[ascii];
        }

        vector<int> getTable() {
            return table;
        }
};

class HuffmanTree {
    public:
        


};

void encode(Node* root, string str, map<char, string> &huffmanCode)
{
    if (root == nullptr) {
        return;
    }
 
    // found a leaf node
    if (root->left == NULL && root->right == NULL) {
        huffmanCode[root->c] = (str != "") ? str : "1";
    }
 
    encode(root->left, str + "0", huffmanCode);
    encode(root->right, str + "1", huffmanCode);
}

string decode(Node* root, string a){
    string out = "";
    struct Node* curr = root;
    for(int i = 0;i <a.size();i++){
        if(a[i] == '0'){
            curr = curr->left;
        }
        else{
            curr = curr->right;
        }

        if(curr->left == NULL && curr->right == NULL){
            out += curr->c; 
            curr = root; 
        }
    }

    return out + '\0';
}


int main(int argc, char** argv) {
    fstream textfile = open_file("test.txt");
    HashTable freq_table;
    string input = "";
    string output = "";

    char ch;

    while(textfile.get(ch)) {
        freq_table.insert(ch);
        input = input + ch;
    }

    priority_queue<Node*, vector<Node*>, compareNode> tree;

    for(int i = 32; i < 128; i++) {
        if(freq_table.getFreq((char)i) != 0) {
            tree.push(new Node((char)i, freq_table.getFreq((char)i)));
        }
    }

    if(freq_table.getFreq('\n') != 0) {
        tree.push(new Node('\n', freq_table.getFreq('\n')));
    }

    while(tree.size() != 1) {
        Node *left = tree.top();
        tree.pop();
        Node *right = tree.top();
        tree.pop();

        int sum_freq = left->freq + right->freq;

        tree.push(new Node('\0', sum_freq, left, right));
    }


    Node *root = tree.top();

    map<char, string> codes;

    encode(root, "", codes);

    for(int i = 0; i < input.length(); i++) {
        output += codes[input[i]];
    }

    //printing encdoed binary string 

    cout << output << "\n";


    //decoding binary string 
    string final = decode(root, output);
    cout<< final <<endl;
    textfile.close();
}
