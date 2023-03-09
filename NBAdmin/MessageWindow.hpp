#ifndef ALERT_H
#define ALERT_H

#include <QDialog>

namespace Ui {
class MessageWindow;
}

class MessageWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MessageWindow(QWidget *parent = nullptr);
    void setText(QString str);
    ~MessageWindow();

private:
    Ui::MessageWindow *ui;
};

#endif // ALERT_H
