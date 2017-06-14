#ifndef PERSONLISTWIDGET_H
#define PERSONLISTWIDGET_H

#include <QListWidget>
#include <QLinkedList>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>

#include <QDebug>

#include "personlistwidgetitem.h"
#include "grplistwidget.h"

class PersonListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit PersonListWidget(QWidget *parent = 0);
    ~PersonListWidget();
    QString getCurrentSelectedItemIP();
    QString getCurrentSelectedItemHost();
    QString getItemHost(QListWidgetItem *item,QString &hostName,QString &hostIP);
    void mousePressEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void addBuddy(QString hostname,QString ip);
    void buddyLeft(QString hostName, QString hostIP);
    void setGrpListWidget(GrpListWidget * g);
private:
    QMenu *mBlankMenu;
    QMenu *mPersonMenu;
    QMenu *mGrpListMenu;
    QAction *mAddBuddyAction;
    QAction *mDelBuddyAction;
    QAction *mAddToGrpAction;
    QListWidgetItem *mCurrentSelectedItem;
    QListWidgetItem *mRightSelectedItem;
    GrpListWidget *mGrpListWidget; // father to delete

    void initMenu();
    void updateGrpMenu();
private slots:
    void addBuddySlot();
    void delBuddySlot();
    void addToGrpSlot(QAction *action);

signals:
    void selectedItemChangedSig(QListWidgetItem *from,QListWidgetItem *to);
    void sendToAddGrpHostSig(QString grpIP,QString hostName,QString hostIP);
};

#endif // PERSONLISTWIDGET_H
