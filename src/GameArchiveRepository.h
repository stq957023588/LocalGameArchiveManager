#pragma once

#include <QObject>
#include <QList>
#include <QSharedPointer>

class GameArchive;

class GameArchiveRepository : public QObject
{

	Q_OBJECT
private:
	explicit GameArchiveRepository(QObject *parent = nullptr);
	~GameArchiveRepository();
	/* data */
public:
	static bool createTable();

	static bool insertRow(QSharedPointer<GameArchive> po);

	static bool deleteRow(int id);

	static bool deleteByGameName(QString gameName);

	static int count(QString gameName, QString md5);

	static QList<GameArchive> selectList(QString gameName);

	static QSharedPointer<GameArchive> latest(QString gameName);

	static QSharedPointer<GameArchive> findById(QString id);
};
