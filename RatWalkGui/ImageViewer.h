#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QImage>

namespace RatWalkGui {

class ImageViewer : public QWidget {
   Q_OBJECT
public:
   ImageViewer(QWidget *parent = 0);
   void setImage(const QImage &img);
protected:
   void paintEvent(QPaintEvent *);
private:
   QImage img;
};

} // namespace RatWalkGui

#endif // IMAGEVIEWER_H
