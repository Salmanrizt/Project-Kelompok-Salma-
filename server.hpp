#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>

bool checkStudentInDatabase(const std::string& id);
void updateAttendance(const std::string& id);
std::string getCurrentTimestamp();

#endif
