#ifndef NETCOMMON_H
#define NETCOMMON_H

#include <QMap>
#include <QString>
#include <QHostInfo>

#define TM_COMMON_IP "238.238.228.238"
#define TM_CLIENT_PORT 35570
#define TM_GROUP_PORT 35560
#define TM_COMMON_PORT 35780

inline void tmjson_mixstring2map(QString s, QMap<QString,QString> &map);
inline void tmjson_map2mixstring(QMap<QString,QString> map, QString &s);
inline QString tm_getLocalHostName();
inline QString tm_getIPv4Addr(QString s);
inline bool tm_isLocalHost(QString hostName,QString hostIP);
inline QString tm_getIP();

void tmjson_mixstring2map(QString s, QMap<QString,QString> &map) {
    QString key, value ,temp;
    bool flag = true;
    int lk, k;
    int i = 0,len = s.size();
    map.clear();
    // "2-aa4-bbbb2-aa4-bbbb"
    while(i < len) {
        k = s.indexOf('-',i);
        if (k < 0) {
            map.clear();
            return;
        }
        temp = s.mid(i,k - i);
        lk = temp.toInt();
        if(flag)
            key = s.mid(k + 1,lk);
        else {
            value = s.mid(k + 1,lk);
            map.insert(key,value);
        }
        flag = !flag;
        i = k + lk + 1;
    }
}

void tmjson_map2mixstring(QMap<QString,QString> map, QString &s) {
    QMap<QString,QString>::iterator itr = map.begin();
    s.clear();
    while ( itr != map.end()) {
        QString key = itr.key();
        QString value = itr.value();
        s += (QString::number(key.size()) + "-" + key + QString::number(value.size()) + "-" + value);
        ++itr;
    }
}

QString tm_getLocalHostName() {
    return QHostInfo::localHostName();
}

QString tm_getIPv4Addr(QString s) {
    return s.mid(s.lastIndexOf(':') + 1);
}

bool tm_isLocalHost(QString hostName,QString hostIP) {
    QString localHostName = QHostInfo::localHostName();
    if(localHostName != hostName)
        return false;

    QHostInfo info = QHostInfo::fromName(localHostName);

    foreach(QHostAddress address, info.addresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address.toString() == hostIP)
            return true;
    }
    return false;
}

QString tm_getIP() {
    QString localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);

    foreach(QHostAddress address, info.addresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol)
            return address.toString();
    }
    return "";
}
#endif // NETCOMMON_H
