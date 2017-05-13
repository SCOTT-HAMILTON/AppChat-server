#include "DataType.h"

std::string DataType::create_client = "This is my client pseudo and code : ";
std::string DataType::connection_client = "I'm trying to connect to server";
std::string DataType::invalid_connection = "Sorry but this is not a valid code";
std::string DataType::valid_connection = "You've been succefully accepted at the server !";
std::string DataType::code = "This is the code : ";
std::string DataType::message = "This is a message : ";
std::string DataType::pseudoUnavailable = "Sorry, This pseudo is already used.";
std::string DataType::pseudoVailable = "You're pseudo has bein register succefully.";
std::string DataType::initMessagesTxt = "Here is the recent messages send to the server";
std::string DataType::getInitMessages = "Can I get the last messages send to server please";
std::string DataType::new_client = "I'm new at the network, can I be registered ?";
std::vector<std::string> DataType::m_types = std::vector<std::string>{ create_client, connection_client, invalid_connection, valid_connection, code, message, pseudoUnavailable, pseudoVailable, initMessagesTxt, getInitMessages, new_client };

bool DataType::isType(std::string value){
    unsigned int i = 0;
    for (i = 0; i < value.size(); i++){
        if (m_types[i] == value)return true;
    }
    return false;
}
