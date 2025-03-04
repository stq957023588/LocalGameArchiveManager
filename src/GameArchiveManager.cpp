#include "GameArchiveManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QBuffer>
#include <QMessageBox>
#include <string>
#include <random>
#include <QDebug>
#include <QHeaderView>
#include <QModelIndex>
#include <QList>
#include <QVariant>
#include <QUuid>
#include <QDateTime>
#include <QCryptographicHash>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QHttpPart>
#include <QHttpMultiPart>
#include "FileUtils.h"
#include "RandomUtils.h"
#include "Game.h"
#include "GameRepository.h"
#include <QVariant>
#include "GameArchiveRepository.h"
#include "Api.h"

int PullArchive(QSharedPointer<GameArchive> remoteGameArchive)
{
    GameArchive localGameArchive = remoteGameArchive->clone();

    QUuid uuid = QUuid::createUuid();
    QString archivePath = QString("%1/archives/%2/%3.zip").arg(QDir::currentPath()).arg(remoteGameArchive->gameName).arg(uuid.toString(QUuid::Id128));
    localGameArchive.archivePath = archivePath;

    QSharedPointer<GameArchive> localGameArchivePtr = QSharedPointer<GameArchive>::create(localGameArchive);
    bool insertResult = GameArchiveRepository::insertRow(localGameArchivePtr);
    if (!insertResult)
    {
        QMessageBox::critical(nullptr, "程序异常", "存档保存错误！");
        return -1;
    }

    QSharedPointer<QByteArray> remoteArchiveFile = getArchiveFromRemote(remoteGameArchive->id, [](QString errorMessage)
                                                                        { QMessageBox::critical(nullptr, "程序异常", errorMessage); });
    if (remoteArchiveFile.isNull())
    {
        return -1;
    }

    QFileInfo archiveFileInfo(archivePath);
    QDir zipDir = archiveFileInfo.absoluteDir();
    if (!zipDir.exists())
    {
        zipDir.mkdir(zipDir.absolutePath());
    }

    QFile zipFile(archivePath);
    if (!zipFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "打开文件失败!" << archivePath;
        return -1;
    };

    zipFile.write(*remoteArchiveFile);
    zipFile.close();

    return localGameArchivePtr->id;
}

void PushArchive(QSharedPointer<GameArchive> localGameArchive)
{
    if (localGameArchive.isNull())
    {
        return;
    }

    QSharedPointer<GameArchive> remoteGameArchive = saveArchiveToRemote(localGameArchive, [](QString errorMessage)
                                                                        { QMessageBox::critical(nullptr, "系统异常", errorMessage); });
    if (remoteGameArchive.isNull())
    {
        return;
    }

    QFile *file = new QFile(localGameArchive->archivePath);
    if (!file->open(QIODevice::ReadOnly))
    {
        qDebug() << "Unable to open file!";
        return;
    }

    pushArchiveFile(remoteGameArchive->id, file, [&](QString errorMessage)
                    { QMessageBox::critical(nullptr, "系统异常", errorMessage); });
}

void clearGameTable(QStandardItemModel *gameTableItemModel)
{
    gameTableItemModel->clear();
    gameTableItemModel->setHorizontalHeaderLabels({"存档时间", "MD5", "操作"});
}

void OverwriteArchiveSelect(int archiveId, QWidget *parent)
{

    QMessageBox messageBox(QMessageBox::NoIcon, "存档覆盖", "是否覆盖?", QMessageBox::Yes | QMessageBox::No, NULL);
    int result = messageBox.exec();

    // 不覆盖
    if (result != QMessageBox::Yes)
    {
        return;
    }

    QSharedPointer<GameArchive> gameArchive = GameArchiveRepository::findById(QString::number(archiveId));
    if (gameArchive.isNull())
    {
        QMessageBox::critical(parent, "系统异常", "未找到存档信息");
        return;
    }
    QSharedPointer<Game> game = GameRepository::findByName(gameArchive->gameName);
    if (game.isNull())
    {
        QMessageBox::critical(parent, "系统异常", "未找到游戏信息");
        return;
    }
    unzipFile(game->archiveDirPath, gameArchive->archivePath);
}

void StoreArchive(QString gameName, QString archiveDirPath, QWidget *parent = nullptr)
{
    qDebug() << "Store archive,path:" << archiveDirPath << ";GameName:" << gameName;
    QUuid uuid = QUuid::createUuid();
    QString archivePath = QString("%1/archives/%2/%3.zip").arg(QDir::currentPath()).arg(gameName).arg(uuid.toString(QUuid::Id128));
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString formattedDateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    qDebug() << "archive path:" << archivePath;
    bool compressResult = compressDirectory(archiveDirPath, archivePath);
    if (!compressResult)
    {
        if (parent)
        {
            QMessageBox::critical(parent, "critical", "压缩存档失败！");
        }
        return;
    }

    QString md5 = calculateFileMd5(archivePath);

    int count = GameArchiveRepository::count(gameName, md5);
    if (count != 0)
    {
        if (parent)
        {
            QMessageBox::critical(parent, "critical", "存在相同MD5的存档！MD5：" + md5);
        }
        QFile zipFile(archivePath);
        zipFile.remove();
        return;
    }
    GameArchive gameArchive;
    gameArchive.gameName = gameName;
    gameArchive.archivePath = archivePath;
    gameArchive.md5 = md5;
    gameArchive.saveTime = QDateTime::currentDateTime();

    bool insertResult = GameArchiveRepository::insertRow(QSharedPointer<GameArchive>::create(gameArchive));
    if (!insertResult)
    {
        if (parent)
        {
            QMessageBox::critical(parent, "critical", "插入记录失败！");
        }
    }
}

GameArchiveManager::GameArchiveManager(QWidget *parent)
    : QWidget(parent)
{
    initView();
    refreshGameList();
}

void GameArchiveManager::initView()
{
    this->setObjectName("home-widget");
    this->setStyleSheet("#home-widget { margin: 0px;padding: 0px}");

    QPushButton *addGameBtn = new QPushButton("添加游戏");

    gameListView = new QListView();
    gameListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    gamesModel = new QStandardItemModel();
    QList<QStandardItem *> items;
    gameListView->setModel(gamesModel);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setContentsMargins(0, 0, 0, 0); // 设置上下左右边距为 0
    leftLayout->addWidget(addGameBtn);
    leftLayout->addWidget(gameListView);

    QWidget *leftWidget = new QWidget();
    leftWidget->setLayout(leftLayout);
    leftWidget->setFixedWidth(300);

    gameIconLabel = new QLabel();
    gameIconLabel->setAlignment(Qt::AlignCenter);
    gameIconLabel->setFixedWidth(85);

    gNameLabel = new QLabel();
    gStartupFilePathLabel = new ElidedLabel("");
    gArchiveDirPathLabel = new ElidedLabel("");

    QPushButton *saveArchiveBtn = new QPushButton("保存");
    saveArchiveBtn->setStyleSheet("QPushButton {width: 50px}");
    QPushButton *startGameBtn = new QPushButton("启动");
    QPushButton *deleteGameBtn = new QPushButton("删除游戏");
    QPushButton *syncArchiveBtn = new QPushButton("同步存档");

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(5);
    btnLayout->setAlignment(Qt::AlignLeft);
    btnLayout->addWidget(saveArchiveBtn);
    btnLayout->addWidget(startGameBtn);
    btnLayout->addWidget(deleteGameBtn);
    btnLayout->addWidget(syncArchiveBtn);

    qDebug() << gNameLabel->size();

    QVBoxLayout *operateLayout = new QVBoxLayout();
    operateLayout->addWidget(gNameLabel);
    operateLayout->addWidget(gStartupFilePathLabel);
    operateLayout->addWidget(gArchiveDirPathLabel);
    operateLayout->addLayout(btnLayout);

    QHBoxLayout *infoLayout = new QHBoxLayout();
    infoLayout->setSpacing(0);
    infoLayout->addWidget(gameIconLabel);
    infoLayout->addLayout(operateLayout);

    gameTableItemModel = new QStandardItemModel();
    gameTable = new QTableView();
    gameTable->setModel(gameTableItemModel);
    gameTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    gameTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 填充表格

    // 然后设置要根据内容使用宽度的列（其他没设置的列自动缩放）
    //  gameTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    //  gameTable->horizontalHeader()->setSectionResizeMode(17, QHeaderView::ResizeToContents);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setContentsMargins(0, 0, 0, 0); // 设置上下左右边距为 0
    rightLayout->addLayout(infoLayout);
    rightLayout->addWidget(gameTable);

    QWidget *rightWidget = new QWidget();
    rightWidget->setLayout(rightLayout);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(leftWidget, 0, Qt::AlignLeft);
    layout->addWidget(rightWidget);

    setLayout(layout);

    // 添加游戏窗口回调
    connect(&addGameWindow, SIGNAL(sendSignal(Game)), this, SLOT(addGameHook(Game)));
    // 添加游戏按钮
    connect(addGameBtn, SIGNAL(clicked()), this, SLOT(openAddGameWindow()));
    // 游戏列表双击事件
    connect(gameListView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doGameListItemDoubleClicked(QModelIndex)));
    // 删除游戏按钮事件绑定
    connect(deleteGameBtn, SIGNAL(clicked()), this, SLOT(doDeleteGameBtnClicked()));
    // 保存存档按钮事件
    connect(saveArchiveBtn, SIGNAL(clicked()), this, SLOT(doArchiveSaveBtnClicked()));
    // 开始游戏按钮事件绑定
    connect(startGameBtn, SIGNAL(clicked()), this, SLOT(doStartGameBtnClicked()));
    // 同步存档事件绑定
    connect(syncArchiveBtn, SIGNAL(clicked()), this, SLOT(doSyncArchiveBtnClicked()));
}

// 普通方法

void GameArchiveManager::refreshGameList()
{
    gamesModel->clear();
    QList<Game> games = GameRepository::select();
    Game selected;
    for (size_t i = 0; i < games.size(); i++)
    {
        if (i == 0)
        {
            selected = games[i];
        }
        addGameListRow(games[i]);
    }
    gameSelected(selected);
    refreshGameArchiveTable(selected.name);
}

void GameArchiveManager::addGameListRow(Game game)
{

    int row = gamesModel->rowCount();
    gamesModel->insertRow(row);

    QStandardItem *item = new QStandardItem();

    QPixmap pixmap;
    pixmap.loadFromData(game.icon, "PNG");
    QIcon icon(pixmap);

    item->setIcon(icon);
    item->setText(game.name);
    item->setData(QVariant::fromValue(game));

    gamesModel->setItem(row, 0, item);
}

void GameArchiveManager::refreshGameArchiveTable(QString gameName)
{
    clearGameTable(gameTableItemModel);
    QList<GameArchive> gameArchives = GameArchiveRepository::selectList(gameName);
    for (size_t i = 0; i < gameArchives.size(); i++)
    {
        addGameArchiveTableRow(gameArchives[i]);
    }
}

void GameArchiveManager::addGameArchiveTableRow(GameArchive gameArchive)
{

    int row = 0;
    gameTableItemModel->insertRow(row);

    gameTableItemModel->setItem(row, 0, new QStandardItem(gameArchive.saveTime.toString("yyyy/MM/dd HH:mm:ss")));
    gameTableItemModel->setItem(row, 1, new QStandardItem(gameArchive.md5));

    QPushButton *enableArchiveBtn = new QPushButton("启用");
    enableArchiveBtn->setProperty("archivePath", gameArchive.archivePath);

    QPushButton *deleteArchiveBtn = new QPushButton("删除");
    deleteArchiveBtn->setProperty("archiveId", gameArchive.id);
    deleteArchiveBtn->setProperty("archivePath", gameArchive.archivePath);

    // 将按钮放入 QWidget 并设置到 TableView
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addWidget(enableArchiveBtn);
    layout->addWidget(deleteArchiveBtn);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0); // 去除边距
    widget->setLayout(layout);

    gameTable->setIndexWidget(gameTableItemModel->index(row, 2), widget);

    connect(enableArchiveBtn, SIGNAL(clicked()), this, SLOT(doArchiveEnableBtnClicked()));
    connect(deleteArchiveBtn, SIGNAL(clicked()), this, SLOT(doArhciveDeleteBtnClicked()));
}

void GameArchiveManager::gameSelected(Game game){
    QPixmap pixmap;
    pixmap.loadFromData(game.icon, "PNG");
    
    gameIconLabel->setPixmap(pixmap);
    gNameLabel->setText(game.name);
    gStartupFilePathLabel->setFullText(game.startupFilePath);
    gArchiveDirPathLabel->setFullText(game.archiveDirPath);

    refreshGameArchiveTable(game.name);
}

// 槽方法

void GameArchiveManager::addGameHook(Game game)
{
    // 校验存档目录是否为空
    if (game.archiveDirPath.isEmpty())
    {
        QMessageBox::critical(this, "critical", "必须选择一个存档目录");
        return;
    }

    // 校验存档目录是否存在
    QFile startupFile(game.archiveDirPath);
    if (!startupFile.exists())
    {
        QMessageBox::critical(this, "critical", "存档目录不存在");
        return;
    }

    // 校验存档唯一
    QString sql = QString("select * from game where archive_dir_path = '%1'").arg(game.archiveDirPath);
    qDebug() << sql;
    QSqlQuery select(sql);
    if (select.next())
    {
        QMessageBox::critical(this, "critical", "当前目录已添加管理");
        return;
    }

    // 如果启动文件不为空，校验启动文件是否存在
    if (!game.startupFilePath.isEmpty())
    {
        QFile startupFile(game.startupFilePath);
        if (!startupFile.exists())
        {
            QMessageBox::critical(this, "critical", "启动文件不存在");
            return;
        }
    }

    // 没有游戏名称初始化游戏名称
    if (game.name.isEmpty())
    {
        game.name = "Game_" + makeRandomString(7);
    }

    // 保存数据
    QFileInfo startupFileInfo(game.startupFilePath);
    // 获取文件的图标
    QFileIconProvider iconProvider;
    QIcon statupFileIcon = iconProvider.icon(startupFileInfo);
    QPixmap pixmap = statupFileIcon.pixmap(32, 32);

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG"); // 保存为 PNG 格式

    game.icon = byteArray;

    bool result = GameRepository::insertRow(game);

    if (!result)
    {
        QMessageBox::critical(this, "程序异常", "添加游戏异常！");
        return;
    }

    addGameListRow(game);
    gameSelected(game);
    refreshGameArchiveTable(game.name);
}

void GameArchiveManager::openAddGameWindow()
{
    addGameWindow.show();
}

void GameArchiveManager::doGameListItemDoubleClicked(QModelIndex index)
{
    QStandardItem *item = gamesModel->itemFromIndex(index);
    Game game = item->data().value<Game>();

    gameSelected(game);
    refreshGameArchiveTable(game.name);
}

void GameArchiveManager::doArchiveSaveBtnClicked()
{
    QString archiveDirPath = gArchiveDirPathLabel->text();
    QString gameName = gNameLabel->text();

    if (archiveDirPath.isEmpty() || gameName.isEmpty())
    {
        return;
    }

    StoreArchive(gameName, archiveDirPath, this);
    refreshGameArchiveTable(gameName);
}

void GameArchiveManager::doDeleteGameBtnClicked()
{
    QString gameName = gNameLabel->text();

    if (gameName.isEmpty())
    {
        return;
    }

    bool deleteGameResult = GameRepository::deleteRow(gameName);
    if (!deleteGameResult)
    {
        QMessageBox::critical(this, "程序异常", "删除游戏失败！");
        return;
    }

    bool deleteGameArchiveResult = GameArchiveRepository::deleteByGameName(gameName);
    if (!deleteGameArchiveResult)
    {
        QMessageBox::critical(this, "程序异常", "删除游戏存档失败！");
        return;
    }

    QString archiveDirPath = QDir::currentPath() + "/archives/" + gameName;
    QDir archiveDir(archiveDirPath);
    if (archiveDir.exists())
    {
        archiveDir.removeRecursively();
    }

    // clearGameTable(gameTableItemModel);
    // gNameLabel->setText("");
    // gStartupFilePathLabel->setText("");
    // gArchiveDirPathLabel->setText("");
    // gameIconLabel->setPixmap(QIcon().pixmap(32, 32));
    refreshGameList();
}

void GameArchiveManager::doStartGameBtnClicked()
{
    QString startupFilePath = gStartupFilePathLabel->getFullText();
    if (startupFilePath.isEmpty())
    {
        QMessageBox::critical(this, "critical", "未设置启动文件");
        return;
    }

    QString gameName = gNameLabel->text();
    QString archiveDirPath = gArchiveDirPathLabel->getFullText();

    qDebug() << "启动：" << startupFilePath;
    QProcess *process = new QProcess();
    process->start(QString("\"%1\"").arg(startupFilePath));

    if (!process->waitForStarted())
    {
        qDebug() << "Failed to start process.";
        delete process; // 立即释放资源
        return;
    }

    connect(process, &QProcess::started, this, []()
            { qDebug() << "Process started."; });
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int exitCode, QProcess::ExitStatus exitStatus)
            {
                qDebug() << "Process finished with exit code:" << exitCode << "and exit status:" << exitStatus;
                StoreArchive(gameName, archiveDirPath);
                process->deleteLater(); // 延迟删除对象，确保资源释放
            });
}

void GameArchiveManager::doSyncArchiveBtnClicked()
{

    QString gameName = gNameLabel->text();
    QString archivePath = gArchiveDirPathLabel->text();

    bool error = false;
    auto remoteArchive = latestArchive(gameName, [&](QString errorMessage)
                                       { 
                                        error = true;
                                        QMessageBox::critical(this, "系统异常", errorMessage); });
    if (error)
    {
        return;
    }

    auto localArchive = GameArchiveRepository::latest(gameName);

    if (remoteArchive.isNull() && localArchive.isNull())
    {
        // 本地和远程均无存档存在
        return;
    }

    // 远程存档为空
    if (remoteArchive.isNull())
    {
        PushArchive(localArchive);
        return;
    }

    // 本地存档为空
    if (localArchive.isNull())
    {
        qDebug() << "无本地存档！拉取远程存档";
        PullArchive(remoteArchive);
        refreshGameArchiveTable(gameName);
        return;
    }

    // 远程存档和本地存档都存在的情况
    if (localArchive->md5 == remoteArchive->md5)
    {
        qDebug() << "当前已是最新存档！";
        return;
    }

    int sameM5dArchiveNumber = GameArchiveRepository::count(gameName, localArchive->md5);
    if (sameM5dArchiveNumber > 0)
    {
        // 相同MD5存在，且不是最新存档，说明需要更新服务端的存档；
        qDebug() << "存在相同MD5，更新远端存档！";
        PushArchive(localArchive);
        return;
    }

    // 不存在相同MD5，需要用户自行判断是插入数据并覆盖存档，还是仅插入数据
    int archiveId = PullArchive(remoteArchive);
    refreshGameArchiveTable(gameName);
    if (archiveId == -1)
    {
        QMessageBox::critical(this, "critical", "存储远程存档失败");
        return;
    }
    OverwriteArchiveSelect(archiveId, this);
}

void GameArchiveManager::doArhciveDeleteBtnClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button)
    {
        return;
    }
    int archiveId = button->property("archiveId").toInt();
    qDebug() << "Button clicked, zip file path:" << archiveId;

    bool deleteResult = GameArchiveRepository::deleteRow(archiveId);
    if (!deleteResult)
    {
        QMessageBox::critical(this, "系统异常", "删除存档数据失败！");
        return;
    }

    QString zipFilePath = button->property("archivePath").toString();
    QFile zipFile(zipFilePath);
    if (zipFile.exists())
    {
        zipFile.remove();
    }

    int y = button->parentWidget()->frameGeometry().y();
    int x = button->parentWidget()->frameGeometry().x();
    QModelIndex index = gameTable->indexAt(QPoint(x, y));
    gameTableItemModel->removeRow(index.row());
}

void GameArchiveManager::doArchiveEnableBtnClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button)
    {
        return;
    }

    QString archivePath = gArchiveDirPathLabel->text();
    // 这里可以添加对 zipFilePath 的操作
    QString zipFilePath = button->property("archivePath").toString();

    unzipFile(zipFilePath, archivePath);
}

GameArchiveManager::~GameArchiveManager()
{
}
