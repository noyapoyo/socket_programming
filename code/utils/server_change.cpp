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
int getStatus(int client_socket)
{
    int my_status;
    pthread_mutex_lock(&statusMutex);
    my_status = clientStatus[client_socket]; // 更新狀態
    pthread_mutex_unlock(&statusMutex);
    return my_status;
}
// 移除客戶端的工具函數
void removeClient(int client_socket) 
{
    pthread_mutex_lock(&statusMutex);
    if (clientStatus.find(client_socket) != clientStatus.end())
        clientStatus.erase(client_socket); // 從狀態表中移除
    for (const auto& [key, value] : clientStatus) 
    {
        cout << "socket_fd: " << key << " -> status: " << value << "\n";
    }
    pthread_mutex_unlock(&statusMutex);
}
void updateUsernameToSocket(int client_socket, const string& client_name)
{
    pthread_mutex_lock(&usernameToSocketMutex);
    usernameToSocket[client_name] = client_socket; // 更新usernameToSocket
    for (const auto& [key, value] : usernameToSocket) 
    {
        cout << "client_name: " << key << " -> socket_fd: " << value << "\n";
    }
    pthread_mutex_unlock(&usernameToSocketMutex);
}
void removeUsernameToSocket(const string& client_name) 
{
    pthread_mutex_lock(&usernameToSocketMutex);
    if (usernameToSocket.find(client_name) != usernameToSocket.end())
        usernameToSocket.erase(client_name); // 從狀態表中移除
    for (const auto& [key, value] : usernameToSocket) 
    {
        cout << "client_name: " << key << " -> socket_fd: " << value << "\n";
    }
    pthread_mutex_unlock(&usernameToSocketMutex);
}
int checkMessage(const pair<string, string>& message, int status)
{
    if (message.first.empty() || message.second.empty())
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
    //string str_type = "NORMAL";
    if (op == '1')
    {
        response = "OK We will help you register!!";
        sendMessage(client_socket, NORMAL, response);
        return 2; // 進入註冊流程
    }
    else if (op == '2')
    {
        response = "OK We will help you login!!";
        sendMessage(client_socket, NORMAL, response);
        return 3; // 進入登入流程
    }
    else if (op == '3')
    {
        response = "OK bye!!";
        sendMessage(client_socket, NORMAL, response);
        return 0; // 結束連線
    }
    else
    {
        response = "Unknown command";
        sendMessage(client_socket, NORMAL, response);
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
        sendMessage(client_socket, NORMAL, response);
        return 1; // 回到主選單
    }
    else
    {
        response = "Fail to register:( " + client_name + " has been used!!";
        cerr << "Error: Registration failed for (" << client_name << ") - Name already used.\n";
        sendMessage(client_socket, NORMAL, response);
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
        sendMessage(client_socket, PROMPT, response);
        return 4; // 進入服務狀態
    }
    else if (verify_result == 2)
    {
        response = "Fail to login:( We cannot find " + client_name;
        cerr << "Error: Login failed for (" << client_name << ") - User not found.\n";
        sendMessage(client_socket, PROMPT, response);
        return 1; // 回到主選單
    }
    else
    {
        response = "Password not match!";
        cerr << "Error: Login failed for (" << client_name << ") - Password mismatch.\n";
        sendMessage(client_socket, PROMPT, response);
        return 1; // 回到主選單
    }
}

// 處理服務狀態邏輯
int handleClientServe(const string& message, const string& client_name, const string& client_pwd, int client_socket)
{
    char op = message[0];
    string response;
    //cout << "Enter operation (1: Chat with someone, 2: Logout, 3: Exit): ";
    //sendMessage(client_socket, NORMAL, "Enter operation (1: Chat with someone, 2: Logout, 3: Exit): ");
    cout << "send success!!\n";
    if (op == '1')
    {
        response = "OK We will help you to chat with your friends!!";
        sendMessage(client_socket, PROMPT, response);
        return 5; // 進入聊天狀態
    }
    else if (op == '2')
    {
        response = "OK we will help you logout!!";
        DelToTable(client_name, client_pwd, LOGIN_TABLE); // 從在線名單中刪除
        sendMessage(client_socket, PROMPT, response);
        return 1; // 回到主選單
    }
    else if (op == '3')
    {
        response = "OK bye!!";
        DelToTable(client_name, client_pwd, LOGIN_TABLE); // 從在線名單中刪除
        sendMessage(client_socket, PROMPT, response);
        return 0; // 結束連線
    }
    else
    {
        response = "Unknown command\n";
        sendMessage(client_socket, PROMPT, response);
        return 4; // 繼續停留在服務狀態
    }
}
int getUserSocket(const string& username) 
{
    pthread_mutex_lock(&statusMutex);
    cout << "here " << username << "\n";
    for (auto data : usernameToSocket)
        cout << data.first << " " << data.second << "\n";
    int userSocket = (usernameToSocket.find(username) == usernameToSocket.end()) ? -1 : usernameToSocket[username];
    cout << "here2 " << userSocket << "\n";
    pthread_mutex_unlock(&statusMutex);
    return userSocket;
}
bool isUserOnline(pair<string, string> user_name)
{
    return getUserSocket(user_name.second) == -1 ? false : true;
}
int handleChatServe(const string& client_name, int client_socket, pair<string, string>* target_name) //這裡要改
{
    sendMessage(client_socket, NORMAL, "Who do you want to chat : ");
    //pair<string, string> NULL_pair;
    pair<string, string> check_data;
    check_data = receiveMessage(client_socket);
    if (checkMessage(check_data, 1) == 0) return 4;
    if (!isUserOnline(check_data))
    {
        cout << "here " << "\n";
        sendMessage(client_socket, PROMPT, "Target user is not online.");
        return 5;
    }
    *target_name = check_data;
    sendMessage(client_socket, PROMPT, "Target user is online.");
    return 6;
}


int handleSendData(const string& client_name, int client_socket, const string& target_name) //這裡要改
{
    sendMessage(client_socket, NORMAL, "Choose what to send:\n"
                "0. quit\n"
                "1. Text Message\n"
                "2. Image\n"
                "3. Video\n"
                "4. Audio\n"
                "5. Live Call\n"
                "6. Live Video\n"
                "Enter your choice: ");
    pair<string, string> Msg = receiveMessage(client_socket);
    char choice = Msg.second[0];
    if (choice == '0') 
    {
        sendMessage(client_socket, PROMPT, "0");
        return 4;
    } 
    else if (choice == '1') 
    {
        handleTextMessage(client_name, target_name, client_socket);
        
        return 6;
    } 
    else 
    {
        sendMessage(client_socket, NORMAL, "This feature is not implemented yet.");
        sendMessage(client_socket, PROMPT, "-1");
        return 6;
    }
    return 6;
}
int handleTextMessage(const string& sender, const string& receiver, int sender_socket) //這裡要改
{
    //sendMessage(sender_socket, "Enter your message:");
    pair<string, string> Msg = receiveMessage(sender_socket); //這裡要改
    string message = Msg.second;
    int receiver_socket = getUserSocket(receiver);
    if (receiver_socket == -1) 
    {
        sendMessage(sender_socket, PROMPT, "Message could not be delivered. Target user went offline.");
        return 5; 
    }
    string full_message = "[" + sender + "]" + " " + message;
    sendMessage(receiver_socket, MESSAGE, full_message);
    sendMessage(sender_socket, NORMAL, "Message sent successfully!");
    updateClientStatus(usernameToSocket[receiver], 7);
    return 5;
}
////////////////////////////////////////////////////此線以下是client端狀態轉移函數
// 主選單邏輯