#include "AnglePlotter.h"
#include "ui_AnglePlotter.h"

AnglePlotter::AnglePlotter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnglePlotter)
{
    ui->setupUi(this);
}

AnglePlotter::~AnglePlotter()
{
    delete ui;
}
