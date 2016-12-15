#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

#include "RatWalkGui/ImageViewer.h"

namespace RatWalkGui {

ImageViewer::ImageViewer(QWidget * parent) : QWidget(parent) {
   setAttribute(Qt::WA_OpaquePaintEvent);
}

void ImageViewer::paintEvent(QPaintEvent *) {
   qDebug() << "Me estoy pintando :)";
   if (img.isNull()) {
      img = QImage(size(), QImage::Format_RGB888);
      img.fill(0);
   }
   QPainter p(this);
   p.drawImage(0, 0, img.scaled(size()));
}

void ImageViewer::setImage(const QImage &img) {
   this->img = img;
   update();
}

} // namespace RatWalkGui
