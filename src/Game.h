#pragma once

#include <QString>
#include <QIcon>

struct Game
{
	int id;
	QString name;
	QByteArray icon;
	QString startupFilePath;
	QString archiveDirPath;
};

