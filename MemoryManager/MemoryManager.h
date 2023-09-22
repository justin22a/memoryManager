#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <fstream>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <cstring>
#include <algorithm>
#include <functional>
using namespace std;
int bestFit(int sizeInWords, void *list);
int worstFit(int sizeInWords, void *list);


//struct node{
//    int size;
//    bool isHole;
//    int head;
//    node* prev;
//    node* next;
//
//    node(int _size , bool _isHole , int _head , node* _prev , node* _next)
//    {
//        this->size = _size;
//        this->isHole = _isHole;
//        this->head = _head;
//        this->prev = _prev;
//        this->next = _next;
//    }
//
//
//
//};
int binaryToDecimal(int n);
struct node{
    int size;
    int startIndex;
    bool isHole;
    node(int _start , int _size , bool _hole)
    {
        startIndex = _start;
        size = _size;
        isHole = _hole;
    }
};

class MemoryManager {
    std::function<int(int, void *)> allocationType;
    unsigned wordSize;
    char* totalSpace;
    vector<node> nodeList;
    int amountOfWords;
    int memLimit;
public:
    void *getBitmap();
    unsigned getMemoryLimit();
    unsigned getWordSize();
    void *getMemoryStart();
    ~MemoryManager();
    void free(void *address);
    MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator);
    //~MemoryManager();
    void initialize(size_t sizeInWords);
    void *allocate(size_t sizeInBytes);
    void *getList();
    void setAllocator(std::function<int(int, void *)> allocator);
    void shutdown();
    int dumpMemoryMap(char *filename);
};

