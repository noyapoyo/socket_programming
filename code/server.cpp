#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <string>
#include <sstream>
#include <queue>
#include <csignal>
#include "./utils/other.h"
#include "./utils/network_utils.h"
#include "./utils/auth_utils.h"
#include "./utils/message_utils.h"
#include "./utils/status_change.h"
#define DEFAULT_TCP_PORT 8080
#define DEFAULT_UDP_PORT 9090
#define THREAD_POOL_SIZE 10
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
//map <string, int> online_map;
queue<int> taskQueue;                // 客戶端 socket 描述符的任務隊列
pthread_mutex_t queueMutex;          // 任務隊列的互斥鎖
pthread_cond_t conditionVar;         // 任務隊列的條件變量
pthread_t threadPool[THREAD_POOL_SIZE]; // 線程池
bool isRunning = true;
void initializeServer(int* TCP_socket_fd, int* UDP_socket_fd, int tcp_port, int udp_port)
{
    *TCP_socket_fd = createSocket();
    //*UDP_socket_fd = createSocket(false, false);
    bindSocket(*TCP_socket_fd, tcp_port);
    listenOnSocket(*TCP_socket_fd, 10);
    //bindSocket(*UDP_socket_fd, udp_port);
}
void handleTCPConnection(int client_socket) 
{
    cout << "Handling client connection: " << client_socket << "\n";
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
    closeSocket(client_socket);
}
void* threadWork(void* arg)
{
    while (isRunning)
    {
        int client_socket;
        // 從任務隊列中取出任務
        pthread_mutex_lock(&queueMutex);
        while (taskQueue.empty())
        {
            pthread_cond_wait(&conditionVar, &queueMutex);
        }
        client_socket = taskQueue.front();
        taskQueue.pop();
        pthread_mutex_unlock(&queueMutex);
        handleTCPConnection(client_socket);
    }
    return nullptr;
}
void initializeThreadPool()
{
    pthread_mutex_init(&queueMutex, nullptr);
    pthread_cond_init(&conditionVar, nullptr);
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        pthread_create(&threadPool[i], nullptr, threadWork, nullptr);
    }
    cout << "Thread pool initialized with " << THREAD_POOL_SIZE << " threads.\n";
}
void handleUDPConnection(int udp_socket)
{
    char buffer[1024];
    struct sockaddr_storage client_address;
    socklen_t addr_len = sizeof(client_address);
}
void signalHandler(int signal)
{
    if (signal == SIGINT)
    {
        cout << "\nSIGINT received. Shutting down server...\n";
        isRunning = false;
        pthread_cond_broadcast(&conditionVar);
    }
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
    initializeThreadPool();
    //signal(SIGTERM, SIG_IGN);
    signal(SIGINT, signalHandler);
    while (true) 
    {
        struct sockaddr_storage client_address;
        socklen_t addr_len = sizeof(client_address);
        int client_socket = acceptConnection(TCP_socket_fd, &client_address);
        //handleTCPConnection(client_socket);
        if (client_socket > 0)
        {
            pthread_mutex_lock(&queueMutex); // 將新客戶端加入任務隊列
            taskQueue.push(client_socket);
            pthread_cond_signal(&conditionVar); // 通知線程池
            pthread_mutex_unlock(&queueMutex);
        }
        // handleUDPConnection(UDP_socket_fd);
    }
    isRunning = false;
    pthread_cond_broadcast(&conditionVar); // 通知所有線程退出
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        pthread_join(threadPool[i], nullptr); // 等待所有線程退出
    }
    pthread_mutex_destroy(&queueMutex); // 銷毁互斥鎖
    pthread_cond_destroy(&conditionVar); // 銷毁條件變量
    closeSocket(TCP_socket_fd);
    return 0;
}
