#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <QThread>
#include <SFML/Network.hpp>

#include "WorkerReceiver.h"

class ThreadManager : public QObject
{
    Q_OBJECT

public:
    ThreadManager(sf::TcpSocket *socket);
    void startWorker();
    ~ThreadManager();

public slots:
    void emitWorkerD(QString);

signals:
    void workerRData(QString);

private:
    WorkerReceiver *m_worker;
    QThread *m_threadWorker;
};

#endif // THREADMANAGER_H
