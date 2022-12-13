#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>
#include <QString>

#include "syntexHightlight.hpp"

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
    void on_output_textChanged();

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


    void on_input_textChanged();

private:
    Ui::MainWindow *ui;
    std::map<QString, int> alp =
    {
        {"select",0},
        {"from",0},
        {"top",0},
        {"where",1},
        {"create",1},
        {"delete",2},
        {"insert",2}
    };
    SyntaxHighlighter* synhig;
};
#endif // MAINWINDOW_H
