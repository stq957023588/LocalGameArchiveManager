#pragma once

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QSharedPointer>

class Game;
class GameRepository : public QObject
{

	Q_OBJECT
private:
	explicit GameRepository(/* args */);
	~GameRepository();
	/* data */
public:
	static bool createTable();

	static bool insertRow(Game game);

	static bool deleteRow(QString gameName);

	static QList<Game> select();

	static QSharedPointer<Game> findByName(QString name);

};



