#include "GameArchiveRepository.h"
#include "SqlUtils.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include "GameArchive.h"

GameArchiveRepository::GameArchiveRepository(QObject *parent) : QObject(parent)
{
}

GameArchiveRepository::~GameArchiveRepository()
{
}

bool GameArchiveRepository::createTable()
{
	QString createTableSql = "create table if not exists game_archive ( id integer primary key autoincrement, game_name text not null, md5 text not null, archive_path text not null, save_time timestamp not null default (datetime('now', 'localtime')) )";
	return executeSql(createTableSql);
}

bool GameArchiveRepository::insertRow(QSharedPointer<GameArchive> po)
{
	QDateTime saveTime = QDateTime::currentDateTime();
	if (po->saveTime.isValid())
	{
		saveTime = po->saveTime;
	}

	QString insertSql = QString("insert into game_archive(game_name, md5, archive_path, save_time) VALUES ('%1','%2','%3','%4')")
							.arg(po->gameName)
							.arg(po->md5)
							.arg(po->archivePath)
							.arg(saveTime.toString("yyyy-MM-dd HH:mm:ss"));

	// 创建所需表
	QSqlQuery query;
	if (!query.exec(insertSql))
	{
		qDebug() << "执行SQL失败!失败原因：" << query.lastError().text();
		qDebug() << insertSql;
		return false;
	}
	po->id = query.lastInsertId().toInt();

	return true;
}
bool GameArchiveRepository::deleteRow(int id)
{
	QString sql = QString("delete from game_archive where id = %1").arg(id);
	return executeSql(sql);
}

bool GameArchiveRepository::deleteByGameName(QString gameName)
{
	QString sql = QString("delete from game_archive where game_name = '%1'").arg(gameName);
	return executeSql(sql);
}


int GameArchiveRepository::count(QString gameName, QString md5)
{
	QString selectSql = QString("select count(*) as `count` from game_archive where game_name = '%1' and md5 = '%2'").arg(gameName).arg(md5);
	QSqlQuery query(selectSql);
	query.next();
	return query.value("count").toInt();
}

QList<GameArchive> GameArchiveRepository::selectList(QString gameName)
{
	QString sql = QString("select * from game_archive where game_name = '%1'").arg(gameName);

	QList<GameArchive> archives;
	QSqlQuery query(sql);
	while (query.next())
	{
		GameArchive gameArchive;

		gameArchive.id = query.value("id").toInt();
		gameArchive.gameName = gameName;
		gameArchive.md5 = query.value("md5").toString();
		gameArchive.archivePath = query.value("archive_path").toString();
		gameArchive.saveTime = query.value("save_time").toDateTime();
		archives.append(gameArchive);
	}

	return archives;
}

QSharedPointer<GameArchive> GameArchiveRepository::latest(QString gameName)
{
	QString sql = QString("select * from game_archive where game_name = '%1' order by id desc limit 1").arg(gameName);

	QSqlQuery query(sql);
	if (query.next())
	{
		GameArchive gameArchive;

		gameArchive.id = query.value("id").toInt();
		gameArchive.gameName = gameName;
		gameArchive.md5 = query.value("md5").toString();
		gameArchive.archivePath = query.value("archive_path").toString();
		gameArchive.saveTime = query.value("save_time").toDateTime();

		return QSharedPointer<GameArchive>::create(gameArchive);
	}
	return QSharedPointer<GameArchive>();
}

QSharedPointer<GameArchive> GameArchiveRepository::findById(QString id)
{

	QSqlQuery query;
	// 覆盖
	QString archiveSql = QString("select * from game_archive where id = %1").arg(id);
	if (!query.exec(archiveSql))
	{
		qDebug() << archiveSql;
		qDebug() << "查询存档失败" << query.lastError().text();
		return QSharedPointer<GameArchive>();
	}
	if (query.next())
	{
		GameArchive gameArchive;

		gameArchive.id = query.value("id").toInt();
		gameArchive.gameName = query.value("game_name").toString();
		gameArchive.md5 = query.value("md5").toString();
		gameArchive.archivePath = query.value("archive_path").toString();
		gameArchive.saveTime = query.value("save_time").toDateTime();

		return QSharedPointer<GameArchive>::create(gameArchive);
	}
	return QSharedPointer<GameArchive>();
}
