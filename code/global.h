#ifndef GLOBAL_H
#define GLOBAL_H

#include <map>
#include <string>
#include <queue>
#include <pthread.h>
using namespace std;
extern map <int, int> clientStatus; // 客户端狀態表
extern map <string, int> usernameToSocket;
extern pthread_mutex_t usernameToSocketMutex;
extern pthread_mutex_t statusMutex;     // 狀態互斥鎖
#endif // GLOBAL_H