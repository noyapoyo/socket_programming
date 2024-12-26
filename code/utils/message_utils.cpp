#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "message_utils.h"
#include "network_utils.h"
#include <fstream>
#include <string>
#include <unistd.h>
using namespace std;
#define FILEID "fileid"
#define END_OF_FILE "end_of_file"
#define FILEDATA "filedata"
/*
這邊未來要引入pthread還有UDP傳送，會大改
*/
// 發送消息函數
void initSSL()
{
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

SSL_CTX* serverCreateSSLContext()
{
    const SSL_METHOD* method = TLS_server_method(); // 使用伺服器模式
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (!ctx)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void serverConfigureSSLContext(SSL_CTX* ctx, const string& cert_file, const string& key_file)
{
    // 加載伺服器證書
    if (SSL_CTX_use_certificate_file(ctx, cert_file.c_str(), SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // 加載伺服器私鑰
    if (SSL_CTX_use_PrivateKey_file(ctx, key_file.c_str(), SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // 驗證私鑰是否匹配證書
    if (!SSL_CTX_check_private_key(ctx))
    {
        cerr << "Private key does not match the certificate public key" << endl;
        exit(EXIT_FAILURE);
    }
}
SSL_CTX* clientCreateSSLContext()
{
    const SSL_METHOD* method = TLS_client_method(); // 使用客戶端模式
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (!ctx)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void clientConfigureSSLContext(SSL_CTX* ctx, const string& ca_file)
{
    // 加載信任的 CA 根證書
    if (!SSL_CTX_load_verify_locations(ctx, ca_file.c_str(), nullptr))
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // 設置證書驗證模式（驗證伺服器證書）
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, nullptr);
    SSL_CTX_set_verify_depth(ctx, 4);
}
void sendMessage(SSL* ssl, const string& identifier, const string& message)
{
    // 並接消息標識符和內容，格式為：<標識符>:<消息內容>
    string full_message = identifier + ":" + message;
    // 發送消息的長度
    int32_t message_length = full_message.size();
    message_length = htonl(message_length); // 將長度轉換為網絡字節序
    if (SSL_write(ssl, &message_length, sizeof(message_length)) <= 0)
    {
        cerr << "Failed to send message length." << "\n";
        ERR_print_errors_fp(stderr);
        return;
    }

    // 發送拼接後的消息
    if (SSL_write(ssl, full_message.c_str(), full_message.size()) <= 0)
    {
        cerr << "Failed to send message content." << "\n";
        ERR_print_errors_fp(stderr);
        return;
    }
}

// 接收消息函數
pair<string, string> receiveMessage(SSL* ssl)
{
    // 接收消息長度
    int32_t message_length_net;
    int bytes_received_length = 0;

    while (bytes_received_length < sizeof(message_length_net))
    {
        int n = SSL_read(ssl, ((char*)&message_length_net) + bytes_received_length,
                         sizeof(message_length_net) - bytes_received_length);
        if (n <= 0)
        {
            cerr << "Failed to receive message length." << "\n";
            ERR_print_errors_fp(stderr);
            return {"", ""};
        }
        bytes_received_length += n;
    }

    int32_t message_length = ntohl(message_length_net);

    // 接收完整消息
    string full_message;
    int total_received = 0;

    while (total_received < message_length)
    {
        int bytes_to_read = message_length - total_received;
        char buffer[1024];
        int chunk_size = (bytes_to_read < sizeof(buffer)) ? bytes_to_read : sizeof(buffer);
        int bytes_received = SSL_read(ssl, buffer, chunk_size);

        if (bytes_received <= 0)
        {
            cerr << "Failed to receive message content." << "\n";
            ERR_print_errors_fp(stderr);
            return {"", ""};
        }

        full_message.append(buffer, bytes_received);
        total_received += bytes_received;
    }

    // 解析標識符和內容
    size_t delimiter_pos = full_message.find(":");

    if (delimiter_pos == string::npos)
    {
        cerr << "Invalid message format received." << "\n";
        return {"", full_message}; // 如果沒有標識符，返回整條消息作為內容
    }

    string identifier = full_message.substr(0, delimiter_pos);
    string message = full_message.substr(delimiter_pos + 1);

    return {identifier, message}; // 返回標識符和內容
}
void sendFile(SSL* ssl, const string& file_path) 
{
    // 打開文件
    FILE* file = fopen(file_path.c_str(), "rb");
    //cout << "now_1\n";
    if (!file) //這邊很重要，路徑複製不能直接在檔案總管複製，不然會出錯
    {
        //cout << "now fail send file\n";
        cerr << "Failed to open file: " << file_path << "\n";
        return;
    }
    char buffer[2048];
    size_t bytes_read;
    int chunk_count = 0; // 計算塊數
    //cout << "now_2\n";
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) 
    {
        //cout << "client sending\n";
        string file_chunk(buffer, bytes_read);
        sendMessage(ssl, FILEDATA, file_chunk); // 使用 FILEDATA 標識符發送文件內容
        chunk_count++;
    }
    //cout << "now_3\n";
    fclose(file);
    // 發送結束標誌
    sendMessage(ssl, END_OF_FILE, ""); // 使用 END_OF_FILE 標識符通知接收方文件傳輸結束
    cout << "File transfer completed: " << file_path << " (" << chunk_count << " chunks sent)\n";
}
void receiveFile(SSL* ssl, const string& output_file) 
{
    FILE* file = fopen(output_file.c_str(), "wb");
    if (!file) 
    {
        cerr << "Failed to open file for writing: " << output_file << "\n";
        return;
    }
    while (true) 
    {
        pair<string, string> fileChunk = receiveMessage(ssl);
        if (fileChunk.first == END_OF_FILE) 
        {
            break;
        }
        else if (fileChunk.first == FILEDATA) 
        {
            fwrite(fileChunk.second.c_str(), 1, fileChunk.second.size(), file);
        }
        else 
        {
            cerr << "Unexpected message identifier: " << fileChunk.first << "\n";
            break;
        }
    }
    fclose(file);
    cout << "File received successfully: " << output_file << "\n";
}
void Print_message(const string &message, int status) 
{
    const string RED = "\033[1;31m";
    const string YELLOW = "\033[1;33m";
    const string RESET = "\033[0m";
    if (status == 1) 
    {
        cout << RED << "[Server]: " << message << RESET << "\n";
    } 
    else if (status == 2) 
    {
        cout << YELLOW << "[Client]: " << message << RESET << "\n";
    } 
    else 
    {
        cout << "[Unknown]: " << message << "\n";
    }
}
/*
bool isDataAvailable(int socket_fd) 
{
    fd_set read_fds;
    struct timeval timeout;

    FD_ZERO(&read_fds);
    FD_SET(socket_fd, &read_fds);

    timeout.tv_sec = 0;  // 无需阻塞
    timeout.tv_usec = 10000;  // 10ms 超时时间

    int result = select(socket_fd + 1, &read_fds, nullptr, nullptr, &timeout);
    return (result > 0 && FD_ISSET(socket_fd, &read_fds));
}
void handleTextMessage(const string& sender, const string& receiver, int sender_socket) {
    int receiver_socket = getUserSocket(receiver);
    if (receiver_socket == -1) {
        sendMessage(sender_socket, "Message could not be delivered. Target user is offline.");
    } else {
        string message = sender + ": " + receiveMessage(sender_socket);
        sendMessage(receiver_socket, message);
        sendMessage(sender_socket, "Message sent successfully.");
    }
}
*/


