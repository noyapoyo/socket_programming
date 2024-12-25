#include "network_utils.h"
#include <iostream>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;
int createSocket(bool use_ipv6, bool is_udp)
{
    int domain = use_ipv6 ? AF_INET6 : AF_INET; 
    int type = is_udp ? SOCK_DGRAM : SOCK_STREAM;
    int socket_fd = socket(domain, type, 0);
    if (socket_fd < 0)
    {
        handleError("Fail to create a socket.", socket_fd);
    }
    return socket_fd;
}
void closeSocket(int socket_fd)
{
    if (socket_fd >= 0)
        close(socket_fd);
}
void bindSocket(int socket_fd, int port, bool use_ipv6)
{
    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        handleError("setsockopt(SO_REUSEADDR) failed", socket_fd);
    }
    if (use_ipv6)
    {
        sockaddr_in6 address;
        memset(&address, 0, sizeof(sockaddr_in6));
        address.sin6_family = AF_INET6;
        address.sin6_addr = in6addr_any; // 綁定到所有 IPv6 地址
        address.sin6_port = htons(port);
        if (bind(socket_fd, (sockaddr*)&address, sizeof(address)) < 0)
        {
            string error_message;
            if (errno == EACCES)
            {
                error_message = "Error: Permission denied when binding IPv6 socket on port " + to_string(port);
            }
            else if (errno == EADDRINUSE)
            {
                error_message = "Error: Address already in use when binding IPv6 socket on port " + to_string(port);
            }
            else if (errno == EADDRNOTAVAIL)
            {
                error_message = "Error: Address not available when binding IPv6 socket on port " + to_string(port);
            }
            else
            {
                error_message = "Failed to bind IPv6 socket: " + string(strerror(errno));
            }
            handleError(error_message, socket_fd);
        }
    }
    else
    {
        sockaddr_in address;
        memset(&address, 0, sizeof(sockaddr_in));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY; // 綁定到所有 IPv4 地址
        address.sin_port = htons(port);
        if (bind(socket_fd, (sockaddr*)&address, sizeof(address)) < 0)
        {
            string error_message;
            if (errno == EACCES)
            {
                error_message = "Error: Permission denied when binding IPv4 socket on port " + to_string(port);
            }
            else if (errno == EADDRINUSE)
            {
                error_message = "Error: Address already in use when binding IPv4 socket on port " + to_string(port);
            }
            else if (errno == EADDRNOTAVAIL)
            {
                error_message = "Error: Address not available when binding IPv4 socket on port " + to_string(port);
            }
            else
            {
                error_message = "Failed to bind IPv4 socket: " + string(strerror(errno));
            }
            handleError(error_message, socket_fd);
        }
    }
}
void listenOnSocket(int socket_fd, int backlog) //未來使用者變多後，需要判斷 listen 發生的錯誤是不可重試錯誤還是可重試錯誤，可重試錯誤應給機會重試，不可重試直接呼叫handleError
{
    if (listen(socket_fd, backlog) < 0) 
    {
        if (errno == EOPNOTSUPP) 
        {
            cerr << "Error: Operation not supported. The socket does not support listen (e.g., it's a UDP socket).\n";
            handleError("EOPNOTSUPP: listen operation not supported on this socket", socket_fd);
        } 
        else 
        {
            handleError("Failed to listen on socket", socket_fd);
        }
    }
}
int acceptConnection(int socket_fd, struct sockaddr_storage  *client_address)
{
    socklen_t client_len = sizeof(*client_address);
    int client_socket = accept(socket_fd, (struct sockaddr*)client_address, &client_len);
    if (client_socket < 0)
    {
        handleError("Failed to accept client connection", socket_fd, client_socket);
    }
    char ip_str[INET6_ADDRSTRLEN];
    if (client_address -> ss_family == AF_INET) // IPv4
    { 
        sockaddr_in *addr_in = (sockaddr_in *)client_address;
        inet_ntop(AF_INET, &(addr_in->sin_addr), ip_str, sizeof(ip_str));
        cout << "Accepted connection from " << ip_str << ":" << ntohs(addr_in->sin_port) << "\n";
    } 
    else if (client_address -> ss_family == AF_INET6) // IPv6
    { 
        sockaddr_in6 *addr_in6 = (sockaddr_in6 *)client_address;
        inet_ntop(AF_INET6, &(addr_in6->sin6_addr), ip_str, sizeof(ip_str));
        cout << "Accepted connection from " << ip_str << ":" << ntohs(addr_in6->sin6_port) << "\n";
    }
    return client_socket;
}
/*
void connectToServer(int socket_fd, const string &ip, const string &port, bool use_ipv6) 
{
    if (use_ipv6) 
    {
        sockaddr_in6 server_address6;
        memset(&server_address6, 0, sizeof(server_address6));
        server_address6.sin6_family = AF_INET6;
        server_address6.sin6_port = htons(port);

        // 將 IP 字符串轉換為二進制並填充到 server_address6 結構
        if (inet_pton(AF_INET6, ip.c_str(), &server_address6.sin6_addr) <= 0) 
        {
            cerr << "Invalid IPv6 address format" << endl;
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
        
        if (connect(socket_fd, (sockaddr*)&server_address6, sizeof(server_address6)) < 0)  // 連接伺服器
        {
            cerr << "Failed to connect to IPv6 server" << endl;
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
    } 
    else 
    {
        sockaddr_in server_address;
        memset(&server_address, 0, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);
        // 將 IP 字符串轉換為二進制並填充到 server_address 結構
        if (inet_pton(AF_INET, ip.c_str(), &server_address.sin_addr) <= 0) 
        {
            cerr << "Invalid IPv4 address format" << endl;
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
        if (connect(socket_fd, (sockaddr*)&server_address, sizeof(server_address)) < 0) // 連接伺服器
        {
            cerr << "Failed to connect to IPv4 server" << endl;
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
    }
    cout << "Connected to server at " << ip << ":" << port << endl;
}
*/
void connectToServer(int socket_fd, const string &hostname, const string &port, bool use_ipv6) 
{
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = use_ipv6 ? AF_INET6 : AF_INET;  
    hints.ai_socktype = SOCK_STREAM;   
    int status = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &res);
    if (status != 0) 
    {
        cerr << "getaddrinfo error: " << gai_strerror(status) << "\n";
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    for (p = res; p != NULL; p = p->ai_next) 
    {
        if (connect(socket_fd, p->ai_addr, p->ai_addrlen) == 0) 
        {
            cout << "Connected to server at " << hostname << ":" << port << "\n";
            freeaddrinfo(res);
            return;
        } 
        else 
        {
            cerr << "Failed to connect: " << strerror(errno) << "\n";
        }
    }
    cerr << "Unable to connect to server at " << hostname << ":" << port << "\n";
    freeaddrinfo(res);
    close(socket_fd);
    exit(EXIT_FAILURE);
}

template <typename... Sockets>
void handleError(const string &error_message, int socket_fd, Sockets... other_sockets) 
{
    cerr << error_message << "\n";
    closeSocket(socket_fd);
    (closeSocket(other_sockets), ...);
    exit(EXIT_FAILURE);
}