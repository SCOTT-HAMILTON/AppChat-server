#include "WinApp.h"
#include <iostream>

WinApp::WinApp(sf::IpAddress Host): QWidget(),
  m_host(Host), updateMessage(false), type()
{

    //pseudo GUI
    m_pseudo = QString::fromStdString(sf::IpAddress::getPublicAddress().toString());
    m_txtPseudo = new QLabel("Entrer votre pseudo : ");
    m_editpseudo = new QLineEdit;
    m_pseudoButton = new QPushButton("valider");

    m_errorCLay = new QHBoxLayout;
    m_errorConnection = new QLabel("le serveur n'est pas disponible pour le moment.");
    m_retryConnection = new QPushButton("réessayer");
    m_errorCLay->addWidget(m_errorConnection);
    m_errorCLay->addWidget(m_retryConnection);
    m_errorConnection->hide();
    m_retryConnection->hide();

    //set Interface
    m_mainLay = new QVBoxLayout;

    m_textRMessage = new QTextEdit;
    m_textRMessage->hide();
    m_textRMessage->setReadOnly(true);

    m_enterMLay = new QFormLayout;
    m_enterMessage = new QLineEdit;
    m_enterMessage->hide();
    m_enterMLay->addRow("Message", m_enterMessage);

    m_mainLay->addWidget(m_txtPseudo);
    m_mainLay->addWidget(m_editpseudo);
    m_mainLay->addWidget(m_pseudoButton);
    m_mainLay->addLayout(m_errorCLay);
    m_mainLay->addWidget(m_textRMessage);

    setLayout(m_mainLay);

    //setConnection
    QObject::connect(m_retryConnection, SIGNAL(clicked(bool)), this, SLOT(connectToHost()));
    connectToHost();
}
void WinApp::keyPressEvent(QKeyEvent *event){
    if (event->key() == 16777220){
        updateMessage = true;
        tryUpdateMessage();
    }
}
void WinApp::keyReleaseEvent(QKeyEvent *event){
    if (event->key() == 16777220){
        updateMessage = false;
    }
}

void WinApp::tryUpdateMessage(){
    if (updateMessage && m_enterMessage->hasFocus() && m_enterMessage->text() != ""){
        m_txtContenue += QString("\nmoi : "+m_enterMessage->text());
        m_textRMessage->append(QString("moi : "+m_enterMessage->text()));
        m_textRMessage->moveCursor(QTextCursor::End);
        sendMessage(QString(m_pseudo+" : "+m_enterMessage->text()));
        m_enterMessage->setText("");
    }
}

void WinApp::sendMessage(const QString &text){
    sf::Packet send;
    if (text != ""){
       send << DataType::message <<text.toStdString();
       m_socket.send(send);
    }
}

void WinApp::receiveData(QString dataR){
    m_txtContenue += QString("\n")+dataR;
    m_textRMessage->append(dataR);
    m_textRMessage->moveCursor(QTextCursor::End);
}

void WinApp::showGui(){
    if (m_editpseudo->text().toStdString() != ""){
        m_pseudo = m_editpseudo->text();
    }

    sf::Packet send, receive;
    std::string dataR, typeR;
    send << DataType::pseudo << m_pseudo.toStdString();
    m_socket.send(send);
    typeR = "";
    m_socket.receive(receive);
    receive >> typeR >> dataR;

    if (typeR == DataType::pseudoVailable){
        send.clear();
        send << DataType::getInitMessages << "";
        m_socket.send(send);
        receive.clear();
        m_socket.receive(receive);
        receive >> typeR >> dataR;
        m_txtContenue = QString::fromStdString(dataR);
        m_textRMessage->setPlainText(m_txtContenue);

        m_errorConnection->hide();
        std::cout << "pseudo available" << std::endl;
        m_txtPseudo->hide();
        m_editpseudo->hide();
        m_pseudoButton->hide();

        m_mainLay->addLayout(m_enterMLay);
        m_textRMessage->show();
        m_enterMessage->show();
        setWindowTitle("Client : "+m_pseudo);
        setFixedSize(300, 300);
        m_ThreadManager = new ThreadManager(&m_socket);
        QObject::connect(m_ThreadManager, SIGNAL(workerRData(QString)), this, SLOT(receiveData(QString)));
        m_ThreadManager->startWorker();
    }
    else if (typeR == DataType::pseudoUnavailable){
        m_errorConnection->hide();
        m_errorConnection->setText("ce pseudo est déjà pris, choisissez en un autre.");
        m_errorConnection->show();
    }



}

bool WinApp::connectToHost(){
    sf::Packet receive, send;
    std::string dataS("You're connected with "+sf::IpAddress::getLocalAddress().toString());
    send << dataS;
    m_errorConnection->hide();
    if (m_socket.connect(m_host, 53000) != sf::Socket::Done){
        m_errorConnection->show();
        m_retryConnection->show();
        return 1;
    }
    if (m_socket.receive(receive) != sf::Socket::Done){
        m_errorConnection->show();
        m_retryConnection->show();
        return 1;
    }
    if (m_socket.send(send) != sf::Socket::Done){
        m_errorConnection->show();
        m_retryConnection->show();
        return 1;
    }
    m_errorConnection->hide();
    m_retryConnection->hide();
    QObject::connect(m_pseudoButton, SIGNAL(clicked(bool)), this, SLOT(showGui()));
    return 0;
}

WinApp::~WinApp(){
    delete m_ThreadManager;
    m_socket.disconnect();
}
