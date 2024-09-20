#ifndef HASH_TABLE_BASE_H
#define HASH_TABLE_BASE_H

// Abstract base class for hash tables
class HashTableBase {
protected:
    int N;  // Number of slots in the hash table

public:
    // Constructor to initialize the number of slots
    HashTableBase(int numSlots) : N(numSlots) {}

    // Virtual destructor to allow proper cleanup of derived classes
    virtual ~HashTableBase() {}

    // Pure virtual function to insert a key into the hash table
    virtual bool insert(int key) = 0;

    // Pure virtual function to search for a key in the hash table
    virtual bool search(int key) = 0;

    // Pure virtual function to remove a key from the hash table
    virtual bool remove(int key) = 0;

    // Optional: A function to display the contents of the hash table (can be overridden)
    virtual void display() const = 0;
};

#endif  // HASH_TABLE_BASE_H
