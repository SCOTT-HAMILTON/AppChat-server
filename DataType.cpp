#include "DataType.h"

std::string DataType::pseudo = "This is my pseudo : ";
std::string DataType::message = "This is a message : ";
std::string DataType::pseudoUnavailable = "Sorry, This pseudo is already use.";
std::string DataType::pseudoVailable = "You're pseudo has bein register succefully.";
std::string DataType::initMessagesTxt = "Here is the recent messages send to the server";
std::string DataType::getInitMessages = "Can I get the last messages send to server please";
std::vector<std::string> DataType::m_types = std::vector<std::string>({pseudo, message, pseudoUnavailable, pseudoVailable, initMessagesTxt, getInitMessages});


bool DataType::isType(std::string value){
    unsigned int i = 0;
    for (i = 0; i < value.size(); i++){
        if (m_types[i] == value)return true;
    }
    return false;
}
