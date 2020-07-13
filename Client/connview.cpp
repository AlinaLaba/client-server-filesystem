#include "connview.h"
#include "ui_connview.h"
#include "clientview.h"

ConnView::ConnView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConnView)
{
    ui->setupUi(this);
}

ConnView::ConnView(ConnViewController* _controller, Client* _model) : ConnView()
{
    controller = _controller;
    client = _model;

    ui->radiobtn_local->setVisible(false);
    ui->radiobtn_LAN->setVisible(false);

    connect(this, SIGNAL(ThreadEnded(bool)), this, SLOT(EndConnection(bool)));
}

ConnView::~ConnView()
{
    delete ui;
}

void ConnView::on_radiobtn_IP_clicked()
{
    if(ui->radiobtn_IP->isChecked() == true)
    {
        ui->labelIP->setVisible(true);
        ui->labelPort->setVisible(true);
        ui->inputIP->setVisible(true);
        ui->inputPort->setVisible(true);

        ui->radiobtn_local->setVisible(false);
        ui->radiobtn_LAN->setVisible(false);
    }
}

void ConnView::on_radiobtn_auto_clicked()
{
    if(ui->radiobtn_auto->isChecked() == true)
    {
        ui->labelIP->setVisible(false);
        ui->labelPort->setVisible(false);
        ui->inputIP->setVisible(false);
        ui->inputPort->setVisible(false);

        ui->radiobtn_local->setVisible(true);
        ui->radiobtn_LAN->setVisible(true);
    }
}

void ConnView::startAutoConnect()
{
    if(ui->radiobtn_local->isChecked() == true)
    {
        emit ThreadEnded(controller->autoConnect(true));
    }
    else if(ui->radiobtn_LAN->isChecked() == true)
    {
        emit ThreadEnded(controller->autoConnect(false));
    }
}

void ConnView::EndConnection(bool result)
{
    ui->statusBar->clearMessage();
    ui->pushButton->setEnabled(true);

    if(result)
    {
        ClientModel* model = new ClientModel(client);
        ClientController* ctrl = new ClientController(model, client);

        ClientView* cv = new ClientView(model, ctrl);
        cv->show();

        this->close();
    }
    else
    {
        QMessageBox::information(this, "Info", "Connection failed.");
    }
}

bool ConnView::ipIsValid(QString ip) const
{
    if(ip.count('.') != 3)
        return false;

    QStringList tmp = ip.split('.');

    for(int i = 0; i < tmp.size(); ++i)
        if(tmp.at(i) < 0 || tmp.at(i).toInt() > 255)
            return false;

    return true;
}

void ConnView::on_pushButton_clicked()
{
    if(ui->radiobtn_IP->isChecked() == true)
    {
        if(ui->inputIP->text() == "")
        {
            QMessageBox::information(this, "Error", "IP not set");
            return;
        }
        if(ui->inputPort->text() == "")
        {
            QMessageBox::information(this, "Error", "Port not set");
            return;
        }
        if(!ipIsValid(ui->inputIP->text()))
        {
            QMessageBox::information(this, "Error", "IP is invalid");
            return;
        }
        if(ui->inputPort->text().toInt() < 1 || ui->inputPort->text().toInt() > 65535)
        {
            QMessageBox::information(this, "Error", "Port is invalid");
            return;
        }

        controller->startConn(ui->inputIP->text().toStdString(), ui->inputPort->text().toInt());

        ClientModel* model = new ClientModel(client);
        ClientController* ctrl = new ClientController(model, client);

        ClientView* cv = new ClientView(model, ctrl);
        cv->show();

        this->close();
    }
    else
    {
        std::thread t(&ConnView::startAutoConnect, this);
        t.detach();
    }

    ui->pushButton->setEnabled(false);
    ui->statusBar->showMessage("In progress...");
}
