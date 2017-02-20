#ifndef ANGLEPLOTTER_H
#define ANGLEPLOTTER_H

#include <QWidget>

namespace Ui {
class AnglePlotter;
}

class AnglePlotter : public QWidget
{
    Q_OBJECT

public:
    explicit AnglePlotter(QWidget *parent = 0);
    ~AnglePlotter();

private:
    Ui::AnglePlotter *ui;
};

#endif // ANGLEPLOTTER_H
