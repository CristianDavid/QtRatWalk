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
#include <RatWalkCore/Project.h>

namespace RatWalkGui {

namespace Ui {
class ExportAnglesDialog;
}

/*!
 * \brief The ExportAnglesDialog class is used to determe the export order
 *        of the steps of each project opened by the aplication.
 */
class ExportAnglesDialog : public QDialog {
   Q_OBJECT
public:
   /*!
    * \brief Creates a new ExportAnglesDialog
    * \param[in] openProjects The names of the projects opened
    *            by the applications.
    * \param parent The dialog's parent
    */
   ExportAnglesDialog(
         std::vector<const char *> &openProjects,
         QWidget *parent = 0
   );

   /*!
    * Destroys the dialog.
    */
   ~ExportAnglesDialog();

   /*!
    * \brief getExportOrder gets the export order determined by this dialog.
    * \return A pair of vectors of integers, the first one contains the indexes
    *         of the left projects, the second one contains the indexed of the
    *         right projects.
    */
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
