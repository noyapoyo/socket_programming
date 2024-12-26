//訊息處理的工具函數
#ifndef MESSAGE_UTILS_H
#define MESSAGE_UTILS_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>
using namespace std;
void sendMessage(SSL* ssl, const string& identifier, const string& message);
pair<string, string> receiveMessage(SSL* ssl);
void Print_message(const string &message, int status); // 1 -> 伺服器對客戶， 2 -> 客戶對伺服器
void sendFile(SSL* ssl, const string& file_path);
void receiveFile(SSL* ssl, const string& output_file);
void initSSL();
SSL_CTX* serverCreateSSLContext();
void serverConfigureSSLContext(SSL_CTX* ctx, const string& cert_file, const string& key_file);
SSL_CTX* clientCreateSSLContext();
void clientConfigureSSLContext(SSL_CTX* ctx, const string& ca_file);
#endif // MESSAGE_UTILS_H