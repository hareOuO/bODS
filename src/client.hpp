#include <map>
#include <string>
#include <set>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <stdio.h>
#include <random>
#include <cassert>
#include <iomanip>
#include <vector>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "precompute.hpp"
#include "Types.hpp"
#include "node.hpp"
// #include "glog/logging.h"

using namespace std;

#define debug 0
#define debug_randompos 1

class client
{
private:
    /* data */
public:
    client(int maxSize, vector<bytes<Key>> k1, bytes<Key> k2, bytes<Key> k3);
    ~client();
    bool init();
    void insert_map(string key, int value);
    int search_map(string key);
    void sendend();

    bool insert(string key, int value);
    kvpair* search(string key);
    void part_init();
    void finish(bool find);

    midNode1* Readmid1Node(string key, int leaf, int mid1L);
    midNode2* Readmid2Node(string key, int leaf);
    leafNode* ReadleafNode(string key, int leaf);
    // void WriteRootNode();
    int Writemid1Node(bool incache, string bid, midNode1* node, int mid1L);
    int Writemid2Node(bool incache, string bid, midNode2* node);
    int WriteleafNode(bool incache, string bid, leafNode* node);
    void Fetchmid1Path(int leaf, int mid1L);
    void Fetchmid2Path(int leaf);
    void FetchleafPath(int leaf);
    void Writemid1Path(int leaf, int d, int mid1L);
    void Writemid2Path(int leaf, int d);
    void WriteleafPath(int leaf, int d);
    void finalizemid1(bool find, int mid1L);
    void finalizemid2(bool find);
    void finalizeleaf(bool find);
    void finalize2mid1(int mid1L);
    void finalize2mid2();
    void finalize2leaf();

    void m1Update(int m1L, midNode1* newNode, vector<midNode1*> m1);
    std::vector<string> GetIntersectingBlocks1(int x, int curDepth, int mid1L);
    std::vector<string> GetIntersectingBlocks2(int x, int curDepth);
    std::vector<string> GetIntersectingBlocks3(int x, int curDepth);
    int RandomPath(int mt);
    int GetNodeOnPath(int maxSize, int leaf, int curDepth);
    block SerialiseBucket(Bucket bucket);

    template <class Node>
    Bucket DeserialiseBucket(block buffer);
    template <class Node>
    Node* convertBlockToNode(block b);
    template <class Node>
    block convertNodeToBlock(Node* node);


    rootNode *root;
    const int L=4;
    //std::map<string, midNode1*> mid1cache;
    std::vector<std::map<string, midNode1*>>  mid1cache;
    std::map<string, midNode2*> mid2cache;
    std::map<string, leafNode*> leafcache;
    //std::set<string> mid1modified;
    std::vector<set<string>> mid1modified;
    std::set<string> mid2modified;
    std::set<string> leafmodified;
    //std::vector<int> leafList1;
    std::vector<vector<int>> leafList1;
    std::vector<int> leafList2;
    std::vector<int> leafList3;
    //std::vector<int> writeviewmap1;
    std::vector<vector<int>> writeviewmap1;
    std::vector<int> writeviewmap2;
    std::vector<int> writeviewmap3;
    // bytes<Key> key1;
    vector<bytes<Key>> key1;
    bytes<Key> key2;
    bytes<Key> key3;
    // int readCntmid1;
    vector<int> readCntmid1;
    int readCntmid2;
    int readCntleaf;
    

    // Randomness
    std::random_device rd;
    std::mt19937 mt;
    std::uniform_int_distribution<int> dis;

    int clientSock;
    struct sockaddr_in serverAddr;
};

