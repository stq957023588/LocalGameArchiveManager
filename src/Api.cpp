#include "Api.h"
#include "NetWorkManager.h"
#include <QHttpPart>
#include <QEventLoop>
#include <QObject>
#include <QNetworkReply>
#include <QDebug>
#include <QCoreApplication>
#include <functional>
#include "SettingRepository.h"
#include <QJsonDocument>
#include <QFile>
#include <QNetworkAccessManager>
#include "Login.h"

QVariant unLoginHandler()
{
	Login loginView;
	loginView.exec();
	QCoreApplication *app = QCoreApplication::instance();
	return app->property("AccessToken");
}

QVariant login(QString username, QString password, std::function<void(QString)> errorCallback)
{
	QNetworkAccessManager *manager = NetworkManager::instance();
	QVariant serverAddress = SettingRepository::settingValue("ServerAddress");

	if (serverAddress.isNull())
	{
		if (errorCallback)
		{
			errorCallback("未配置服务端地址（serverAddress）");
		}

		return QVariant();
	}

	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

	QHttpPart usernamePart;
	usernamePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"username\""));
	usernamePart.setBody(username.toUtf8());
	multiPart->append(usernamePart);

	QHttpPart passwordPart;
	passwordPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"password\""));
	passwordPart.setBody(password.toUtf8());
	multiPart->append(passwordPart);

	QEventLoop loop; // 循环
	QString loginUrlString = QString("%1/login").arg(serverAddress.toString());
	qDebug() << loginUrlString;
	QUrl loginUrl(loginUrlString);
	QNetworkRequest loginRequest(loginUrl);
	QNetworkReply *reply = manager->post(loginRequest, multiPart);	  // 这里是请求网址
	QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit())); // 绑定回复事件
	loop.exec();													  // 循环直到有回复
	if (reply->error() != QNetworkReply::NoError)
	{
		QString errorString = reply->errorString();
		reply->deleteLater(); // 释放内存
		if (errorCallback)
		{
			errorCallback("服务器异常：" + errorString);
		}
		return QVariant();
	}
	QString read = reply->readAll();
	reply->deleteLater(); // 释放内存
	QJsonDocument jsonDoc = QJsonDocument::fromJson(read.toUtf8());
	if (!jsonDoc.isObject())
	{
		if (errorCallback)
		{
			errorCallback("返回数据异常");
		}
		return QVariant();
	}
	QJsonObject json = jsonDoc.object();
	QString accessToken = json.value("access_token").toString();
	return accessToken;
}

QSharedPointer<GameArchive> latestArchive(QString gameName, std::function<void(QString)> errorCallback)
{

	QNetworkAccessManager *manager = NetworkManager::instance();
	QVariant serverAddress = SettingRepository::settingValue("ServerAddress");

	if (serverAddress.isNull())
	{
		if (errorCallback)
		{
			errorCallback("未配置服务端地址（serverAddress）");
		}
		return QSharedPointer<GameArchive>();
	}
	QVariant accessToken = SettingRepository::settingValue("AccessToken");
	if (accessToken.isNull())
	{
		accessToken = unLoginHandler();
	}
	if (accessToken.isNull())
	{
		if (errorCallback)
		{
			errorCallback("未登录");
		}
		return QSharedPointer<GameArchive>();
	}

	QEventLoop loop; // 循环
	QString detailUrlString = QString("%1/game/archive/latest?gameName=%2").arg(serverAddress.toString()).arg(gameName);
	qDebug() << detailUrlString;
	QUrl detailUrl(detailUrlString);
	QNetworkRequest detailRequest(detailUrl);
	detailRequest.setRawHeader("Authorization", accessToken.toString().toUtf8());
	QNetworkReply *reply = manager->get(detailRequest);				  // 这里是请求网址
	QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit())); // 绑定回复事件
	loop.exec();													  // 循环直到有回复

	if (reply->error() != QNetworkReply::NoError)
	{
		if (errorCallback)
		{
			errorCallback("服务器异常！" + reply->errorString());
		}
		reply->deleteLater(); // 释放内存
		return QSharedPointer<GameArchive>();
	}
	QString read = reply->readAll();
	QJsonDocument jsonDoc = QJsonDocument::fromJson(read.toUtf8());
	if (!jsonDoc.isObject())
	{
		QString errorMessage("Invalid JSON!" + read);
		if (errorCallback)
		{
			errorCallback("服务器异常！" + reply->errorString());
		}
		reply->deleteLater(); // 释放内存
		return QSharedPointer<GameArchive>();
	}

	QJsonObject json = jsonDoc.object();
	QJsonObject data = json.value("data").toObject();

	GameArchive gameArchive = GameArchive::fromJson(data);
	return QSharedPointer<GameArchive>::create(gameArchive);
}

QList<GameArchive> archiveList(QString gameName)
{
	QList<GameArchive> result;
	return result;
}

QSharedPointer<GameArchive> saveArchiveToRemote(QSharedPointer<GameArchive> localGameArchive, std::function<void(QString)> errorCallback)
{

	QNetworkAccessManager *manager = NetworkManager::instance();
	QVariant serverAddress = SettingRepository::settingValue("ServerAddress");

	if (serverAddress.isNull())
	{
		if (errorCallback)
		{
			errorCallback("未配置服务端地址（serverAddress）");
		}
		return QSharedPointer<GameArchive>();
	}

	QVariant accessToken = SettingRepository::settingValue("AccessToken");
	if (accessToken.isNull())
	{
		accessToken = unLoginHandler();
	}
	if (accessToken.isNull())
	{
		return QSharedPointer<GameArchive>();
	}

	QEventLoop loop; // 循环
	QString detailUrlString = QString("%1/game/archive").arg(serverAddress.toString());
	QUrl detailUrl(detailUrlString);
	QNetworkRequest detailRequest(detailUrl);
	detailRequest.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
	detailRequest.setRawHeader("Authorization", accessToken.toString().toUtf8());

	QJsonObject requestJson = localGameArchive->toJson();
	QByteArray requestByteArray = QJsonDocument(requestJson).toJson();

	QNetworkReply *reply = manager->post(detailRequest, requestByteArray); // 这里是请求网址
	QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));	   // 绑定回复事件
	loop.exec();														   // 循环直到有回复

	if (reply->error() != QNetworkReply::NoError)
	{
		if (errorCallback)
		{
			errorCallback("服务器异常！" + reply->errorString());
		}
		reply->deleteLater(); // 释放内存
		return QSharedPointer<GameArchive>();
	}
	QString read = reply->readAll();
	qDebug() << read;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(read.toUtf8());
	if (!jsonDoc.isObject())
	{
		QString errorMessage("Invalid JSON!" + read);
		if (errorCallback)
		{
			errorCallback("服务器异常！" + reply->errorString());
		}
		reply->deleteLater(); // 释放内存
		return QSharedPointer<GameArchive>();
	}

	QJsonObject json = jsonDoc.object();
	QJsonObject data = json.value("data").toObject();

	GameArchive remoteGameArchive = GameArchive::fromJson(data);
	qDebug() << "ID" << data["id"].toInt() << "IIII" << remoteGameArchive.id;
	return QSharedPointer<GameArchive>::create(remoteGameArchive);
}

void pushArchiveFile(int archiveId, QFile *file, std::function<void(QString)> errorCallback)
{
	QNetworkAccessManager *manager = NetworkManager::instance();
	QVariant serverAddress = SettingRepository::settingValue("ServerAddress");

	if (serverAddress.isNull())
	{
		if (errorCallback)
		{
			errorCallback("未配置服务端地址（serverAddress）");
		}
		return;
	}

	QVariant accessToken = SettingRepository::settingValue("AccessToken");
	if (accessToken.isNull())
	{
		accessToken = unLoginHandler();
	}
	if (accessToken.isNull())
	{
		return;
	}

	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

	QHttpPart archiveIdPart;
	archiveIdPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"archiveId\";"));
	archiveIdPart.setBody(QString::number(archiveId).toUtf8());
	multiPart->append(archiveIdPart);

	// 添加文件参数
	QHttpPart filePart;
	filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"archive.zip\""));
	filePart.setBodyDevice(file);
	file->setParent(multiPart);
	multiPart->append(filePart);

	QString saveUrlString = QString("%1/file/game/archive").arg(serverAddress.toString());
	QUrl saveUrl(saveUrlString);
	QNetworkRequest saveRequest(saveUrl);

	saveRequest.setRawHeader("Authorization", accessToken.toString().toUtf8());
	QNetworkReply *reply = manager->post(saveRequest, multiPart); // 这里是请求网址

	QEventLoop loop;												  // 循环
	QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit())); // 绑定回复事件
	loop.exec();
	// 循环直到有回复

	if (reply->error() != QNetworkReply::NoError)
	{
		if (errorCallback)
		{
			errorCallback(reply->errorString());
		}
	}
	reply->deleteLater(); // 释放内存
}

QSharedPointer<QByteArray> getArchiveFromRemote(int archiveId, std::function<void(QString)> errorCallback)
{
	QNetworkAccessManager *manager = NetworkManager::instance();
	QVariant serverAddress = SettingRepository::settingValue("ServerAddress");

	if (serverAddress.isNull())
	{
		if (errorCallback)
		{
			errorCallback("未配置服务端地址（serverAddress）");
		}
		return QSharedPointer<QByteArray>();
	}

	QVariant accessToken = SettingRepository::settingValue("AccessToken");
	if (accessToken.isNull())
	{
		accessToken = unLoginHandler();
	}
	if (accessToken.isNull())
	{
		return QSharedPointer<QByteArray>();
	}

	QEventLoop loop; // 循环
	QString detailUrlString = QString("%1/file/game/archive?archiveId=%2").arg(serverAddress.toString()).arg(archiveId);
	QUrl detailUrl(detailUrlString);
	QNetworkRequest detailRequest(detailUrl);
	detailRequest.setRawHeader("Authorization", accessToken.toString().toUtf8());
	QNetworkReply *reply = manager->get(detailRequest);				  // 这里是请求网址
	QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit())); // 绑定回复事件
	loop.exec();													  // 循环直到有回复

	if (reply->error() != QNetworkReply::NoError)
	{
		if (errorCallback)
		{
			errorCallback("服务器异常！" + reply->errorString());
		}
		reply->deleteLater(); // 释放内存
		return QSharedPointer<QByteArray>();
	}
	QByteArray read = reply->readAll();
	reply->deleteLater();
	return  QSharedPointer<QByteArray>(new QByteArray(read));
}
