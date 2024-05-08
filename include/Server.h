#pragma once

#include "Client.h"

#include <memory>
#include <string>

/// @brief Klasa reprezentująca serwer
class Server {

public:
    /// @brief Konstruktor
    /// @param ip ip
    /// @param port port
    Server(int port);

    /// @brief Destruktor
    ~Server();


    /// @brief Akceptuje połączenie
    /// @return nowy klient
    std::shared_ptr<Client> Accept();
    

    /// @brief Zwraca port
    /// @return port
    inline int GetPort() const { return m_port; }

    /// @brief Sprawdza czy serwer działa
    /// @return zwraca 'true' jeżeli serwer, 'false' w przeciwnym przypadku
    inline int IsRunning() const { return m_running; }

private:
    /// @brief Port
    int m_port;

    /// @brief Gniazdo
    int m_socket;

    /// @brief Czy serwer działa
    bool m_running;

};
