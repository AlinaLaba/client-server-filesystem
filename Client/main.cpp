#include "connview.h"
#include "clientview.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client client;
    ConnViewController conncontroller(&client);
    ConnView w(&conncontroller, &client);
    w.show();

    return a.exec();
}
