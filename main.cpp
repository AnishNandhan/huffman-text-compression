#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <cstring>
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
	result.append(8 - temp.size(), '0');
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

        Node() {
            this->c = '\0';
            this->freq = 0;
            this->left = NULL;
            this->right = NULL;
        }

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

void saveEncode(const string& infile, priority_queue<Node*, vector<Node*>, compareNode> q, map<char, string>& codes) {
    fstream outfile, textfile;
    textfile.open(infile, ios::in);
    outfile.open("encoded.ng", ios::out | ios::binary);

    string in = "", s = "";

    in += (char)q.size();

    while (!q.empty())
	{
		Node* current = q.top();
		in += current->c;
		s.assign(127 - codes[current->c].size(), '0'); 											
		s += '1';
		s.append(codes[current->c]);
		in += (char)binary_to_decimal(s.substr(0, 8));										
		for (int i = 0; i < 15; i++)
		{
			s = s.substr(8);
			in += (char)binary_to_decimal(s.substr(0, 8));
		}
		q.pop();
	}
	s.clear();

    char id;
	
	while (textfile.get(id))
	{
		s += codes[id];

		while (s.size() > 8)
		{
			in += (char)binary_to_decimal(s.substr(0, 8));
			s = s.substr(8);
		}
	}
	int count = 8 - s.size();
	if (s.size() < 8)
	{
		s.append(count, '0');
	}
	in += (char)binary_to_decimal(s);
	in += (char)count;

	outfile.write(in.c_str(), in.size());

	cout << "Encoded file saves as 'encoded.ng'\n";

	textfile.close();
	outfile.close();
}

inline void buildTree(string& path, char a_code, Node* root)
{
	Node* current = root;
	for (int i = 0; i < path.size(); i++)
	{
		if (path[i] == '0')
		{
			if (current->left == NULL)
				current->left = new Node;
			current = current->left;
		}
		else if (path[i] == '1')
		{
			if (current->right == NULL)
				current->right = new Node;
			current = current->right;														 
		}
	}
	current->c = a_code;					
}

Node* rebuildHuffmanTree(const string& in_file_name){
    fstream in_file;
	in_file.open(in_file_name, ios::in | ios::binary);

	unsigned char size;
	in_file.read(reinterpret_cast<char*>(&size), 1);

	Node *root = new Node;
	for (int i = 0; i < size; i++)
	{
		char a_code;
		unsigned char h_code_c[16];	
		in_file.read(&a_code, 1);
		in_file.read(reinterpret_cast<char*>(h_code_c), 16);
		string h_code_s = "";
		
		for (int i = 0; i < 16; i++)
		{
			h_code_s += decimal_to_binary(h_code_c[i]);
		}
		int j = 0;

		while (h_code_s[j] == '0')
		{
			//delete the added '000...­1' to get the real huffman code
			j++;
		}

		h_code_s = h_code_s.substr(j + 1);
		buildTree(h_code_s, a_code, root);
	}
	in_file.close();

    return root;
}

void saveDecode(const string& in_file_name, Node* root){
    fstream in_file, out_file;

	in_file.open(in_file_name, ios::in | ios::binary);
	out_file.open("decoded.txt", ios::out);

	unsigned char size;		

	in_file.read(reinterpret_cast<char*>(&size), 1);
	in_file.seekg(-1, ios::end);	

	char count0;

	in_file.read(&count0, 1);
	in_file.seekg(1 + 17 * size, ios::beg);

	vector<unsigned char> text;
	unsigned char textseg;
	
	in_file.read(reinterpret_cast<char*>(&textseg), 1);
	while (!in_file.eof())
	{
		text.push_back(textseg);
		in_file.read(reinterpret_cast<char*>(&textseg), 1);
	}
	Node* current = root;
	string path;
	for (int i = 0; i < text.size() - 1; i++)
	{
		path = decimal_to_binary(text[i]);
		if (i == text.size() - 2)
			path = path.substr(0, 8 - count0);
		for (int j = 0; j < path.size(); j++)
		{
			if (path[j] == '0')
				current = current->left;
			else
				current = current->right;
			if (current->left == NULL && current->right == NULL)
			{
				out_file.put(current->c);
				current = root;
			}
		}
	}

	cout << "File decoded into 'decoded.txt'\n";

	in_file.close();
	out_file.close();
}


int main(int argc, char** argv) {
	if (argc != 3) {
		cout << "Usage: ./a.out [-d | -e] <filename>\n";
		exit(1);
	}

	if (argc == 3 && strcmp(argv[1], "-e") == 0) {
		string file(argv[2], argv[2] + strlen(argv[2]));
		priority_queue<Node*, vector<Node*>, compareNode> q = makePQ(file);

		Node* tree = buildHuffmanTree(q);

		map<char, string> codes;

		encode(tree, "", codes);

		saveEncode("test.txt", q, codes);
	}

	else if (argc == 3 && strcmp(argv[1], "-d") == 0) {
		string file(argv[2], argv[2] + strlen(argv[2]));
		Node* tree = rebuildHuffmanTree(file);
		saveDecode(file, tree);
	}
}
