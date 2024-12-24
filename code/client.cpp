#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "./utils/other.h"
#include "./utils/network_utils.h"
#include "./utils/auth_utils.h"
#include "./utils/message_utils.h"
#include "./utils/client_change.h"
#include <string>
#include <iostream>
#include <openssl/ssl.h>
#include <openssl/err.h>
#define PROMPT "prompt"
#define MESSAGE "message"
#define NORMAL "normal"
using namespace std;
SSL_CTX* ssl_ctx; // 全局 SSL 上下文
void initializeSSL()
{
    initSSL();
    ssl_ctx = clientCreateSSLContext();
    clientConfigureSSLContext(ssl_ctx, "ca.crt");
}
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
    connectToServer(server_socket, hostname, port);
    int status = 1;
    int pre_status;
    bool hasPrompted = false; // 用於標記是否已經提示
    pair<string, string> incomingMessage;
    initializeSSL();
    SSL* ssl = SSL_new(ssl_ctx);
    SSL_set_fd(ssl, server_socket);
    if (SSL_connect(ssl) <= 0)
    {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        closeSocket(server_socket);
        return 1;
    }
    while (status != 0)
    {
        if (status < 4)
        {
            if (status == 1)
            {
                //cout << "here\n";
                status = clientMainMenu(ssl, status);
            }
            else if (status == 2 || status == 3)
            {
                status = clientAuthProcess(ssl, status);
            }
        }
        else // 狀態 4 及其後，使用 select
        {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(STDIN_FILENO, &readfds); // 監視標準输入
            FD_SET(server_socket, &readfds); // 監視服務器消息
            int max_fd = max(STDIN_FILENO, server_socket);
            int activity = select(max_fd + 1, &readfds, nullptr, nullptr, nullptr);
            if (activity < 0)
            {
                perror("select");
                break;
            }
            
            // 處理服務器消息
            if (FD_ISSET(server_socket, &readfds))
            {
                incomingMessage = receiveMessage(ssl);
                if (!checkMessage(incomingMessage, status))
                {
                    cerr << "Server disconnected or invalid message received." << "\n";
                    break;
                }
                if (incomingMessage.first == NORMAL)
                {
                    cout << incomingMessage.second << "\n";
                    //cout << incomingMessage.second; //不能正常顯示
                    //Print_message(incomingMessage.second, 1);
                    //cout << incomingMessage.second;
                    //status = MsgStatusChange(incomingMessage.second, status);
                }
                else if (incomingMessage.first == PROMPT)
                {
                    Print_message(incomingMessage.second, 1);
                    //cout << "now my status -> " << status << "\n";
                    status = MsgStatusChange(incomingMessage.second, status);
                    continue;
                }
                else if (incomingMessage.first == MESSAGE)
                {
                    cout << "\n--- New Message ---" << "\n";
                    cout << incomingMessage.second << "\n";
                    cout << "Press Enter to continue." << "\n";
                    cout << "Message come my status = " << status << "\n";
                    pre_status = status;
                    status = 7;
                }
            }

            // 處理用戶輸入
            if (FD_ISSET(STDIN_FILENO, &readfds))
            {
                //PrintPrompt(status);
                if (status == 4)
                {
                    clientServiceMenu(ssl, status);
                }
                if (status == 5) //選和誰聊天
                {
                    chatSome(ssl);
                }
                if (status == 6) //選要用甚麼聊
                {
                    chatAndSendData(ssl);
                }
                if (status == 7)
                {
                    //cout << "here\n";
                    string temp;
                    getline(cin, temp); // 等待用户按下 Enter
                    //cout << "I need to change my status from " << status << " to " << pre_status << "\n";
                    status = pre_status;
                    PrintPrompt(status);
                    //sendMessage(ssl, MESSAGE, to_string(status));
                }
            }
        }
    }
    SSL_shutdown(ssl);
    SSL_free(ssl);
    closeSocket(server_socket);
    return 0;
}
