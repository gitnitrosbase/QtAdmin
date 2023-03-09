#include <QApplication>
#include <MainWindow.hpp>

#include "LoggerFile.hpp"

int main(int argc, char *argv[])
{
    Logger mainLogger("MAIN");
    QApplication a(argc, argv);
    MainWindow window;
    mainLogger.save(Logger::LogType::INFO_TYPE, "create window");
    window.show();
    mainLogger.save(Logger::LogType::WARNING_TYPE, "set size");
    window.setWindowTitle("NBAdmin");
    window.setWindowIcon(QIcon(":/images/favicon.ico"));
    mainLogger.save(Logger::LogType::ERROR_TYPE, "error");
    mainLogger.save(Logger::LogType::ERROR_TYPE, "error");
    return QApplication::exec();
}
