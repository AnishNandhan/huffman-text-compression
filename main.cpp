#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include "hashtable.cpp"

using namespace std;

fstream open_file(char* filename) {
    fstream myfile;
    myfile.open(filename, ios::in);

    return myfile;
}

int binary_to_decimal(string in)
{
	int result = 0;
	for (int i = 0; i < in.size(); i++)
		result = result * 2 + in[i] - '0';
	return result;
}

string decimal_to_binary(int in)
{
	string temp = "";
	string result = "";
	while (in)
	{
		temp += ('0' + in % 2);
		in /= 2;
	}
	result.append(8 - temp.size(), '0');													//append '0' ahead to let the result become fixed length of 8
	for (int i = temp.size() - 1; i >= 0; i--)												
	{
		result += temp[i];
	}
	return result;
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

priority_queue<Node*, vector<Node*>, compareNode> makePQ(const string& infile) {
    fstream textfile;
    textfile.open(infile, ios::in);
    HashTable freq_table;
    char ch;
    while(textfile.get(ch)) {
        freq_table.insert(ch);
    }

    textfile.close();

    priority_queue<Node*, vector<Node*>, compareNode> q;

    for(int i = 0; i < 128; i++) {
        if(freq_table.getFreq((char)i) != 0) {
            q.push(new Node((char)i, freq_table.getFreq((char)i)));
        }
    }

    if(freq_table.getFreq('\n') != 0) {
        q.push(new Node('\n', freq_table.getFreq('\n')));
    }

    return q;
}

Node* buildHuffmanTree(priority_queue<Node*, vector<Node*>, compareNode> temp) {
    while(temp.size() != 1) {
        Node *left = temp.top();
        temp.pop();
        Node *right = temp.top();
        temp.pop();

        int sum_freq = left->freq + right->freq;

        temp.push(new Node('\0', sum_freq, left, right));
    }

    return temp.top();
}

void saveEncode(string& infile, priority_queue<Node*, vector<Node*>, compareNode> q, map<char, string>& codes) {
    fstream outfile, textfile;
    textfile.open(infile, ios::in);
    outfile.open("encoded", ios::out | ios::binary);

    string in = "", s = "";

    while (!q.empty())
	{//get all characters and their huffman codes for output
		Node* current = q.top();
		in += current->c;
		s.assign(127 - codes[current->c].size(), '0'); 											//set the codes with a fixed 128-bit string form[000¡­¡­1 + real code]
		s += '1';																			//'1' indicates the start of huffman code
		s.append(codes[current->c]);
		in += (char)binary_to_decimal(s.substr(0, 8));										
		for (int i = 0; i < 15; i++)
		{//cut into 8-bit binary codes that can convert into saving char needed for binary file
			s = s.substr(8);
			in += (char)binary_to_decimal(s.substr(0, 8));
		}
		q.pop();
	}
	s.clear();

    char id;
	
	while (textfile.get(id))
	{//get the huffman code
		s += codes[id];
		while (s.size() > 8)
		{//cut into 8-bit binary codes that can convert into saving char needed for binary file
			in += (char)binary_to_decimal(s.substr(0, 8));
			s = s.substr(8);
		}
	}
	int count = 8 - s.size();
	if (s.size() < 8)
	{//append number of 'count' '0' to the last few codes to create the last byte of text
		s.append(count, '0');
	}
	in += (char)binary_to_decimal(s);															//save number of 'count' at last
	in += (char)count;

	outfile.write(in.c_str(), in.size());
	textfile.close();
	outfile.close();
}




int main(int argc, char** argv) {
    string input = "";
    string output = "";

    char ch;

    priority_queue<Node*, vector<Node*>, compareNode> q = makePQ("test.txt");

    Node* tree = buildHuffmanTree(q);

    map<char, string> codes;

    encode(tree, "", codes);


    for(int i = 0; i < input.length(); i++) {
        output += codes[input[i]];
    }

    

    //printing encdoed binary string 

    cout << output << "\n";


    //decoding binary string 
    string final = decode(tree, output);
    cout<< final <<endl;
}
