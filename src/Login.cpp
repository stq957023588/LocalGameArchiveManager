#include "Login.h"
#include <QVariant>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QEventLoop>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpPart>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlQuery>
#include <QVBoxLayout>
#include "Api.h"
#include "SettingView.h"
#include "SettingRepository.h"
#include <QCoreApplication>

Login::Login(QWidget *parent) : QDialog()
{
	resize(400, 300);

	QLabel *usernameLable = new QLabel("用户名");
	usernameInput = new QLineEdit();

	QHBoxLayout *usernameLayout = new QHBoxLayout();
	usernameLayout->addWidget(usernameLable);
	usernameLayout->addWidget(usernameInput);

	QLabel *passwordLabel = new QLabel("密码");
	passwordInput = new QLineEdit();

	QHBoxLayout *passwordLayout = new QHBoxLayout();
	passwordLayout->addWidget(passwordLabel);
	passwordLayout->addWidget(passwordInput);

	QPushButton *loginBtn = new QPushButton("登录");
	QPushButton *cancelBtn = new QPushButton("取消");
	QHBoxLayout *btnLayout = new QHBoxLayout();
	btnLayout->addWidget(loginBtn, 0, Qt::AlignRight);
	btnLayout->addWidget(cancelBtn, 0, Qt::AlignRight);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(usernameLayout);
	layout->addLayout(passwordLayout);
	layout->addLayout(btnLayout);

	setLayout(layout);

	connect(loginBtn, SIGNAL(clicked()), this, SLOT(loginBtnClicked()));
	connect(cancelBtn, SIGNAL(clicked()), this, SLOT(cancelBtnClicked()));
}

void Login::loginBtnClicked()
{
	qDebug() << "Login btn clicked";
	QString username = usernameInput->text();
	QString password = passwordInput->text();

	QVariant accessToken = login(username, password, [=](QString errorMessage)
								 { QMessageBox::critical(nullptr, "登录失败", errorMessage); });

	if (accessToken.isNull())
	{
		return;
	}

	QCoreApplication *app = QCoreApplication::instance();
	app->setProperty("AccessToken", accessToken);
	bool updateResult = SettingRepository::updateSetting("AccessToken", accessToken.toString());

	if (!updateResult)
	{
		QMessageBox::critical(this, "系统异常", "存储Token失败！");
	}

	this->close();
}

void Login::cancelBtnClicked()
{
	this->close();
}

Login::~Login()
{
}
