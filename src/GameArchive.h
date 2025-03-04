#pragma once

#include <QString>
#include <QDateTime>
#include <QJsonObject>

struct GameArchive
{
    int id;
    QString gameName;
    QString md5;
    QString archivePath;
    QDateTime saveTime;


    // 从 QJsonObject 初始化
    static GameArchive fromJson(const QJsonObject &json)
    {
        GameArchive gameArchive;
        gameArchive.id = json.value("id").toInt();
        gameArchive.gameName = json.value("gameName").toString();
        gameArchive.md5 = json.value("md5").toString();
        gameArchive.archivePath = json.value("archivePath").toString();

        QString datetimeString = json.value("saveTime").toString();
        gameArchive.saveTime = QDateTime::fromString(datetimeString, "yyyy-MM-dd HH:mm:ss");
        return gameArchive;
    }

    QJsonObject toJson() const
    {
        QJsonObject json;
        json["id"] = id;
        json["gameName"] = gameName;
        json["md5"] = md5;
        json["archivePath"] = archivePath;
        if (saveTime.isValid())
        {
            json["saveTime"] = saveTime.toString("yyyy-MM-dd HH:mm:ss");
        }
        return json;
    }

    GameArchive clone()
    {
        GameArchive gameArchive;
        gameArchive.id = id;
        gameArchive.gameName = gameName;
        gameArchive.md5 = md5;
        gameArchive.archivePath = archivePath;
        gameArchive.saveTime = saveTime;
        return gameArchive;
    }
};
