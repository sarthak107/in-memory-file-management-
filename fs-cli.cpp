#include <bits/stdc++.h>
using namespace std;
#include "FileSystem.h"




bool fileExists(const string & filename) {
    ifstream file(filename);
    return file.good();
}

int main() {

    FileSystem fs;
    string str;
    bool stateChanged = false;
    cin >> str;
    cin.ignore();

    if (str == "fs-cli") {
        if (fileExists("filesystem_state.txt")) {
            cout << "Do you want to load the saved state? (yes/no): ";
            string loadResponse;
            cin >> loadResponse;
            cin.ignore();
            if (loadResponse == "yes") {
                fs.loadState("filesystem_state.txt");
            }
        }
        string command;
        while (true) {
            cout << fs.getCurrentPath() << ": ";
            getline(cin, command);
            if (command == "exit") {
                if (stateChanged) {
                    cout << "Do you want to save the state before exiting? (yes/no): ";
                    string saveResponse;
                    cin >> saveResponse;
                    if (saveResponse == "yes") {
                        fs.saveState("filesystem_state.txt");
                    }
                }
                cout << "fs-cli exit." << endl;
                break;
            }
            if (command.substr(0, 4) == "echo") {
                fs.echo(command);
                stateChanged = true;
            } else if (command.substr(0, 3) == "cat") {
                string fileName = command.substr(4);
                fs.cat(fileName);
            } else if (command.substr(0, 2) == "rm") {
                string fileName = command.substr(3);
                fs.rm(fileName);
                stateChanged = true;
            } else if (command.substr(0, 5) == "mkdir") {
                string path = command.substr(6);
                fs.mkdir(path);
                stateChanged = true;
            } else if (command.substr(0, 2) == "ls") {
                string path = command.size() > 3 ? command.substr(3) : "";
                fs.ls(path); // Call ls with the specified path
            } else if (command.substr(0, 2) == "cd") {
                string path = command.size() > 3 ? command.substr(3) : "/";
                fs.changeDir(path); // Call cd with the specified path
            } else if (command.substr(0, 5) == "touch") {
                string fileName = command.substr(6);
                fs.touch(fileName);
                stateChanged = true;

            } else if (command.substr(0, 2) == "mv") {
                istringstream iss(command.substr(3));
                string srcPath, destPath;
                iss >> srcPath >> destPath; // Extract source and destination paths
                fs.mv(srcPath, destPath);
                stateChanged = true;

            } else if (command.substr(0, 2) == "cp") {
                istringstream iss(command.substr(3));
                string srcPath, destPath;
                iss >> srcPath >> destPath; // Extract source and destination paths
                fs.cp(srcPath, destPath);
                stateChanged = true;

            } else if (command.substr(0, 4) == "grep") {
                istringstream iss(command.substr(5));
                string fileName, pattern;
                iss >> fileName >> ws; // Extract filename
                getline(iss, pattern); // Extract the remaining part as the pattern

                fs.grep(fileName, pattern);

            } else {
                cout << "Unknown command" << endl;
            }

        }
    }
    return 0;
}