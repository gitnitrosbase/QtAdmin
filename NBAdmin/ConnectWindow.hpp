#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>

#include <iostream>

class ConnectWindow : public QWidget
{
    Q_OBJECT
public:
    ConnectWindow(QWidget* parent = nullptr) : QWidget(parent)
    {
        address_ = new QLineEdit(this);
        port_ = new QLineEdit(this);
        username_ = new QLineEdit(this);
        password_ = new QLineEdit(this);
        createConnect_ = new QPushButton("Connect", this);
        gridLayout = new QGridLayout(this);

        gridLayout->addWidget(address_, 0, 0);
        gridLayout->addWidget(port_, 1, 0);
        gridLayout->addWidget(username_, 2, 0);
        gridLayout->addWidget(password_, 3, 0);
        gridLayout->addWidget(createConnect_, 4, 0);

        connect(createConnect_, SIGNAL(clicked()), this, SLOT(to_connect()));
    }
    ~ConnectWindow() = default;


public slots:
    void to_connect()
    {
        std::cout<<"check"<<std::endl;
    }

private:
    QLineEdit* address_ = nullptr;
    QLineEdit* port_ = nullptr;
    QLineEdit* username_ = nullptr;
    QLineEdit* password_ = nullptr;
    QPushButton* createConnect_ = nullptr;
    QGridLayout* gridLayout = nullptr;
};
