#include <vector>

using namespace std;

class HashTable {
    private:
        vector<int> table;

    public:
        HashTable() {
            for(int i = 0; i < 128; i++) {
                table.push_back(0);
            }
        }

        void insert(char c) {
            int ascii = (int)c;
            table[ascii]++;
        }

        int getFreq(char c) {
            int ascii = (int)c;
            return table[ascii];
        }

        vector<int> getTable() {
            return table;
        }
};