#include "clientcast.h"

ClientCast::ClientCast()
{
    init();
}

ClientCast::~ClientCast()
{
    if (mClientSocket->isOpen())
        mClientSocket->close();
    if (mCommonSocket->isOpen())
        mCommonSocket->close();
    delete mClientSocket;
    delete mCommonSocket;
}

void ClientCast::init()
{
    mIsConnected = false;

    initSocket();

    if(mIsConnected) {
        sendClientLogIn();
    }
}

void ClientCast::initSocket()
{
    mClientSocket = new QUdpSocket();
    mCommonSocket = new QUdpSocket();

    if(!mClientSocket->bind(TM_CLIENT_PORT,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint)) {
        mConnectError = mClientSocket->errorString(); return;
    }
    if(!mCommonSocket->bind(QHostAddress::AnyIPv4, TM_COMMON_PORT, QUdpSocket::ShareAddress)) {
        mConnectError = mCommonSocket->errorString(); return;
    }
    if(!mCommonSocket->joinMulticastGroup(QHostAddress(TM_COMMON_IP))) {
        mConnectError = mCommonSocket->errorString(); return;
    }

    mIsConnected = true;

    mClientSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption,8*1024*1024);
    mCommonSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption,1024);

    this->connect(mClientSocket,SIGNAL(readyRead()),this,SLOT(clientDataRecievedSlot()));
    this->connect(mCommonSocket,SIGNAL(readyRead()),this,SLOT(commonDataRecievedSlot()));
}

bool ClientCast::isConnected()
{
    return mIsConnected;
}

QString ClientCast::getConnectError()
{
    return mConnectError;
}

void ClientCast::sendData(QByteArray ba,QString ip,int port)
{
    qDebug() << mClientSocket->writeDatagram(ba,ba.size(),QHostAddress(ip),port);
}

void ClientCast::close()
{
    if (mClientSocket->isOpen())
        mClientSocket->close();
    if (mCommonSocket->isOpen())
        mCommonSocket->close();
}

void ClientCast::sendClientLogIn()
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out << NewBuddy;
    out << tm_getLocalHostName();
    sendData(data,TM_COMMON_IP,TM_COMMON_PORT);
}

/**
 * NewBuddy HostName
 */

void ClientCast::clientDataRecievedSlot()
{
    while (mClientSocket->hasPendingDatagrams()){
        QByteArray datagram;
        QHostAddress host;
        QString hostName,hostIP,grpName,grpIP,msg;
        int type;

        datagram.resize(mClientSocket->pendingDatagramSize());
        mClientSocket->readDatagram(datagram.data(),datagram.size(),&host);
        QDataStream in(&datagram,QIODevice::ReadOnly);
        in >> type;
        hostIP = tm_getIPv4Addr(host.toString());
        switch (type) {
        case Message:
            in >> hostName >> msg;
            emit newMsgReceivedSig(hostName + "-" + hostIP,msg);
            break;
        case NewBuddy:
            in >> hostName;
            emit newHostLogInSig(hostName,hostIP);
            break;
        case NewGrpBuddy:
            in >> grpName >> grpIP >> hostName >> hostIP;
            emit newGrpHostSig(grpName,grpIP,hostName,hostIP);
            break;
        case BuddyLeft:
            in >> hostName >> hostIP;
            emit BuddyLeftSig(hostName,hostIP);
            break;
        default:
            break;
        }
    }
}

/**
 * NewBuddy HostName 
 * 0- down
 * 2- still on
 */

void ClientCast::commonDataRecievedSlot()
{
    while (mCommonSocket->hasPendingDatagrams()){
        QByteArray datagram;
        QHostAddress host;
        QString hostName,hostIP;
        int type;

        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);

        datagram.resize(mCommonSocket->pendingDatagramSize());
        mCommonSocket->readDatagram(datagram.data(),datagram.size(),&host);
        hostIP = tm_getIPv4Addr(host.toString());
        QDataStream in(&datagram,QIODevice::ReadOnly);
        in >>type;
        switch (type) {
        case NewBuddy:
            in >> hostName;
            emit newHostLogInSig(hostName,hostIP);
            out << NewBuddy << tm_getLocalHostName();
            sendData(data,hostIP,TM_CLIENT_PORT);
            break;
        case BuddyLeft:
            in >> hostName >> hostIP;
            emit BuddyLeftSig(hostName,hostIP);
            break;
        default:
            break;
        }
    }
}
