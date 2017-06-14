#include "personlistwidgetitem.h"

PersonListWidgetItem::PersonListWidgetItem(QWidget *parent) : QWidget(parent)
{
    initUI();
}
//初始化Ui
void PersonListWidgetItem::initUI()
{
    //初始化
    head=new QWidget(this);
    name=new QLabel(this);
    sign=new QLabel(this);
    //设置头像大小
    head->setFixedSize(40,40);
    //设置个性签名字体为灰色
    QPalette color;
    color.setColor(QPalette::Text,Qt::gray);
    sign->setPalette(color);
    //布局
    head->move(7,7);
    name->move(54,10);
    sign->move(54,27);
    //装载事件过滤器
    head->installEventFilter(this);
}
//事件过滤器，主要是为了让图片能够全部填充在head里面
bool PersonListWidgetItem::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == head)
    {
        if(event->type() == QEvent::Paint)
        {
            QPainter painter(head);
            painter.drawPixmap(head->rect(), QPixmap(headPath));
        }
    }
    return QWidget::eventFilter(obj, event);
}

QString PersonListWidgetItem::getHost()
{
    return name->text() + "-" + sign->text();
}
