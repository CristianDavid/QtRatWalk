#include <QApplication>
#include <QStyle>
#include <QDesktopWidget>
#include <QKeyEvent>
#include "RatWalkGui/MainWindow.h"

#include "RatWalkGui/Plotter.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   RatWalkGui::MainWindow w;

   w.setGeometry(
      QStyle::alignedRect(
         Qt::LeftToRight,
         Qt::AlignCenter,
         w.size(),
         qApp->desktop()->availableGeometry()
      )
   );
   w.show();

   RatWalkGui::Plotter p;
   p.setGeometry(
      QStyle::alignedRect(
         Qt::LeftToRight,
         Qt::AlignCenter,
         p.size(),
         qApp->desktop()->availableGeometry()
      )
   );

#include <vector>
#include <QPointF> // Quitar esto, se ve horrible
   std::vector<QPointF> points = {QPointF(0, 100), QPointF(10, 1), QPointF(20, 90), QPointF(30, 1),
             QPointF(40, 100), QPointF(50, 1), QPointF(60, 90), QPointF(70, 1),
             QPointF(80, 100), QPointF(90, 1), QPointF(100, 90)};
   for (auto &point : points) {
       p.addPoint(point.x(), point.y());
   }
   p.show();

   return a.exec();
}
