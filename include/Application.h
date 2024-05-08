#pragma once

#include "Client.h"
#include "Server.h"
#include "Command.h"

#include <memory>
#include <vector>
#include <queue>

/// @brief Klasa reprezentująca aplikację
class Application {

public:
    /// @brief Zwraca instancję aplikacji
    /// @return instancja aplikacji
    static Application* GetInstance();

    /// @brief Destruktor
    ~Application();

    /// @brief Uruchamia aplikację
    void Run();

    /// @brief Dodaje klienta do listy klientów
    inline void AddClient(std::shared_ptr<Client> client) { m_clients.push_back(client); }

    /// @brief Dodaje polecenie do kolejki
    inline void AddCommand(std::shared_ptr<Command> command) { m_commandQueue.push(command); }

    /// @brief Zwraca port serwera
    inline int GetPort() const { return m_port; }

    /// @brief Zwraca folder z plikami
    /// @return folder z plikami
    inline const std::string& GetDirectory() const { return m_directory; }
    
    /// @brief Zwraca listę klientów
    /// @return lista klientów
    inline std::vector<std::shared_ptr<Client>> GetClients() const { return m_clients; }

private:
    /// @brief Konstruktor prywatny, aby zapobiec tworzeniu instancji
    Application();

    /// @brief Funkcja wątku serwera
    /// @param arg argumenty
    /// @return zwraca NULL
    static void* ServerThread(void* arg);

    /// @brief Funkcja wątku wejścia z klawiatury
    /// @param arg argumenty
    /// @return zwraca NULL
    static void* InputThread(void* arg);


    /// @brief Wyświetla powitanie
    void Welcome();

    /// @brief Wyświetla listę dostępnych poleceń
    void Help();

    /// @brief Wyświetla menu połączenia z innym peerem
    void Connect();

    /// @brief Wyświetla listę zdalnych plików
    void ListRemoteFiles();

    /// @brief Wyświetla listę lokalnych plików
    void ListLocalFiles();

    /// @brief Wyświetla menu pobierania pliku
    void DownloadFile();

    /// @brief Wyświetla listę klientów
    void ListClients();

    /// @brief Wyświetla stan aplikacji
    void Debug();

private:
    /// @brief Statyczna instancja aplikacji
    static Application* m_instance;

    /// @brief Folder z plikami
    std::string m_directory;

    /// @brief Port serwera
    int m_port;

    /// @brief Serwer
    std::shared_ptr<Server> m_server;

    /// @brief Lista klientów
    std::vector<std::shared_ptr<Client>> m_clients;

    /// @brief Koljka poleceń
    std::queue<std::shared_ptr<Command>> m_commandQueue;

    /// @brief Flaga uruchomienia aplikacji
    bool m_running;

};