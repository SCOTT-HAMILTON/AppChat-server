#include <QApplication>
#include "WinApp.h"

using namespace sf;

int main (int argc, char *argv[]){
    QApplication app(argc, argv);
    IpAddress Host("5.51.51.245");
    if (IpAddress::getPublicAddress() == IpAddress("5.51.51.245")){
        Host = "192.168.1.19";
    };
    WinApp fenetre(Host);
    fenetre.show();

    app.exec();

    return 0;
}
