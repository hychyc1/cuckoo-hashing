#include <iostream>
#include <vector>
#include <functional>
#include <random>
#include <chrono>
#include <cmath>
#include <limits>

// Abstract base class for hash tables
class HashTableBase {
protected:
    int N;  // Number of slots in the hash table

public:
    HashTableBase(int numSlots) : N(numSlots) {}
    virtual ~HashTableBase() {}

    // Pure virtual functions
    virtual bool insert(int key) = 0;
    virtual bool search(int key) = 0;
    virtual bool remove(int key) = 0;

    // Optional: A function to display the hash table (can be overridden)
    virtual void display() const = 0;
};

// Element stored in the hash table
struct Element {
    int key;
    int choice;

    Element(int k = -1, int c = 0) : key(k), choice(c) {}
};

// Bubble-up hash table implementation
class BubbleUpHashTable : public HashTableBase {
private:
    int d;  // Number of hash functions
    int MAX_RELOCATIONS;  // Maximum number of allowed relocations
    std::vector<Element> table;
    std::vector<std::function<int(int)>> hashFunctions;

    // Initialize hash functions with different seeds
    void initializeHashFunctions() {
        std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        for (int i = 0; i < d; ++i) {
            int seed = rng();
            hashFunctions.push_back([seed, this](int key) {
                std::hash<int> hasher;
                return (hasher(key ^ seed) % N);
            });
        }
    }

public:
    // Constructor
    BubbleUpHashTable(int numSlots, double epsilon)
        : HashTableBase(numSlots), d(std :: ceil(3 * std :: log(1/epsilon) + 1)) {
        // For MAX_RELOCATIONS, we can use a function of log(N)
        MAX_RELOCATIONS = static_cast<int>(6 * std::log(N));
        table.resize(N);
        initializeHashFunctions();
    }

    // Insert an element into the hash table
    bool insert(int key) override {
        Element x(key, 0);
        int relocations = 0;
        while (relocations < MAX_RELOCATIONS) {
            int pos = 0;

            if (x.choice == d) {
                // Type 1 Move
                x.choice = d - 1;
                relocations++;
            } else if (x.choice == d - 1) {
                // Type 2 Move
                x.choice = d;
                relocations++;
            } else {
                // Type 3 Move: Check h_{choice(x)+1} to h_{d-2}
                bool foundFreeSlot = false;
                for (int i = x.choice + 1; i <= d - 2; ++i) {
                    int tempPos = hashFunctions[i](x.key);
                    if (table[tempPos].key == -1) {
                        x.choice = i;
                        foundFreeSlot = true;
                        break;
                    }
                }
                // Type 4 Move
                if (foundFreeSlot == false)
                    x.choice = d-1;
            }

            pos = hashFunctions[x.choice](x.key);
            if (table[pos].key == -1) {
                // Slot is empty, place x here
                table[pos] = x;
                return true;
            } else {
                // Evict the current element
                Element y = table[pos];
                table[pos] = x;
                x = y;
            }
        }

        // Failed to insert after too many relocations
        std::cerr << "Insertion failed for key " << key << std::endl;
        return false;
    }

    // Search for an element in the hash table
    bool search(int key) override {
        for (int i = 0; i < d; ++i) {
            int pos = hashFunctions[i](key);
            if (table[pos].key == key) {
                return true;
            }
        }
        return false;
    }

    // Remove an element from the hash table
    bool remove(int key) override {
        for (int i = 0; i < d; ++i) {
            int pos = hashFunctions[i](key);
            if (table[pos].key == key) {
                table[pos] = Element();
                return true;
            }
        }
        return false;
    }

    // Display the hash table (for testing purposes)
    void display() const override {
        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i].key != -1) {
                std::cout << "Slot " << i << ": Key = " << table[i].key
                          << ", Choice = " << table[i].choice << std::endl;
            }
        }
    }
};

// Main function to test the hash table
int main() {
    int N = 10007;     // Number of slots
    // int d = 4;         // Number of hash functions
    double epsilon = 0.01;

    // Create an instance of the BubbleUpHashTable
    BubbleUpHashTable ht(N, epsilon);

    // Insert elements
    for (int i = 1; i <= 1000; ++i) {
        ht.insert(i);
    }

    // Search for elements
    for (int i = 1; i <= 1000; ++i) {
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
