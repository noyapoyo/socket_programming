#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include "./utils/network_utils.h"
#include "./utils/message_utils.h"
#include <string>
using namespace std;
int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <hostname or IP> <port>" << "\n";
        return 1;
    }
    int server_socket = createSocket();
    string hostname = argv[1];
    string port = argv[2];
    connectToServer(server_socket, (const string)hostname, (const string)port);
    int status = 1;
    while (status != 0)
    {
        string message_to_server;
        string server_to_client;
        while (status == 1) 
        {
            string op;
            cout << "Enter operation (1: Register, 2: Login, 3: Exit): ";
            cin >> op;
            if (op == "1") 
            {
                message_to_server = op + " I want to register!!";
                status = 2;
            } 
            else if (op == "2") 
            {
                message_to_server = op + " I want to login!!";
                status = 3;
            } 
            else if (op == "3") 
            {
                message_to_server = op + " I want to exit!!";
                status = 0;
            } 
            else 
            {
                cout << "Unknown operation." << endl;
                continue;
            }
            sendMessage(server_socket, message_to_server);
            server_to_client = receiveMessage(server_socket);
            Print_message(server_to_client, 1);
        }
        while (status == 2 || status == 3)
        {
            string my_name;
            string my_pwd;
            cout << "Input your name : ";
            cin >> my_name;
            cout << "Input your password : ";
            cin >> my_pwd;
            sendMessage(server_socket, my_name);
            sendMessage(server_socket, my_pwd);
            server_to_client = receiveMessage(server_socket);
            Print_message(server_to_client, 1);
            if (server_to_client == "Register success!!")
                status = 1;
            else if (server_to_client == "login success!!")
                status = 4;
            else
                status = 1;
        }
        while (status == 4)
        {
            string op;
            string message_to_server;
            string server_to_client;
            cout << "Enter operation (1: Chat with someone, 2: Logout, 3: Exit): ";
            cin >> op;
            if (op == "1") 
            {
                message_to_server = op + " I want to chat with someone!!";
            } 
            else if (op == "2") 
            {
                message_to_server = op + " I want to logout!!";
                status = 1;
            } 
            else if (op == "3") 
            {
                message_to_server = op + " I want to exit!!";
                status = 0;
            } 
            else 
            {
                cout << "Unknown operation." << endl;
                continue;
            }
            sendMessage(server_socket, message_to_server);
            server_to_client = receiveMessage(server_socket);
            Print_message(server_to_client, 1);
        }
    }
    
    closeSocket(server_socket);
    return 0;
}