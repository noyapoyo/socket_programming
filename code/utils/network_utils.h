//網絡相關的工具函數（例如IP轉換、錯誤處理）
#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
using namespace std;
int createSocket(bool use_ipv6 = false, bool is_udp = false); // 創建 socket 並返回描述符，這邊呢，默認使用IPv4和TCP
void bindSocket(int socket_fd, int port, bool use_ipv6 = false); // 將 socket 綁定到指定的端口
void listenOnSocket(int socket_fd, int backlog = 10); // 讓伺服器開始監聽連接
int acceptConnection(int socket_fd, struct sockaddr_storage  *client_address); // 接受客戶端的連線並返回客戶端 socket 描述符(client_fd)
//void connectToServer(int socket_fd, const string &hostname, const string &port) 
void connectToServer(int socket_fd, const string &hostname, const string &port, bool use_ipv6 = false); // 讓客戶端連接到伺服器的指定 IP 和端口
void closeSocket(int socket_fd); // 關閉指定的 socket 連接
//void closeSocket(int socket_fd, SSL *ssl = nullptr)這會是未來的closeSocket!!!(因為要加密)
string getIpAddress(const sockaddr_storage &address); // 從 sockaddr_in 結構中獲取並返回 IP 地址
template <typename... Sockets>
void handleError(const string &error_message, int socket_fd, Sockets... other_sockets); // 處理錯誤並顯示錯誤訊息
// SSL 支持
/*
void initSSL(); // 初始化 OpenSSL
SSL_CTX* createSSLContext(); // 創建 SSL 上下文
void configureSSLContext(SSL_CTX *ctx, const string &cert_file, const string &key_file); // 設置證書和私鑰
SSL* acceptSSLConnection(SSL_CTX *ctx, int client_fd); // SSL 客戶端連接接受
void cleanupSSL(SSL *ssl, SSL_CTX *ctx); // 清理 SSL
*/
#endif // NETWORK_UTILS_H