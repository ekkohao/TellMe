#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMap>
#include <QHostInfo>
#include <QLinkedList>
#include <QDateTime>

#include "personlistwidget.h"
#include "grplistwidget.h"
#include "multicast.h"
#include "clientcast.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ClientCast *mClientCast;
    PersonListWidget *mPersonListWidget;
    GrpListWidget *mGrpListWidget;
    QLinkedList<QString> *mPersonList;
    QMap<QString,QString> *mChatLogs;
    QMap<QString,QString> *mChatLogsGrp;
    QVBoxLayout *mLayoutPersonList;
    QVBoxLayout *mLayoutGrpList;
    QLinkedList<MultiCast *> *mMultiCastList;
    QString mLocalIP;

    void init();
    void initClientLogging();
    QString getPersonChatLog(QString host);
    QString getGrpChatLog(QString grpIP);
    void setPersonChatLog(QString host,QString s);
    void setGrpChatLog(QString grpIP,QString s);
private slots:
    void newHostLogInSlot(QString hostname,QString ip);
    void selectedItemChangedSlot(QListWidgetItem *from,QListWidgetItem *to);
    void selectedItemChangedGrpSlot(QListWidgetItem *from,QListWidgetItem *to);
    void newMsgReceivedSlot(QString host,QString msg);
    void newGrpAddedSlot(QListWidgetItem*,QString ip);
    void newGrpHostSlot(QString grpName,QString grpIP,QString hostName,QString hostIP);
    void newGrpMagReceivedSlot(QString grpIP,QString hostName,QString hostIP,QString msg);
    void sendToAddGrpHostSlot(QString grpIP,QString hostName,QString hostIP);
    void BuddyLeftSlot(QString hostName,QString hostIP);
    void buddyGrpLeftSlot(QString grpIP,QString hostName,QString hostIP);
    void grpLeftSlot(QString grpIP);
    void on_bth_send_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // MAINWINDOW_H
