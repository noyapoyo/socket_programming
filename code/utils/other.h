#ifndef OTHER_H
#define OTHER_H
#include <fstream>
#include <ctime>
#include <iomanip>
#include <iostream>
using namespace std;
// 寫入日誌的函數
void writeLog(const string &log_message);
// 寫入錯誤日誌的函數
void writeErrorLog(const string &error_message);
#endif // OTHER_H