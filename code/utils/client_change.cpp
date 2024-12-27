#include "client_change.h"
#include <iostream>
using namespace std;

#define REGISTER_TABLE "./data/users_table.txt"
#define LOGIN_TABLE "./data/login_table.txt"
#define ONLINE_TABLE "./data/online_table.txt"
#define FILEID "fileid"
#define END_OF_FILE "end_of_file"
#define FILEDATA "filedata"
#define AUDIO "audio"
#define AUDIO_DATA "audio_data"
#define AUDIO_END "audio_end"
int checkMessage(const pair<string, string>& message, int status)
{
    if (message.first.empty() || message.second.empty())
    {
        cerr << "Error: Received an empty message.\n";
        return 0;
    }
    return status;
}
int clientMainMenu(SSL* ssl, int status)
{
    string op;
    string message_to_server;
    pair <string, string> server_response;
    int result = status;
    cout << "Enter operation (1: Register, 2: Login, 3: Exit): ";
    cin >> op;
    if (op == "1") 
    {
        message_to_server = op + " I want to register!!";
        //sendMessage(server_socket, "MENU", message_to_server);
        result = 2; // 進入註冊流程
    } 
    else if (op == "2") 
    {
        message_to_server = op + " I want to login!!";
        //sendMessage(server_socket, "MENU", message_to_server);
        result = 3; // 進入登入流程
    } 
    else if (op == "3") 
    {
        message_to_server = op + " I want to exit!!";
        //sendMessage(server_socket, "MENU", message_to_server);
        //sendMessage(server_socket, message_to_server);
        //return 0;
        result = 0; // 結束
    } 
    else 
    {
        cout << "Unknown operation." << "\n";
        result = 1; // 返回主選單
    }
    sendMessage(ssl, NORMAL, message_to_server);
    server_response = receiveMessage(ssl);
    result = checkMessage(server_response, result);
    if (!result)
    {
        cout << "Server break!\n";
        return result;
    }
    Print_message(server_response.second, 1);
    return result;
    
}

// 認證邏輯（註冊/登入）
int clientAuthProcess(SSL* ssl, int status)
{
    string my_name, my_pwd;
    pair<string, string> server_response;
    cout << "Input your name: ";
    cin >> my_name;
    cout << "Input your password: ";
    cin >> my_pwd;
    sendMessage(ssl, NORMAL, my_name);
    sendMessage(ssl, NORMAL, my_pwd);
    server_response = receiveMessage(ssl);
    if (!checkMessage(server_response, status))
    {
        cout << "Server break!\n";
        return 0;
    }
    Print_message(server_response.second, 1);
    if (server_response.second == "Register success!!")
    {
        return 1; // 返回主選單
    }
    else if (server_response.second == "login success!!")
    {
        return 4; // 登入成功，進入服務狀態
    }
    else
    {
        return 1; // 認證失敗，返回主選單
    }
    return 1;
}

// 服務選單邏輯
void clientServiceMenu(SSL* ssl, int status)
{
    string op, message_to_server;
    //pair<string, string> server_response;
    int result = status;
    cin >> op;
    if (op == "1") 
    {
        message_to_server = op + " I want to chat with someone!!";
        //result = 5; // 正式進入使用app模式
    } 
    else if (op == "2") 
    {
        message_to_server = op + " I want to logout!!";
        //result = 1; // 返回主選單
    } 
    else if (op == "3") 
    {
        message_to_server = op + " I want to exit!!";
        //result = 0; // 結束
    } 
    else 
    {
        cout << "Unknown operation." << "\n";
        //result = 4; // 停留在服務狀態
    }
    sendMessage(ssl, PROMPT, message_to_server);
    /*
    server_response = receiveMessage(server_socket);
    result = checkMessage(server_response, result);
    if (!result)
    {
        cout << "Server break!\n";
        return result;
    }
    Print_message(server_response.second, 1);
    */
    //return result;
}
void chatSome(SSL* ssl) 
{
    //cout << "Entering Chat Mode..." << "\n";
    //string prompt = receiveMessage(server_socket);
    string target_user;
    //Print_message(prompt, 1);
    cin >> target_user;
    sendMessage(ssl, NORMAL, target_user);
}
void chatAndSendData(SSL* ssl)
{
    string choice;
    //cout << "Enter your choice: ";
    cin >> choice;
    sendMessage(ssl, NORMAL, choice);
    if (choice == "1") 
    {
        cout << "Input your message: ";
        string message;
        cin.ignore();
        getline(cin, message);
        sendMessage(ssl, MESSAGE, message);
        //string confirmation = receiveMessage(server_socket);
        //cout << confirmation << "\n";
    }
    else if (choice == "2")
    {
        cout << "Input your file path: ";
        string file_path;
        cin.ignore();
        getline(cin, file_path);
        string send_message = "I will send " + file_path;
        sendMessage(ssl, FILEID, send_message);
        sendFile(ssl, file_path);
        cout << "File sent successfully!" << "\n";
    }
    else if (choice == "3")
    {
        cout << "Input your audio path: ";
        string file_path;
        cin.ignore();
        getline(cin, file_path);
        string send_message = "I will send " + file_path;
        sendMessage(ssl, AUDIO, send_message);
        cout << "You need to wait for audio playback finishing\n";
        sendAudioBasedData(ssl, file_path);
        cout << "Audio send successfully!\n";
    }
    
}
int MsgStatusChange(const string& Msg, int status)
{
    
    if (status == 4 || status == 7)
    {
        if (Msg == "OK We will help you to chat with your friends!!")
            return 5;
        if (Msg == "OK we will help you logout!!")
            return 1;
        if (Msg == "OK bye!!")
            return 0;
        if (Msg == "Unknown command")
            return 4;
    }
    if (status == 5 || status == 7)
    {
        if (Msg == "Target user is not online.")
            return 4;
        if (Msg == "Target user is online.")
            return 6;
    }
    if (status == 6 || status == 7)
    {
        if (Msg == "0")
            return 4;
        return 6;
    }
    return 6;
}
void PrintPrompt(int status)
{
    if (status == 4)
    {
        cout << "Enter operation (1: Chat with someone, 2: Logout, 3: Exit): " << flush;
    }
    else if (status == 5)
    {
        cout << "Who do you want to chat : " << flush;
    }
    if (status == 6)
    {
        cout << "Choose what to send:\n"
                "0. Quit\n"
                "1. Text Message\n"
                "2. File\n"
                "3. Audio\n"
                "Enter your choice: " << flush;
    }
}