#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    static QNetworkAccessManager *instance();

private:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    static QNetworkAccessManager *m_instance;  // 静态实例
    QNetworkAccessManager *networkManager;  // 实际的 QNetworkAccessManager 实例
};

#endif // NETWORKMANAGER_H
