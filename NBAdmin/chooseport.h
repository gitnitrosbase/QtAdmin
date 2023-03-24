#ifndef CHOOSEPORT_H
#define CHOOSEPORT_H

#include <QWidget>

namespace Ui {
class ChoosePort;
}

class ChoosePort : public QWidget
{
    Q_OBJECT

public:
    explicit ChoosePort(QWidget *parent = nullptr);
    ~ChoosePort();

private:
    Ui::ChoosePort *ui;
};

#endif // CHOOSEPORT_H
