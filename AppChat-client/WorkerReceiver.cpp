#include "WorkerReceiver.h"

WorkerReceiver::WorkerReceiver(sf::TcpSocket *socket):
    m_dataR(""), m_socket(socket)
{
}

void WorkerReceiver::receiveData(){
    while (true){
        sf::Packet receive;
        std::string dataR;
        receive.clear();
        m_socket->receive(receive);
        receive >> dataR;
        m_dataR = QString::fromUtf8(dataR.c_str());
        emit datareceived(m_dataR);
    }

}
