#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Directory.h"
#include <bits/stdc++.h>
using namespace std;

//Filesystem class to hold dict structure
class FileSystem{
    unique_ptr < Directory > root;
    Directory * current;
    vector < string > currentPath;
    public:
        FileSystem() {
            root = make_unique < Directory > ("root");
            current = root.get();
            currentPath.push_back("fs-cli");
        }
    Directory * navigateTo(const vector < string > & path, Directory * startDir) {
        Directory * node = startDir;

        for (auto it = path.begin(); it != path.end(); ++it) {
            const string & dir = * it;

            if (dir.empty() || dir == ".") continue; // Skip empty and current directory symbol

            if (dir == "..") {
                if (node -> parent) node = node -> parent;
                continue;
            }

            // Check if it's the last element and a file
            if (next(it) == path.end() && node -> files.find(dir) != node -> files.end()) {
                return node; // Return parent directory of the file
            }

            // Navigate to subdirectory
            if (node -> subdirs.find(dir) != node -> subdirs.end()) {
                node = node -> subdirs[dir].get();
            } else {
                return nullptr; // Directory not found
            }
        }

        return node;
    }

    string getCurrentPath() {
        string fullPath;
        for (const auto & dir: currentPath) {
            fullPath += dir + "/";
        }
        return fullPath;
    }

    //mkdir functionality
    void mkdir(const string & path) {
        vector < string > dirs = splitPath(path);
        Directory * currentDir = current;

        for (const string & dir: dirs) {
            if (currentDir -> subdirs.find(dir) == currentDir -> subdirs.end()) {
                currentDir -> subdirs[dir] = make_unique < Directory > (dir, currentDir);
            }
            currentDir = currentDir -> subdirs[dir].get();
        }
    }





    // ls functionality
    void ls(const string & path) {
        vector < string > dirs = splitPath(path);
        Directory * targetDir = (dirs.empty()) ? current : navigateTo(dirs, current);

        if (!targetDir) {
            cout << "Directory not found: " << path << endl;
            return;
        }

        cout << "Contents of directory " << targetDir -> name << ":" << endl;
        for (const auto & subdir: targetDir -> subdirs) {
            cout << subdir.first << "/" << endl;
        }
        for (const auto & file: targetDir -> files) {
            cout << file.first << endl;
        }
    }





    //cd functionality
    void changeDir(const string & path) {
        if (path == "/" || path == "~") {
            current = root.get();
            currentPath = {
                "fs-cli"
            };
            return;
        }

        vector < string > dirs = splitPath(path);
        Directory * newCurrent = navigateTo(dirs, current);
        if (!newCurrent) {
            cout << "Directory not found: " << path << endl;
            return;
        }

        current = newCurrent;
        currentPath = {
            "fs-cli"
        }; // Rebuild currentPath based on newCurrent
        Directory * temp = newCurrent;
        stack < string > reversePath;
        while (temp != root.get()) {
            reversePath.push(temp -> name);
            temp = temp -> parent;
        }
        while (!reversePath.empty()) {
            currentPath.push_back(reversePath.top());
            reversePath.pop();
        }
    }




    // echo functionality
    void echo(const string & command) {
        size_t redSymbol = command.find(">");

        if (redSymbol != string::npos) {
            string text = command.substr(5, redSymbol - 6);
            string fileName = command.substr(redSymbol + 2);
            current -> files[fileName] = text; // Write to the file in the current directory
        } else {
            cout << command.substr(5, command.length() - 5) << endl;
        }
    }




    // cat functionality
    void cat(const string & fileName) {
        if (current -> files.find(fileName) != current -> files.end()) {
            cout << current -> files[fileName] << endl;
        } else {
            cout << "File not found: " << fileName << endl;
        }
    }






    // rm functionality
    void rm(const string & name) {
        // Check if it's a file and remove it
        if (current -> files.find(name) != current -> files.end()) {
            current -> files.erase(name);
            cout << "File '" << name << "' deleted successfully." << endl;
            return;
        }



        // Check if it's a directory and remove it
        if (current -> subdirs.find(name) != current -> subdirs.end()) {
            current -> subdirs.erase(name);
            cout << "Directory '" << name << "' deleted successfully." << endl;
            return;
        }

        cout << "File or directory not found: " << name << endl;
    }






    //grep functionality
    void grep(const string & fileName,
        const string & pattern) {
        if (current -> files.find(fileName) != current -> files.end()) {
            istringstream iss(current -> files[fileName]);
            string line;
            while (getline(iss, line)) {
                if (line.find(pattern) != string::npos) {
                    cout << line << endl;
                }
            }
        } else {
            cout << "File not found: " << fileName << endl;
        }

    }




    //touch functionality
    void touch(const string & fileName) {
        current -> files[fileName] = ""; // Create an empty file
        cout << "File created: " << fileName << endl;
    }





    //mv functionality
    void mv(const string & srcPath,
        const string & destPath) {
        vector < string > srcDirs = splitPath(srcPath);
        vector < string > destDirs = splitPath(destPath);

        Directory * srcParent = navigateTo(srcDirs, current);
        if (!srcParent || srcParent -> files.find(srcDirs.back()) == srcParent -> files.end()) {
            cout << "Source file not found: " << srcDirs.back() << endl;
            return;
        }

        string srcName = srcDirs.back();
        string destName = destDirs.back();

        // If the destination path is a directory, use the same file name
        Directory * destParent = navigateTo(destDirs, current);
        if (destParent && destParent -> subdirs.find(destName) != destParent -> subdirs.end()) {
            destParent = destParent -> subdirs[destName].get();
            destName = srcName;
        }

        // If destination directory is not found or it is a file, then return error
        if (!destParent || destParent -> files.find(destName) != destParent -> files.end()) {
            cout << "Invalid destination directory." << endl;
            return;
        }

        // Move the file
        destParent -> files[srcName] = srcParent -> files[srcName];
        srcParent -> files.erase(srcName);
    }

    //cp functionality
    void cp(const string & srcPath,
        const string & destPath) {
        vector < string > srcDirs = splitPath(srcPath);
        vector < string > destDirs = splitPath(destPath);

        // Resolve the full path for the source and destination
        Directory * srcParent = (srcDirs.size() == 1) ? current : navigateTo(srcDirs, current);
        Directory * destParent = (destDirs.size() == 1) ? current : navigateTo(destDirs, current);

        if (!srcParent || !destParent) {
            cout << "Invalid source or destination directory." << endl;
            return;
        }

        string srcName = srcDirs.back();
        string destName = destDirs.back();

        // Check if the source is a file
        if (srcParent -> files.find(srcName) != srcParent -> files.end()) {
            // Check if the file already exists in the destination
            if (destParent -> files.find(destName) != destParent -> files.end()) {
                cout << "File already exists at the destination." << endl;
                return;
            }
            // Copy the file
            destParent -> files[destName] = srcParent -> files[srcName];
        } else {
            cout << "Source file not found: " << srcName << endl;
        }
    }

    // Method to write to a file in the current directory
    void writeFile(const string & fileName,
        const string & content) {
        current -> files[fileName] = content; // Save the file content
    }
    void saveState(const string & filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Unable to open file for saving state." << endl;
            return;
        }
        saveDirectoryState(file, root.get(), "");
        file.close();
    }

    void saveDirectoryState(ofstream & file, Directory * dir,
        const string & path) {
        for (const auto & subdir: dir -> subdirs) {
            saveDirectoryState(file, subdir.second.get(), path + subdir.first + "/");
        }
        for (const auto & fileEntry: dir -> files) {
            file << path << fileEntry.first << ":" << fileEntry.second << endl;
        }
    }

    void loadState(const string & filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Unable to open file for loading state." << endl;
            return;
        }
        string line;
        while (getline(file, line)) {
            auto pos = line.find_last_of(':');
            string filePath = line.substr(0, pos);
            string fileContent = line.substr(pos + 1);
            createFileWithPath(filePath, fileContent);
        }
        file.close();
        currentPath = {
            "fs-cli"
        };
    }

    void createFileWithPath(const string & filePath,
        const string & content) {
        auto pos = filePath.find_last_of('/');
        string dirPath = filePath.substr(0, pos);
        string fileName = filePath.substr(pos + 1);
        // Create directories if they don't exist
        mkdir(dirPath);
        // Set the current directory to the file's directory
        Directory * prevCurrent = current;
        changeDir(dirPath);
        // Create the file
        current -> files[fileName] = content;
        // Restore the current directory
        current = prevCurrent;
    }

    private:
        vector < string > splitPath(const string & path) {
            vector < string > result;
            if (path.front() == '/' || path.front() == '~') {
                result.push_back("fs-cli"); // Root directory
            }
            stringstream ss(path);
            string dir;
            while (getline(ss, dir, '/')) {
                if (!dir.empty()) {
                    result.push_back(dir);
                }
            }
            return result;
        }

};


#endif