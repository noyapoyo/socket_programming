#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include "./utils/other.h"
#include "./utils/network_utils.h"
#include "./utils/auth_utils.h"
#include "./utils/message_utils.h"
#include "./utils/client_change.h"
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
    bool hasMessage = false; // 表示是否有新消息
    string incomingMessage;
    while (status != 0)
    {
        string message_to_server;
        string server_to_client;
        int tem_status;
        /*
        if (!hasMessage) 
        {
            if (isDataAvailable(server_socket)) 
            {
                incomingMessage = receiveMessage(server_socket);
                hasMessage = true;
                tem_status = status;
                status = 7;
            }
        }
        */
        if (status == 1) 
        {
            status = clientMainMenu(server_socket, status);
        }
        if (status == 2 || status == 3)
        {
            status = clientAuthProcess(server_socket, status);
        }
        if (status == 4)
        {
            status = clientServiceMenu(server_socket, status);
        }
        /*
        if (status == 5) //選擇和哪個客戶聊天的狀態
        {
            string target_name;
            status = chatSome(server_socket, status, &target_name);
        }
        if (status == 6) //一個持續與客戶B傳訊息的狀態
        {

        }
        if (status == 7) 
        {
            cout << "\n--- New Message ---\n";
            cout << incomingMessage << "\n";
            cout << "Press Enter to return.\n";
            cin.ignore(); // 等待用户按下回车
            hasMessage = false; // 重置消息标志
            status = 5; // 返回聊天状态
        }
        */
    }
    closeSocket(server_socket);
    return 0;
}