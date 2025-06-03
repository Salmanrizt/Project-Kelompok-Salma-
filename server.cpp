#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Fungsi cek apakah ID ada di studentdatabase.csv
bool checkStudentInDatabase(const std::string& id) {
    std::ifstream file("studentdatabase.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open studentdatabase.csv\n";
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string studentId;
        if (std::getline(ss, studentId, ',')) {
            if (studentId == id) {
                file.close();
                return true;
            }
        }
    }
    file.close();
    return false;
}
// Fungsi untuk timestamp dalam format YYYY-MM-DD HH:MM:SS
std::string getCurrentTimestamp() {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t now_time = system_clock::to_time_t(now);
    std::tm* ptm = std::localtime(&now_time);

    char buffer[20];
    std::strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", ptm);
    return std::string(buffer);
}

// Update attendance.csv dengan ID dan timestamp, lalu sorting berdasarkan timestamp ascending
void updateAttendance(const std::string& id) {
    // Tambah record baru
    std::ofstream outfile("attendance.csv", std::ios::app);
    if (!outfile.is_open()) {
        std::cerr << "Failed to open attendance.csv\n";
        return;
    }
    std::string timestamp = getCurrentTimestamp();
    outfile << id << "," << timestamp << "\n";
    outfile.close();

    // Baca ulang seluruh attendance
    std::ifstream infile("attendance.csv");
    if (!infile.is_open()) {
        std::cerr << "Failed to open attendance.csv\n";
        return;
    }
    std::vector<std::pair<std::string, std::string>> records;
    std::string line;
    while (std::getline(infile, line)) {
        std::istringstream ss(line);
        std::string studentId, time;
        if (std::getline(ss, studentId, ',') && std::getline(ss, time)) {
            records.emplace_back(studentId, time);
        }
    }
    infile.close();

    // Sort timestamp ascending
    std::sort(records.begin(), records.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    // Tambah ke attendance.csv
    std::ofstream outfile2("attendance.csv");
    if (!outfile2.is_open()) {
        std::cerr << "Failed to open attendance.csv for writing\n";
        return;
    }
    for (const auto& rec : records) {
        outfile2 << rec.first << "," << rec.second << "\n";
    }
    outfile2.close();
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // buat socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // set reuse address
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind socket ke port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen koneksi masuk
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server running on port " << PORT << std::endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        memset(buffer, 0, BUFFER_SIZE);
        ssize_t valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread > 0) {
            std::string id(buffer);

            if (checkStudentInDatabase(id)) {
                updateAttendance(id);
                std::string msg = "Attendance recorded for ID: " + id + "\n";
                send(new_socket, msg.c_str(), msg.size(), 0);
            } else {
                std::string msg = "Error: Student ID not found\n";
                send(new_socket, msg.c_str(), msg.size(), 0);
            }
        }
        close(new_socket);
    }

    return 0;
}
