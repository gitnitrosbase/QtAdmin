#pragma once

#include <QFile>
#include <QDir>
#include <QString>
#include <QDateTime>

class Logger
{
public:
    enum LogType
    {
        INFO_TYPE,
        WARNING_TYPE,
        ERROR_TYPE
    };


public:
    Logger(QString fileName) : fileName_(fileName)
    {
        createFile(fileName);
    }
    ~Logger()
    {

    }
    void createFile(QString fileName)
    {
        QDir currentDir("./");
        currentDir.mkdir("logs");
        QDir logDir("./logs");
        file_.setFileName(QString("logs/.%1.log").arg(fileName));

        file_.open(QIODevice::WriteOnly);
        file_.write("");
        file_.close();
    }

    void save(LogType type, QString message)
    {
        if (!file_.open(QIODevice::OpenModeFlag::Append)) createFile(fileName_);
        switch (type)
        {
        case LogType::INFO_TYPE:
        {
            QString tmp = logString_.arg("INFO").arg(QDateTime::currentDateTime().toString()).arg(message);
            file_.write(tmp.toUtf8());
            break;
        }
        case LogType::WARNING_TYPE:
        {
            QString tmp = logString_.arg("  WARNING").arg(QDateTime::currentDateTime().toString()).arg(message);
            file_.write(tmp.toUtf8());
            break;
        }
        case LogType::ERROR_TYPE:
        {
            {
                QString tmp = logString_.arg("\tERROR").arg(QDateTime::currentDateTime().toString()).arg(message);
                file_.write(tmp.toUtf8());
                break;
            }
        }
        }
        if (file_.isOpen()) file_.close();
    }
private:

private:
    QString fullPath_ = "";
    QString fileName_ = "";
    QFile file_;
    QString logString_ = "%1 [ %2 ] -> %3\n";
};
