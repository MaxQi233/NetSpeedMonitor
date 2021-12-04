#ifndef MYSETTING_H
#define MYSETTING_H

#include <QDialog>

namespace Ui {
class MySetting;
}

class MySetting : public QDialog
{
    Q_OBJECT

public:
    explicit MySetting(QWidget *parent = nullptr);
    ~MySetting();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MySetting *ui;
};

#endif // MYSETTING_H
