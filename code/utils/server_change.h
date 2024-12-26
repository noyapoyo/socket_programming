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
#include <openssl/ssl.h>
#define PROMPT "prompt"
#define MESSAGE "message"
#define NORMAL "normal"
using namespace std;
void updateClientStatus(SSL* ssl, int status);
void removeClient(SSL* ssl);
int getStatus(SSL* ssl);
void updateUsernameToSSL(SSL* ssl, const string& client_name);
void removeUsernameToSSL(const string& client_name);
SSL* getUserSSL(const string& receiver);
int checkMessage(const pair<string, string>& message, int status);
int handleClientMenu(const string& message, SSL* ssl);
int handleClientRegister(const string& client_name, const string& client_pwd, SSL* ssl);
int handleClientLogin(const string& client_name, const string& client_pwd, SSL* ssl);
int handleClientServe(const string& message, const string& client_name, const string& client_pwd, SSL* ssl);
bool isUserOnline(pair<string, string> user_name);
int handleChatServe(const string& client_name, SSL* ssl, pair<string, string>* target_name);
int handleTextMessage(const string& client_name, const string& receiver, SSL* ssl);
int handleSendFile(const string& client_name, const string& receiver, SSL* sender_ssl);
int handleSendData(const string& client_name, SSL* ssl, const string& target_name);
string parseFileName(const string& send_message);

#endif // STATUS_CHANGE_H
