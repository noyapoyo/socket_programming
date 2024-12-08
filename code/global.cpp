#include "global.h"
using namespace std;
map<int, int> clientStatus; // 客户端狀態表
map <string, int> usernameToSocket;
pthread_mutex_t usernameToSocketMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t statusMutex = PTHREAD_MUTEX_INITIALIZER;     // 狀態互斥鎖