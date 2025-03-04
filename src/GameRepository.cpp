#include "GameRepository.h"
#include "SqlUtils.h"
#include <QSqlError>
#include <QSqlQuery>
#include "Game.h"

bool GameRepository::createTable()
{
	QString createTableSql = "create table if not exists game(name text PRIMARY KEY,icon blob,startup_file_path text,archive_dir_path text not null);";
	return executeSql(createTableSql);
}

bool GameRepository::insertRow(Game game)
{
	QString insertString = QString("insert into game (name, icon, startup_file_path, archive_dir_path) values('%1', :icon, '%2','%3') ")
							   .arg(game.name)
							   .arg(game.startupFilePath)
							   .arg(game.archiveDirPath);
	QSqlQuery query; // 执行sql语句
	query.prepare(insertString);
	query.bindValue(":icon", game.icon);
	if (!query.exec())
	{
		qDebug() << "添加游戏存档失败！失败原因：" << query.lastError().text();
		return false;
	}
	return true;
}

bool GameRepository::deleteRow(QString gameName)
{
	QString deleteSql = QString("delete from game where name ='%1'").arg(gameName);
	return executeSql(deleteSql);
}

QList<Game> GameRepository::select()
{
	QList<Game> result;
	QString sql = "select * from game";
	QSqlQuery query(sql);
	while (query.next())
	{
		Game game;
		game.name = query.value("name").toString();
		game.icon = query.value("icon").toByteArray();
		game.startupFilePath = query.value("startup_file_path").toString();
		game.archiveDirPath = query.value("archive_dir_path").toString();

		result.append(game);
	}

	return result;
}

QSharedPointer<Game> GameRepository::findByName(QString name)
{

	QString sql = QString("select * from game where name = '%1'").arg(name);
	QSqlQuery query(sql);
	if (query.next())
	{
		Game game;
		game.name = query.value("name").toString();
		game.icon = query.value("icon").toByteArray();
		game.startupFilePath = query.value("startup_file_path").toString();
		game.archiveDirPath = query.value("archive_dir_path").toString();
		return QSharedPointer<Game>::create(game);
	}
	return QSharedPointer<Game>();
}
GameRepository::GameRepository(/* args */)
{
}

GameRepository::~GameRepository()
{
}