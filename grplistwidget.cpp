#include "grplistwidget.h"

GrpListWidget::GrpListWidget(QWidget *parent) :
    QListWidget(parent)
{
    setFocusPolicy(Qt::NoFocus);       // 去除item选中时的虚线边框  
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//水平滚动条关闭
    mGrpMap = new QMap<QListWidgetItem *,QLinkedList<QListWidgetItem *> *>();
    mGrpIsHide = new QMap<QListWidgetItem *,bool>();
    mGrpIPMap = new QMap<QListWidgetItem *,QString>();
    mBanList = new QLinkedList<QString>();
    mBanList->append(TM_COMMON_IP);
    initMenu();
}

GrpListWidget::~GrpListWidget()
{
    delGrpAll();
    delete mBanList;
    delete mGrpIPMap;
    delete mGrpMap;
    delete mGrpIsHide;
    delete mBlankMenu;
    delete mGrpMenu;
    delete mAddGrpAction;
    delete mDelGrpACtion;
    delete mGrpReName;
    delete mGrpNameEdit;
}

QString GrpListWidget::getCurrentSelectedGrpIP()
{
    return mGrpIPMap->value(mCurrentSelectedGrp);
}

QString GrpListWidget::getGrpItemIP(QListWidgetItem *item)
{
    return mGrpIPMap->value(item);
}

void GrpListWidget::mousePressEvent(QMouseEvent *event)
{
    QListWidgetItem * item;
    if(mGrpNameEdit->isVisible() && !(mGrpNameEdit->rect().contains(event->pos())))  
    {  
        if(mGrpNameEdit->text()!=NULL)  
            mCurrentSelectedGrp->setText(getUniqueGrpName(mGrpNameEdit->text()));
        mGrpMap->insert(mCurrentSelectedGrp,nullptr);//加到容器groupMap里，key和value都为组
        mGrpIsHide->insert(mCurrentSelectedGrp,true);  //设置该组隐藏状态
        mGrpIPMap->insert(mCurrentSelectedGrp,generateGrpIP());
        emit newGrpAddedSig(mCurrentSelectedGrp,mGrpIPMap->value(mCurrentSelectedGrp));
        mGrpNameEdit->setText("");
        mGrpNameEdit->hide();  
    }
    if(event->button() != Qt::LeftButton)
        return;
    item = this->itemAt(mapFromGlobal(QCursor::pos()));//鼠标位置的Item，不管右键左键都获取

    if (item != NULL && mGrpMap->contains(item)) {
        QListWidget::mousePressEvent(event);
        if(mCurrentSelectedGrp != item)
            emit selectedItemChangedSig(mCurrentSelectedGrp,item);
        mCurrentSelectedGrp = item;
        if(mGrpIsHide->value(item)) {                              //如果先前是隐藏，则显示
            grpShow(item);
        }  
        else {
            grpHide(item);
        }
    }
}

void GrpListWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QListWidget::contextMenuEvent(event);
    QListWidgetItem * item;

    item = this->itemAt(mapFromGlobal(QCursor::pos()));
    mRightSelectedGrp = item;
    if (item == NULL) {
        mBlankMenu->exec(QCursor::pos());
    }
    else if (mGrpMap->contains(item)) {
        mGrpMenu->exec(QCursor::pos());
    }
}

void GrpListWidget::keyPressEvent(QKeyEvent *event)
{
    QListWidget::keyPressEvent(event);

    if(mGrpNameEdit->isVisible() && event->key() == Qt::Key_Return)
    {
        if(mGrpNameEdit->text()!=NULL)
            mCurrentSelectedGrp->setText(getUniqueGrpName(mGrpNameEdit->text()));
        mGrpMap->insert(mCurrentSelectedGrp,nullptr);//加到容器groupMap里，key和value都为组
        mGrpIsHide->insert(mCurrentSelectedGrp,true);  //设置该组隐藏状态
        mGrpIPMap->insert(mCurrentSelectedGrp,generateGrpIP());
        emit newGrpAddedSig(mCurrentSelectedGrp,mGrpIPMap->value(mCurrentSelectedGrp));
        mGrpNameEdit->setText("");
        mGrpNameEdit->hide();
    }
}

QListWidgetItem *GrpListWidget::getGrpItemByName(QString name)
{
    foreach (QListWidgetItem *item, mGrpMap->keys()) {
        if(item->text() == name)
            return item;
    }
    return nullptr;
}

QListWidgetItem *GrpListWidget::getGrpItemByIP(QString ip)
{
    foreach (QListWidgetItem *item, mGrpMap->keys()) {
        if(mGrpIPMap->value(item) == ip)
            return item;
    }
    return nullptr;
}

void GrpListWidget::setAllGrpsName(QLinkedList<QString> &list)
{
    foreach (QListWidgetItem *item, mGrpMap->keys()) {
        list.append(item->text());
    }
}

void GrpListWidget::initMenu()
{
    mBlankMenu = new QMenu();
    mGrpMenu = new QMenu();
    mAddGrpAction = new QAction("添加群聊",this);
    mDelGrpACtion = new QAction("退出群聊",this);
    mGrpReName = new QAction("重命名",this);
    mGrpNameEdit = new QLineEdit();
    mCurrentSelectedGrp = NULL;
    
    mBlankMenu->addAction(mAddGrpAction);
    mGrpMenu->addAction(mDelGrpACtion);
    //mGrpMenu->addAction(mGrpReName);
  
    mGrpNameEdit->setParent(this);  //设置父类  
    mGrpNameEdit->hide(); //设置初始时隐藏  
    mGrpNameEdit->setPlaceholderText("未命名");//设置初始时的内容

    this->connect(mAddGrpAction,SIGNAL(triggered()),this,SLOT(addGrpSlot()));
    this->connect(mDelGrpACtion,SIGNAL(triggered()),this,SLOT(buddyGrpLeftSlot()));
}

void GrpListWidget::addGrpMap(QListWidgetItem *item, QWidget *subitem)
{
    if(!mGrpMap->contains(item))
        return;
    QLinkedList<QListWidgetItem *> *list = mGrpMap->value(item);
    if(list == nullptr) {
        QListWidgetItem * newitem = new QListWidgetItem();
        list = new QLinkedList<QListWidgetItem *>();
        list->append(newitem);
        mGrpMap->insert(item,list);
        this->insertItem(row(item) + 1,newitem);
        this->setItemWidget(newitem,newPersonListWidgetItem(dynamic_cast<PersonListWidgetItem *>(subitem)));
        if(mGrpIsHide->value(item))
            newitem->setHidden(true);
    }
    else if(!itemListContains(list,subitem)) {
        QListWidgetItem * newitem = new QListWidgetItem();
        list->append(newitem);
        mGrpMap->insert(item,list);
        this->insertItem(row(item) + 1,newitem);
        this->setItemWidget(newitem,newPersonListWidgetItem(dynamic_cast<PersonListWidgetItem *>(subitem)));
        if(mGrpIsHide->value(item))
            newitem->setHidden(true);
    }
}

void GrpListWidget::addGrpMap(QString grpName,QString grpIP, QString hostName, QString hostIP)
{

    qDebug() << grpName;
    QListWidgetItem *item = getGrpItemByIP(grpIP);
    if(item == nullptr) {
        QListWidgetItem *newItem=new QListWidgetItem(QIcon(":/images/images/arrowRight.png"),grpName);
        newItem->setSizeHint(QSize(this->width(),25));
        this->addItem(newItem);
        mGrpMap->insert(newItem,nullptr);//加到容器groupMap里，key和value都为组
        mGrpIsHide->insert(newItem,true);  //设置该组隐藏状态
        mGrpIPMap->insert(newItem,grpIP);
        emit newGrpAddedSig(newItem,mGrpIPMap->value(newItem));
        item = newItem;
    }
    QLinkedList<QListWidgetItem *> *list = mGrpMap->value(item);
    if(list == nullptr) {
        QListWidgetItem * newitem = new QListWidgetItem();
        PersonListWidgetItem *personItem = new PersonListWidgetItem();
        list = new QLinkedList<QListWidgetItem *>();
        list->append(newitem);
        mGrpMap->insert(item,list);
        personItem->headPath = ":/images/images/touxiang.png";
        personItem->name->setText(hostName);
        personItem->sign->setText(hostIP);
        this->insertItem(row(item) + 1,newitem);
        this->setItemWidget(newitem,personItem);
        if(mGrpIsHide->value(item))
            newitem->setHidden(true);
    }
    else if(!itemListContains(list,hostName + "-" + hostIP)) {
        QListWidgetItem * newitem = new QListWidgetItem();
        PersonListWidgetItem *personItem = new PersonListWidgetItem();
        list->append(newitem);
        mGrpMap->insert(item,list);
        personItem->headPath = ":/images/images/touxiang.png";
        personItem->name->setText(hostName);
        personItem->sign->setText(hostIP);
        this->insertItem(row(item) + 1,newitem);
        this->setItemWidget(newitem,personItem);
        if(mGrpIsHide->value(item))
            newitem->setHidden(true);
    }
}

void GrpListWidget::buddyGrpLeft(QString hostName, QString hostIP)
{
    foreach (QString ip, mGrpIPMap->values()) {
        buddyGrpLeft(ip,hostName,hostIP);
    }
}

void GrpListWidget::buddyGrpLeft(QString grpIP, QString hostName, QString hostIP)
{
    qDebug() << grpIP << hostName << hostIP;
    QListWidgetItem *item = nullptr;
    foreach (item, mGrpIPMap->keys()) {
        if(mGrpIPMap->value(item) == grpIP)
            break;
    }
    QLinkedList<QListWidgetItem *> *list = mGrpMap->value(item);
    int row = this->row(item);
    int size = this->count();
    QString name,ip;
    for(++row;row < size;++row) {
        item = this->item(row);
        if(mGrpIPMap->contains(item))
            break;
        getItemHost(item,name,ip);
        if(hostName == name && hostIP == ip) {
            list->removeOne(item);
            this->removeItemWidget(item);
            delete item;
            break;
        }
    }
}

void GrpListWidget::delGrpMap(QListWidgetItem *item, QListWidgetItem *subitem)
{
    if(!mGrpMap->contains(item))
        return;
    QLinkedList<QListWidgetItem *> *list = mGrpMap->value(item);
    if(list == nullptr)
        return;
    list->removeOne(subitem);
}

void GrpListWidget::banIP(QString ip)
{
    if(!mBanList->contains(ip))
        mBanList->append(ip);
}

void GrpListWidget::delGrpAll()
{
    QMap<QListWidgetItem *,QLinkedList<QListWidgetItem *> *>::iterator itr = mGrpMap->begin();
    while(itr != mGrpMap->end()) {
        delete itr.value();
        ++itr;
    }
}

bool GrpListWidget::itemListContains(QLinkedList<QListWidgetItem *> *list, QWidget *w)
{
    PersonListWidgetItem * item;
    QString host;
    if ((item = dynamic_cast<PersonListWidgetItem *>(w))) {
        host = item->getHost();
    }
    foreach (QListWidgetItem* ii, *list) {
        w = this->itemWidget(ii);
        if ((item = dynamic_cast<PersonListWidgetItem *>(w))) {
            if(host == item->getHost())
                return true;
        }
    }
    return false;
}

bool GrpListWidget::itemListContains(QLinkedList<QListWidgetItem *> *list, QString host)
{
    PersonListWidgetItem * item;
    QWidget *w;
    foreach (QListWidgetItem* ii, *list) {
        w = this->itemWidget(ii);
        if ((item = dynamic_cast<PersonListWidgetItem *>(w))) {
            if(host == item->getHost())
                return true;
        }
    }
    return false;
}

PersonListWidgetItem *GrpListWidget::newPersonListWidgetItem(PersonListWidgetItem *item)
{
    PersonListWidgetItem * newitem = new PersonListWidgetItem();
    newitem->headPath = ":/images/images/touxiang.png";
    newitem->name->setText(item->name->text());
    newitem->sign->setText(item->sign->text());
    return newitem;
}

void GrpListWidget::grpShow(QListWidgetItem *item)
{
    foreach (QListWidgetItem *i, mGrpMap->keys()) {
        if(!mGrpIsHide->value(i)) {
            grpHide(i);
            break;
        }
    }
    QLinkedList<QListWidgetItem *> *list = mGrpMap->value(item);
    if(list != nullptr)
        foreach(QListWidgetItem* subItem, *(list))//遍历组的对应的项（包括自身和好友）
            subItem->setHidden(false);
    mGrpIsHide->insert(item,false);                          //设置该组为显示状态
    item->setIcon(QIcon(":/images/images/arrowDown.png"));
}

void GrpListWidget::grpHide(QListWidgetItem *item)
{
    QLinkedList<QListWidgetItem *> *list = mGrpMap->value(item);
    if(list != nullptr)
        foreach(QListWidgetItem* subItem, *(list))//遍历组的对应的项（包括自身和好友）
            subItem->setHidden(true);                           //好友全部隐藏
    mGrpIsHide->insert(item,true);                         //设置该组为隐藏状态
    item->setIcon(QIcon(":/images/images/arrowRight.png"));
}

QString GrpListWidget::generateGrpIP()
{
    int a,b,c,d;
    QString ip;
    for(a = 224;a < 239;++a) {
        for(b = 0;b < 256;++b) {
            for(c = 1;b < 256;++b) {
                for(d = 0;b < 256;++b) {
                    ip = QString("%1.%2.%3.%4").arg(a).arg(b).arg(c).arg(d);
                    if(!mBanList->contains(ip) && !mGrpIPMap->values().contains(ip))
                        return ip;
                }
            }
        }
    }
    return "";
}

void GrpListWidget::setGrpItemIp(QListWidgetItem *item, QString ip)
{
    mGrpIPMap->insert(item,ip);
}

QString GrpListWidget::getItemHost(QListWidgetItem *item, QString &hostName, QString &hostIP)
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

void GrpListWidget::addGrpSlot()
{
    QListWidgetItem *newItem=new QListWidgetItem(QIcon(":/images/images/arrowRight.png"),"未命名");    //创建一个Item
    newItem->setSizeHint(QSize(this->width(),25));//设置宽度、高度
    this->addItem(newItem);         //加到QListWidget中
    mGrpNameEdit->raise();
    mGrpNameEdit->setText(tr("未命名")); //设置默认内容
    mGrpNameEdit->selectAll();        //设置全选
    mGrpNameEdit->setGeometry(this->visualItemRect(newItem).left()+25,this->visualItemRect(newItem).top() + 2,this->visualItemRect(newItem).width() - 28,this->visualItemRect(newItem).height()-2);//出现的位置
    mGrpNameEdit->show();              //显示
    mGrpNameEdit->setFocus();          //获取焦点
    emit selectedItemChangedSig(mCurrentSelectedGrp,newItem);
    mCurrentSelectedGrp = newItem;     // 因为要给group命名，所以当前的currentItem设为该group
}

void GrpListWidget::buddyGrpLeftSlot()
{
    QString ip = getGrpItemIP(mRightSelectedGrp);

    foreach (QListWidgetItem *item, *(mGrpMap->value(mRightSelectedGrp))) {
        this->removeItemWidget(item);
        delete item;
        item = nullptr;
    }

    this->removeItemWidget(mRightSelectedGrp);

    mGrpIPMap->remove(mRightSelectedGrp);
    mGrpMap->remove(mRightSelectedGrp);
    mGrpIsHide->remove(mRightSelectedGrp);

    delete mRightSelectedGrp;
    mRightSelectedGrp = nullptr;
    emit grpLeftSig(ip);
}

QString GrpListWidget::getUniqueGrpName(QString name)
{
    int id = 1;
    if(getGrpItemByName(name) == nullptr)
        return name;
    QString newname(name + "_" + QString::number(id));
    while(getGrpItemByName(newname) != nullptr) {
        newname = name + "_" + QString::number(++id);
    }
    return newname;
}
