#include "../include/Application.h"
#include "../include/Server.h"
#include "../include/Client.h"
#include "../include/Log.h"

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <dirent.h>

Application* Application::m_instance = NULL;

Application* Application::GetInstance() {
    if (m_instance == NULL) {
        m_instance = new Application();
    }
    return m_instance;
}

Application::Application() {
    m_clients = std::vector<std::shared_ptr<Client>>();
    m_commandQueue = std::queue<std::shared_ptr<Command>>();
}

Application::~Application() {
    m_clients.clear();
}

void Application::Run() {
    m_running = true;

    Welcome();

    std::cout << Log::ColorBlue << "Enter port: " << Log::ResetColor;
    std::cin >> m_port;

    std::cout << Log::ColorBlue << "Enter directory: " << Log::ResetColor;
    std::cin >> m_directory;

    pthread_t serverThread;
    pthread_t inputThread;
    pthread_create(&serverThread, NULL, &Application::ServerThread, NULL);
    pthread_create(&inputThread, NULL, &Application::InputThread, NULL);

    while (m_running) {
        while (!m_commandQueue.empty()) {
            auto command = m_commandQueue.front();
            m_commandQueue.pop();
            command->Execute();
        }

        for (auto client : m_clients) {
            std::shared_ptr<Message> message = client->Receive();
            if (message.get() == NULL) {
                continue;
            }

            if (message->GetType() == (uint32_t) MessageType::serverPort) {
                AddCommand(std::make_shared<ServerPortReceiveCommand>(client, message));
            }
            if (message->GetType() == (uint32_t) MessageType::clientList) {
                AddCommand(std::make_shared<ClientListReceiveCommand>(client, message));
            }
            if (message->GetType() == (uint32_t) MessageType::fileListRequest) {
                AddCommand(std::make_shared<FileListSendCommand>(client));
            }
            if (message->GetType() == (uint32_t) MessageType::fileListResponse) {
                AddCommand(std::make_shared<FileListReceiveCommand>(client, message));
            }
            if (message->GetType() == (uint32_t) MessageType::fileRequest) {
                AddCommand(std::make_shared<FileSendCommand>(client, message));
            }
            if (message->GetType() == (uint32_t) MessageType::fileResponse) {
                AddCommand(std::make_shared<FileReceiveCommand>(client, message));
            }
            if (message->GetType() == (uint32_t) MessageType::error) {
                char* data = message->GetData();
                std::cout << Log::ColorRed << "Error from " << client->GetIp() << ":" << client->GetServerPort() << " - ";
                for (int i = 0; i < message->GetDataLength(); i++) {
                    std::cout << data[i];
                }
                std::cout << Log::ResetColor << std::endl;
            }
        }

        for (auto client : m_clients) {
            client->Send();
        }

        for (std::vector<std::shared_ptr<Client>>::iterator it = m_clients.begin(); it < m_clients.end(); ) {
            if (!(*it)->IsConnected()) {
                std::cout << Log::ColorYellow << "Client " << (*it)->GetIp() << ":" << (*it)->GetServerPort() << " disconnected" << Log::ResetColor << std::endl;
                it = m_clients.erase(it);
            }
            else {
                it++;
            }
        }

    }

    pthread_cancel(serverThread);
    pthread_cancel(inputThread);
}

void* Application::ServerThread(void* arg) {
    Application* app = Application::GetInstance();

    std::cout << Log::ColorBlue << "Server started on port " << app->m_port << Log::ResetColor << std::endl;
    app->m_server = std::make_shared<Server>(app->m_port);

    if (!app->m_server->IsRunning()) {
        app->m_running = false;
        return NULL;
    }

    while (true) {
        std::shared_ptr<Client> client = app->m_server->Accept();
        app->m_commandQueue.push(std::make_shared<ClientAddCommand>(client));
        app->m_commandQueue.push(std::make_shared<ClientListSendCommand>(client));
    }

    return NULL;
}

void* Application::InputThread(void* arg) {
    Application* app = Application::GetInstance();

    while (true) {
        std::string command;
        std::cin >> command;

        /// Wyświetla pomoc
        if (command == "help") {
            app->Help();
        }

        /// Łączy się z peerem
        if (command == "connect") {
            app->Connect();
        }

        /// Pobiera plik od klienta
        if (command == "get") {
            app->DownloadFile();
        }

        /// Wyświetla pliki zdalne
        if (command == "files") {
            app->ListRemoteFiles();
        }

        /// Wyświetla pliki lokalne
        if (command == "local") {
            app->ListLocalFiles();
        }

        /// Wyświetla klientów
        if (command == "clients") {
            app->ListClients();
        }

        /// Wyświetla status aplikacji
        if (command == "debug") {
            app->Debug();
        }

        /// Kończy dzialanie aplikacji
        if (command == "exit") {
            app->m_running = false;
        }
    }

    return NULL;
}

void Application::Welcome() {
    Log::Green("Welcome to P2P File Sharing Application");
    Log::Blue("Type 'help' to see available commands");
}

void Application::Help() {
    Log::Blue("\tAvailable commands:");
    Log::Blue("connect - connect to peer");
    Log::Blue("files - list remote files");
    Log::Blue("local - list local files");
    Log::Blue("get - get file from peer");
    Log::Blue("clients - list connected peers");
    Log::Blue("debug - show application status");
    Log::Blue("exit - exit application");
}

void Application::Connect() {
    std::string ip;
    int port;
    
    std::cout << Log::ColorBlue << "Enter ip: " << Log::ResetColor;
    std::cin >> ip;

    std::cout << Log::ColorBlue << "Enter port: " << Log::ResetColor;
    std::cin >> port;

    m_commandQueue.push(std::make_shared<ConnectCommand>(ip, port));
}

void Application::ListRemoteFiles() {
    m_commandQueue.push(std::make_shared<FileListRequestCommand>());
}

void Application::ListLocalFiles() {
    Log::Blue("\tFiles in directory: " + m_directory);
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(m_directory.c_str())) == NULL) {
        Log::Red("Could not open directory");
        return;
    }
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {
            Log::Blue(ent->d_name);
        }
    }
    closedir(dir);
}

void Application::DownloadFile() {
    std::string file;
    std::string ip;
    int port;

    std::cout << Log::ColorBlue << "Enter file name: " << Log::ResetColor;
    std::cin >> file;

    std::cout << Log::ColorBlue << "Enter ip: " << Log::ResetColor;
    std::cin >> ip;

    std::cout << Log::ColorBlue << "Enter port: " << Log::ResetColor;
    std::cin >> port;

    m_commandQueue.push(std::make_shared<FileRequestCommand>(file, ip, port));
}

void Application::ListClients() {
    m_commandQueue.push(std::make_shared<ClientListCommand>());
}

void Application::Debug() {
    Log::Blue("\tDebug info");
    Log::Blue("Port: " + std::to_string(m_port));
    Log::Blue("Directory: " + m_directory);
    Log::Blue("Clients: " + std::to_string(m_clients.size()));
    for (auto client : m_clients) {
        Log::Blue("\t" + client->GetIp() + ":" + std::to_string(client->GetPort()) + "\t" + std::to_string(client->GetServerPort()));
    }
    Log::Blue("Command queue size: " + std::to_string(m_commandQueue.size()));
}