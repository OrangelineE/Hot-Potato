#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <string>
/** This function is to print the error message out
 * @param msg represents the error message we want to print
 */
void report_error(std::string msg) {
    std::cerr<< "Error: " << msg << std::endl;
}
/** This function is to print the error message out
 * @param msg represents the error message we want to print
 * @param host indicates the host name
 * @param port is the port we use
 */
void report_error_plus(std::string msg, const char* host, const char* port) {
    std::cerr<< "Error: " << msg << " for host:" <<  host << "(port: " << port << ")" << std::endl;
}
/** This function is to initialize the server side and start to listen to the indicated port
 * @param port specifies the port used to communicate with clients
 * @return socket_fd if everything goes well. Otherwise, report the error ans exit
 */
int server_init(const char* port) {
    int status;
    int socket_fd;
    const char* host_name = NULL;
    struct addrinfo host_info;
    struct addrinfo* host_info_list;

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;
    status = getaddrinfo(host_name, port, &host_info, &host_info_list);
    if (status != 0) {
        report_error_plus("server side cannot get address info", host_name, port);
        std::cerr << "getaddrinfo failed: " << gai_strerror(status) << std::endl;
        exit(EXIT_FAILURE);
    }
    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if(socket_fd == -1) {
        report_error_plus("cannot create the socket", host_name, port);
        exit(EXIT_FAILURE);
    }
    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        report_error_plus("cannot bind the socket", host_name, port);
        exit(EXIT_FAILURE);
    }
    status = listen(socket_fd, 100);
    if (status == -1) {
        report_error_plus("cannot listen on the soket", host_name, port);
        exit(EXIT_FAILURE);        
    }
    freeaddrinfo(host_info_list);
    return socket_fd;
}

/** This function is to accept the client by giving information like socket fd and ip address
 * @param socket_fd specifies the socket file descriptor
 * @param ip_addr specifies the ip address information
 * @return client_connection_fd if everything goes well. Otherwise, report the error ans exit
 */
int accept_connection(int socket_fd, std::string *ip_addr) {
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connection_fd = accept(socket_fd, (struct sockaddr* )&socket_addr, &socket_addr_len);
    if(client_connection_fd == -1) {
        report_error("cannot accept connection on socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in* addr = (struct sockaddr_in*)&socket_addr;
    *ip_addr = inet_ntoa(addr->sin_addr);
    return client_connection_fd;
}

/** This function is to initialize the client side with hostname and port number
 * @param host_name specifies the host name
 * @param port specifies the port used to communicate with clients
 * @return socket_fd if everything goes well. Otherwise, report the error ans exit
 */
int client_init(const char * host_name, const char * port) {
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo* host_info_list;
    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    
    status = getaddrinfo(host_name, port, &host_info, &host_info_list);
    if (status != 0) {
        report_error_plus("client side cannot get address info", host_name, port);
        std::cerr << "getaddrinfo failed: " << gai_strerror(status) << std::endl;
        exit(EXIT_FAILURE);
    }
    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if(socket_fd == -1) {
        report_error_plus("cannot create the socket", host_name, port);
        exit(EXIT_FAILURE);
    }
    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        report_error_plus("cannot connect to the socket", host_name, port);
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(host_info_list);
    return socket_fd;
}

