#include <QApplication>
#include <MainWindow.hpp>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow window;
    window.show();
    window.setMinimumSize(800, 500);
    window.setWindowTitle("NBAdmin");
    window.setWindowIcon(QIcon(":/images/favicon.ico"));
    return QApplication::exec();
}
