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
using namespace std;
void updateClientStatus(int client_socket, int status);
void removeClient(int client_socket);
int checkMessage(const string& message, int status);
int handleClientMenu(const string& message, int client_socket);
int handleClientRegister(const string& client_name, const string& client_pwd, int client_socket);
int handleClientLogin(const string& client_name, const string& client_pwd, int client_socket);
int handleClientServe(const string& message, const string& client_name, const string& client_pwd, int client_socket);
int handleChatServe(const string& client_name, int client_socket);
#endif // STATUS_CHANGE_H
