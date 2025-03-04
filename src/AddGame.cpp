
#include "AddGame.h"
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QToolButton>
#include <QWidgetAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFrame>
#include <QFileDialog>
#include <QFileIconProvider>
#include "Game.h"

AddGame::AddGame(QFrame *parent) : QFrame()
{
    resize(400, 300);
    // 游戏名称行
    QLabel *gameNameLabel = new QLabel("游戏名称");

    gameNameInput = new QLineEdit();

    QHBoxLayout *gameNameLayout = new QHBoxLayout();
    gameNameLayout->addWidget(gameNameLabel);
    gameNameLayout->addWidget(gameNameInput);

    // 启动文件行
    QLabel *startupFilePathLabel = new QLabel("启动文件");

    startupFilePathInput = new QLineEdit();
    QToolButton *startupFilePathInputToolBtn = new QToolButton();
    startupFilePathInputToolBtn->setText("...");
    startupFilePathInputToolBtn->setCursor(Qt::ArrowCursor); // 如果不设置鼠标样式，鼠标移动到按钮上依旧显示为编辑框的鼠标样式。
    QWidgetAction *startupFilePathInputAction = new QWidgetAction(startupFilePathInput);
    startupFilePathInputAction->setDefaultWidget(startupFilePathInputToolBtn);
    startupFilePathInput->addAction(startupFilePathInputAction, QLineEdit::TrailingPosition);

    QHBoxLayout *startupFilePathLayout = new QHBoxLayout();
    startupFilePathLayout->addWidget(startupFilePathLabel);
    startupFilePathLayout->addWidget(startupFilePathInput);

    // 存档地址行
    QLabel *archivePathLabel = new QLabel("存档地址");

    archivePathInput = new QLineEdit();
    QToolButton *archivePathInputToolBtn = new QToolButton();
    archivePathInputToolBtn->setText("...");
    archivePathInputToolBtn->setCursor(Qt::ArrowCursor); // 如果不设置鼠标样式，鼠标移动到按钮上依旧显示为编辑框的鼠标样式。
    QWidgetAction *archivePathInputAction = new QWidgetAction(archivePathInput);
    archivePathInputAction->setDefaultWidget(archivePathInputToolBtn);
    archivePathInput->addAction(archivePathInputAction, QLineEdit::TrailingPosition);

    QHBoxLayout *archivePathLayout = new QHBoxLayout();
    archivePathLayout->addWidget(archivePathLabel);
    archivePathLayout->addWidget(archivePathInput);

    // 按钮行
    QPushButton *addBtn = new QPushButton("Add");
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(addBtn, 0, Qt::AlignRight);

    QVBoxLayout *body = new QVBoxLayout();
    body->addLayout(gameNameLayout);
    body->addLayout(startupFilePathLayout);
    body->addLayout(archivePathLayout);
    body->addLayout(btnLayout);

    setLayout(body);

    connect(addBtn, SIGNAL(clicked()), this, SLOT(addBtnClicked()));
    connect(startupFilePathInputToolBtn, SIGNAL(clicked()), this, SLOT(doSelectStartupFile()));
    connect(archivePathInputToolBtn, SIGNAL(clicked()), this, SLOT(doSelectArchivePath()));
}

void AddGame::addBtnClicked()
{
    Game game;
    game.name = gameNameInput->text();
    game.startupFilePath = startupFilePathInput->text();
    game.archiveDirPath = archivePathInput -> text();
    gameNameInput->clear();
    startupFilePathInput->clear();
    archivePathInput->clear();
    emit sendSignal(game);
    this->close();
}


void AddGame::doSelectStartupFile(){
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择游戏启动文件"), "", tr("All Files (*.exe)"));
    startupFilePathInput->setText(filePath);

    QFileInfo fileInfo(filePath);
    gameNameInput->setText(fileInfo.fileName());
}

void AddGame::doSelectArchivePath(){
    QString archiveDirName = QFileDialog::getExistingDirectory(this, tr("选择游戏存档目录"), "", QFileDialog::ShowDirsOnly);
    archivePathInput->setText(archiveDirName);
}


AddGame::~AddGame()
{
}
