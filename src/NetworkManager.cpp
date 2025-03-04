#include "NetworkManager.h"

// 初始化静态成员
QNetworkAccessManager *NetworkManager::m_instance = nullptr;

NetworkManager::NetworkManager(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
}

NetworkManager::~NetworkManager()
{
    delete networkManager;
}

QNetworkAccessManager *NetworkManager::instance()
{
    if (!m_instance) {
        m_instance = new QNetworkAccessManager();  // 确保只创建一个实例
    }
    return m_instance;
}
