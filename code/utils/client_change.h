#ifndef CLIENT_CHANGE_H
#define CLIENT_CHANGE_H
#include <string>
#include <map>
#include "other.h"
#include <pthread.h>
#include "network_utils.h"
#include "auth_utils.h"
#include "message_utils.h"
using namespace std;
int checkMessage(const string& message, int status);
int clientMainMenu(int server_socketint, int status);
int clientAuthProcess(int server_socket, int status);
int clientServiceMenu(int server_socket, int status);
int chatSome(int server_socket, int status);
#endif // CLIENT_CHANGE_H