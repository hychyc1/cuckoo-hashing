#include "hashTableBase.h"
#include "basic.h"
#include "advanced.h"

// Main function to test the hash table
using namespace std;
int main() {
    int N = 100007;     // Number of slots
    // int d = 4;         // Number of hash functions
    double epsilon = 0.01;

    // Create an instance of the BubbleUpHashTable
    // BasicBubbleUpHashTable ht(N, computeBasicD(epsilon));
    AdvancedCuckooHashing ht(N, 0.5, 0.01);
    ht.display();

    // Insert elements
    for (int i = 1; i <= 10000; ++i) {
        ht.insert(i);
        if(i % 1000 == 0) cout << i << endl;
        // ht.display();
    }

    ht.displayParams();
    // Search for elements
    for (int i = 1; i <= 10005; ++i) {
        if(i % 1000 == 0) cout << i << endl;
        if (!ht.search(i)) {
            std::cerr << "Key " << i << " not found!" << std::endl;
        }
    }
    
    // Remove elements
    for (int i = 1; i <= 500; ++i) {
        ht.remove(i);
    }

    // Search for removed elements
    for (int i = 1; i <= 500; ++i) {
        if (ht.search(i)) {
            std::cerr << "Key " << i << " should have been removed!" << std::endl;
        }
    }

    // Display hash table
    // ht.display();

    return 0;
}
