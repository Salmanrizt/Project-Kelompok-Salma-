#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>

std::string getStudentNameById(const std::string& id);
std::string getCurrentTimestamp();
void updateAttendance(const std::string& id, const std::string& name);
void runServer();
void showStudentDatabase();
void showAttendance();

#endif
