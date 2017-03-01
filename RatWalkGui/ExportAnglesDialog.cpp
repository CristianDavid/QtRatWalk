#include "ExportAnglesDialog.h"
#include "ui_ExportAnglesDialog.h"

namespace RatWalkGui {

ExportAnglesDialog::ExportAnglesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportAnglesDialog)
{
    ui->setupUi(this);
}

ExportAnglesDialog::~ExportAnglesDialog()
{
    delete ui;
}

} // namespace RatWalkGui
