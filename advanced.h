#include <iostream>
#include <vector>
#include <cmath>
#include <random>

int randomInRange(int l, int r) {
    return rand() % (r-l+1) + l;
}


class AdvancedCuckooHashing : public HashTableBase {
private:

// Element structure to store key and its hash choice in the hash table
struct Element {
    int key;
    int choice;

    Element(int k = -1, int c = 0) : key(k), choice(c) {}
};

// parameters 
    int dcore, dmax, d, gamma;
    double alpha;
    double eps, eps_core;
    
    int current_size, phase;
    
    int MAX_RELOCATIONS;  // Maximum number of allowed relocations
    std::vector<Element> table;
    std::vector<std::function<int(int)>> hashFunctions;

    // Initialize hash functions with different seeds
    void initializeHashFunctions() {
        std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        for (int i = 0; i <= d; ++i) {
            int seed = rng();
            hashFunctions.push_back([seed, this](int key) {
                std::hash<int> hasher;
                return (hasher(key ^ seed) % N);
            });
        }
    }
    
    // parameters with place holder functions
    void computeParameters() {
        // take dcore = 1/alpha + 5 satisfies the first condition
        d = std :: ceil(std :: log (1 / eps) + alpha) + 0.001;
        dcore = 1/alpha;
        double D = exp(-dcore);
        // where is epsilon core used ?
        eps_core = exp(-dcore + D);
        gamma = int(std :: ceil(std :: log(1 / eps) + alpha) + 0.001) % dcore;
        dmax = gamma + dcore; // starting at phase 1
    }

    // The moves only update the choices, need to explicitly place x
    void type1Move(Element x) {
        x.choice = randomInRange(dmax-dcore+1, dmax);
    }

    bool type2Move(Element x) {
        // std :: cout << "AA" << std :: endl;
        bool foundFreeSlot = false;
        for (int i = std :: max(0, x.choice - dcore); i <= dmax - dcore; ++i) {
            int tempPos = hashFunctions[i](x.key);
            if (table[tempPos].choice == -1) {
                x.choice = i;
                foundFreeSlot = true;
                return true;
            }
        }
        return false;
    }

    void incrPhase() {
        double load_factor = current_size / N;
        if(load_factor > 1 - std :: exp(-dmax + alpha))
            ++phase, dmax += dcore;
    }

public:
    AdvancedCuckooHashing(int numSlots, double _alpha, double _eps)
        : HashTableBase(numSlots), current_size(0), phase(1), eps(_eps), alpha(_alpha) {
        table.resize(N);  // Initialize table with N slots
        MAX_RELOCATIONS = static_cast<int>(6 * std :: pow(std::log(N), 5));
        computeParameters();
        initializeHashFunctions();
    }

    bool insert(int key) override {
        Element x(key, 0);
        int relocations = 0, pos = 0;
        while (relocations < MAX_RELOCATIONS) {
            if (x.choice >= dmax-dcore + 1) type1Move(x), ++relocations;
            else {
                if(!type2Move(x)) type1Move(x), ++relocations;
            }
            pos = hashFunctions[x.choice](x.key);
            if (table[pos].key == -1) {
                // Slot is empty, place x here
                table[pos] = x;
                current_size++; 
                incrPhase();
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

    // Search for a key in the table
    bool search(int key) override {
        for (int i = dmax; ~i; --i) {
            int pos = hashFunctions[i](key);
            if (table[pos].key == key) {
                return true;
            }
        }
        return false;
    }

    // Remove a key from the table
    bool remove(int key) override {
        for (int i = dmax; ~i; --i) {
            int pos = hashFunctions[i](key);
            if (table[pos].key == key) {
                table[pos].key = -1;
                current_size--;
                return true;
            }
        }
        return false;
    }


    void displayParams() {
        std :: cout << "alpha " << alpha << "; dcore: " << dcore << "; gamma: " << gamma << "; dmax: " << dmax << "; d: " << d << std :: endl;
    }
    // Display the hash table
    void display() const override {
        std :: cout << "alpha " << alpha << "; dcore: " << dcore << "; gamma: " << gamma << "; dmax: " << dmax << "; d: " << d << std :: endl;
        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i].key != -1) {
                std::cout << "Slot " << i << ": Key = " << table[i].key
                          << ", Choice = " << table[i].choice << std::endl;
            }
        }
    }
};