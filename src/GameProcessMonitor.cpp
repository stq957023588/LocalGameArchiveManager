#include "GameProcessMonitor.h"
#include <QDebug>

GameProcessMonitor::GameProcessMonitor(const QString &program, const QStringList &arguments, QObject *parent)
    : QObject(parent), process(new QProcess(this)), program(program), arguments(arguments)
{
    connect(process, &QProcess::started, this, &GameProcessMonitor::onStarted);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &GameProcessMonitor::onFinished);
    connect(process, &QProcess::errorOccurred, this, &GameProcessMonitor::onErrorOccurred);
    connect(process, &QProcess::stateChanged, this, &GameProcessMonitor::onStateChanged);
}

void GameProcessMonitor::start()
{
    process->start(program, arguments);
    if (!process->waitForStarted()) {
        qDebug() << "Failed to start process.";
    } else {
        qDebug() << "Process started successfully.";
    }
}

void GameProcessMonitor::onStarted()
{
    qDebug() << "Process started.";
}

void GameProcessMonitor::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Process finished with exit code:" << exitCode << "and exit status:" << exitStatus;
}

void GameProcessMonitor::onErrorOccurred(QProcess::ProcessError error)
{
    switch (error) {
    case QProcess::FailedToStart:
        qDebug() << "The process failed to start.";
        break;
    case QProcess::Crashed:
        qDebug() << "The process crashed.";
        break;
    case QProcess::Timedout:
        qDebug() << "The process timed out.";
        break;
    case QProcess::WriteError:
        qDebug() << "A write error occurred.";
        break;
    case QProcess::ReadError:
        qDebug() << "A read error occurred.";
        break;
    case QProcess::UnknownError:
    default:
        qDebug() << "An unknown error occurred.";
        break;
    }
}

void GameProcessMonitor::onStateChanged(QProcess::ProcessState newState)
{
    qDebug() << "Process state changed:" << newState;
    if (newState == QProcess::NotRunning) {
        qDebug() << "Process has exited.";
    }
}
