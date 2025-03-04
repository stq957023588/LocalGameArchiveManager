#pragma oncev
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "AddGame.h"
#include <QListView>
#include <QTableView>
#include <QStandardItemModel>
#include "ElidedLabel.h"
#include "Login.h"

class Game;
class GameArchive;

class GameArchiveManager : public QWidget
{
    Q_OBJECT

private:
    AddGame addGameWindow;
    Login *loginWindow;

    QTableView *gameTable;
    QStandardItemModel *gameTableItemModel;

    QListView *gameListView;
    QStandardItemModel *gamesModel;

    QLabel *gameIconLabel;
    QLabel *gNameLabel;
    ElidedLabel *gStartupFilePathLabel;
    ElidedLabel *gArchiveDirPathLabel;

    QPushButton *addBtn;

    void initView();
    void refreshGameList();
    void addGameListRow(Game game);
    void refreshGameArchiveTable(QString gameName);
    void addGameArchiveTableRow(GameArchive gameArchive);

    void gameSelected(Game game);

private slots:
    void addGameHook(Game game);
    void openAddGameWindow();
    void doGameListItemDoubleClicked(QModelIndex index);
    void doArchiveSaveBtnClicked();
    void doDeleteGameBtnClicked();
    void doStartGameBtnClicked();
    void doSyncArchiveBtnClicked();
    void doArhciveDeleteBtnClicked();
    void doArchiveEnableBtnClicked();

signals:
    void sendSignal(QString value);

public:
    GameArchiveManager(QWidget *parent = nullptr);
    ~GameArchiveManager();
};