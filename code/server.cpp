#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <string>
#include <sstream>
#include "./utils/other.h"
#include "./utils/network_utils.h"
#include "./utils/auth_utils.h"
#include "./utils/message_utils.h"
#include "./utils/status_change.h"
#define DEFAULT_TCP_PORT 8080
#define DEFAULT_UDP_PORT 9090
#define REGISTER_TABLE "./data/users_table.txt"
#define LOGIN_TABLE "./data/online_table.txt"
using namespace std;
//公共IP -> ssh 41147009s@ws4.csie.ntu.edu.tw    
//連線方法 ./client ws4.csie.ntu.edu.tw 8080
//connectToServer 尚未完成
//我需要有個傳輸訊息的函數
//我需要將關於網路的連接的成功或錯誤寫進伺服器日誌，終端機單純做訊息的接收和傳輸


//要 include utils (工具函數的目錄)內的.h檔
//要從 data (存放用戶資料或訊息紀錄)抓取用戶資料
//media檔案處理麥克風和鏡頭
//127.0.0.1本地固定 ip
//ws3.csie.ntu.edu.tw


/*
需傳送狀態碼給客戶，也需接收給server的狀態碼
*/
map <string, int> online_map;
void initializeServer(int* TCP_socket_fd, int* UDP_socket_fd, int tcp_port, int udp_port)
{
    *TCP_socket_fd = createSocket();
    //*UDP_socket_fd = createSocket(false, false);
    bindSocket(*TCP_socket_fd, tcp_port);
    listenOnSocket(*TCP_socket_fd, 10);
    //bindSocket(*UDP_socket_fd, udp_port);
}
void handleTCPConnection(int tcp_socket) 
{
    struct sockaddr_storage client_address;
    socklen_t addr_len = sizeof(client_address);
    int client_socket = acceptConnection(tcp_socket, &client_address);
    if (client_socket > 0)
    {
        cout << "Accept new client, client_socket is " << client_socket << "\n";
        string message_to_client;
        string message_to_server;
        string client_name;
        string client_pwd;
        int status = 1;
        while (status != 0)
        {
            while (status == 1) // client 還在選要 register 還是 login 還是 exit
            {
                message_to_server = receiveMessage(client_socket);
                status = checkMessage(message_to_server, status);
                if (!status) break;
                Print_message(message_to_server, 2);
                status = handleClientMenu(message_to_server, client_socket);
            }
            while (status == 2) // client 在 Register 中
            {
                cout << "client now is register\n";
                client_name = receiveMessage(client_socket);
                status = checkMessage(client_name, status);
                if (!status) break;
                client_pwd = receiveMessage(client_socket);
                status = checkMessage(client_pwd, status);
                if (!status) break;
                status = handleClientRegister(client_name, client_pwd, client_socket);
            }
            while (status == 3) // client 在 login 中
            {
                client_name = receiveMessage(client_socket);
                status = checkMessage(client_name, status);
                if (!status) break;
                client_pwd = receiveMessage(client_socket);
                status = checkMessage(client_pwd, status);
                if (!status) break;
                status = handleClientLogin(client_name, client_pwd, client_socket);
            }
            while (status == 4) // login成功
            {
                message_to_server = receiveMessage(client_socket);
                status = checkMessage(message_to_server, status);
                if (!status) break;
                Print_message(message_to_server, 2);
                status = handleClientServe(message_to_server, client_name, client_pwd, client_socket);
            }
        }
        DelToTable(client_name, client_pwd, LOGIN_TABLE); // 清除在線名單
    }
    closeSocket(client_socket);
}
/*
1. message
*/
void handleUDPConnection(int udp_socket)
{
    char buffer[1024];
    struct sockaddr_storage client_address;
    socklen_t addr_len = sizeof(client_address);
}
int main(int argc, char* argv[]) 
{
    int tcp_port = DEFAULT_TCP_PORT;
    int udp_port = DEFAULT_UDP_PORT;
    if (argc >= 2) 
    {
        tcp_port = atoi(argv[1]);
    }
    if (argc >= 3) 
    {
        udp_port = atoi(argv[2]);
    }
    cout << "Starting server on TCP port " << tcp_port << " and UDP port " << udp_port << "\n";
    int TCP_socket_fd = -1;
    int UDP_socket_fd = -1;
    initializeServer(&TCP_socket_fd, &UDP_socket_fd, tcp_port, udp_port);
    while (true) 
    {
        handleTCPConnection(TCP_socket_fd);
        // handleUDPConnection(UDP_socket_fd);
    }
    closeSocket(TCP_socket_fd);
    //closeSocket(UDP_socket_fd);
    return 0;
}