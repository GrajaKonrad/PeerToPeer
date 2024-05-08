#include "../include/Client.h"
#include "../include/Log.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>

Client::Client(std::string ip, int port) {
    m_ip = ip;
    m_port = port;
    m_serverPort = 0;

    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);

    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    int on = 1;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));

    if (connect(m_socket, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        m_connected = false;
        return;
    }

    m_connected = true;
}

Client::Client(std::string ip, int port, int socket) {
    m_socket = socket;
    m_ip = ip;
    m_port = port;
    m_connected = true;
    m_serverPort = 0;
}

Client::~Client() {
    close(m_socket);
}

bool Client::CanRecive() {
    fd_set set;
    FD_ZERO(&set);
    FD_SET(m_socket, &set);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    select(m_socket + 1, &set, NULL, NULL, &timeout);
    return FD_ISSET(m_socket, &set);
}

bool Client::CanSend() {
    fd_set set;
    FD_ZERO(&set);
    FD_SET(m_socket, &set);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    select(m_socket + 1, NULL, &set, NULL, &timeout);
    return FD_ISSET(m_socket, &set);
}

bool Client::HasError() {
    fd_set set;
    FD_ZERO(&set);
    FD_SET(m_socket, &set);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    select(m_socket + 1, NULL, NULL, &set, &timeout);
    return FD_ISSET(m_socket, &set);
}

std::shared_ptr<Message> Client::Receive() {
    if (!CanRecive()) {
        return nullptr;
    }

    char buffer[Message::MaxLength];
    int recivedTotal = 0;

    int recived = read(m_socket, buffer, Message::MaxLength);
    recivedTotal += recived;

    if (recivedTotal == 0) {
        m_connected = false;
        return nullptr;
    }

    while (recivedTotal < Message::HeaderLength) {
        recived = read(m_socket, buffer + recivedTotal, Message::MaxLength - recivedTotal);
        recivedTotal += recived;
    }

    uint32_t dataLength = Message::BytesToUint32(buffer + Message::DataLengthOffset);

    while (recivedTotal < Message::HeaderLength + dataLength) {
        recived = read(m_socket, buffer + recivedTotal, Message::MaxLength - recivedTotal);
        recivedTotal += recived;
    }

    return std::make_shared<Message>(buffer);
}

void Client::Send() {
    while (m_messageQueue.size() > 0 && CanSend()) {
        std::shared_ptr<Message> message = m_messageQueue.front();
        m_messageQueue.pop();
        write(m_socket, message->ToBytes(), message->GetLength());
    }
}