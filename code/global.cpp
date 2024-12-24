#include "global.h"
using namespace std;
map<SSL*, int> clientStatus; // 客户端狀態表
map <string, SSL*> usernameToSSL;
pthread_mutex_t usernameToSSLMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t statusMutex = PTHREAD_MUTEX_INITIALIZER;     // 狀態互斥鎖