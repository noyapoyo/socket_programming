#ifndef CLIENT_CHANGE_H
#define CLIENT_CHANGE_H
#include <string>
#include <map>
#include "other.h"
#include <pthread.h>
#include "network_utils.h"
#include "auth_utils.h"
#include "message_utils.h"
#define PROMPT "prompt"
#define MESSAGE "message"
#define NORMAL "normal"
using namespace std;
int checkMessage(const pair<string, string>& message, int status);
int clientMainMenu(SSL* ssl, int status);
int clientAuthProcess(SSL* ssl, int status);
void clientServiceMenu(SSL* ssl, int status);
void chatSome(SSL* ssl);
void chatAndSendData(SSL* ssl);
int MsgStatusChange(const string& Msg, int status);
void PrintPrompt(int status);
#endif // CLIENT_CHANGE_H