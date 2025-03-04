#include <QMainWindow>
#include "MainWindow.h"
#include <QToolBar>
#include <QMessageBox>
#include <QStackedWidget>
#include <QHBoxLayout>
#include "GameArchiveManager.h"
#include "SettingView.h"
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
        this->resize(800, 600);

        QAction *homeAction = new QAction("主页");
        QAction *configAction = new QAction("设置");

        QToolBar *toolBar = new QToolBar(this);
        addToolBar(Qt::TopToolBarArea, toolBar);

        toolBar->addAction(homeAction);
        toolBar->addAction(configAction);

        toolBar->setFloatable(false);
        toolBar->setMovable(false);
        toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        GameArchiveManager *home = new GameArchiveManager();
        SettingView *settingView = new SettingView();

        QScrollArea *scrollArea = new QScrollArea();
        scrollArea->setObjectName("scrollArea");
        scrollArea->setStyleSheet("#scrollArea {background-color: rgba(0, 0, 0, 0); border: none}");
        scrollArea->setWidget(settingView);
        scrollArea->setWidgetResizable(true); // 自动调整子控件大小
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        QStackedWidget *stackedWidget = new QStackedWidget(this);
        stackedWidget->addWidget(home);
        stackedWidget->addWidget(scrollArea);

        setCentralWidget(stackedWidget);

        connect(homeAction, &QAction::triggered, [=]()
                { stackedWidget->setCurrentIndex(0); });
        connect(configAction, &QAction::triggered, [=]()
                { stackedWidget->setCurrentIndex(1); });
}


MainWindow::~MainWindow()
{
}
