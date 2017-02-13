#include "MdiMainWindow.h"
#include "ui_MdiMainWindow.h"

namespace RatWalkGui {

MdiMainWindow::MdiMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MdiMainWindow)
{
    ui->setupUi(this);
}

MdiMainWindow::~MdiMainWindow()
{
    delete ui;
}

} // namespace RatWalkGui
