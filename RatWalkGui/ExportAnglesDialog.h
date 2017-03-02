#ifndef RATWALKGUI_EXPORTANGLESDIALOG_H
#define RATWALKGUI_EXPORTANGLESDIALOG_H

#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <QComboBox>
#include <QDialog>
#include <QFrame>
#include <QLabel>
#include <RatWalkCore/Tracker.h>

namespace RatWalkGui {

namespace Ui {
class ExportAnglesDialog;
}

class ExportAnglesDialog : public QDialog {
   Q_OBJECT
public:
   ExportAnglesDialog(
         std::vector<const char *> &openProjects,
         QWidget *parent = 0
   );
   ~ExportAnglesDialog();
   std::pair<std::vector<int>, std::vector<int>> getExportOrder();
private slots:
   void onTakeNumberChanged(int index);
   void onTakeOrientationChanged(int index);
private:
   class TakeInfoFrame : public QFrame {
   public:
      TakeInfoFrame(const char *openTakeName, QWidget *parent = 0);
      QLabel    takeNameLabel;
      QComboBox takeNumberCombo,
                takeOrientationCombo;
   };

   static const char *PREVIOUS_INDEX_PROPERTY;
   static const char *IS_RIGHT_ORIENTATION_PROPERTY;
   static const char *ORIGINAL_INDEX_PROPERTY;

   Ui::ExportAnglesDialog *ui;
   std::map<int, QComboBox*> leftTakesIndexMap,
                             rightTakesIndexMap;
};


} // namespace RatWalkGui
#endif // RATWALKGUI_EXPORTANGLESDIALOG_H
