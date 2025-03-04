#pragma once

#include <QObject>
#include <QList>
#include "Setting.h"

class SettingRepository : QObject
{
private:
	/* data */
public:
	SettingRepository(/* args */);
	~SettingRepository();

	static bool createTable();

	static bool insertRow(Setting setting);

	static bool updateSetting(QString code, QString value);

	static QVariant settingValue(QString code);

	static QList<Setting> select();
};
