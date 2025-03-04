#include "ApplicationInitUtils.h"
#include "SettingRepository.h"
#include "GameRepository.h"
#include "GameArchiveRepository.h"

#include <QCoreApplication>
#include <QVariant>
#include "GameRepository.h"

void init()
{
	initDatabase();
	initData();
}


void initDatabase()
{
	SettingRepository::createTable();
	GameRepository::createTable();
	GameArchiveRepository::createTable();
}

void initData()
{
	QVariant initalizeNextTime = SettingRepository::settingValue("Initialized");
	if (initalizeNextTime == "1")
	{
		qDebug() << "app has been initialized，skip init data";
		return;
	}

	Setting serverAddressSetting;
	serverAddressSetting.name = "ServerAddress";
	serverAddressSetting.description = "服务器地址";
	serverAddressSetting.value = "";
	serverAddressSetting.display = 1;

	SettingRepository::insertRow(serverAddressSetting);

	Setting userTokenSetting;
	userTokenSetting.name = "AccessToken";
	userTokenSetting.description = "用户请求Token";
	userTokenSetting.value = "";
	userTokenSetting.display = 0;

	SettingRepository::insertRow(userTokenSetting);

	Setting initializeNextTimeSetting;
	initializeNextTimeSetting.name = "Initialized";
	initializeNextTimeSetting.description = "是否已经初始化，0：否，1：是";
	initializeNextTimeSetting.value = "1";
	initializeNextTimeSetting.display = 0;

	SettingRepository::insertRow(initializeNextTimeSetting);
}