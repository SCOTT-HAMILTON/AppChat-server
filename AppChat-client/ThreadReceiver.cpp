#include "ThreadReceiver.h"

ThreadReceiver::ThreadReceiver(sf::TcpSocket *socket):
    m_socket(socket)
{
}

void ThreadReceiver::receiveData(){
    sf::Packet receive;
    std::string dataR;
    std::cout << "cout in client Worker receiver !" << std::endl;
    receive.clear();
    m_socket->receive(receive);
    receive >> dataR;
    m_dataR = QString::fromUtf8(dataR.c_str());
    std::cout << "data received ! : " << dataR << std::endl;
    emit datareceived();
}

QString ThreadReceiver::getdataR(){
    return m_dataR;
}


