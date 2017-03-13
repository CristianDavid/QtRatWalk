#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QImage>

namespace RatWalkGui {

/*!
 * \brief The ImageViewer class is used to visualize a QImage on the widget.
 */
class ImageViewer : public QWidget {
   Q_OBJECT
public:
   /*!
    * \brief ImageViewer creates a new image viewer
    * \param parent The widget's parent
    */
   ImageViewer(QWidget *parent = 0);

   /*!
    * \brief setImage
    * \param[in] img Image to visualize.
    */
   void setImage(const QImage &img);
protected:
   void paintEvent(QPaintEvent *);
private:
   QImage img;
};

} // namespace RatWalkGui

#endif // IMAGEVIEWER_H
