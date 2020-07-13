#ifndef CONNVIEW_H
#define CONNVIEW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <thread>
#include "connviewcontroller.h"
#include "clientmodel.h"

namespace Ui {
class ConnView;
}

class ConnView : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConnView(QWidget *parent = nullptr);
    ConnView(ConnViewController* _controller, Client* _model);
    ~ConnView();
    void startAutoConnect();
    bool ipIsValid(QString ip) const;

signals:
    void ThreadEnded(bool result);

private slots:
    void on_radiobtn_IP_clicked();
    void on_radiobtn_auto_clicked();
    void on_pushButton_clicked();
    void EndConnection(bool result);

private:
    Ui::ConnView* ui;

    ConnViewController* controller;
    Client* client;
};

#endif // CONNVIEW_H
