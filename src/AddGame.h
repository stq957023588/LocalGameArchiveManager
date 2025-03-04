#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QFrame>
#include "Game.h"

class AddGame : public QFrame
{
    Q_OBJECT

private:
    QLineEdit *gameNameInput;
    QLineEdit *startupFilePathInput;
    QLineEdit *archivePathInput;

private slots:
    void addBtnClicked();
    void doSelectStartupFile();
    void doSelectArchivePath();

signals:
    void sendSignal(Game game);

public:
    AddGame(QFrame *parent = nullptr);
    ~AddGame();
};