#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <cstring>
#include <thread>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "server.hpp"

#define PORT 8080
#define BUFFER_SIZE 1024

std::atomic<bool> serverRunning(false);

std::string getStudentNameById(const std::string& id) {
    std::ifstream file("studentdatabase.csv");
    if (!file.is_open()) return "";

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string studentId, name;
        if (std::getline(ss, studentId, ',') && std::getline(ss, name)) {
            if (studentId == id) {
                return name;
            }
        }
    }
    return "";
}

std::string getCurrentTimestamp() {
    using namespace std::chrono;
    auto now = system_clock::now() + hours(7);
    std::time_t now_time = system_clock::to_time_t(now);
    std::tm* ptm = std::localtime(&now_time);
    char buffer[20];
    std::strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", ptm);
    return std::string(buffer);
}

void updateAttendance(const std::string& id, const std::string& name) {
    std::string timestamp = getCurrentTimestamp();

    std::ofstream outfile("attendance.csv", std::ios::app);
    if (!outfile.is_open()) return;
    outfile << id << "," << name << "," << timestamp << "\n";
    outfile.close();

    std::ifstream infile("attendance.csv");
    if (!infile.is_open()) return;
    std::vector<std::tuple<std::string, std::string, std::string>> records;
    std::string line;
    while (std::getline(infile, line)) {
        std::istringstream ss(line);
        std::string sid, sname, time;
        if (std::getline(ss, sid, ',') && std::getline(ss, sname, ',') && std::getline(ss, time)) {
            records.emplace_back(sid, sname, time);
        }
    }
    infile.close();

    std::sort(records.begin(), records.end(), [](const auto& a, const auto& b) {
        return std::get<2>(a) < std::get<2>(b);
    });

    std::ofstream outfile2("attendance.csv");
    for (const auto& rec : records) {
        outfile2 << std::get<0>(rec) << "," << std::get<1>(rec) << "," << std::get<2>(rec) << "\n";
    }
    outfile2.close();
}

void serverLoop() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Failed to create socket.\n";
        return;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "setsockopt failed.\n";
        close(server_fd);
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed.\n";
        close(server_fd);
        return;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed.\n";
        close(server_fd);
        return;
    }

    std::cout << "=== Server is running on port " << PORT << " ===\n";

    while (serverRunning) {
        std::cout << "[INFO] Waiting for client connection...\n";
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            if (serverRunning) {
                std::cerr << "Accept failed.\n";
            }
            continue;
        }

        std::cout << "[INFO] Client connected.\n";

        memset(buffer, 0, BUFFER_SIZE);
        ssize_t valread = read(new_socket, buffer, BUFFER_SIZE - 1);
        if (valread < 0) {
            std::cerr << "Read error\n";
            close(new_socket);
            continue;
        }
        buffer[valread] = '\0';

        std::string id(buffer);
        id.erase(std::remove(id.begin(), id.end(), '\n'), id.end());

        std::cout << "[REQUEST] Received ID: " << id << "\n";

        std::string name = getStudentNameById(id);
        if (!name.empty()) {
            updateAttendance(id, name);
            std::string msg = "Attendance recorded for ID: " + id + ", Name: " + name + "\n";
            send(new_socket, msg.c_str(), msg.size(), 0);
            std::cout << "[SUCCESS] Attendance recorded for " << id << " - " << name << "\n";
        } else {
            std::string msg = "Error: Student ID not found\n";
            send(new_socket, msg.c_str(), msg.size(), 0);
            std::cout << "[ERROR] Student ID not found: " << id << "\n";
        }

        close(new_socket);
        std::cout << "[INFO] Connection closed.\n";
    }

    close(server_fd);
    std::cout << "[INFO] Server stopped.\n";
}

void showStudentDatabase() {
    std::ifstream file("studentdatabase.csv");
    std::string line;
    std::cout << "\n--- Student Database ---\n";
    while (std::getline(file, line)) {
        std::cout << line << "\n";
    }
    file.close();
}

void showAttendance() {
    std::ifstream file("attendance.csv");
    std::string line;
    std::cout << "\n--- Attendance Log ---\n";
    while (std::getline(file, line)) {
        std::cout << line << "\n";
    }
    file.close();
}

int main() {
    std::thread serverThread;
    int choice;

    do {
        std::cout << "\n===== SERVER MENU =====\n";
        std::cout << "1. Run Server\n";
        std::cout << "2. Stop Server\n";
        std::cout << "3. Show Student Database\n";
        std::cout << "4. Show Attendance Log\n";
        std::cout << "0. Exit\n";
        std::cout << "Choose: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                if (!serverRunning) {
                    serverRunning = true;
                    serverThread = std::thread(serverLoop);
                } else {
                    std::cout << "[INFO] Server is already running.\n";
                }
                break;
            case 2:
                if (serverRunning) {
                    serverRunning = false;
                    // Create dummy connection to unblock accept()
                    int sock = socket(AF_INET, SOCK_STREAM, 0);
                    sockaddr_in serv_addr{};
                    serv_addr.sin_family = AF_INET;
                    serv_addr.sin_port = htons(PORT);
                    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
                    close(sock);
                    serverThread.join();
                } else {
                    std::cout << "[INFO] Server is not running.\n";
                }
                break;
            case 3:
                showStudentDatabase();
                break;
            case 4:
                showAttendance();
                break;
            case 0:
                if (serverRunning) {
                    std::cout << "[INFO] Stopping server before exiting...\n";
                    serverRunning = false;
                    int sock = socket(AF_INET, SOCK_STREAM, 0);
                    sockaddr_in serv_addr{};
                    serv_addr.sin_family = AF_INET;
                    serv_addr.sin_port = htons(PORT);
                    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
                    close(sock);
                    serverThread.join();
                }
                std::cout << "Exiting...\n";
                break;
            default:
                std::cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}
