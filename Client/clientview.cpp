#include "clientview.h"
#include "ui_clientview.h"

ClientView::ClientView(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::ClientView)
{
    ui->setupUi(this);
}

void ClientView::setFileList() const
{
    QString buf = "";
    controller ->getListFromServer(buf);

    int size = model->getName().size();
    QStringList name = model->getName();
    QStringList type = model->getType();
    QStringList change_time = model->getChange_time();
    QStringList file_size = model->getFile_size();

    ui->table_files->setRowCount(size);

    for(int i = 0; i < size; ++i)
        for(int j = 0; j < 4; ++j)
        {
            QTableWidgetItem* tmp = new QTableWidgetItem;
            if(j == 0)
                tmp->setText(name.at(i));
            else if(j == 1)
                tmp->setText(type.at(i));
            else if(j == 2)
                tmp->setText(change_time.at(i));
            else if(j == 3 && type.at(i) != "Dir")
                tmp->setText(file_size.at(i));
            ui->table_files->setItem(i, j, tmp);
        }

    ui->edit_path->setText(model->getPath());

    ui->table_files->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->table_files->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->table_files->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
}

ClientView::ClientView(ClientModel* _model, ClientController* _controller) : ClientView()
{
    model = _model;
    controller = _controller;

    setFileList();

    ui->table_files->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->table_files, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));

    ui->RenameOK->hide();
    ui->editRename->hide();
    ui->RenameCancel->hide();

    ui->table_files->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(this, SIGNAL(ThreadEnded(bool, bool)), this, SLOT(ShowThreadResult(bool, bool)));

    done = true;
}

ClientView::~ClientView()
{
    delete ui;
}

void ClientView::slotCustomMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);

    QAction *copy = new QAction("Copy", this);
    QAction *move = new QAction("Move", this);
    QAction *rename = new QAction("Rename", this);
    QAction *del = new QAction("Delete", this);
    QAction *download = new QAction("Download", this);
    QAction *upload = new QAction("Upload", this);

    connect(copy, SIGNAL(triggered()), this, SLOT(slotCopy()));
    connect(move, SIGNAL(triggered()), this, SLOT(slotMove()));
    connect(rename, SIGNAL(triggered()), this, SLOT(slotRename()));
    connect(del, SIGNAL(triggered()), this, SLOT(slotDelete()));
    connect(download, SIGNAL(triggered()), this, SLOT(slotDownload()));
    connect(upload, SIGNAL(triggered()), this, SLOT(slotUpload()));

    menu->addAction(copy);
    menu->addAction(move);
    menu->addAction(rename);
    menu->addAction(del);
    menu->addAction(download);
    menu->addAction(upload);

    menu->popup(ui->table_files->viewport()->mapToGlobal(pos));
}

void ClientView::ShowThreadResult(bool result, bool needUpdate)
{
    if(needUpdate)
    {
        controller->sendCommand(9);
        if(controller->getRespond())
            setFileList();
    }

    if(result)
        ui->statusbar->showMessage("Operation succeded!", 5000);
    else
        ui->statusbar->showMessage("Operation failed!", 5000);
}

void ClientView::WaitForRespond(int size, bool needUpdate)
{
    int counter = 0;
    for(int i = 0; i < size; ++i)
        if(controller->getRespond())
            counter++;

    if(counter == size)
        emit ThreadEnded(true, needUpdate);
    else
        emit ThreadEnded(false, false);

    done = true;
}

void ClientView::WaitForReceivingFile(QStringList serverpath, QString dir)
{
    QList<int> rows;

    for (int i = 0; i < ui->table_files->selectedItems().size(); ++i)
    {
        if(!rows.contains(ui->table_files->selectedItems().at(i)->row()))
            rows.append(ui->table_files->selectedItems().at(i)->row());
        serverpath.append(ui->edit_path->text() + "/" + model->getName().at(rows.at(i)));
    }

    int counter = 0;
    for(int i = 0; i < serverpath.size(); ++i)
    {
        controller->sendCommand(7, serverpath.at(i));
        controller->ReceiveFile(dir + model->getName().at(rows.at(i)));

        if(controller->getRespond())
            counter++;
    }

    if(counter == serverpath.size())
        emit ThreadEnded(true, false);
    else
        emit ThreadEnded(false, false);

    done = true;
}

void ClientView::WaitForSendingFile(QStringList files, QString serverdir)
{
    int counter = 0;

    for(int i = 0; i < files.size(); ++i)
    {
        controller->sendCommand(8, serverdir + files.at(i).section('/', -1));
        controller->SendFile(files.at(i));

        if(controller->getRespond())
            counter++;
    }

    if(counter == files.size())
        emit ThreadEnded(true, true);
    else
        emit ThreadEnded(false, false);

    done = true;
}

void ClientView::slotCopy()
{
    if(!done)
    {
        QMessageBox::information(this, "Info", "Wait while I am processing long operation!");
        return;
    }

    QList<int> rows;
    QString dir;

    for (int i = 0; i < ui->table_files->selectedItems().size(); ++i)
        if(!rows.contains(ui->table_files->selectedItems().at(i)->row()))
            rows.append(ui->table_files->selectedItems().at(i)->row());

    if(rows.size() > 0)
    {
         dir = QFileDialog::getExistingDirectory(this, tr("Copy to directory"), ui->edit_path->text(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if(dir == "")
            return;

        dir.append("/");
    }

    done = false;
    std::thread t(&ClientView::WaitForRespond, this, rows.size(), false);

    for(int i = 0; i < rows.size(); ++i)
        controller->sendCommand(1, model->getName().at(rows.at(i)), dir + model->getName().at(rows.at(i)));

    t.detach();
}

void ClientView::slotMove()
{
    if(!done)
    {
        QMessageBox::information(this, "Info", "Wait while I am processing long operation!");
        return;
    }

    QList<int> rows;

    for (int i = 0; i < ui->table_files->selectedItems().size(); ++i)
        if(!rows.contains(ui->table_files->selectedItems().at(i)->row()))
            rows.append(ui->table_files->selectedItems().at(i)->row());

    if(rows.size() > 0)
    {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Move to directory"), ui->edit_path->text(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if(dir == "")
            return;

        dir.append("/");

        std::sort(rows.rbegin(), rows.rend());

        done = false;
        std::thread t(&ClientView::WaitForRespond, this, rows.size(), true);

        for(int i = 0; i < rows.size(); ++i)
            controller->sendCommand(2, model->getName().at(rows.at(i)), dir + model->getName().at(rows.at(i)));

        t.detach();
    }
}

void ClientView::slotRename()
{   
    if(!done)
    {
        QMessageBox::information(this, "Info", "Wait while I am processing long operation!");
        return;
    }

    if(ui->table_files->selectedItems().size() == 1)
    {
        ui->RenameOK->show();
        ui->editRename->show();
        ui->RenameCancel->show();
    }
    else
        QMessageBox::information(this, "Info", "Rename one file!");
}

void ClientView::slotDelete()
{
    if(!done)
    {
        QMessageBox::information(this, "Info", "Wait while I am processing long operation!");
        return;
    }

    QList<int> rows;
    int counter = 0;

    for (int i = 0; i < ui->table_files->selectedItems().size(); ++i)
        if(!rows.contains(ui->table_files->selectedItems().at(i)->row()))
            rows.append(ui->table_files->selectedItems().at(i)->row());

    if(rows.size() > 0)
    {
        while(rows.size() > 0)
        {
            std::sort(rows.rbegin(), rows.rend());

            controller->sendCommand(4, model->getName().at(rows.at(0)));

            if(controller->getRespond())
            {
                ui->table_files->removeRow(rows.at(0));
                counter++;
            }

            rows.removeAt(0);
        }

        if(counter)
            QMessageBox::information(this, "Info", "File removed successfully!");
        else
            QMessageBox::information(this, "Info", "Cannot remove file!");
    }
}

void ClientView::slotDownload()
{
    if(!done)
    {
        QMessageBox::information(this, "Info", "Wait while I am processing long operation!");
        return;
    }

    QStringList serverpath;

    if(ui->table_files->selectedItems().size() > 0)
    {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Download file"), ui->edit_path->text(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if(dir == "")
            return;

        if(dir.at(dir.size() - 1) != '/')
            dir.append("/");

        done = false;
        std::thread t(&ClientView::WaitForReceivingFile, this, serverpath, dir);
        t.detach();
    }
}

void ClientView::slotUpload()
{
    if(!done)
    {
        QMessageBox::information(this, "Info", "Wait while I am processing long operation!");
        return;
    }

    QStringList files = QFileDialog::getOpenFileNames(this, "Upload file", "/home");
    QString serverdir = ui->edit_path->text();

    if(serverdir.at(serverdir.size() - 1) != '/')
        serverdir.append('/');

    if(files.size() == 0)
        return;

    done = false;
    std::thread t(&ClientView::WaitForSendingFile, this, files, serverdir);
    t.detach();
}

void ClientView::on_RenameOK_clicked()
{
    int row = ui->table_files->selectionModel()->currentIndex().row();

    if(row >= 0 && ui->editRename->text() != "")
    {
        QString newname = ui->edit_path->text();
        if(newname.at(newname.size() - 1) != '/')
            newname.append('/');
        newname.append(ui->editRename->text());

        controller->sendCommand(3, model->getName().at(row), newname);
        if(controller->getRespond())
        {
            QTableWidgetItem* tmp = new QTableWidgetItem;
            tmp->setText(ui->editRename->text());
            ui->table_files->setItem(row, 0, tmp);

            QMessageBox::information(this, "Info", "File renamed successfully!");
        }
        else
            QMessageBox::information(this, "Info", "Cannot rename file!");

        ui->RenameCancel->hide();
        ui->RenameOK->hide();
        ui->editRename->setText("");
        ui->editRename->hide();
    }
}

void ClientView::on_RenameCancel_clicked()
{
    ui->RenameOK->hide();
    ui->editRename->setText("");
    ui->editRename->hide();
    ui->RenameCancel->hide();
}

void ClientView::on_btn_cdUp_clicked()
{
    if(!done)
    {
        QMessageBox::information(this, "Info", "Wait while I am processing long operation!");
        return;
    }

    controller->sendCommand(5);

    if(controller->getRespond())
        setFileList();
    else
        QMessageBox::information(this, "Info", "Cannot change directory!");
}

void ClientView::on_edit_path_returnPressed()
{
    if(!done)
    {
        QMessageBox::information(this, "Info", "Wait while I am processing long operation!");
        return;
    }

    controller->sendCommand(6, ui->edit_path->text());

    if(controller->getRespond())
        setFileList();
    else
        QMessageBox::information(this, "Info", "Cannot change directory!");
}

void ClientView::on_table_files_cellDoubleClicked(int row, int column)
{
    if(!done)
    {
        QMessageBox::information(this, "Info", "Wait while I am processing long operation!");
        return;
    }

    if(model->getType().at(row) == "Dir")
    {
        QString dir = model->getPath();
        if(dir.at(dir.size() - 1) != '/')
            dir.append("/");

        dir.append(model->getName().at(row));

        controller->sendCommand(6, dir);

        if(controller->getRespond())
            setFileList();
        else
            QMessageBox::information(this, "Info", "Cannot change directory!");
    }
}

void ClientView::on_actionReconnect_triggered()
{
    Client *client = new Client;
    ConnViewController *connctrl = new ConnViewController(client);
    ConnView *view = new ConnView(connctrl, client);

    view->show();
    this->close();
}

void ClientView::on_actionInfo_triggered()
{
    QMessageBox::information(this, "Help", "You can use mouse right click menu to copy, move, rename, "
                                           "delete files, download files from server or upload some.\n"
                                           "There are three ways to change directory:\n"
                                           "1. Double click on directory.\n"
                                           "2. Press button in left top corner to move to previous directory.\n"
                                           "3. Use line edit to set path of directory and press 'Enter'.\n\n"
                                           "You can connect by IP and port or use default scanner to find the "
                                           "server on current host or in local network.\n"
                                           "Scannig may take a lot of time!");
}
