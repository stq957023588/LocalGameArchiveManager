#pragma once

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>

class GameProcessMonitor : public QObject {
    Q_OBJECT  // 必须包含此宏以启用信号与槽的机制

public:
    explicit GameProcessMonitor(const QString &program, const QStringList &arguments, QObject *parent = nullptr);
    void start();

private:
    QProcess *process;
    QString program;
    QStringList arguments;

private slots:
    void onStarted();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onErrorOccurred(QProcess::ProcessError error);
    void onStateChanged(QProcess::ProcessState newState);
};

