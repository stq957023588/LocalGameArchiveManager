#include "SettingRepository.h"
#include "SqlUtils.h"
#include <QSqlQuery>

bool SettingRepository::createTable()
{
	QString sql = "create table if not exists setting(name text PRIMARY KEY, description text, value text, display int default 1);";
	return executeSql(sql);
}

bool SettingRepository::insertRow(Setting setting)
{
	QString sql = QString("insert into setting values('%1', '%2', '%3', %4)")
					  .arg(setting.name)
					  .arg(setting.description)
					  .arg(setting.value)
					  .arg(setting.display);

	return executeSql(sql);
}
bool SettingRepository::updateSetting(QString name, QString value)
{
	QString sql = QString("update setting set value = '%1' where name = '%2'")
					  .arg(value)
					  .arg(name);
	return executeSql(sql);
}
QVariant SettingRepository::settingValue(QString name)
{
	QString sql = QString("select * from setting where name = '%1'").arg(name);
	QSqlQuery query(sql);
	if (query.next())
	{
		return query.value("value");
	}
	return QVariant();
}
QList<Setting> SettingRepository::select()
{
	QList<Setting> result;
	QString sql = "select * from setting where display = 1";
	QSqlQuery query(sql);
	while (query.next())
	{
		Setting setting;
		setting.name = query.value("name").toString();
		setting.description = query.value("description").toString();
		setting.value = query.value("value").toString();

		result.append(setting);
	}

	return result;
}