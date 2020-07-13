#ifndef CLIENTVIEW_H
#define CLIENTVIEW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <thread>
#include "clientmodel.h"
#include "clientcontroller.h"
#include "connview.h"

namespace Ui {
class ClientView;
}

class ClientView : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientView(QWidget *parent = nullptr);
    ClientView(ClientModel* _model, ClientController* _controller);
    ~ClientView();
    void setFileList() const;
    void WaitForRespond(int size, bool needUpdate);
    void WaitForReceivingFile(QStringList path, QString dir);
    void WaitForSendingFile(QStringList files, QString serverdir);

signals:
    void ThreadEnded(bool result, bool needUpdate);

private:
    Ui::ClientView *ui;

    ClientModel* model;
    ClientController* controller;
    std::atomic<bool> done;

private slots:
    void slotCustomMenuRequested(QPoint pos);
    void slotCopy();
    void slotMove();
    void slotRename();
    void slotDelete();
    void slotDownload();
    void slotUpload();
    void on_RenameOK_clicked();
    void on_RenameCancel_clicked();
    void on_btn_cdUp_clicked();
    void on_edit_path_returnPressed();
    void on_table_files_cellDoubleClicked(int row, int column);
    void ShowThreadResult(bool result, bool needUpdate);
    void on_actionReconnect_triggered();
    void on_actionInfo_triggered();
};

#endif // CLIENTVIEW_H
