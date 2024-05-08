#include "../include/Server.h"
#include "../include/Log.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

Server::Server(int port) {
    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    int on = 1;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));

    if (bind(m_socket, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        Log::Red("Serwer port bind error");
        m_running = false;
        return;
    }
    
    if (listen(m_socket, 10) == -1) {
        Log::Red("Serwer port listen error");
        m_running = false;
        return;
    }

    m_running = true;
}

Server::~Server() {
    close(m_socket);
}

std::shared_ptr<Client> Server::Accept() {
    struct sockaddr_in addr;
    socklen_t size = sizeof(addr);
    int client = accept(m_socket, (struct sockaddr*)&addr, &size);
    if (client == -1) {
        Log::Red("Accept error");
    }
    return std::make_shared<Client>(inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), client);
}