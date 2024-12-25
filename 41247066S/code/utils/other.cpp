#include "other.h"
#include <fstream>
#include <ctime>
#include <iomanip>
#include <iostream>
#include "other.h"
using namespace std;
#define LOG_FILE "./data/Log.log"
void writeLog(const string &log_message) 
{
    ofstream log_file(LOG_FILE, ios::app); // 以追加方式打開日誌文件
    if (log_file.is_open()) 
    {
        // 獲取當前時間
        time_t now = time(nullptr);
        tm *time_info = localtime(&now);

        // 寫入時間戳和日誌訊息
        log_file << "[" << put_time(time_info, "%Y-%m-%d %H:%M:%S") << "] INFO: " << log_message << "\n";
        log_file.close();
    } 
    else 
    {
        cerr << "Failed to write log to " << LOG_FILE << "\n";
    }
}
void writeErrorLog(const string &error_message) 
{
    ofstream log_file(LOG_FILE, ios::app); // 以追加方式打開日誌文件
    if (log_file.is_open()) 
    {
        // 獲取當前時間
        time_t now = time(nullptr);
        tm *time_info = localtime(&now);
        // 寫入時間戳和錯誤訊息
        log_file << "[" << put_time(time_info, "%Y-%m-%d %H:%M:%S") << "] ERROR: " << error_message << "\n";
        log_file.close();
    } 
    else 
    {
        cerr << "Failed to write error log to " << LOG_FILE << "\n";
    }
}