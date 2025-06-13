#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "client.hpp"


#define PORT 8080
#define BUFFER_SIZE 1024

// Fungsi baca NPM dari file kartu seperti student1.csv, student2.csv, dst
std::string getNPMFromCard(const std::string& cardFileName) {
    std::ifstream file(cardFileName);
    if (!file.is_open()) return "";

    std::string line;
    if (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string npm;
        if (std::getline(ss, npm, ',')) {
            npm.erase(std::remove_if(npm.begin(), npm.end(), ::isspace), npm.end());
            return npm;
        }
    }
    return "";
}

int main() {
    int choice;
    std::string idToSend;

    std::cout << "=== CLIENT MENU ===\n";
    std::cout << "1. Use ID Number\n";
    std::cout << "2. Use Card ID\n";
    std::cout << "Choose: ";
    std::cin >> choice;
    std::cin.ignore();

    if (choice == 1) {
        std::cout << "Enter ID number: ";
        std::getline(std::cin, idToSend);
    } else if (choice == 2) {
        std::string cardId;
        std::cout << "Enter card ID (e.g. student1.csv): ";
        std::getline(std::cin, cardId);

        idToSend = getNPMFromCard(cardId);
        if (idToSend.empty()) {
            std::cerr << "Card ID not found or file empty.\n";
            return 1;
        }
    } else {
        std::cerr << "Invalid choice.\n";
        return 1;
    }

    // Setup socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation error\n";
        return 1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported\n";
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed\n";
        close(sock);
        return 1;
    }

    // Kirim ID ke server
    send(sock, idToSend.c_str(), idToSend.size(), 0);

    // Terima balasan dari server
    char buffer[BUFFER_SIZE] = {0};
    ssize_t valread = read(sock, buffer, BUFFER_SIZE - 1);
    if (valread > 0) {
        buffer[valread] = '\0';
        std::cout << "Server response: " << buffer;
    } else {
        std::cerr << "No response from server.\n";
    }

    close(sock);
    return 0;
}
