#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include "auth_utils.h"
using namespace std;

bool checkRegister(const string &client_name) 
{
    ifstream file("./data/users_table.txt");
    if (!file.is_open()) 
    {
        cerr << "Failed to open users_table.txt" << "\n";
        return false;
    }
    string line;
    while (getline(file, line)) 
    {
        istringstream iss(line);
        string username, password;
        iss >> username >> password;
        if (username == client_name) 
        {
            file.close();
            return false;
        }
    }
    file.close();
    return true;
}
int verifyPassword(const string &client_name, const string &client_pwd) 
{
    ifstream file("./data/users_table.txt");
    if (!file.is_open()) 
    {
        cerr << "Failed to open users_table.txt" << "\n";
        return -1;
    }
    string line;
    while (getline(file, line)) 
    {
        istringstream iss(line);
        string username, password;
        iss >> username >> password;
        if (username == client_name) 
        {
            if (password == client_pwd) 
            {
                file.close();
                return 1;
            } 
            else 
            {
                file.close();
                return 0;
            }
        }
    }
    file.close();
    return 2;
}
void WriteToTable(const string &client_name, const string &client_pwd, const string &f_path) 
{
    ofstream file(f_path, ios::app);  // 使用 ios::app 以追加模式打開文件
    if (!file.is_open()) 
    {
        cerr << "Failed to open file: " << f_path << "\n";
        return;
    }
    file << client_name << " " << client_pwd << "\n";
    cout << "Successfully added " << client_name << " to " << f_path << "\n";
    file.close();
}
void DelToTable(const string &client_name, const string &client_pwd, const string &f_path) 
{
    ifstream file(f_path);
    if (!file.is_open()) 
    {
        cerr << "Failed to open file: " << f_path << "\n";
        return;
    }
    vector<string> lines;
    string line;
    bool found = false;
    while (getline(file, line)) 
    {
        istringstream iss(line);
        string username, password;
        iss >> username >> password;
        if (username == client_name && password == client_pwd) 
        {
            found = true;
            continue;
        }
        lines.push_back(line);
    }
    file.close();
    if (!found) 
    {
        cout << "No matching record found for " << client_name << " in " << f_path << "\n";
        return;
    }
    ofstream outfile(f_path, ios::trunc);
    if (!outfile.is_open()) 
    {
        cerr << "Failed to open file for writing: " << f_path << "\n";
        return;
    }
    for (const string &output_line : lines) 
    {
        outfile << output_line << "\n";
    }
    cout << "Successfully removed " << client_name << " from " << f_path << "\n";
    outfile.close();
}
void PrintAllRegister() 
{
    ifstream file("./data/users_table.txt");
    if (!file.is_open()) 
    {
        cerr << "Failed to open users_table.txt" << "\n";
        return;
    }
    cout << "Registered Users:" << "\n";
    string line;
    while (getline(file, line)) 
    {
        cout << line << "\n";
    }

    file.close();
}
void PrintAllLogin() 
{
    ifstream file("./data/online_table.txt");
    if (!file.is_open()) 
    {
        cerr << "Failed to open online_table.txt" << "\n";
        return;
    }
    cout << "Logged-in Users:" << "\n";
    string line;
    while (getline(file, line)) 
    {
        cout << line << "\n";
    }
    file.close();
}