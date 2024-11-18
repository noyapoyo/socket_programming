#ifndef STATUS_CHANGE_H
#define STATUS_CHANGE_H
#include <string>
#include "other.h"
#include "network_utils.h"
#include "auth_utils.h"
#include "message_utils.h"
using namespace std;
int checkMessage(const string& message, int status);
int handleClientMenu(const string& message, int client_socket);
int handleClientRegister(const string& client_name, const string& client_pwd, int client_socket);
int handleClientLogin(const string& client_name, const string& client_pwd, int client_socket);
int handleClientServe(const string& message, const string& client_name, const string& client_pwd, int client_socket);
int clientMainMenu(int server_socketint, int status);
int clientAuthProcess(int server_socket, int status);
int clientServiceMenu(int server_socket, int status);
#endif // STATUS_CHANGE_H
