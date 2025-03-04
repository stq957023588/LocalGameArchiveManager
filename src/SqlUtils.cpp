#include "SqlUtils.h"

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>

bool executeSql(QString sql)
{
	// 创建所需表
	QSqlQuery query;
	if (!query.exec(sql))
	{
		qDebug() << "执行SQL失败!失败原因：" << query.lastError().text();
		qDebug() << sql;
		return false;
	}

	return true;
}

void connectSqliteDb()
{
	QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
	database.setDatabaseName("mydatabase.db");

	if (!database.open())
	{
		qDebug() << "链接数据库失败！失败原因：" << database.lastError().text();
	}
}

