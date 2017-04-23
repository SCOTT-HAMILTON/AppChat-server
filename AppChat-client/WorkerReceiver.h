#ifndef WORKERRECEIVER
#define WORKERRECEIVER

#include <QObject>
#include <QThread>
#include <SFML/Network.hpp>
#include <QString>

class WorkerReceiver : public QObject
{
    Q_OBJECT
public:
    WorkerReceiver(sf::TcpSocket *socket);

signals:
    void datareceived(QString);

public slots:
    void receiveData();

private:

    QString m_dataR;
    sf::TcpSocket *m_socket;

};

#endif // THREADRECEIVER_H
