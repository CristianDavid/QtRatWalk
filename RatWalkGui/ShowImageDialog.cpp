#include "ShowImageDialog.h"
#include "ui_ShowImageDialog.h"

#include <QImage>
#include <QString>

namespace RatWalkGui {

ShowImageDialog::ShowImageDialog(QString title, QImage image, QWidget *parent) :
   QDialog(parent),
   ui(new Ui::ShowImageDialog) {
   ui->setupUi(this);
   setWindowTitle(title);
   ui->imageViewer->setImage(image);
}

ShowImageDialog::~ShowImageDialog() {
   delete ui;
}

} // namespace RatWalkGui
