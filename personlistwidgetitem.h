#ifndef PERSONLISTWIDGETITEM_H
#define PERSONLISTWIDGETITEM_H

#include <QWidget>
#include <QLabel>
#include <QEvent>
#include <QPainter>

class PersonListWidgetItem : public QWidget
{
    Q_OBJECT
public:
    explicit PersonListWidgetItem(QWidget *parent = nullptr);

    void initUI();//初始化Ui
    QWidget *head;  //头像
    QLabel *name;  //用户名
    QLabel *sign;  //个性签名
    QString headPath;//头像路径
    bool eventFilter(QObject *obj, QEvent *event);//事件过滤器
    QString getHost();

signals:

public slots:
};

#endif // PERSONLISTWIDGETITEM_H
