#ifndef GLOBAL_H
#define GLOBAL_H

#include <map>
#include <string>
#include <queue>
#include <pthread.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
using namespace std;
extern map <SSL*, int> clientStatus; // 客户端狀態表
extern map <string, SSL*> usernameToSSL;
extern pthread_mutex_t usernameToSSLMutex;
extern pthread_mutex_t statusMutex;     // 狀態互斥鎖
#endif // GLOBAL_H