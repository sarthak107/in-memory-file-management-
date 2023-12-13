#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <bits/stdc++.h>
using namespace std;
#include <memory>


//Directory Node Struct
struct Directory {
    string name;
    Directory * parent;
    unordered_map < string, unique_ptr < Directory >> subdirs;
    map < string, string > files;
    Directory(string dirName, Directory * parentDir = nullptr): name(dirName), parent(parentDir) {}
};

#endif // DIRECTORY_H