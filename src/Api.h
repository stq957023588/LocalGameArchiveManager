#ifndef C65B28BA_3E87_455D_949D_5E0572595526
#define C65B28BA_3E87_455D_949D_5E0572595526

#include <QString>
#include "GameArchive.h"
#include <QList>
#include <QByteArray>
#include <functional>

class QFile;

QVariant login(QString username, QString password, std::function<void(QString)> errorCallback = nullptr);

QSharedPointer<GameArchive> latestArchive(QString gameName, std::function<void(QString)> errorCallback = nullptr);

QList<GameArchive> archiveList(QString gameName);

QSharedPointer<GameArchive> saveArchiveToRemote(QSharedPointer<GameArchive> gameArchive, std::function<void(QString)> errorCallback = nullptr);

void pushArchiveFile(int archiveId, QFile *file, std::function<void(QString)> errorCallback = nullptr);

QSharedPointer<QByteArray> getArchiveFromRemote(int archiveId, std::function<void(QString)> errorCallback = nullptr);

#endif /* C65B28BA_3E87_455D_949D_5E0572595526 */
