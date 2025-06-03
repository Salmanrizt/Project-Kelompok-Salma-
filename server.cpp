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
