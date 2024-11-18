#include "status_change.h"
#include <iostream>
using namespace std;

#define REGISTER_TABLE "./data/users_table.txt"
#define LOGIN_TABLE "./data/online_table.txt"

// 檢查訊息是否有效
int checkMessage(const string& message, int status)
{
    if (message.empty())
    {
        cerr << "Error: Received an empty message.\n";
        return 0;
    }
    return status;
}

// 主選單處理邏輯，返回下一個狀態
int handleClientMenu(const string& message, int client_socket)
{
    char op = message[0];
    string response;

    if (op == '1')
    {
        response = "OK We will help you register!!";
        sendMessage(client_socket, response);
        return 2; // 進入註冊流程
    }
    else if (op == '2')
    {
        response = "OK We will help you login!!";
        sendMessage(client_socket, response);
        return 3; // 進入登入流程
    }
    else if (op == '3')
    {
        response = "OK bye!!";
        sendMessage(client_socket, response);
        return 0; // 結束連線
    }
    else
    {
        response = "Unknown command";
        sendMessage(client_socket, response);
        return 1; // 回到主選單
    }
}

// 處理用戶註冊邏輯
int handleClientRegister(const string& client_name, const string& client_pwd, int client_socket)
{
    string response;

    if (checkRegister(client_name))
    {
        WriteToTable(client_name, client_pwd, REGISTER_TABLE);
        response = "Register success!!";
        cout << "Client (" << client_name << ") registered successfully.\n";
        sendMessage(client_socket, response);
        return 1; // 回到主選單
    }
    else
    {
        response = "Fail to register:( " + client_name + " has been used!!";
        cerr << "Error: Registration failed for (" << client_name << ") - Name already used.\n";
        sendMessage(client_socket, response);
        return 1; // 回到主選單
    }
}

// 處理用戶登入邏輯
int handleClientLogin(const string& client_name, const string& client_pwd, int client_socket)
{
    string response;
    int verify_result = verifyPassword(client_name, client_pwd);

    if (verify_result == 1)
    {
        WriteToTable(client_name, client_pwd, LOGIN_TABLE);
        response = "login success!!";
        cout << "Client (" << client_name << ") logged in successfully.\n";
        sendMessage(client_socket, response);
        return 4; // 進入服務狀態
    }
    else if (verify_result == 2)
    {
        response = "Fail to login:( We cannot find " + client_name;
        cerr << "Error: Login failed for (" << client_name << ") - User not found.\n";
        sendMessage(client_socket, response);
        return 1; // 回到主選單
    }
    else
    {
        response = "Password not match!";
        cerr << "Error: Login failed for (" << client_name << ") - Password mismatch.\n";
        sendMessage(client_socket, response);
        return 1; // 回到主選單
    }
}

// 處理服務狀態邏輯
int handleClientServe(const string& message, const string& client_name, const string& client_pwd, int client_socket)
{
    char op = message[0];
    string response;

    if (op == '1')
    {
        response = "OK We will help you to chat with your friends!! (not implemented)";
        sendMessage(client_socket, response);
        return 4; // 繼續停留在服務狀態
    }
    else if (op == '2')
    {
        response = "OK we will help you logout!!";
        DelToTable(client_name, client_pwd, LOGIN_TABLE); // 從在線名單中刪除
        sendMessage(client_socket, response);
        return 1; // 回到主選單
    }
    else if (op == '3')
    {
        response = "OK bye!!";
        DelToTable(client_name, client_pwd, LOGIN_TABLE); // 從在線名單中刪除
        sendMessage(client_socket, response);
        return 0; // 結束連線
    }
    else
    {
        response = "Unknown command";
        sendMessage(client_socket, response);
        return 4; // 繼續停留在服務狀態
    }
}
////////////////////////////////////////////////////此線以下是client端狀態轉移函數
// 主選單邏輯
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
        result = 4; // 停留在服務狀態
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
