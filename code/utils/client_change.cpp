#include "client_change.h"
#include <iostream>
using namespace std;

#define REGISTER_TABLE "./data/users_table.txt"
#define LOGIN_TABLE "./data/login_table.txt"
#define ONLINE_TABLE "./data/online_table.txt"
int checkMessage(const string& message, int status)
{
    if (message.empty())
    {
        cerr << "Error: Received an empty message.\n";
        return 0;
    }
    return status;
}
int clientMainMenu(int server_socket, int status)
{
    string op;
    string message_to_server, server_to_client;
    int result = status;
    cout << "Enter operation (1: Register, 2: Login, 3: Exit): ";
    cin >> op;
    if (op == "1") 
    {
        message_to_server = op + " I want to register!!";
        result = 2; // 進入註冊流程
    } 
    else if (op == "2") 
    {
        message_to_server = op + " I want to login!!";
        result = 3; // 進入登入流程
    } 
    else if (op == "3") 
    {
        message_to_server = op + " I want to exit!!";
        //sendMessage(server_socket, message_to_server);
        //return 0;
        result = 0; // 結束
    } 
    else 
    {
        cout << "Unknown operation." << "\n";
        result = 1; // 返回主選單
    }
    sendMessage(server_socket, message_to_server);
    server_to_client = receiveMessage(server_socket);
    result = checkMessage(server_to_client, result);
    if (!result)
    {
        cout << "Server break!\n";
        return result;
    }
    Print_message(server_to_client, 1);
    return result;
    
}

// 認證邏輯（註冊/登入）
int clientAuthProcess(int server_socket, int status)
{
    string my_name, my_pwd, server_to_client;
    cout << "Input your name: ";
    cin >> my_name;
    cout << "Input your password: ";
    cin >> my_pwd;
    sendMessage(server_socket, my_name);
    sendMessage(server_socket, my_pwd);
    server_to_client = receiveMessage(server_socket);
    if (!checkMessage(server_to_client, status))
    {
        cout << "Server break!\n";
        return 0;
    }
    Print_message(server_to_client, 1);
    if (server_to_client == "Register success!!")
    {
        return 1; // 返回主選單
    }
    else if (server_to_client == "login success!!")
    {
        return 4; // 登入成功，進入服務狀態
    }
    else
    {
        return 1; // 認證失敗，返回主選單
    }
}

// 服務選單邏輯
int clientServiceMenu(int server_socket, int status)
{
    string op, message_to_server, server_to_client;
    int result = status;
    cout << "Enter operation (1: Chat with someone, 2: Logout, 3: Exit): ";
    cin >> op;
    if (op == "1") 
    {
        message_to_server = op + " I want to chat with someone!!";
        result = 4; // 正式進入使用app模式
    } 
    else if (op == "2") 
    {
        message_to_server = op + " I want to logout!!";
        result = 1; // 返回主選單
    } 
    else if (op == "3") 
    {
        message_to_server = op + " I want to exit!!";
        result = 0; // 結束
    } 
    else 
    {
        cout << "Unknown operation." << "\n";
        result = 4; // 停留在服務狀態
    }
    sendMessage(server_socket, message_to_server);
    server_to_client = receiveMessage(server_socket);
    result = checkMessage(server_to_client, result);
    if (!result)
    {
        cout << "Server break!\n";
        return result;
    }
    Print_message(server_to_client, 1);
    return result;
}
/*
int chatSome(int server_socket) 
{
    cout << "Entering Chat Mode..." << "\n";
    // 接收服务器的聊天对象提示
    string prompt = receiveMessage(server_socket);
    string target_user;
    Print_message(prompt, 1);
    cin >> target_user;
    sendMessage(server_socket, target_user);
    string feedback = receiveMessage(server_socket);
    if (feedback == "Target user is not online.") 
    {
        cout << feedback << "\n";
        return 5;
    }
    else
        return 6;
    string options = receiveMessage(server_socket);
    cout << options << "\n";
    string choice;
    cout << "Enter your choice: ";
    cin >> choice;
    sendMessage(server_socket, choice);

    if (choice == "1") 
    {
        cout << "Enter your message: ";
        string message;
        cin.ignore();
        getline(cin, message);
        sendMessage(server_socket, message);

        string confirmation = receiveMessage(server_socket);
        cout << confirmation << "\n";
    } 
    else 
    {
        string not_implemented = receiveMessage(server_socket);
        cout << not_implemented << "\n";
    }
}
int chatAndSendData()
*/