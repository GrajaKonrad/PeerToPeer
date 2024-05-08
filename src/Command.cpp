#include "../include/Application.h"
#include "../include/Command.h"
#include "../include/Log.h"

#include <iostream>
#include <fstream>
#include <dirent.h>
#include <string.h>
#include <unistd.h>


void ConnectCommand::Execute() {
    Application* app = Application::GetInstance();

    for (auto client : app->GetClients()) {
        if (client->GetIp() == m_ip && client->GetServerPort() == m_port) {
            return;
        }
    }

    std::shared_ptr<Client> client = std::make_shared<Client>(m_ip, m_port);
    if (!client->IsConnected()) {
        Log::Red("Failed to connect to " + m_ip + ":" + std::to_string(m_port));
        return;
    }

    client->SetServerPort(m_port);

    app->AddClient(client);
    app->AddCommand(std::make_shared<ServerPortSendCommand>(client));
    app->AddCommand(std::make_shared<ClientListSendCommand>(client));
    Log::Blue("Connected to " + m_ip + ":" + std::to_string(m_port));
}

void ClientAddCommand::Execute() {
    Application* app = Application::GetInstance();
    app->AddClient(m_client);
}

void ServerPortSendCommand::Execute() {
    Application* app = Application::GetInstance();
    std::string port = std::to_string(app->GetPort());
    std::shared_ptr<Message> message = std::make_shared<Message>(0, 1, (uint32_t) MessageType::serverPort, port);
    m_client->AddMessage(message);
}

void ServerPortReceiveCommand::Execute() {
    Application* app = Application::GetInstance();
    char* data = m_message->GetData();

    std::string port;
    for (int i = 0; i < m_message->GetDataLength(); i++) {
        port += data[i];
    }
    m_client->SetServerPort(std::stoi(port));
    app->AddCommand(std::make_shared<ClientListSendCommand>(m_client));
}

void ClientListSendCommand::Execute() {
    Application* app = Application::GetInstance();
    std::vector<std::shared_ptr<Client>> clients = app->GetClients();

    std::vector<std::string> dataList;
    std::string data = "";

    for (auto client : clients) {
        if (client == m_client) {
            continue;
        }

        std::string ip = client->GetIp();
        std::string port = std::to_string(client->GetServerPort());

        if (data.length() + ip.length() + port.length() + 2 > Message::MaxDataLength) {
            dataList.push_back(data);
            data = "";
        }

        data += ip + ":" + port + "/";
    }

    if (data.length() > 0) {
        dataList.push_back(data);
    }
    
    for (int i = 0; i < dataList.size(); i++) {
        std::shared_ptr<Message> message = std::make_shared<Message>(i, dataList.size(), (uint32_t) MessageType::clientList, dataList[i]);
        m_client->AddMessage(message);
    }
}

void ClientListReceiveCommand::Execute() {
    Application* app = Application::GetInstance();
    char* data = m_message->GetData();

    std::string ip = "";
    std::string port = "";
    bool colon = false;

    for (int i = 0; i < m_message->GetDataLength(); i++) {
        if (data[i] == ':') {
            colon = true;
        }
        else if (data[i] == '/') {
            app->AddCommand(std::make_shared<ConnectCommand>(ip, std::stoi(port)));

            ip = "";
            port = "";
            colon = false;
        }
        else {
            if (colon) {
                port += data[i];
            }
            else {
                ip += data[i];
            }
        }
    }
}

void FileListRequestCommand::Execute() {
    Application* app = Application::GetInstance();
    std::vector<std::shared_ptr<Client>> clients = app->GetClients();

    for (auto client : clients) {
        std::shared_ptr<Message> message = std::make_shared<Message>(0, 1, (uint32_t) MessageType::fileListRequest, "");
        client->AddMessage(message);
    }
}

void FileListSendCommand::Execute() {
    Application* app = Application::GetInstance();

    std::vector<std::string> files;
    std::string part = "";
    DIR* dir;
    struct dirent* ent;

    if ((dir = opendir(app->GetDirectory().c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                std::string fileName = ent->d_name;

                if (fileName.length() + 1 > Message::MaxDataLength) {
                    continue;
                }

                if (part.length() + fileName.length() + 1 > Message::MaxDataLength) {
                    files.push_back(part);
                    part = "";
                }

                part += fileName;
                part += "/";
            }
        }
        closedir(dir);
    }

    if (part.length() > 0) {
        files.push_back(part);
    }

    for (int i = 0; i < files.size(); i++) {
        std::shared_ptr<Message> message = std::make_shared<Message>(i, files.size(), (uint32_t) MessageType::fileListResponse, files[i]);
        m_client->AddMessage(message);
    }
}

void FileListReceiveCommand::Execute() {
    Application* app = Application::GetInstance();
    char* data = m_message->GetData();

    std::vector<std::string> files;
    std::string file = "";
    for (int i = 0; i < m_message->GetDataLength(); i++) {
        if (data[i] == '/') {
            files.push_back(file);
            file = "";
        } else {
            file += data[i];
        }
    }

    for (auto file : files) {
        Log::Blue(file + " (" + m_client->GetIp() + ":" + std::to_string(m_client->GetServerPort()) + ")");
    }
}

void FileRequestCommand::Execute() {
    Application* app = Application::GetInstance();

    for (auto client : app->GetClients()) {
        if (client->GetIp() == m_ip && client->GetServerPort() == m_port) {
            std::shared_ptr<Message> message = std::make_shared<Message>(0, 1, (uint32_t) MessageType::fileRequest, m_fileName);
            client->AddMessage(message);
            return;
        }
    }

    std::cout << Log::ColorRed << "Client " << m_ip << ":" << m_port << " not found" << Log::ResetColor << std::endl;
}

void FileSendCommand::Execute() {
    Application* app = Application::GetInstance();
    char* data = m_message->GetData();
    std::string fileName;
    for (int i = 0; i < m_message->GetDataLength(); i++) {
        fileName += data[i];
    }

    std::ifstream file(app->GetDirectory() + '/' + fileName, std::ios::binary);
    if (!file.is_open()) {
        std::shared_ptr<Message> message = std::make_shared<Message>(0, 1, (uint32_t) MessageType::error, "Could not open file \"" + fileName + "\".");
        m_client->AddMessage(message);
        return;
    }

    uint32_t maxDataSize = Message::MaxDataLength - fileName.length() - 1;
    
    file.seekg(0, std::ios::end);
    int fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    int messageCount = (fileSize - 1) / maxDataSize + 1;
    
    for (int i = 0; i < messageCount; i++) {
        uint32_t dataSize = i == messageCount - 1 ? fileSize % maxDataSize : maxDataSize;

        char* fileData = new char[dataSize];
        file.read(fileData, dataSize);

        uint32_t messageSize = dataSize + fileName.length() + 1;
        char* messageData = new char[dataSize + fileName.length() + 1];
        memcpy(messageData, fileName.c_str(), fileName.length());
        messageData[fileName.length()] = '/';
        memcpy(messageData + fileName.length() + 1, fileData, dataSize);

        std::shared_ptr<Message> message = std::make_shared<Message>(i, messageCount, (uint32_t) MessageType::fileResponse, messageData, messageSize);
        m_client->AddMessage(message);

        delete[] fileData;
    }
}

void FileReceiveCommand::Execute() {
    Application* app = Application::GetInstance();

    std::string fileName = "";
    char* data = m_message->GetData();

    int i = 0;
    while (data[i] != '/' || i >= m_message->GetDataLength() - 1) {
        fileName += data[i];
        i++;
    }
    i++;

    std::ofstream file(app->GetDirectory() + '/' + fileName, m_message->GetSegmentIndex() == 0 ? std::ios::binary : (std::ios::binary | std::ios::app));
    if (!file.is_open()) {
        Log::Red("Could not open file \"" + fileName + "\".");
        return;
    }

    file.write(data + i, m_message->GetDataLength() - fileName.length()- 1);
    file.close();

    Log::Blue("Received file \"" + fileName + "\" from " + m_client->GetIp() + ":" + std::to_string(m_client->GetServerPort()) + " (" + std::to_string(m_message->GetSegmentIndex() + 1) + "/" + std::to_string(m_message->GetSegmentTotal()) + ")");
}

void ClientListCommand::Execute() {
    Application* app = Application::GetInstance();
    std::vector<std::shared_ptr<Client>> clients = app->GetClients();

    Log::Blue("\tClients:");
    for (auto client : clients) {
        Log::Blue(client->GetIp() + ":" + std::to_string(client->GetServerPort()));
    }
}
