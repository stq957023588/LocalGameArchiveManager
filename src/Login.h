#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QFrame>
#include <QDialog>
#include <QNetworkAccessManager>

class Login : public QDialog
{
    Q_OBJECT

private:
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;

	QNetworkAccessManager *manager;

private slots:
    void loginBtnClicked();
    void cancelBtnClicked();

signals:
    void sendSignal(QStringList);

public:
    Login(QWidget *parent = nullptr);
    ~Login();
};

