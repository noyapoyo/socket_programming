#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "message_utils.h"
#include "network_utils.h"
using namespace std;
/*
這邊未來要引入pthread還有UDP傳送，會大改
*/
void sendMessage(int socket_fd, const string &message) 
{
    int32_t message_length = message.size(); // 先發送消息的長度
    message_length = htonl(message_length); // 將長度轉換為網絡字節序
    if (send(socket_fd, &message_length, sizeof(message_length), 0) < 0) 
    {
        cerr << "Failed to send message length." << "\n";
        closeSocket(socket_fd);
        return;
    }
    if (send(socket_fd, message.c_str(), message.size(), 0) < 0) // 發送實際消息內容
    {
        cerr << "Failed to send message content." << "\n";
        closeSocket(socket_fd);
        return;
    }
    cout << "Message sent successfully." << "\n";
}
string receiveMessage(int socket_fd) 
{
    int32_t message_length_net;
    int bytes_received_length = 0;
    while (bytes_received_length < sizeof(message_length_net))
    {
        int n = recv(socket_fd, ((char*)&message_length_net) + bytes_received_length, sizeof(message_length_net) - bytes_received_length, 0);
        if (n <= 0)
        {
            cerr << "Failed to receive message length." << "\n";
            closeSocket(socket_fd);
            return "";
        }
        bytes_received_length += n;
    }
    int32_t message_length = ntohl(message_length_net);
    string message;
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
            closeSocket(socket_fd);
            return "";
        }
        message.append(buffer, bytes_received);
        total_received += bytes_received;
    }
    cout << "Message received successfully." << "\n";
    return message;
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