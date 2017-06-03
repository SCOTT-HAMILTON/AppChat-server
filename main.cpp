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
#include <stdio.h>

#if defined(_WIN32)
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif // defined

char *CONFIG_PATH;
using namespace sf;

struct pseudoIp2 {
	sf::IpAddress address;
	std::string pseudo;
};

typedef pseudoIp2 pseudoIp;

bool continuer = true;

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

bool getContinuer();
void setContinuer(bool value);

void control();

int main(int argc, char *argv[]) {
    int i = 0;
    CONFIG_PATH = (char*) malloc(sizeof(char)*100);
    std::string current_path("");
    std::string tmp(""), tmp_path(argv[0]);
    for (i = 0; (unsigned int)i < tmp_path.size(); i++){
        if (argv[0][i] == '\\' || (int)argv[0][i] == 92){
            current_path += std::string (tmp+"\\");
            tmp = "";
        }else {
            tmp += argv[0][i];
        }
    }
    sprintf(CONFIG_PATH, "%s%s", current_path.c_str(), "config.appcs_cf");

    if (argv[1] != NULL){
        std::string file(argv[1]);
        for (i = 0; (unsigned int)i < file.size(); i++){
            if (file[i] == '\\' || (int)file[i] == 92){
                file[i] = '/';
            }
        }
        std::string ext("");
        i = file.size();
        bool stop = false;
        for (i = file.size(); i > 0 && !stop; i--){
            if (file[i]=='.')stop=true;
        }for (i = i+2; (unsigned int)i < file.size(); i++)ext+=file[i];
        if (ext == "appcs_cf" || ext == "txt"){
            char reponse = 'n';
            while (reponse != '1' && reponse != '2'){
                std::cout << "Que voulez vous faire ?\n\n" << "1. lire le fichier\n2. lancer l'aplication avec ce dernier en tant que fichier de configuration ..." << std::endl;
                std::cin >> reponse;
                if (reponse != '1' && reponse != '2'){
                    std::cout << "option invalide, recommencez ...";
                    std::cin.ignore();
                    std::cout << std::endl;
                }
            }
            if (reponse == '1'){
                    std::cout << std::endl << toString((char*)file.c_str()) << std::endl;
            }
            if (reponse == '2'){
                CONFIG_PATH = (char*)file.c_str();
                system(CLEAR);
            }
        }else{
            std::cout << "extension de fichier non supportee : " << ext << std::endl;
            std::cin.ignore();
            return 0;
        }
    }
	Packet send, receive;
	Packet sendC, receiveC;
	std::string dataSConnect("You're connected to Server !");
	std::string dataR(""), typeR("");
	sendC << dataSConnect;
	TcpListener listener;
	int j = 0;
	listener.listen(53000);

	SocketSelector selector;
	selector.add(listener);
	std::cout << "Server ready ! " << std::endl;

	pseudoIp tmpPseudo;

	Time timeout = seconds(1);

	unsigned long long int p = 509, q = 9403, d, e, n;
	getPUKey(p, q, n, e);
	getPRKey(p, q, n, d);

	sf::Thread threadControl(&control);
	threadControl.launch();
	std::string pseudo;

	while (continuer) {
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

    threadControl.wait();
	std::cout << "server closing connexion !" << std::endl;
	listener.close();

	//getchar();
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
    std::cout << ">>";
    bool def = true;

    while (getContinuer()){
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

        if (bash == "q"){
            setContinuer(false);
        }

        if (def){
            if (containsIp(getPseudoIp(), IpAddress(bash))){
            std::cout << "client " << getPseudoFromIp(getPseudoIp(), sf::IpAddress(bash)) << std::endl;
            }
            if (containsPseudo(getPseudoIp(), bash)){
                std::cout << "client " << getIpFromPseudo(getPseudoIp(), bash) << std::endl;
            }
            if (PseudoExist(bash, CONFIG_PATH))std::cout << "this pseudo is registered on the server !" << std::endl;
        }


        std::cout << ">>";
    }
}

bool getContinuer(){
    return continuer;
}
void setContinuer(bool value){
    continuer = value;
}



