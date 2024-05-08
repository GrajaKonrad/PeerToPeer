#pragma once

#include "../include/Message.h"

#include <string>
#include <queue>
#include <memory>

/// @brief  Klasa reprezentująca klienta
class Client {

public:
    /// @brief Konstruktor
    /// @param ip ip
    /// @param port port
    Client(std::string ip, int port);

    /// @brief Konsruktor
    /// @param ip ip
    /// @param port port
    /// @param socket gniazdo
    Client(std::string ip, int port, int socket);

    /// @brief Destruktor
    ~Client();


    /// @brief Sprawdza czy można odbierać
    /// @return `true` jeśli można odbierać, `false` w przeciwnym wypadku
    bool CanRecive();

    /// @brief Sprawdza czy można wysyłać
    /// @return `true` jeśli można wysyłać, `false` w przeciwnym wypadku
    bool CanSend();

    /// @brief Sprawdza czy wystąpił błąd
    /// @return `true` jeśli wystąpił błąd, `false` w przeciwnym wypadku
    bool HasError();


    /// @brief Dodaje wiadomość do kolejki
    /// @param message wiadomość
    inline void AddMessage(std::shared_ptr<Message> message) { m_messageQueue.push(message); }

    /// @brief Odbiera wiadomość
    /// @return wiadomość
    std::shared_ptr<Message> Receive();

    /// @brief Wysyła pierwszą wiadomość z kolejki
    void Send();
    
    // TODO: remove
    // void SendSegment(MessageType type, std::string data, uint32_t segment, uint32_t segmentCount);
    // void SendBytes(MessageType type, char* data, uint32_t size, uint32_t segment, uint32_t segmentCount);
    // void SendAll(MessageType type, std::string data);
    // Message Receive();

    /// @brief Zwraca informację czy jest połączony
    /// @return `true` jeśli jest połączony, `false` w przeciwnym wypadku
    inline bool IsConnected() const { return m_connected; }

    /// @brief Zwraca adres ip
    /// @return adres ip
    inline const std::string& GetIp() const { return m_ip; }
    
    /// @brief Zwraca port
    /// @return port
    inline int GetPort() const { return m_port; }

    /// @brief Zwraca port serwera
    /// @return port serwera
    inline int GetServerPort() const { return m_serverPort; }

    /// @brief Ustawia port serwera
    /// @param port port serwera
    inline void SetServerPort(int port) { m_serverPort = port; }

private:
    /// @brief Czy jest połączony
    bool m_connected;

    /// @brief Adres ip
    std::string m_ip;

    /// @brief Port
    int m_port;

    /// @brief Port serwera
    int m_serverPort;

    /// @brief Gniazdo
    int m_socket;

    /// @brief Czy można wysyłać
    bool m_canSend;

    /// @brief Kolejka wiadomości
    std::queue<std::shared_ptr<Message>> m_messageQueue;

};
