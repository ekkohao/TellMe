#ifndef GRPLISTWIDGET_H
#define GRPLISTWIDGET_H

#include <QListWidget>
#include <QLinkedList>
#include <QLineEdit>
#include <QMouseEvent>
#include <QAction>
#include <QMenu>
#include <QKeyEvent>

#include "personlistwidgetitem.h"
#include "netcommon.h"

#include <QDebug>

class GrpListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit GrpListWidget(QWidget *parent = 0);
    ~GrpListWidget();
    QString getCurrentSelectedGrpIP();
    QString getGrpItemIP(QListWidgetItem *item);
    void mousePressEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void keyPressEvent(QKeyEvent *event);
    QListWidgetItem *getGrpItemByName(QString name);
    QListWidgetItem *getGrpItemByIP(QString ip);
    void setAllGrpsName(QLinkedList<QString> &list);
    void addGrpMap(QListWidgetItem *item, QWidget *subitem);
    void addGrpMap(QString grpName,QString grpIP, QString hostName,QString hostIP);
    void buddyGrpLeft(QString hostName,QString hostIP);
    void buddyGrpLeft(QString grpIP, QString hostName,QString hostIP);
    void delGrpMap(QListWidgetItem *item,QListWidgetItem *subitem);
    void banIP(QString ip);
    QString generateGrpIP();
    void setGrpItemIp(QListWidgetItem *item,QString ip);
    QString getItemHost(QListWidgetItem *item, QString &hostName, QString &hostIP);
private:
    QMenu *mBlankMenu;
    QMenu *mGrpMenu;
    QAction *mAddGrpAction;
    QAction *mGrpReName;
    QAction *mDelGrpACtion;
    QLineEdit *mGrpNameEdit;
    QListWidgetItem *mCurrentSelectedGrp;
    QListWidgetItem *mRightSelectedGrp;
    QMap<QListWidgetItem *,QLinkedList<QListWidgetItem *> *> *mGrpMap;
    QMap<QListWidgetItem *,bool> *mGrpIsHide;
    QMap<QListWidgetItem *,QString> *mGrpIPMap;
    QLinkedList<QString> *mBanList;

    void initMenu();
    void delGrpAll();
    bool itemListContains(QLinkedList<QListWidgetItem *> *list, QWidget *w);
    bool itemListContains(QLinkedList<QListWidgetItem *> *list, QString host);
    PersonListWidgetItem * newPersonListWidgetItem(PersonListWidgetItem *item);
    void grpShow(QListWidgetItem *item);
    void grpHide(QListWidgetItem *item);
private slots:
    void addGrpSlot();
    void buddyGrpLeftSlot();
    QString getUniqueGrpName(QString name);

signals:
    void selectedItemChangedSig(QListWidgetItem *from,QListWidgetItem *to);
    void grpLeftSig(QString ip);
    void newGrpAddedSig(QListWidgetItem *item,QString ip);
};

#endif // GRPLISTWIDGET_H
