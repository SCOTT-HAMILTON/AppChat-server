#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <stdio.h>
#include <RSA-Crypt.h>
#include <cstdlib>

#include "DataType.h"
#include "ClientManager.h"

#if defined(_WIN32)
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif // defined

#define CONFIG_PATH "config/config.txt"

using namespace sf;

struct pseudoIp2 {
	sf::IpAddress address;
	std::string pseudo;
};

typedef pseudoIp2 pseudoIp;

int nb_clients = 0;
std::string serverChatText;

std::vector<std::unique_ptr<TcpSocket>> socketClients;
std::vector<pseudoIp> clientsPseudo;

bool containsPseudo(std::vector<pseudoIp> tab, std::string value);
bool containsIp(std::vector<pseudoIp> tab, sf::IpAddress value);
std::string getPseudoFromIp(std::vector<pseudoIp> tab, sf::IpAddress ip);
sf::IpAddress getIpFromPseudo(std::vector<pseudoIp> tab, std::string pseudo);
int getIndexFromIp(std::vector<pseudoIp> tab, sf::IpAddress ip);

int getNb_clients();
std::string getAllMessages();
std::vector<pseudoIp> getPseudoIp();

void control();

int main(int argc, char *argv[]) {
	Packet send, receive;
	Packet sendC, receiveC;
	std::string dataSConnect("You're connected to Server !");
	std::string dataR(""), typeR("");
	sendC << dataSConnect;
	TcpListener listener;

	int i = 0, j = 0;
	listener.listen(53000);

	SocketSelector selector;
	selector.add(listener);
	std::cout << "Server ready ! " << std::endl;

	pseudoIp tmpPseudo;

	Time timeout = seconds(2);

	unsigned long long int p = 509, q = 9403, d, e, n;
	getPUKey(p, q, n, e);
	getPRKey(p, q, n, d);

	sf::Thread threadControl(&control);
	threadControl.launch();
	std::string pseudo;

	while (true) {
		if (nb_clients <= 0) {
			serverChatText = "";
		}
		selector.wait(timeout);
		for (i = 0; i < nb_clients; i++) {
			if (selector.isReady((*(socketClients[i].get())))) {
				dataR = "";
				typeR = "";
				receive.clear();
				if ((socketClients[i].get())->receive(receive) != sf::Socket::Done) {
					nb_clients--;
					if (containsIp(clientsPseudo, (socketClients[i].get()->getRemoteAddress()))) {
						//erasing pseudo
						clientsPseudo.erase(clientsPseudo.begin() + getIndexFromIp(clientsPseudo, (socketClients[i].get()->getRemoteAddress())));
					}
					else {
					}
					selector.remove((*(socketClients[i].get())));
					(socketClients[i].get())->disconnect();
					socketClients.erase(socketClients.begin() + i);
				}
				else {
					if (!(receive >> typeR >> dataR)) {
						break;
					}
					if (dataR == "" && !DataType::isType(typeR)) {
						nb_clients--;
						selector.remove((*(socketClients[i].get())));
						(socketClients[i].get())->disconnect();
						socketClients.erase(socketClients.begin() + i);
					}
					else {
						if (typeR == DataType::message) {
							send.clear();
							send << dataR;
							serverChatText += std::string(dataR+ToStr(crypt(n, e, "\n")));
							for (j = 0; j < nb_clients; j++) {
								if (j != i) {
									(socketClients[j].get())->send(send);
								}
							}
						}
						if (typeR == DataType::create_client) {
                            if (!PseudoExist(dataR, CONFIG_PATH)){
                                pseudo = dataR;

                                send.clear();
								send << DataType::pseudoVailable << "";
								(socketClients[i].get())->send(send);

                                receive.clear();
                                (socketClients[i].get())->receive(receive);
                                receive >> typeR >> dataR;
								createClient(pseudo, dataR, CONFIG_PATH);
                            }
							else {
								send.clear();
								send << DataType::pseudoUnavailable << "";
								(socketClients[i].get())->send(send);
							}

						}

						if (typeR == DataType::connection_client){
                            tmpPseudo.pseudo = dataR;
                            tmpPseudo.address = (socketClients[i].get())->getRemoteAddress();
                            receive.clear();
                            (socketClients[i].get())->receive(receive);
                            receive >> typeR >> dataR;
                            send.clear();
                            if (!compare(tmpPseudo.pseudo, dataR, CONFIG_PATH)){
                                send << DataType::invalid_connection << "";
								(socketClients[i].get())->send(send);
                            }else{
                                send << DataType::valid_connection << "";
								(socketClients[i].get())->send(send);
								clientsPseudo.push_back(tmpPseudo);
                            }
						}

						if (typeR == DataType::getInitMessages) {
							send.clear();
							send << DataType::initMessagesTxt << serverChatText;
							(socketClients[i].get())->send(send);
						}
					}

				}

			}
		}

		if (selector.isReady(listener)) {
			if (nb_clients < 30) {
				nb_clients++;
				socketClients.push_back(std::unique_ptr<TcpSocket>(new TcpSocket()));
				if (listener.accept((*(socketClients[nb_clients - 1].get()))) != sf::Socket::Done) {
				}
				else {
					sendC.clear();
					sendC << dataSConnect;
					(socketClients[nb_clients - 1].get())->send(sendC);
					receiveC.clear();
					(socketClients[nb_clients - 1].get())->receive(receiveC);
					selector.add((*(socketClients[nb_clients - 1].get())));
				}
			}
		}
	}

    threadControl.terminate();
	std::cout << "server closing connexion !" << std::endl;

	listener.close();

	getchar();
	return EXIT_SUCCESS;
}

bool containsPseudo(std::vector<pseudoIp> tab, std::string value) {
	unsigned int i = 0;
	for (i = 0; i < tab.size(); i++) {
		if (tab[i].pseudo == value) return true;
	}
	return false;
}

bool containsIp(std::vector<pseudoIp> tab, sf::IpAddress value) {
	unsigned int i = 0;
	for (i = 0; i < tab.size(); i++) {
		if (tab[i].address == value) return true;
	}
	return false;
}

std::string getPseudoFromIp(std::vector<pseudoIp> tab, sf::IpAddress ip) {
	unsigned int i = 0;
	for (i = 0; i < tab.size(); i++) {
		if (tab[i].address == ip) return tab[i].pseudo;
	}
	return "";
}

sf::IpAddress getIpFromPseudo(std::vector<pseudoIp> tab, std::string pseudo){
    unsigned int i = 0;
	for (i = 0; i < tab.size(); i++) {
		if (tab[i].pseudo == pseudo) return tab[i].address;
	}
	return sf::IpAddress("");
}

int getNb_clients(){
    return nb_clients;
}

std::string getAllMessages(){
    return serverChatText;
}

std::vector<pseudoIp> getPseudoIp(){
    return clientsPseudo;
}

int getIndexFromIp(std::vector<pseudoIp> tab, sf::IpAddress ip) {
	unsigned int i = 0;
	for (i = 0; i < tab.size(); i++) {
		if (tab[i].address == ip) return i;
	}
	return -1;
}

void control(){

    unsigned long long int p = 509, q = 9403, d, e, n;
	getPUKey(p, q, n, e);
	getPRKey(p, q, n, d);

    char tmp[100];
    std::string bash;
    std::cout << "entrer une option : " << std::endl;

    bool def = true;

    while (true){
        std::cin.getline(tmp, sizeof(tmp));
        bash = std::string(tmp);
        def = true;
        if (bash == "s") {
            std::cout << "nb de client(s) connecte(s) : " << getNb_clients() << std::endl;
            def = false;
        }
        if (bash == "t") {
            std::cout << decrypt(d, n, FromStr(getAllMessages())) << std::endl;
            def = false;
        }

        if (bash == "c"){
            std::system(CLEAR);
            def = false;
        }

        if (def){
            if (containsIp(getPseudoIp(), IpAddress(bash))){
            std::cout << "client " << getPseudoFromIp(getPseudoIp(), sf::IpAddress(bash)) << std::endl;
            }
            if (containsPseudo(getPseudoIp(), bash)){
                std::cout << "client " << getIpFromPseudo(getPseudoIp(), bash) << std::endl;
            }
        }


        std::cout << ">>";
    }
}

