#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "message_utils.h"
#include "network_utils.h"
#include <fstream>
#include <string>
#include <unistd.h>
using namespace std;
/*
這邊未來要引入pthread還有UDP傳送，會大改
*/
// 發送消息函數
void sendMessage(int socket_fd, const string& identifier, const string& message)
{
    // 並接消息標識符和內容，格式為：<標識符>:<消息內容>
    string full_message = identifier + ":" + message;

    // 發送消息的長度
    int32_t message_length = full_message.size();
    message_length = htonl(message_length); // 將長度轉換為網絡字節序

    if (send(socket_fd, &message_length, sizeof(message_length), 0) < 0)
    {
        cerr << "Failed to send message length." << "\n";
        close(socket_fd);
        return;
    }

    // 發送拼接後的消息
    if (send(socket_fd, full_message.c_str(), full_message.size(), 0) < 0)
    {
        cerr << "Failed to send message content." << "\n";
        close(socket_fd);
        return;
    }
}



// 接收消息函數
pair<string, string> receiveMessage(int socket_fd)
{
    // 接收消息長度
    int32_t message_length_net;
    int bytes_received_length = 0;

    while (bytes_received_length < sizeof(message_length_net))
    {
        int n = recv(socket_fd, ((char*)&message_length_net) + bytes_received_length,
                     sizeof(message_length_net) - bytes_received_length, 0);

        if (n <= 0)
        {
            cerr << "Failed to receive message length." << "\n";
            close(socket_fd);
            return {"", ""};
        }

        bytes_received_length += n;
    }

    int32_t message_length = ntohl(message_length_net);

    // 接收完整消息
    string full_message;
    int total_received = 0;

    while (total_received < message_length)
    {
        int bytes_to_read = message_length - total_received;
        char buffer[1024];
        int chunk_size = (bytes_to_read < sizeof(buffer)) ? bytes_to_read : sizeof(buffer);
        int bytes_received = recv(socket_fd, buffer, chunk_size, 0);

        if (bytes_received <= 0)
        {
            cerr << "Failed to receive message content." << "\n";
            close(socket_fd);
            return {"", ""};
        }

        full_message.append(buffer, bytes_received);
        total_received += bytes_received;
    }

    // 解析標識符和內容
    size_t delimiter_pos = full_message.find(":");

    if (delimiter_pos == string::npos)
    {
        cerr << "Invalid message format received." << "\n";
        return {"", full_message}; // 如果沒有標識符，返回整條消息作為內容
    }

    string identifier = full_message.substr(0, delimiter_pos);
    string message = full_message.substr(delimiter_pos + 1);

    return {identifier, message}; // 返回標識符和內容
}
int sendFile(const string& file_path, int client_socket) 
{
    ifstream file(file_path, ios::binary);
    if (!file.is_open()) 
    {
        cerr << "Error: Unable to open file " << file_path << "\n";
        return -1;
    }
    char buffer[1024];
    while (!file.eof()) 
    {
        file.read(buffer, sizeof(buffer));
        int bytes_read = file.gcount();
        if (send(client_socket, buffer, bytes_read, 0) < 0) 
        {
            cerr << "Error: Failed to send file data." << "\n";
            file.close();
            return -1;
        }
    }
    string end_of_file = "END_OF_FILE";
    send(client_socket, end_of_file.c_str(), end_of_file.size(), 0);

    file.close();
    return 0;
}
int receiveFile(const string& file_path, int server_socket) 
{
    ofstream file(file_path, ios::binary);
    if (!file.is_open()) 
    {
        cerr << "Error: Unable to create file " << file_path << "\n";
        return -1;
    }
    char buffer[1024];
    while (true) 
    {
        int bytes_received = recv(server_socket, buffer, sizeof(buffer), 0);
        if (bytes_received < 0) 
        {
            cerr << "Error: Failed to receive file data." << "\n";
            file.close();
            return -1;
        }
        if (bytes_received == 0 || string(buffer, bytes_received).find("END_OF_FILE") != string::npos) 
        {
            break;
        }
        file.write(buffer, bytes_received);
    }
    file.close();
    return 0;
}
void Print_message(const string &message, int status) 
{
    const string RED = "\033[1;31m";
    const string YELLOW = "\033[1;33m";
    const string RESET = "\033[0m";
    if (status == 1) 
    {
        cout << RED << "[Server]: " << message << RESET << "\n";
    } 
    else if (status == 2) 
    {
        cout << YELLOW << "[Client]: " << message << RESET << "\n";
    } 
    else 
    {
        cout << "[Unknown]: " << message << "\n";
    }
}
/*
bool isDataAvailable(int socket_fd) 
{
    fd_set read_fds;
    struct timeval timeout;

    FD_ZERO(&read_fds);
    FD_SET(socket_fd, &read_fds);

    timeout.tv_sec = 0;  // 无需阻塞
    timeout.tv_usec = 10000;  // 10ms 超时时间

    int result = select(socket_fd + 1, &read_fds, nullptr, nullptr, &timeout);
    return (result > 0 && FD_ISSET(socket_fd, &read_fds));
}
void handleTextMessage(const string& sender, const string& receiver, int sender_socket) {
    int receiver_socket = getUserSocket(receiver);
    if (receiver_socket == -1) {
        sendMessage(sender_socket, "Message could not be delivered. Target user is offline.");
    } else {
        string message = sender + ": " + receiveMessage(sender_socket);
        sendMessage(receiver_socket, message);
        sendMessage(sender_socket, "Message sent successfully.");
    }
}
*/


