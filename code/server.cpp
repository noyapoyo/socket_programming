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
#include "./utils/server_change.h"
#include "global.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#define DEFAULT_TCP_PORT 8080
#define DEFAULT_UDP_PORT 9090
#define THREAD_POOL_SIZE 10
#define REGISTER_TABLE "./data/users_table.txt"
#define LOGIN_TABLE "./data/login_table.txt"
#define ONLINE_TABLE "./data/online_table.txt"
using namespace std;
//這邊先關掉 git.enabled
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


/*
map<int, int> clientStatus; // 客户端狀態表
pthread_mutex_t statusMutex;     // 狀態互斥鎖
*/
queue<int> taskQueue;                // 客戶端 socket 描述符的任務隊列
pthread_mutex_t queueMutex;          // 任務隊列的互斥鎖
pthread_cond_t conditionVar;         // 任務隊列的條件變量
bool isRunning = true;
static int TCP_socket_fd = -1;
static int UDP_socket_fd = -1;
pthread_t threadPool[THREAD_POOL_SIZE]; // 線程池
SSL_CTX* ssl_ctx; // 全局 SSL 上下文
void initializeSSL()
{
    initSSL();
    ssl_ctx = serverCreateSSLContext();
    serverConfigureSSLContext(ssl_ctx, "server.crt", "server.key");
}

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
    // SSL 初始化
    //initializeSSL();
    SSL* ssl = SSL_new(ssl_ctx);
    SSL_set_fd(ssl, client_socket);
    //cout << "Before SSL_accept\n";
    if (SSL_accept(ssl) <= 0)
    {
        //cout << "SSL_accept failed\n";
        int err = SSL_get_error(ssl, -1);
        switch (err)
        {
            case SSL_ERROR_ZERO_RETURN:
                cerr << "SSL connection closed by client.\n";
                break;
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                cerr << "SSL_accept needs more data to proceed.\n";
                break;
            case SSL_ERROR_SYSCALL:
                perror("SSL_accept system call error");
                break;
            case SSL_ERROR_SSL:
            default:
                cerr << "SSL_accept failed.\n";
                ERR_print_errors_fp(stderr);
                break;
        }
        closeSocket(client_socket);
        return;
    }
    //cout << "After SSL_accept\n";
    string message_to_client;
    pair<string, string> message_to_server;
    string Msg;
    //string Idt;
    string client_name;
    string client_pwd;
    string my_target;
    int status = 1;
    //cout << "here\n";
    updateClientStatus(ssl, status);
    //cout << "here_2\n";
    while (status != 0)
    {
        //cout << "here\n";
        //cout << "now status " << status << "\n";
        if (status == 1) // client 還在選要 register 還是 login 還是 exit
        {
            message_to_server = receiveMessage(ssl);
            status = checkMessage(message_to_server, status);
            if (!status) continue;
            //Idt = message_to_server.first;
            Msg = message_to_server.second;
            Print_message(Msg, 2);
            status = handleClientMenu(Msg, ssl);
            updateClientStatus(ssl, status);
            status = getStatus(ssl);
        }
        if (status == 2) // client 在 Register 中
        {
            cout << "client now is register\n";
            message_to_server = receiveMessage(ssl);
            status = checkMessage(message_to_server, status);
            if (!status) continue;
            //Idt = message_to_server.first;
            client_name = message_to_server.second;
            message_to_server = receiveMessage(ssl);
            status = checkMessage(message_to_server, status);
            if (!status) continue;
            client_pwd = message_to_server.second;
            status = handleClientRegister(client_name, client_pwd, ssl);
            updateClientStatus(ssl, status);
            status = getStatus(ssl);
        }
        if (status == 3) // client 在 login 中
        {
            message_to_server = receiveMessage(ssl);
            status = checkMessage(message_to_server, status);
            if (!status) continue;
            //Idt = message_to_server.first;
            client_name = message_to_server.second;
            message_to_server = receiveMessage(ssl);
            status = checkMessage(message_to_server, status);
            if (!status) continue;
            client_pwd = message_to_server.second;
            status = handleClientLogin(client_name, client_pwd, ssl);
            updateUsernameToSSL(ssl, client_name);
            updateClientStatus(ssl, status);
            status = getStatus(ssl);
        }
        if (status == 4) // login成功
        {
            string client_prompt = "Enter operation (1: Chat with someone, 2: Logout, 3: Exit): ";
            sendMessage(ssl, NORMAL, client_prompt);
            message_to_server = receiveMessage(ssl);
            status = checkMessage(message_to_server, status);
            if (!status) continue;
            Msg = message_to_server.second;
            Print_message(Msg, 2);
            status = handleClientServe(Msg, client_name, client_pwd, ssl);
            updateClientStatus(ssl, status);
            if (status < 4)
                removeUsernameToSSL(client_name);
            status = getStatus(ssl);
        }
        if (status == 5) // 客戶在線中
        {
            pair<string, string> target_name = {"", ""};
            status = handleChatServe(client_name, ssl, &target_name);
            if (target_name.first != "")
                my_target = target_name.second;
            updateClientStatus(ssl, status);
            status = getStatus(ssl);
        }
        if (status == 6) // 客戶在線中
        {
            status = handleSendData(client_name, ssl, my_target);
            updateClientStatus(ssl, status);
            status = getStatus(ssl);
        }
        if (status == 7) // 客戶在線中
        {
            //status = handleSendData(client_name, client_socket, my_target);
            //updateClientStatus(client_socket, status);
            message_to_server = receiveMessage(ssl);
            status = checkMessage(message_to_server, status);
            status = stoi(message_to_server.second);
            updateClientStatus(ssl, status);
            status = getStatus(ssl);
        }
    }

    removeClient(ssl);
    removeUsernameToSSL(client_name);
    DelToTable(client_name, client_pwd, LOGIN_TABLE); // 清除在線名單
    SSL_shutdown(ssl);
    SSL_free(ssl);
    closeSocket(client_socket);
}
void* threadWork(void* arg)
{
    while (isRunning)
    {
        int client_socket;
        // 從任務隊列中取出任務
        pthread_mutex_lock(&queueMutex);
        while (taskQueue.empty() && isRunning)
        {
            pthread_cond_wait(&conditionVar, &queueMutex);
        }
        if (!isRunning)
        {
            pthread_mutex_unlock(&queueMutex);
            break;
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
    pthread_mutex_init(&statusMutex, nullptr);
    pthread_cond_init(&conditionVar, nullptr);
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        pthread_create(&threadPool[i], nullptr, threadWork, nullptr);
    }
    cout << "Thread pool initialized with " << THREAD_POOL_SIZE << " threads.\n";
}
void signalHandler(int signal)
{
    if (signal == SIGINT)
    {
        isRunning = false;
        cout << "server break!!\n";
        pthread_cond_broadcast(&conditionVar); // 通知所有線程退出
        for (int i = 0; i < THREAD_POOL_SIZE; i++)
        {
            pthread_join(threadPool[i], nullptr); // 等待所有線程退出
        }
        pthread_mutex_destroy(&queueMutex); // 銷毁互斥鎖
        pthread_mutex_destroy(&statusMutex); // 銷毀 statusMutex
        pthread_cond_destroy(&conditionVar); // 銷毁條件變量
        if (TCP_socket_fd > 0)
        {
            closeSocket(TCP_socket_fd);
            TCP_socket_fd = -1;
        }
        if (ssl_ctx)
        {
            SSL_CTX_free(ssl_ctx);
        }
        cout << "Server has shut down gracefully.\n";
        exit(0);
    }
}
int main(int argc, char* argv[]) 
{
    int tcp_port = DEFAULT_TCP_PORT;
    int udp_port = DEFAULT_UDP_PORT;

    initializeSSL(); // 初始化 SSL

    if (argc >= 2) 
    {
        tcp_port = atoi(argv[1]);
    }
    if (argc >= 3) 
    {
        udp_port = atoi(argv[2]);
    }
    cout << "Starting server on TCP port " << tcp_port << " and UDP port " << udp_port << "\n";
    
    initializeServer(&TCP_socket_fd, &UDP_socket_fd, tcp_port, udp_port);
    initializeThreadPool();
    signal(SIGINT, signalHandler);

    while (isRunning) 
    {
        struct sockaddr_storage client_address;
        socklen_t addr_len = sizeof(client_address);
        int client_socket = acceptConnection(TCP_socket_fd, &client_address);

        if (!isRunning)
        {
            break;
        }

        if (client_socket > 0)
        {
            pthread_mutex_lock(&queueMutex); // 將新客戶端加入任務隊列
            taskQueue.push(client_socket);
            pthread_cond_signal(&conditionVar); // 通知線程池
            pthread_mutex_unlock(&queueMutex);
        }
    }

    return 0;
}
