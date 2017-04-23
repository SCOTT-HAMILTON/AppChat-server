#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <stdio.h>

#include "DataType.h"

using namespace sf;

struct pseudoIp2 {
	sf::IpAddress address;
	std::string pseudo;
};

typedef pseudoIp2 pseudoIp;

int nb_clients = 0;
std::vector<std::unique_ptr<TcpSocket>> socketClients;
std::vector<pseudoIp> clientsPseudo;

bool containsPseudo(std::vector<pseudoIp> tab, std::string value);
bool containsIp(std::vector<pseudoIp> tab, sf::IpAddress value);
std::string getPseudoFromIp(std::vector<pseudoIp> tab, sf::IpAddress ip);
int getIndexFromIp(std::vector<pseudoIp> tab, sf::IpAddress ip);

int main(int argc, char *argv[]) {
	std::string serverChatText;

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

	DataType type;
	Time timeout = seconds(2);
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
				std::cout << "receiving data from clients!" << std::endl;
				if ((socketClients[i].get())->receive(receive) != sf::Socket::Done) {
					std::cout << "receive data error" << std::endl;
					nb_clients--;
					if (containsIp(clientsPseudo, (socketClients[i].get()->getRemoteAddress()))) {
						std::cout << "Client " << getPseudoFromIp(clientsPseudo, (socketClients[i].get()->getRemoteAddress())) <<" : "<< (socketClients[i].get()->getRemoteAddress()) << " is disconnected !" << std::endl;
						//erasing pseudo
						clientsPseudo.erase(clientsPseudo.begin() + getIndexFromIp(clientsPseudo, (socketClients[i].get()->getRemoteAddress())));
					}
					else {
						std::cout << "Client " << (socketClients[i].get()->getRemoteAddress()) << " is disconnected !" << std::endl;
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
						std::cout << "invalid data error" << std::endl;
						nb_clients--;
						std::cout << "Client " << (socketClients[i].get()->getRemoteAddress()) << " is disconnected !" << std::endl;
						selector.remove((*(socketClients[i].get())));
						(socketClients[i].get())->disconnect();
						socketClients.erase(socketClients.begin() + i);
					}
					else {
						if (typeR == DataType::message) {
							send.clear();
							send << dataR;
							std::cout << dataR << std::endl;
							serverChatText += std::string(dataR + '\n');
							for (j = 0; j < nb_clients; j++) {
								if (j != i) {
									(socketClients[j].get())->send(send);
								}
							}
						}
						if (typeR == DataType::pseudo) {
							std::cout << "pseudo type !" << std::endl;
							tmpPseudo.address = (socketClients[i].get())->getRemoteAddress();
							tmpPseudo.pseudo = dataR;

							if (!containsPseudo(clientsPseudo, dataR)) {
								std::cout << "pseudo available" << std::endl;
								send.clear();
								send << DataType::pseudoVailable << "";
								(socketClients[i].get())->send(send);
								clientsPseudo.push_back(tmpPseudo);
							}
							else {
								std::cout << "pseudo unvailable" << std::endl;
								send.clear();
								send << DataType::pseudoUnavailable << "";
								(socketClients[i].get())->send(send);
							}
							
						}

						if (typeR == DataType::getInitMessages) {
							std::cout << "get init messages type" << std::endl;
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
				std::cout << "accepting client !" << std::endl;
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
				std::cout << "one client accepted !" << std::endl;
			}
			else {
				std::cout << "client connection refused, to much clients ! " << std::endl;
			}
		}
	}

	
	std::cout << "server closing connexion !" << std::endl;

	listener.close();

	getchar();
	return EXIT_SUCCESS;
}

bool containsPseudo(std::vector<pseudoIp> tab, std::string value) {
	int i = 0;
	for (i = 0; i < tab.size(); i++) {
		if (tab[i].pseudo == value) return true;
	}
	return false;
}

bool containsIp(std::vector<pseudoIp> tab, sf::IpAddress value) {
	int i = 0;
	for (i = 0; i < tab.size(); i++) {
		if (tab[i].address == value) return true;
	}
	return false;
}

std::string getPseudoFromIp(std::vector<pseudoIp> tab, sf::IpAddress ip) {
	int i = 0;
	for (i = 0; i < tab.size(); i++) {
		if (tab[i].address == ip) return tab[i].pseudo;
	}
	return "";
}

int getIndexFromIp(std::vector<pseudoIp> tab, sf::IpAddress ip) {
	int i = 0;
	for (i = 0; i < tab.size(); i++) {
		if (tab[i].address == ip) return i;
	}
	return -1;
}

