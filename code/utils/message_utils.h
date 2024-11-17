//訊息處理的工具函數
#ifndef MESSAGE_UTILS_H
#define MESSAGE_UTILS_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
using namespace std;
void sendMessage(int socket_fd, const string &message);
string receiveMessage(int socket_fd);
void Print_message(const string &message, int status); // 1 -> 伺服器對客戶， 2 -> 客戶對伺服器
#endif // MESSAGE_UTILS_H