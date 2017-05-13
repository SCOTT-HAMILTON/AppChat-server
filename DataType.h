#ifndef DATATYPE_H
#define DATATYPE_H

#include <string>
#include <vector>

class DataType
{
public:
    static std::string create_client;
    static std::string connection_client;
    static std::string invalid_connection;
    static std::string valid_connection;
    static std::string code;
    static std::string message;
    static std::string pseudoUnavailable;
    static std::string pseudoVailable;
    static std::string initMessagesTxt;
    static std::string getInitMessages;
    static std::string new_client;

    static bool isType(std::string value);

private:
    static std::vector<std::string> m_types;
};

#endif // DATATYPE_H
