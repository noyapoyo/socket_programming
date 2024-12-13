#ifndef STATUS_CHANGE_H
#define STATUS_CHANGE_H
#include <string>
#include <map>
#include "other.h"
#include <pthread.h>
#include "network_utils.h"
#include "auth_utils.h"
#include "message_utils.h"
#include "../global.h"
#define PROMPT "prompt"
#define MESSAGE "message"
#define NORMAL "normal"
using namespace std;
void updateClientStatus(int client_socket, int status);
void removeClient(int client_socket);
int getStatus(int client_socket);
void updateUsernameToSocket(int client_socket, const string& client_name);
void removeUsernameToSocket(const string& client_name);
int checkMessage(const pair<string, string>& message, int status);
int handleClientMenu(const string& message, int client_socket);
int handleClientRegister(const string& client_name, const string& client_pwd, int client_socket);
int handleClientLogin(const string& client_name, const string& client_pwd, int client_socket);
int handleClientServe(const string& message, const string& client_name, const string& client_pwd, int client_socket);
int handleChatServe(const string& client_name, int client_socket, pair<string, string> *target_name);
int handleTextMessage(const string& client_name, const string& receiver, int client_socket);
int handleSendData(const string& client_name, int client_socket, const string& target_name);
#endif // STATUS_CHANGE_H
