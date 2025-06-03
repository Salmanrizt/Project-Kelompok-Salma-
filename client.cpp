Coding Client.cpp 

#include "client.hpp"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

void sendIDToServer(const std::string& id) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return;
    }

    send(sock, id.c_str(), id.size(), 0);

    ssize_t valread = read(sock, buffer, BUFFER_SIZE);
    if (valread > 0) {
        std::cout << "Server response: " << buffer << std::endl;
    }
    close(sock);
}

int main() {
    std::string id;
    std::cout << "Enter Student ID (RFID): ";
    std::getline(std::cin, id);

    sendIDToServer(id);

    return 0;
}




