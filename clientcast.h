#ifndef CLIENTCAST_H
#define CLIENTCAST_H

#include <QUdpSocket>
#include <QDataStream>
#include <QObject>
#include <QDebug>

#include "netcommon.h"

class ClientCast : public QObject
{
    Q_OBJECT

public:
    ClientCast();
    ~ClientCast();
    void init();
    void initSocket();
    bool isConnected();
    QString getConnectError();
    void sendData(QByteArray ba, QString ip, int port = TM_CLIENT_PORT);
    void close();

    enum MessageType{Message, NewBuddy, NewGrpBuddy, BuddyLeft, BuddyGrpLeft};

private:
    QUdpSocket *mClientSocket;
    QUdpSocket *mCommonSocket;
    bool mIsConnected;
    QString  mConnectError;

    void sendClientLogIn();

private slots:
    void clientDataRecievedSlot();
    void commonDataRecievedSlot();

signals:
    void newHostLogInSig(QString hostname,QString ip);
    void newMsgReceivedSig(QString host,QString msg);
    void newGrpHostSig(QString grpName,QString grpIP,QString hostName,QString hostIP);
    void BuddyLeftSig(QString hostName,QString hostIP);
};

#endif // CLIENTCAST_H
