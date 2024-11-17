//用戶認證的工具函數
#ifndef AUTH_UTILS_H
#define AUTH_UTILS_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;
// 用戶身份驗證的工具函數
// 用戶註冊
// 傳入用戶名和密碼，返回是否註冊成功
//bool registerUser(const string &username, const string &password); 來不及抽象化這步，之後再做
// 用戶登入
// 驗證用戶名和密碼是否匹配，成功返回 true，失敗返回 false
//bool loginUser(const string &username, const string &password); 來不及抽象化這步，之後再做
// 用戶登出
// 傳入用戶名，執行登出操作，成功返回 true
bool logoutUser(const string &username);
// 用戶驗證狀態
// 檢查用戶是否已登入，返回 true 表示已登入
bool isUserLoggedIn(const string &username);
// 密碼驗證
// 傳入用戶名和密碼，檢查密碼是否匹配，返回 true 表示匹配
void WriteToTable(const string &client_name, const string &client_pwd, const string &f_path);
void DelToTable(const string &client_name, const string &client_pwd, const string &f_path);
int verifyPassword(const string &username, const string &password);
bool checkRegister(const string &username);
void PrintAllRegister();
void PrintAllLogin();
// 更新密碼
// 傳入用戶名、新舊密碼，更新密碼成功返回 true
//bool updatePassword(const string &username, const string &old_password, const string &new_password);

/*
如果用戶尚未登入，則一直停留在狀態1，authenticateUser也會把用戶的所有選擇回傳給伺服器
如果用戶選擇registerUser則更新已註冊名單(如果成功)並回傳狀態1
如果用戶選擇login並成功登入則更新在線名單，則回傳狀態2
如果用戶選擇exit則更新在線名單回傳狀態3
*/
//int authenticateUser(int socket_fd); 來不及抽象化這步，之後再做
#endif // AUTH_UTILS_H