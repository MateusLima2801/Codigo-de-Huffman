#include <iostream>
#include <string>
#include <queue>
#include <exception>
#include <fstream>
#include <vector>
using namespace std;

static string st[256];
static int freq[256];

class Node;
class Huffman;

class Huffman
{
    public:
        static string compress(string message);
        static string expand(string bit_input, Node *root);
};

class Node 
{
    public:
        char ch;   // usado só nas folhas
        int freq;  // usado só para construção da trie
        Node *left, *right;

        Node(char ch, int freq) {
            this->ch = ch;
            this->freq = freq;
            this->left = nullptr;
            this->right = nullptr;
        }
         Node(char ch, int freq, Node *left, Node *right) {
            this->ch = ch;
            this->freq = freq;
            this->left = left;
            this->right = right;
        }

        Node()
        {
            this->ch = '\0';
            this->freq = 0;
            this->left = nullptr;
            this->right = nullptr;
        }

        bool isLeaf() { 
            return (left == nullptr && right == nullptr); 
        }
    

        // int compareTo(Node that) { 
        //     return this.freq - that.freq; 
        // }
};

struct comp
{
	bool operator()(Node* l, Node* r)
	{
		// highest priority item has lowest frequency
		return l->freq > r->freq;
	}
};

void buildFreqTable(string message);
Node* buildTree(string message);
void buildCode(Node *x, string s);
Node* getNode(char ch, int freq, Node* left, Node* right);
void print2Leaf(const Node * cur);
string writeTree(Node *x, string s);
Node *readTree(string input, int& pos);
void print2Leaf_ch(const Node * cur);
bool writeMessage(string text, string fileName);
bool readMessage(string &message, string fileName);
bool putEncodedMessage(string message, string writtenTree, string fileName);
bool getEncodedMessage(string &message, string &writtenTree, string fileName);

int main()
{
    //=====================================================
    //                    DECODIFICATION
    //=====================================================
    string message = "";
    readMessage(message, "baseMessage.txt");

    Node* root = buildTree(message);
    string writtenTree;

    buildCode(root, "");


    string codified_message = Huffman::compress(message);
    writtenTree = writeTree(root, "");
    putEncodedMessage(codified_message, writtenTree, "encodedMessage.txt");
    //=====================================================
    //                    DECODIFICATION
    //=====================================================
    string enc_writtenTree="";
    string enc_codified_message="";
    getEncodedMessage(enc_codified_message, enc_writtenTree, "encodedMessage.txt");
    int pos = 0;
    Node *root_dec = readTree(enc_writtenTree, pos);

    
    string decodified_message = Huffman::expand(enc_codified_message, root_dec);
    writeMessage(decodified_message, "decodifiedMessage.txt");

    //=====================================================
    //                    COMPARISON
    //=====================================================

    cout <<endl<< "ORIGINAL SIZE: " << 8*message.length() << endl;
    cout << "COMPRESSED SIZE: " << codified_message.length() << endl;
    cout << "DECOMPRESSED SIZE: " << 8*decodified_message.length() << endl;
    cout << "COMPRESSIBILITY: -" << 100*(8*message.length() - codified_message.length())/(8.0*message.length())<< "%" << endl;
    
    return 0;
}

bool putEncodedMessage(string message, string writtenTree, string fileName)
{
    ofstream filestr;
    filestr.open(fileName, ios::binary);

    if(!filestr.is_open())
    {
        return false;
    }

    filestr << writtenTree << "\n" << message;
    filestr.close();
    return true;
}

bool getEncodedMessage(string &message, string &writtenTree, string fileName)
{
    ifstream filestr;
    filestr.open(fileName, ios::in);

    if(!filestr.is_open())
    {
        return false;
    }

    while(!filestr.eof())
    {
        string line;
        getline(filestr, line, '\n');
        if(!filestr.eof())
        {
            writtenTree+=line;
            writtenTree+="\n";
        }
        else
        {
            message+=line;
        }
        
    }
    filestr.close();
    return true;
}

bool writeMessage(string text, string fileName)
{
    ofstream filestr;
    filestr.open(fileName, ios::out);

    if(!filestr.is_open())
    {
        return false;
    }

    filestr << text;
    filestr.close();
    return true;
}

bool readMessage(string &message, string fileName)
{
    ifstream filestr;
    filestr.open(fileName, ios::in);

    if(!filestr.is_open())
    {
        return false;
    }

    while(!filestr.eof())
    {
        string line;
        getline(filestr, line, '\n');
        message+=line;
        message+="\n";
    }
    filestr.close();
    return true;
}

string writeTree(Node *x, string s)
{
    //cout << s << endl;

    if(x->isLeaf())
    {
        s+='1';
        s+=x->ch;
        return s;
    }
    s+='0';
    s = writeTree(x->left, s);
    s = writeTree(x->right, s);

    return s;
}

Node *readTree(string input, int &pos) 
{
    if(pos == input.length())
    {
        cout << "Deu maior!" << endl;
        return nullptr;
    }

    //cout << input[pos];
    if (input[pos] == '1')
    {
       
        char c = input[++pos];
        //cout << c;
        pos++;
        return new Node(c, 1, nullptr, nullptr);
        
    }

    pos++;
    return new Node('\0', 0, readTree(input, pos), readTree(input, pos));
    
}

void buildCode(Node *x, string s)
{
    if(x->isLeaf())
    {
        st[x->ch] = s;
        //cout << x->ch << ": " << s <<endl;
        return;
    }
    buildCode(x->left, s+'0');
    buildCode(x->right, s+'1');
} 

Node* getNode(char ch, int freq, Node* left, Node* right)
{
	Node* node = new Node();

	node->ch = ch;
	node->freq = freq;
	node->left = left;
	node->right = right;

	return node;
}

Node* buildTree(string message)
{
    buildFreqTable(message);

    priority_queue<Node*, vector<Node*>, comp> collection;

	for (int i = 0; i< 256; i++) {
        if(freq[i]>0)
		{
            collection.push(getNode(i, freq[i], nullptr, nullptr));
        }
	}

	while (collection.size() > 1)
	{
		Node *left = collection.top(); 
        collection.pop();
		Node *right = collection.top();
        collection.pop();

		int sum = left->freq + right->freq;
		collection.push(getNode('\0', sum, left, right));
	}

	Node *root = collection.top();
    // cout <<"CODE TREE" << endl;
    // print2Leaf(root);
    // cout << endl;

    
    return root;
}

void print2Leaf(const Node * cur)
{
        if(!( cur->left ==nullptr && cur->right == nullptr))
        {
            cout << "(" << cur->freq << ")"<< " -> (" << cur->left->ch << ") ("<< cur->right->ch  << ")"<< endl;
            print2Leaf(cur->left);
            print2Leaf(cur->right);
        }       
}

void print2Leaf_ch(const Node * cur)
{
        if(!( cur->left ==nullptr && cur->right == nullptr))
        {
            cout << "(" << cur->freq << ")"<< " -> (" << cur->left->ch << ") ("<< cur->right->ch  << ")"<< endl;
            print2Leaf(cur->left);
            print2Leaf(cur->right);
        }      
}

void buildFreqTable(string message)
{
    for(int i=0; i<256; i++)
    {
        freq[i] = 0;
    }
    
    for(int j = 0; j<message.length(); j++)
    {
        freq[message[j]]++;
    }

    // cout << "FREQ TABLE"<< endl;
    // for(int i=0; i<256; i++)
    // {
    //     if(freq[i]>0) cout << (char)i << ": " << freq[i] << endl;
    // }
    // cout << endl;
}

string Huffman::compress(string message)
{
    
    string bit_output = "";
    const int size = message.length();
    char input[size];

    for(int i=0; i<size; i++) input[i] = message[i];

    for(int i = 0; i < size; i++)
    {
        string code = st[input[i]];
        bit_output.append(code);
    }
    return bit_output;
}

string Huffman::expand(string bit_input, Node *root)
{
    string output ="";
    int j = 0;
    Node* x = root;

    while(j <bit_input.length())
    {
        x = root;
        while(!x->isLeaf())
        {
            if(bit_input[j] == '1') x = x->right;
            else x = x->left;
            ++j;
        }
        //cout << x->ch;
        output+={x->ch};

    }
    return output;
}
