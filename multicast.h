#ifndef MULTICAST_H
#define MULTICAST_H

#include <QUdpSocket>
#include <QObject>
#include <QDebug>
#include <QDataStream>

#include "netcommon.h"

class MultiCast : public QObject
{
    Q_OBJECT

public:
    MultiCast(QString ip, int port);
    ~MultiCast();
    void sendData(QString s);
    bool isConnected();
    bool reJoinMultiAddr(QString ip);

    enum MessageType{Message, NewBuddy, NewGrpBuddy, BuddyLeft, BuddyGrpLeft};

private:
    QString mIP;
    int mPort;
    QUdpSocket *mQUdpSocket;
    bool mIsConnected;

    void initSocket();

private slots:
    void dataReceivedSlot();

signals:
    void newGrpHostSig(QString grpName,QString grpIP,QString hostName,QString hostIP);
    void newGrpMagReceivedSig(QString grpIP,QString hostName,QString hostIP,QString msg);
    void buddyGrpLeftSig(QString grpIP,QString hostName,QString hostIP);
};

#endif // MULTICAST_H
