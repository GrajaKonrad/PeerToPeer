#pragma once

#include "Client.h"
#include "Message.h"

#include <string>
#include <vector>

// Bazowa klasa dla wszystkich komend
class Command {

public:
    /// @brief Funkcja Wykonywująca komendę
    virtual void Execute() = 0;

};

// Komenda do łączenia się z innym klientem
class ConnectCommand : public Command {

public:
    ConnectCommand(std::string ip, int port) : m_ip(ip), m_port(port) {}
    virtual void Execute() override;

private:
    std::string m_ip;
    int m_port;

};

// Komenda do dodawania nowego klienta do listy klientów
class ClientAddCommand : public Command {

public:
    ClientAddCommand(std::shared_ptr<Client> client) : m_client(client) {}
    virtual void Execute() override;

private:
    std::shared_ptr<Client> m_client;

};

// Komenda do wysyłania portu serwera do innego peer'a
class ServerPortSendCommand : public Command {

public:
    ServerPortSendCommand(std::shared_ptr<Client> client) : m_client(client) {}
    virtual void Execute() override;

private:
    std::shared_ptr<Client> m_client;

};

// Komenda do odbierania portu serwera od innego peer'a
class ServerPortReceiveCommand : public Command {

public:
    ServerPortReceiveCommand(std::shared_ptr<Client> client, std::shared_ptr<Message> message) : m_client(client), m_message(message) {}
    virtual void Execute() override;

private:
    std::shared_ptr<Client> m_client;
    std::shared_ptr<Message> m_message;

};

// Komenda do wysyłania listy peer'ów do innego peer'a
class ClientListSendCommand : public Command {

public:
    ClientListSendCommand(std::shared_ptr<Client> client) : m_client(client) {}
    virtual void Execute() override;

private:
    std::shared_ptr<Client> m_client;

};

// Komenda do odbierania listy peer'ów od innego peer'a
class ClientListReceiveCommand : public Command {

public:
    ClientListReceiveCommand(std::shared_ptr<Client> client, std::shared_ptr<Message> message) : m_client(client), m_message(message) {}
    virtual void Execute() override;

private:
    std::shared_ptr<Client> m_client;
    std::shared_ptr<Message> m_message;

};

// Komenda do wysyłania zapytań o listę plików
class FileListRequestCommand : public Command {

public:
    virtual void Execute() override;

};

// Komenda do wysyłania listy plików do innego peer'a
class FileListSendCommand : public Command {

public:
    FileListSendCommand(std::shared_ptr<Client> client) : m_client(client) {}
    virtual void Execute() override;

private:
    std::shared_ptr<Client> m_client;

};

// Komenda do odebrania listy plików od innego peer'a
class FileListReceiveCommand : public Command {

public:
    FileListReceiveCommand(std::shared_ptr<Client> client, std::shared_ptr<Message> message) : m_client(client), m_message(message) {}
    virtual void Execute() override;

private:
    std::shared_ptr<Client> m_client;
    std::shared_ptr<Message> m_message;

};

// Komenda do odbierania listy peer'ów od innego peer'a
class ClientListCommand : public Command {

public:
    virtual void Execute() override;

};

// Komenda do wysyłania zapytań o plik
class FileRequestCommand : public Command {

public:
    FileRequestCommand(std::string fileName, std::string ip, int port) : m_fileName(fileName), m_ip(ip), m_port(port) {}
    virtual void Execute() override;

private:
    std::string m_fileName;
    std::string m_ip;
    int m_port;

};

// Komenda do wysyłania pliku do innego peer'a
class FileSendCommand : public Command {
    
public:
    FileSendCommand(std::shared_ptr<Client> client, std::shared_ptr<Message> message) : m_client(client), m_message(message) {}
    virtual void Execute() override;

private:
    std::shared_ptr<Client> m_client;
    std::shared_ptr<Message> m_message;

};

// Komenda do odbierania pliku od innego peer'a
class FileReceiveCommand : public Command {

public:
    FileReceiveCommand(std::shared_ptr<Client> client, std::shared_ptr<Message> message) : m_client(client), m_message(message) {}
    virtual void Execute() override;

private:
    std::shared_ptr<Client> m_client;
    std::shared_ptr<Message> m_message;

};
