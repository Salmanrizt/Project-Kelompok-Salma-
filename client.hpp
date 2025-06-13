#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

std::string getNPMFromCard(const std::string& cardFileName);
void sendIDToServer(const std::string& id);

#endif


