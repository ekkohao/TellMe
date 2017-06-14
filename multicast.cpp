#include "multicast.h"

MultiCast::MultiCast(QString ip,int port)
{
    mIP = ip;
    mPort = port;
    mIsConnected = false;
    initSocket();
}

MultiCast::~MultiCast()
{
    delete mQUdpSocket;
}

bool MultiCast::isConnected()
{
    return mIsConnected;
}

bool MultiCast::reJoinMultiAddr(QString ip)
{
    mIsConnected = mQUdpSocket->joinMulticastGroup(QHostAddress(ip));
    if(mIsConnected)
        mIP = ip;
    return mIsConnected;
}


void MultiCast::initSocket()
{
    mQUdpSocket = new QUdpSocket();
    if(!mQUdpSocket->bind(QHostAddress::AnyIPv4, mPort, QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint))
        return;
    if(!mQUdpSocket->joinMulticastGroup(QHostAddress(mIP)))
        return;
    mIsConnected = true;
    mQUdpSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption,8*1024*1024);
    this->connect(mQUdpSocket,SIGNAL(readyRead()),this,SLOT(dataReceivedSlot()));
}

/**
 * NewBuddy groupIP hostName hostIP
 * type 1-client up/down
 * type
 *
 */
void MultiCast::dataReceivedSlot()
{
    while (mQUdpSocket->hasPendingDatagrams()){
        QByteArray datagram;
        QHostAddress host;
        QString hostName,hostIP,grpName,grpIP,msg;
        int type;

        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);

        datagram.resize(mQUdpSocket->pendingDatagramSize());
        mQUdpSocket->readDatagram(datagram.data(),datagram.size(),&host);
        hostIP = tm_getIPv4Addr(host.toString());
        QDataStream in(&datagram,QIODevice::ReadOnly);
        in >>type;
        switch (type) {
        case NewGrpBuddy:
            in >> grpName >> grpIP >> hostName >> hostIP;
            emit newGrpHostSig(grpName,grpIP,hostName,hostIP);
            break;
        case Message:
            in >> grpIP >> hostName >> hostIP >> msg;
            if(!tm_isLocalHost(hostName,hostIP))
                emit newGrpMagReceivedSig(grpIP,hostName,hostIP,msg);
        case BuddyGrpLeft:
            in >> grpIP >> hostName >> hostIP;
            emit buddyGrpLeftSig(grpIP,hostName,hostIP);
            break;
        default:
            break;
        }
    }
}
