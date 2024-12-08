#include "server_change.h"
#include <iostream>
using namespace std;

#define REGISTER_TABLE "./data/users_table.txt"
#define LOGIN_TABLE "./data/login_table.txt"
#define ONLINE_TABLE "./data/online_table.txt"
// 檢查訊息是否有效
void updateClientStatus(int client_socket, int status) 
{
    //cout << "here\n";
    pthread_mutex_lock(&statusMutex);
    clientStatus[client_socket] = status; // 更新狀態
    //cout << "Updated clientStatus: [";
    for (const auto& [key, value] : clientStatus) 
    {
        cout << "socket_fd: " << key << " -> status: " << value << "\n";
    }
    //cout << "]" << "\n";
    pthread_mutex_unlock(&statusMutex);
}
// 移除客戶端的工具函數
void removeClient(int client_socket) 
{
    pthread_mutex_lock(&statusMutex);
    clientStatus.erase(client_socket); // 從狀態表中移除
    for (const auto& [key, value] : clientStatus) 
    {
        cout << "socket_fd: " << key << " -> status: " << value << "\n";
    }
    pthread_mutex_unlock(&statusMutex);
}
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
int getUserSocket(const string& username) 
{
    pthread_mutex_lock(&statusMutex);
    int userSocket = usernameToSocket.find(username) == usernameToSocket.end() ? -1 : usernameToSocket[username];
    pthread_mutex_unlock(&statusMutex);
    return userSocket;
}
bool isUserOnline(const string& user_name)
{
    return getUserSocket(user_name) != -1 ? false : true;
}
/*
int handleTextMessage(const string& sender, const string& receiver, int sender_socket) 
{
    //sendMessage(sender_socket, "Enter your message:");
    string message = receiveMessage(sender_socket);
    int receiver_socket = getUserSocket(receiver);
    if (receiver_socket == -1) 
    {
        sendMessage(sender_socket, "Message could not be delivered. Target user went offline.");
        return -1; 
    }
    string full_message = sender + ": " + message;
    sendMessage(receiver_socket, full_message);
    sendMessage(sender_socket, "Message sent successfully!");
    return 5;
}
int handleChatServe(const string& client_name, int client_socket, string* target_name) 
{
    sendMessage(client_socket, "Who do you want to chat?");
    *target_name = receiveMessage(client_socket);
    if (isUserOnline(*target_name) == -1)
    {
        sendMessage(client_socket, "Target user is not online.");
        *target_name == NULL;
        return 5;
    }
    return 6;
}
int handleSendData(const string& client_name, int client_socket, const string& target_name)
{
    sendMessage(client_socket, 
                "Choose what to send:\n"
                "0. quit\n"
                "1. Text Message\n"
                "2. Image\n"
                "3. Video\n"
                "4. Audio\n"
                "5. Live Call\n"
                "6. Live Video");
    string choice = receiveMessage(client_socket);
    if (choice == "0") 
    {
        return 4;
    } 
    else if (choice == "1") 
    {
        handleTextMessage(client_name, target_name, client_socket);
        return 5;
    } 
    else 
    {
        sendMessage(client_socket, "This feature is not implemented yet.");
        return 5;
    }
    return 5;
}
*/
////////////////////////////////////////////////////此線以下是client端狀態轉移函數
// 主選單邏輯
