#include <QApplication>
#include <QStyle>
#include <QDesktopWidget>
#include <QKeyEvent>

#include "RatWalkGui/MainWindow.h"
#include "RatWalkGui/MdiMainWindow.h"

#include "RatWalkGui/Plotter.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   RatWalkGui::MdiMainWindow w;

   w.setGeometry(
      QStyle::alignedRect(
         Qt::LeftToRight,
         Qt::AlignCenter,
         w.size(),
         qApp->desktop()->availableGeometry()
      )
   );
   w.show();

   return a.exec();
}
