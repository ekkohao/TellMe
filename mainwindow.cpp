#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    init();
}

MainWindow::~MainWindow()
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out << ClientCast::BuddyLeft;
    out << tm_getLocalHostName();
    out << mLocalIP;
    mClientCast->sendData(data,TM_COMMON_IP,TM_COMMON_PORT);

    foreach (MultiCast *m, *mMultiCastList) {
        delete m;
    }
    delete mMultiCastList;
    delete mPersonList;
    delete mChatLogsGrp;
    delete mChatLogs;
    delete mLayoutPersonList;
    delete mLayoutGrpList;
    delete mPersonListWidget;
    delete mClientCast;
    delete ui;
}

void MainWindow::init()
{
    ui->setupUi(this);
    //ui->centralWidget->setGeometry(200,100,950,650);
    mLocalIP = tm_getIP();
    mChatLogsGrp = new QMap<QString,QString>();
    mPersonList = new QLinkedList<QString>();
    mChatLogs =new QMap<QString,QString>();
    mPersonListWidget = new PersonListWidget(this);
    mGrpListWidget = new GrpListWidget(this);
    mPersonListWidget->setGrpListWidget(mGrpListWidget);

    ui->stackedWidgetPersonList->setCurrentIndex(0);
    ui->stackedWidgetChatLog->setCurrentIndex(0);

    mLayoutPersonList=new QVBoxLayout(ui->pagePersonList);
    mLayoutPersonList->addWidget(mPersonListWidget);
    mLayoutGrpList=new QVBoxLayout(ui->pageGroupList);
    mLayoutGrpList->addWidget(mGrpListWidget);
    mLayoutPersonList->setMargin(0);
    mLayoutGrpList->setMargin(0);

    this->connect(mPersonListWidget,SIGNAL(selectedItemChangedSig(QListWidgetItem*,QListWidgetItem*)),
                  this,SLOT(selectedItemChangedSlot(QListWidgetItem*,QListWidgetItem*)));
    this->connect(mPersonListWidget,SIGNAL(sendToAddGrpHostSig(QString,QString,QString)),
                  this,SLOT(sendToAddGrpHostSlot(QString,QString,QString)));
    this->connect(mGrpListWidget,SIGNAL(selectedItemChangedSig(QListWidgetItem*,QListWidgetItem*)),
                  this,SLOT(selectedItemChangedGrpSlot(QListWidgetItem*,QListWidgetItem*)));
    this->connect(mGrpListWidget,SIGNAL(grpLeftSig(QString)),this,SLOT(grpLeftSlot(QString)));

    initClientLogging();
}

void MainWindow::initClientLogging()
{
    mClientCast = new ClientCast();
    if(!mClientCast->isConnected()) {
        qDebug() << "connected error";
        qDebug() << mClientCast->getConnectError();
        exit(0);
    }
    this->connect(mClientCast,SIGNAL(newHostLogInSig(QString,QString)),
                  this,SLOT(newHostLogInSlot(QString,QString)));
    this->connect(mClientCast,SIGNAL(newMsgReceivedSig(QString,QString)),
                  this,SLOT(newMsgReceivedSlot(QString,QString)));
    this->connect(mClientCast,SIGNAL(newGrpHostSig(QString,QString,QString,QString)),
                  this,SLOT(newGrpHostSlot(QString,QString,QString,QString)));
    this->connect(mClientCast,SIGNAL(BuddyLeftSig(QString,QString)),
                  this,SLOT(BuddyLeftSlot(QString,QString)));
    mMultiCastList = new QLinkedList<MultiCast *>();
    this->connect(mGrpListWidget,SIGNAL(newGrpAddedSig(QListWidgetItem*,QString)),
                  this,SLOT(newGrpAddedSlot(QListWidgetItem*,QString)));
    /*
    QString localHostName = QHostInfo::localHostName();
    qDebug() << "LocalHostName:" << localHostName;

    //获取IP地址
    QHostInfo info = QHostInfo::fromName(localHostName);
    qDebug() << "IP Address:" << info.addresses();

    foreach(QHostAddress address, info.addresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol)
            qDebug() << "IPv4 Address:" << address.toString();
    }


    QMap<QString,QString> map;
    QString s;
    mMultiCast = new MultiCast(TM_SERVER_IP,TM_PORT,8*1024);
    //mMultiCast->sendData("000000-" + s);

    this->connect(mMultiCast,SIGNAL(clientLoggingMsgSig(QString)),this,SLOT(clientLoggingMsgSlot(QString)));

    QUdpSocket *revWBudp;
    revWBudp = new QUdpSocket;
    bool result = revWBudp->bind(44325,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    if(result)
    {
        revWBudp->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption,1024*1024*8);//设置缓冲区
        //连接接收信号槽
        //发送数据 目的地址：serverIP  目的IP:serverPort
        QByteArray line;
        QString aaa = "0-hhhhhhh";
        qDebug() << revWBudp->writeDatagram(aaa.toLatin1() , QHostAddress(TM_SERVER_IP),TM_PORT);
    }
    */
}

QString MainWindow::getPersonChatLog(QString host)
{
    if(mChatLogs->contains(host))
        return mChatLogs->value(host);
    return "";
}

QString MainWindow::getGrpChatLog(QString grpIP)
{
    if(mChatLogsGrp->contains(grpIP))
        return mChatLogsGrp->value(grpIP);
    return "";
}

void MainWindow::setPersonChatLog(QString host, QString s)
{
    mChatLogs->insert(host,s);
}

void MainWindow::setGrpChatLog(QString grpIP, QString s)
{
    mChatLogsGrp->insert(grpIP,s);
}

void MainWindow::newHostLogInSlot(QString hostname, QString ip)
{
    if(mPersonList->contains(hostname + "-" + ip))
        return;
    if(tm_isLocalHost(hostname,ip))
        mLocalIP = ip;
    mPersonList->append(hostname + "-" + ip);
    mPersonListWidget->addBuddy(hostname,ip);
}

void MainWindow::selectedItemChangedSlot(QListWidgetItem *from, QListWidgetItem *to)
{
    PersonListWidgetItem * item;
    QWidget * w;
    if (from != nullptr) {
        w = mPersonListWidget->itemWidget(from);
        if ((item = dynamic_cast<PersonListWidgetItem *>(w))) {
            setPersonChatLog(item->getHost(),ui->textChatLogs->toHtml());
        }

    }
    w = mPersonListWidget->itemWidget(to);
    if ((item = dynamic_cast<PersonListWidgetItem *>(w))) {
        ui->textChatLogs->setHtml(getPersonChatLog(item->getHost()));
    }
}

void MainWindow::selectedItemChangedGrpSlot(QListWidgetItem *from, QListWidgetItem *to)
{
    QString ip;
    if (from != nullptr) {
        ip = mGrpListWidget->getGrpItemIP(from);
        setGrpChatLog(ip,ui->textChatLogsGrp->toHtml());
    }
    ip = mGrpListWidget->getGrpItemIP(to);
    ui->textChatLogsGrp->setHtml(getGrpChatLog(ip));
}

void MainWindow::newMsgReceivedSlot(QString host, QString msg)
{
    QString hostName = host.left(host.lastIndexOf("-"));
    QString time = QDateTime::currentDateTime().toString(" yyyy-MM-dd hh:mm:ss");
    if(mPersonListWidget->getCurrentSelectedItemHost() == host) {

        ui->textChatLogs->append("<font color=\"green\"><b>" + hostName + "</b></font>" + time);
        ui->textChatLogs->append(msg);
    }
    else {
        QString hs = getPersonChatLog(host);
        setPersonChatLog(host,hs + "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><font color=\"green\"><b>" + hostName + "</b></font>" + time + "</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">" + msg + "</p>");
    }

}

void MainWindow::newGrpAddedSlot(QListWidgetItem *item,QString ip)
{
    mChatLogsGrp->insert(ip,"");
    MultiCast *m = new MultiCast(ip,TM_GROUP_PORT);
    if(!m->isConnected()) {
        mGrpListWidget->banIP(ip);
        ip = mGrpListWidget->generateGrpIP();
        while(!m->reJoinMultiAddr(ip)) {
            mGrpListWidget->banIP(ip);
            ip = mGrpListWidget->generateGrpIP();
        }
        mGrpListWidget->setGrpItemIp(item,ip);
    }
    mMultiCastList->append(m);
    this->connect(m,SIGNAL(newGrpMagReceivedSig(QString,QString,QString,QString)),
                  this,SLOT(newGrpMagReceivedSlot(QString,QString,QString,QString)));
    this->connect(m,SIGNAL(newGrpHostSig(QString,QString,QString,QString)),
                  this,SLOT(newGrpHostSlot(QString,QString,QString,QString)));
    this->connect(m,SIGNAL(buddyGrpLeftSig(QString,QString,QString)),
                  this,SLOT(buddyGrpLeftSlot(QString,QString,QString)));
}

void MainWindow::newGrpHostSlot(QString grpName, QString grpIP, QString hostName, QString hostIP)
{
    mGrpListWidget->addGrpMap(grpName,grpIP,hostName,hostIP);
}

void MainWindow::newGrpMagReceivedSlot(QString grpIP, QString hostName, QString hostIP, QString msg)
{
    QString time = QDateTime::currentDateTime().toString(" yyyy-MM-dd hh:mm:ss");
    if(mGrpListWidget->getCurrentSelectedGrpIP() == grpIP) {

        ui->textChatLogsGrp->append("<font color=\"green\"><b>" + hostName + "</b></font>(" + hostIP + ")" + time);
        ui->textChatLogsGrp->append(msg);
    }
    else {
        QString hs = getGrpChatLog(grpIP);
        setGrpChatLog(grpIP,hs + "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><font color=\"green\"><b>" + hostName + "</b></font>(" + hostIP + ")" + time + "</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">" + msg + "</p>");
    }
}

void MainWindow::sendToAddGrpHostSlot(QString grpIP, QString hostName, QString hostIP)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString grpName = mGrpListWidget->getGrpItemByIP(grpIP)->text();
    out << MultiCast::NewGrpBuddy;
    out << grpName << grpIP << hostName << hostIP;
    mClientCast->sendData(data,grpIP,TM_GROUP_PORT);
    mClientCast->sendData(data,hostIP);
    QByteArray data2;
    QDataStream out2(&data2,QIODevice::WriteOnly);
    out2 << MultiCast::NewGrpBuddy;
    out2 << grpName << grpIP << tm_getLocalHostName() << mLocalIP;
    mClientCast->sendData(data2,grpIP,TM_GROUP_PORT);
    mClientCast->sendData(data2,hostIP);
}

void MainWindow::BuddyLeftSlot(QString hostName, QString hostIP)
{
    mPersonListWidget->buddyLeft(hostName,hostIP);
    mPersonList->removeAll(hostName + "-" + hostIP);
    mGrpListWidget->buddyGrpLeft(hostName,hostIP);
}

void MainWindow::buddyGrpLeftSlot(QString grpIP, QString hostName, QString hostIP)
{
    mGrpListWidget->buddyGrpLeft(grpIP,hostName,hostIP);
}

void MainWindow::grpLeftSlot(QString grpIP)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out << MultiCast::BuddyGrpLeft;
    out << grpIP << tm_getLocalHostName() << mLocalIP;
    mClientCast->sendData(data,grpIP,TM_GROUP_PORT);
}



void MainWindow::on_bth_send_clicked()
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString time = QDateTime::currentDateTime().toString(" yyyy-MM-dd hh:mm:ss");
    QString ip;
    if (ui->stackedWidgetPersonList->currentIndex() == 0) {
        ip = mPersonListWidget->getCurrentSelectedItemIP();

        ui->textChatLogs->append("<font color=\"orange\"><b>" + tm_getLocalHostName() + "</b></font>" + time);
        ui->textChatLogs->append(ui->textSend->toHtml());

        out << ClientCast::Message;
        out << tm_getLocalHostName();
        out << ui->textSend->toHtml();
        mClientCast->sendData(data,ip);
    }
    else {
        ip = mGrpListWidget->getCurrentSelectedGrpIP();
        ui->textChatLogsGrp->append("<font color=\"orange\"><b>" + tm_getLocalHostName() + "</b></font>" + time);
        ui->textChatLogsGrp->append(ui->textSend->toHtml());

        out << MultiCast::Message;
        out << ip;
        out << tm_getLocalHostName();
        out << mLocalIP;
        out << ui->textSend->toHtml();
        mClientCast->sendData(data,ip,TM_GROUP_PORT);
    }
    ui->textSend->setText("");
}

void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidgetPersonList->setCurrentIndex(0);
    ui->stackedWidgetChatLog->setCurrentIndex(0);

}

void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidgetPersonList->setCurrentIndex(1);
    ui->stackedWidgetChatLog->setCurrentIndex(1);
}
