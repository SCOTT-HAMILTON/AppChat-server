#include "ThreadManager.h"

ThreadManager::ThreadManager(sf::TcpSocket *socket)
{
    m_worker = new WorkerReceiver(socket);
    m_threadWorker = new QThread;
    m_worker->moveToThread(m_threadWorker);
    connect(m_threadWorker, SIGNAL(finished()), m_worker, SLOT(deleteLater()));
    connect(m_worker, SIGNAL(datareceived(QString)), this, SLOT(emitWorkerD(QString)));
    connect(m_threadWorker, SIGNAL(started()), m_worker, SLOT(receiveData()));
}

void ThreadManager::startWorker(){
    m_threadWorker->start();
}

void ThreadManager::emitWorkerD(QString dataR){
    emit workerRData(dataR);
}

ThreadManager::~ThreadManager(){
    m_threadWorker->exit(0);
}


