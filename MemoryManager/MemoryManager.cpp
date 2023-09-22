#include "MemoryManager.h"


MemoryManager::MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator)
{
    //construct some crap
    this->wordSize = wordSize;
    this->allocationType = allocator;
}

void MemoryManager::initialize(size_t sizeInWords) {
    if(sizeInWords <= 65536) {
        memLimit = sizeInWords * wordSize;
        //to be allocated
        totalSpace = new char[memLimit];
        //enter a node as a hole
        node firstHole = node(0, sizeInWords, true);
        nodeList.push_back(firstHole);
        amountOfWords = sizeInWords;
    }
}

void *MemoryManager::allocate(size_t sizeInBytes) {

    int words = ceil((float)sizeInBytes/wordSize);
    uint16_t* holes = static_cast<uint16_t*>(getList());
    int offset = allocationType(words , holes);
    int indexToInsert = -1;
    delete [] holes;
    if(offset == -1)
    {
        return nullptr;
    }

    for(unsigned int i = 0; i < nodeList.size(); i++)
    {
        if(offset == nodeList.at(i).startIndex)
        {
            indexToInsert = i;
        }
    }

    if(nodeList.at(indexToInsert).size == words)
    {
        nodeList.at(indexToInsert).isHole = false;
    }
    else
    {
        node mustInsert = node(offset,words,false);
        nodeList.insert(nodeList.begin() + indexToInsert , mustInsert);
        nodeList.at(indexToInsert+1).size -= words;
        nodeList.at(indexToInsert+1).startIndex = nodeList.at(indexToInsert).startIndex + words;
    }

    return (void*)(totalSpace + offset * wordSize);
}



int bestFit(int sizeInWords, void *list)
{

    auto* holes = static_cast<uint16_t*>(list);
    uint16_t length = *holes++;
    int offset = -1;
    int _size = -1;
    for(uint16_t i = 1; i < (length) * 2; i+= 2)
    {
        if(offset == -1 && holes[i] >= sizeInWords)
        {
            offset = holes[i-1];
            _size = holes[i];
        }
        else if(holes[i] >= sizeInWords && _size > holes[i])
        {
            offset = holes[i-1];
            _size = holes[i];
        }
    }


    return offset;
}

int worstFit(int sizeInWords , void *list)
{
    auto* holes = static_cast<uint16_t*>(list);
    uint16_t length = *holes++;
    int offset = -1;
    int _size = -1;
    for(uint16_t i = 1; i < (length) * 2; i+= 2)
    {
        if(offset == -1 && holes[i] >= sizeInWords)
        {
            offset = holes[i-1];
            _size = holes[i];
        }
        else if(holes[i] >= sizeInWords && _size < holes[i])
        {
            offset = holes[i-1];
            _size = holes[i];
        }
    }


    return offset;
}

void *MemoryManager::getList() {
    vector<node> currHoles;
for(unsigned int i = 0; i < nodeList.size(); i++)
{
    if(nodeList.at(i).isHole)
    {
        currHoles.push_back(nodeList.at(i));
    }
}


int howManyHoles = currHoles.size();
uint16_t* list = new uint16_t[1+sizeof(::uint16_t)*currHoles.size()];

list[0] = howManyHoles;



int index = 1;
for(unsigned int i = 0; i < nodeList.size(); i++)
{
    if(nodeList.at(i).isHole)
    {
        list[index] = nodeList.at(i).startIndex;
        index++;
        list[index] = nodeList.at(i).size;
        index++;
    }
}

return list;

//return nullptr;
}

void MemoryManager::setAllocator(std::function<int(int, void *)> allocator) {
    allocationType = allocator;
}

void MemoryManager::free(void *address) {
    int offset = ((char*)address - totalSpace)/wordSize;
    int index = -1;
    for(unsigned int i = 0; i < nodeList.size(); i++)
    {
        if(nodeList.at(i).startIndex == offset)
        {
            index = i;
            break;
        }
    }

    //if before node is a hole then combine
    if(index+1 < nodeList.size() && nodeList.at(index+1).isHole)
    {
        nodeList.at(index).size += nodeList.at(index+1).size;
        nodeList.erase(nodeList.begin()+index+1);
    }
    //if after node is a hole then combine
    if(index-1 >= 0 && nodeList.at(index-1).isHole) {

        nodeList.at(index).size += nodeList.at(index-1).size;
        nodeList.at(index).startIndex = nodeList.at(index-1).startIndex;
        //remove the old node
        nodeList.erase(nodeList.begin() + index -1);
        index = index -1;
    }
    nodeList.at(index).isHole = true;

}

void *MemoryManager::getBitmap() {
    string container = "";
    string curr = "";
    int count = 0;
    int endOfStringCount = 0;
    vector<int>numbers;
    numbers.push_back(0);
    numbers.push_back(0);
    for(unsigned int i = 0; i < nodeList.size(); i++)
    {
        int currSize = nodeList.at(i).size;
        int truth = nodeList.at(i).isHole;
        for(unsigned int k = 0; k < currSize; k++)
        {
            endOfStringCount++;
            count++;
            if(truth){curr+="0";}
            else{curr+="1";}
            if(count == 8 || endOfStringCount == amountOfWords)
            {
                reverse(curr.begin() , curr.end());
                container+= curr;
                count = 0;
                numbers.push_back(binaryToDecimal(stoi(curr)));
                curr = "";
            }
        }
    }

    int mapSize = ceil((float)container.size()/8);
    uint8_t * map = new uint8_t[2+mapSize];

    numbers.at(0) = mapSize%256;
    numbers.at(1) = mapSize >> 8;

    for(uint8_t i = 0; i < mapSize+2; i++)
    {
        int current = numbers.at(i);
        map[i] = numbers.at(i);

    }

    return map;

//return nullptr;
}

int binaryToDecimal(int n)
{
    int number = n;
    int start = 1;
    int dec = 0;
    int t = number;
    while (t) {
        int last = t%10;
        t = t/10;
        dec += last*start;
        start = start*2;
    }

    return dec;
}

unsigned MemoryManager::getMemoryLimit() {
    return memLimit;
}

unsigned MemoryManager::getWordSize() {
    return wordSize;
}

void *MemoryManager::getMemoryStart() {
    return totalSpace;
}

MemoryManager::~MemoryManager() {
delete[] totalSpace;
    totalSpace = nullptr;
    shutdown();
}

void MemoryManager::shutdown() {
    nodeList.clear();
    delete[] totalSpace;
    totalSpace = nullptr;
}

int MemoryManager::dumpMemoryMap(char *filename) {
    string final = "";
    int count = 0;
    for(unsigned int i = 0; i < nodeList.size(); i++)
    {
        if(count == 0 && nodeList.at(i).isHole)
        {
            final += "[" + to_string(nodeList.at(i).startIndex) + ", " + to_string(nodeList.at(i).size) + "]";
            count++;
        } else if (nodeList.at(i).isHole){
            final += " - [" + to_string(nodeList.at(i).startIndex) + ", " + to_string(nodeList.at(i).size) + "]";
        }
    }

    int fileResult = open(filename , O_CREAT|O_WRONLY , 0666);
    if(fileResult < 0)
    {
        return -1;
    }


//    void* shit = holes;
//    delete static_cast<::uint16_t*>(shit);

   // cout << "here is the spot to look " << final << endl;

    const char * toPut = final.c_str();
    //write the file, and make sure it's okay
    if(write(fileResult, toPut , std::strlen(toPut)) != std::strlen(toPut))
    {
        return -1;
    }

    int checkClose = close(fileResult);
    if(checkClose == -1)
    {
        return -1;
    }


    return 0;
}
