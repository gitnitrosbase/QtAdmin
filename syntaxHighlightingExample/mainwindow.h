#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    QString paintText(QString str, int color)
    {
        QString output;
        switch (color)
        {
            case 0 : output = QString("<span style=\" color:#ff0000;\">%1</span>").arg(str); break;
            case 1 : output = QString("<span style=\" color:#008000;\">%1</span>").arg(str); break;
            case 2 : output = QString("<span style=\" color:#0000FF;\">%1</span>").arg(str); break;
        }
        return output;
    }


    void on_textEdit_textChanged();

private:
    Ui::MainWindow *ui;
    std::map<QString, QString> alp =
    {
        {"select","blue"},
        {"from","blue"},
        {"top","blue"},
        {"where","blue"},
        {"create","blue"},
        {"delete","blue"},
        {"insert","blue"}
    };
};
#endif // MAINWINDOW_H
