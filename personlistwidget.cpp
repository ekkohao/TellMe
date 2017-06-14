#include "personlistwidget.h"

PersonListWidget::PersonListWidget(QWidget *parent):
    QListWidget(parent)
{
    setFocusPolicy(Qt::NoFocus);       // 去除item选中时的虚线边框
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//水平滚动条关闭
    mCurrentSelectedItem = nullptr;
    initMenu();
    //addBuddySlot();
}

PersonListWidget::~PersonListWidget()
{
    delete mGrpListMenu;
    delete mAddToGrpAction;
    delete mAddBuddyAction;
    delete mDelBuddyAction;
    delete mBlankMenu;
    delete mPersonMenu;
}

QString PersonListWidget::getCurrentSelectedItemHost()
{
    if(mCurrentSelectedItem == nullptr)
        return "";
    PersonListWidgetItem *item;
    QWidget * w;
    w = this->itemWidget(mCurrentSelectedItem);
    if ((item = dynamic_cast<PersonListWidgetItem *>(w))) {
        return item->getHost();
    }
    return "";
}

QString PersonListWidget::getItemHost(QListWidgetItem *item, QString &hostName, QString &hostIP)
{
    if(item == NULL)
        return "";
    PersonListWidgetItem *subitem;
    QWidget * w;
    w = this->itemWidget(item);
    if ((subitem = dynamic_cast<PersonListWidgetItem *>(w))) {
        hostName = subitem->name->text();
        hostIP = subitem->sign->text();
    }
    return "";
}

QString PersonListWidget::getCurrentSelectedItemIP()
{
    if(mCurrentSelectedItem == NULL)
        return "";
    PersonListWidgetItem * item;
    QWidget * w;
    w = this->itemWidget(mCurrentSelectedItem);
    if ((item = dynamic_cast<PersonListWidgetItem *>(w))) {
        return item->sign->text();
    }
    return "";
}

void PersonListWidget::mousePressEvent(QMouseEvent *event)
{
    QListWidgetItem * item;
    QListWidget::mousePressEvent(event);
    item = this->itemAt(QWidget::mapFromGlobal(QCursor::pos()));
    if(event->button() != Qt::LeftButton || item == NULL)
        return;
    if(mCurrentSelectedItem != item) {
        emit selectedItemChangedSig(mCurrentSelectedItem,item);
        mCurrentSelectedItem = item;
    }
}

void PersonListWidget::contextMenuEvent(QContextMenuEvent *event)
{

    QListWidget::contextMenuEvent(event);
    mRightSelectedItem = this->itemAt(QWidget::mapFromGlobal(QCursor::pos()));
    if (mRightSelectedItem == NULL) {
       mBlankMenu->exec(QCursor::pos());
       return;
    }
    updateGrpMenu();
    mPersonMenu->exec(QCursor::pos());
}

void PersonListWidget::addBuddy(QString hostname, QString ip)
{
    PersonListWidgetItem *buddy=new PersonListWidgetItem();   //创建一个自己定义的信息类
    buddy->headPath=":/images/images/touxiang.png";                          //设置头像路径
    buddy->name->setText(hostname);                  //设置用户名
    buddy->sign->setText(ip);   //设置个性签名
    //关键代码
    QListWidgetItem *newItem = new QListWidgetItem();       //创建一个newItem
    this->insertItem(this->count(),newItem); //将该newItem插入到后面
    this->setItemWidget(newItem, buddy); //将buddy赋给该newItem
}

void PersonListWidget::buddyLeft(QString hostName, QString hostIP)
{
    int size = this->count();
    QString name,ip;
    QListWidgetItem *item;
    for(int i = 0;i < size;++i) {
        item = this->item(i);
        getItemHost(item,name,ip);
        if(name == hostName && ip == hostIP) {
            this->removeItemWidget(item);
            delete item;
            break;
        }
    }

}

void PersonListWidget::setGrpListWidget(GrpListWidget *g)
{
    this->mGrpListWidget = g;
}

void PersonListWidget::initMenu()
{
    mBlankMenu = new QMenu();
    mPersonMenu = new QMenu();
    mGrpListMenu = new QMenu();
    mAddBuddyAction = new QAction("添加好友", this);
    mDelBuddyAction = new QAction("删除好友", this);
    mAddToGrpAction = new QAction("添加到组",this);
    mAddToGrpAction->setMenu(mGrpListMenu);
    
    mBlankMenu->addAction(mAddBuddyAction);
    mPersonMenu->addAction(mDelBuddyAction);
    mPersonMenu->addAction(mAddToGrpAction);

    this->connect(mAddBuddyAction,SIGNAL(triggered()),this,SLOT(addBuddySlot()));
    this->connect(mDelBuddyAction,SIGNAL(triggered()),this,SLOT(delBuddySlot()));
    this->connect(mGrpListMenu,SIGNAL(triggered(QAction*)),this,SLOT(addToGrpSlot(QAction*)));
}

void PersonListWidget::updateGrpMenu()
{
    mGrpListMenu->clear();
    QLinkedList<QString> list;
    mGrpListWidget->setAllGrpsName(list);
    if (list.isEmpty()) {
        QAction *action = mGrpListMenu->addAction("空");
        action->setDisabled(true);
    }
    else {
        foreach (QString name, list) {
            mGrpListMenu->addAction(name);
        }
    }

}

void PersonListWidget::addBuddySlot()
{

}

void PersonListWidget::delBuddySlot()
{

}

void PersonListWidget::addToGrpSlot(QAction *action)
{
    QListWidgetItem *item = mGrpListWidget->getGrpItemByName(action->text());
    mGrpListWidget->addGrpMap(item,this->itemWidget(mRightSelectedItem));

    QString grpIP = mGrpListWidget->getGrpItemIP(item);
    QString hostName,hostIP;
    getItemHost(mRightSelectedItem,hostName,hostIP);
    emit sendToAddGrpHostSig(grpIP,hostName,hostIP);
}
