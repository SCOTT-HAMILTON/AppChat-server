#ifndef WINAPP_H
#define WINAPP_H

#include <QObject>
#include <SFML/Network.hpp>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QKeyEvent>
#include <QString>
#include <QThread>
#include <QLabel>
#include <QPushButton>

#include <string>

#include "ThreadManager.h"
#include "DataType.h"

class WinApp : public QWidget
{
    Q_OBJECT

public:
    WinApp(sf::IpAddress Host);
    ~WinApp();

public slots:
    void receiveData(QString);
    void showGui();
    bool  connectToHost();

private:

    sf::TcpSocket m_socket;
    sf::IpAddress m_host;

    QLabel *m_txtPseudo;
    QLineEdit *m_editpseudo;
    QPushButton *m_pseudoButton;

    QHBoxLayout *m_errorCLay;
    QLabel *m_errorConnection;
    QPushButton *m_retryConnection;
    QString m_pseudo;

    QVBoxLayout *m_mainLay;
    QTextEdit *m_textRMessage;

    QFormLayout *m_enterMLay;
    QLineEdit *m_enterMessage;

    bool updateMessage;
    QString m_txtContenue;
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    void tryUpdateMessage();
    void sendMessage(const QString &text);

    ThreadManager *m_ThreadManager;

    DataType type;
};

#endif // WINAPP_H
