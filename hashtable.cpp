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
            ascii = ascii - 32;
            table[ascii]++;
        }

        int getFreq(char c) {
            int ascii = (int)c;
            ascii = ascii - 32;

            return table[ascii];
        }

        vector<int> getTable() {
            return table;
        }
};