#include "mysetting.h"
#include "ui_mysetting.h"

MySetting::MySetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MySetting)
{
    ui->setupUi(this);

    setWindowTitle("设置");
}

MySetting::~MySetting()
{
    delete ui;
}

void MySetting::on_pushButton_clicked()
{
    accept();
}

