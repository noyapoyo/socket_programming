#include "server_change.h"
#include <iostream>
#include <openssl/ssl.h>
#include <openssl/err.h>

using namespace std;

#define REGISTER_TABLE "./data/users_table.txt"
#define LOGIN_TABLE "./data/login_table.txt"
#define ONLINE_TABLE "./data/online_table.txt"

// 檢查訊息是否有效
void updateClientStatus(SSL* ssl, int status)
{
    pthread_mutex_lock(&statusMutex);
    clientStatus[ssl] = status; // 更新狀態
    for (const auto& [key, value] : clientStatus)
    {
        cout << "socket_fd: " << key << " -> status: " << value << "\n";
    }
    pthread_mutex_unlock(&statusMutex);
}

int getStatus(SSL* ssl)
{
    int my_status;
    pthread_mutex_lock(&statusMutex);
    my_status = clientStatus[ssl]; // 更新狀態
    pthread_mutex_unlock(&statusMutex);
    return my_status;
}

// 移除客戶端的工具函數
void removeClient(SSL* ssl)
{
    pthread_mutex_lock(&statusMutex);
    if (clientStatus.find(ssl) != clientStatus.end())
    {
        clientStatus.erase(ssl); // 從狀態表中移除
    }
    for (const auto& [key, value] : clientStatus)
    {
        cout << "ssl: " << key << " -> status: " << value << "\n";
    }
    pthread_mutex_unlock(&statusMutex);
}

void updateUsernameToSSL(SSL* ssl, const string& client_name)
{
    pthread_mutex_lock(&usernameToSSLMutex);
    usernameToSSL[client_name] = ssl; // 更新usernameToSSL
    for (const auto& [key, value] : usernameToSSL)
    {
        cout << "client_name: " << key << "\n";
    }
    pthread_mutex_unlock(&usernameToSSLMutex);
}
SSL* getUserSSL(const string& receiver)
{
    SSL* my_ssl;
    pthread_mutex_lock(&usernameToSSLMutex);
    if (usernameToSSL.find(receiver) != usernameToSSL.end())
        my_ssl = usernameToSSL[receiver]; // 更新狀態
    else
        my_ssl = NULL;
    pthread_mutex_unlock(&usernameToSSLMutex);
    return my_ssl;
}

void removeUsernameToSSL(const string& client_name)
{
    pthread_mutex_lock(&usernameToSSLMutex);
    if (usernameToSSL.find(client_name) != usernameToSSL.end())
    {
        usernameToSSL.erase(client_name); // 從狀態表中移除
    }
    for (const auto& [key, value] : usernameToSSL)
    {
        cout << "client_name: " << key << "\n";
    }
    pthread_mutex_unlock(&usernameToSSLMutex);
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
int handleClientMenu(const string& message, SSL* ssl)
{
    char op = message[0];
    string response;
    if (op == '1')
    {
        response = "OK We will help you register!!";
        sendMessage(ssl, NORMAL, response);
        return 2; // 進入註冊流程
    }
    else if (op == '2')
    {
        response = "OK We will help you login!!";
        sendMessage(ssl, NORMAL, response);
        return 3; // 進入登入流程
    }
    else if (op == '3')
    {
        response = "OK bye!!";
        sendMessage(ssl, NORMAL, response);
        return 0; // 結束連線
    }
    else
    {
        response = "Unknown command";
        sendMessage(ssl, NORMAL, response);
        return 1; // 回到主選單
    }
}

// 處理用戶註冊邏輯
int handleClientRegister(const string& client_name, const string& client_pwd, SSL* ssl)
{
    string response;

    if (checkRegister(client_name))
    {
        WriteToTable(client_name, client_pwd, REGISTER_TABLE);
        response = "Register success!!";
        cout << "Client (" << client_name << ") registered successfully.\n";
        sendMessage(ssl, NORMAL, response);
        return 1; // 回到主選單
    }
    else
    {
        response = "Fail to register:( " + client_name + " has been used!!";
        cerr << "Error: Registration failed for (" << client_name << ") - Name already used.\n";
        sendMessage(ssl, NORMAL, response);
        return 1; // 回到主選單
    }
}

// 處理用戶登入邏輯
int handleClientLogin(const string& client_name, const string& client_pwd, SSL* ssl)
{
    string response;
    int verify_result = verifyPassword(client_name, client_pwd);

    if (verify_result == 1)
    {
        WriteToTable(client_name, client_pwd, LOGIN_TABLE);
        response = "login success!!";
        cout << "Client (" << client_name << ") logged in successfully.\n";
        sendMessage(ssl, PROMPT, response);
        return 4; // 進入服務狀態
    }
    else if (verify_result == 2)
    {
        response = "Fail to login:( We cannot find " + client_name;
        cerr << "Error: Login failed for (" << client_name << ") - User not found.\n";
        sendMessage(ssl, PROMPT, response);
        return 1; // 回到主選單
    }
    else
    {
        response = "Password not match!";
        cerr << "Error: Login failed for (" << client_name << ") - Password mismatch.\n";
        sendMessage(ssl, PROMPT, response);
        return 1; // 回到主選單
    }
}

// 處理服務狀態邏輯
int handleClientServe(const string& message, const string& client_name, const string& client_pwd, SSL* ssl)
{
    char op = message[0];
    string response;
    cout << "send success!!\n";
    if (op == '1')
    {
        response = "OK We will help you to chat with your friends!!";
        sendMessage(ssl, PROMPT, response);
        return 5; // 進入聊天狀態
    }
    else if (op == '2')
    {
        response = "OK we will help you logout!!";
        DelToTable(client_name, client_pwd, LOGIN_TABLE); // 從在線名單中刪除
        sendMessage(ssl, PROMPT, response);
        return 1; // 回到主選單
    }
    else if (op == '3')
    {
        response = "OK bye!!";
        DelToTable(client_name, client_pwd, LOGIN_TABLE); // 從在線名單中刪除
        sendMessage(ssl, PROMPT, response);
        return 0; // 結束連線
    }
    else
    {
        response = "Unknown command\n";
        sendMessage(ssl, PROMPT, response);
        return 4; // 繼續停留在服務狀態
    }
}

int handleTextMessage(const string& sender, const string& receiver, SSL* ssl)
{
    pair<string, string> Msg = receiveMessage(ssl);
    string message = Msg.second;
    SSL* receiver_ssl = getUserSSL(receiver); //這行要大改
    if (receiver_ssl == NULL)
    {
        sendMessage(ssl, PROMPT, "Message could not be delivered. Target user went offline.");
        return 5;
    }
    string full_message = "[" + sender + "]" + " " + message;
    sendMessage(receiver_ssl, MESSAGE, full_message); ///這裡有天大的問題，要修改一些東西
    sendMessage(ssl, NORMAL, "Message sent successfully!");
    updateClientStatus(usernameToSSL[receiver], 7);
    return 5;
}
bool isUserOnline(pair<string, string> user_name)
{
    return getUserSSL(user_name.second) == NULL ? false : true;
}
int handleChatServe(const string& client_name, SSL* ssl, pair<string, string>* target_name) //這裡要改
{
    sendMessage(ssl, NORMAL, "Who do you want to chat : ");
    //pair<string, string> NULL_pair;
    pair<string, string> check_data;
    check_data = receiveMessage(ssl);
    if (checkMessage(check_data, 1) == 0) return 4;
    if (!isUserOnline(check_data))
    {
        cout << "here " << "\n";
        sendMessage(ssl, PROMPT, "Target user is not online.");
        return 5;
    }
    *target_name = check_data;
    sendMessage(ssl, PROMPT, "Target user is online.");
    return 6;
}


int handleSendData(const string& client_name, SSL* ssl, const string& target_name) //這裡要改
{
    sendMessage(ssl, NORMAL, "Choose what to send:\n"
                "0. quit\n"
                "1. Text Message\n"
                "2. Image\n"
                "3. Video\n"
                "4. Audio\n"
                "5. Live Call\n"
                "6. Live Video\n"
                "Enter your choice: ");
    pair<string, string> Msg = receiveMessage(ssl);
    char choice = Msg.second[0];
    if (choice == '0') 
    {
        sendMessage(ssl, PROMPT, "0");
        return 4;
    } 
    else if (choice == '1') 
    {
        handleTextMessage(client_name, target_name, ssl);
        
        return 6;
    } 
    else 
    {
        sendMessage(ssl, NORMAL, "This feature is not implemented yet.");
        sendMessage(ssl, PROMPT, "-1");
        return 6;
    }
    return 6;
}