#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include "./utils/other.h"
#include "./utils/network_utils.h"
#include "./utils/auth_utils.h"
#include "./utils/message_utils.h"
#include "./utils/status_change.h"
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
            status = clientMainMenu(server_socket, status);
        }
        while (status == 2 || status == 3)
        {
            status = clientAuthProcess(server_socket, status);
        }
        while (status == 4)
        {
            status = clientServiceMenu(server_socket, status);
        }
    }
    closeSocket(server_socket);
    return 0;
}