#include "ExportAnglesDialog.h"
#include "ui_ExportAnglesDialog.h"

#include <QDebug>
#include <QFrame>
#include <QHBoxLayout>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVBoxLayout>

namespace RatWalkGui {

const char *ExportAnglesDialog::PREVIOUS_INDEX_PROPERTY = "previousIndex";
const char *ExportAnglesDialog::IS_RIGHT_ORIENTATION_PROPERTY = "isRightOrientation";
const char *ExportAnglesDialog::ORIGINAL_INDEX_PROPERTY = "originalIndex";

ExportAnglesDialog::ExportAnglesDialog(
      std::vector<const char *> &openProjects,
      QWidget *parent
   ) :
   QDialog(parent),
   ui(new Ui::ExportAnglesDialog) {
   QVBoxLayout *layout = new QVBoxLayout;
   ui->setupUi(this);
   ui->contentFrame->setLayout(layout);
   int projectsSize = (int)openProjects.size();
   QStringList takeNumberValues;
   for (int i = 1; i <= projectsSize; i++) {
      takeNumberValues.append(QString::number(i));
   }
   for (int i = 0; i < projectsSize; i++) {
      const char *projectName = openProjects[i];
      TakeInfoFrame *frame = new TakeInfoFrame(projectName, ui->contentFrame);
      layout->addWidget(frame);
      frame->takeNumberCombo.addItems(takeNumberValues);
      frame->takeNumberCombo.setCurrentIndex(i);
      frame->takeNumberCombo.setProperty(ORIGINAL_INDEX_PROPERTY, i);
      frame->takeNumberCombo.setProperty(PREVIOUS_INDEX_PROPERTY, i);
      frame->takeNumberCombo.setProperty(IS_RIGHT_ORIENTATION_PROPERTY, true);
      rightTakesIndexMap[i] = &frame->takeNumberCombo;
      QObject::connect(
            &frame->takeNumberCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onTakeNumberChanged(int))
      );
      QObject::connect(
            &frame->takeOrientationCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onTakeOrientationChanged(int))
      );
   }
}

ExportAnglesDialog::~ExportAnglesDialog() {
   delete ui;
}

std::pair<std::vector<int>, std::vector<int> > ExportAnglesDialog::getExportOrder() {
   std::vector<int> leftTakes,
                    rightTakes;
   for (int i = 0; i < (int)leftTakesIndexMap.size(); i++) {
      QComboBox &combo = *leftTakesIndexMap[i];
      leftTakes.push_back(combo.property(ORIGINAL_INDEX_PROPERTY).toInt());
   }
   for (int i = 0; i < (int)rightTakesIndexMap.size(); i++) {
      QComboBox &combo = *rightTakesIndexMap[i];
      rightTakes.push_back(combo.property(ORIGINAL_INDEX_PROPERTY).toInt());
   }
   return std::make_pair(leftTakes, rightTakes);
}

void ExportAnglesDialog::onTakeNumberChanged(int index) {
   QComboBox *takeNumberCombo = qobject_cast<QComboBox*>(sender());
   int previousIndex = takeNumberCombo
         ->property(PREVIOUS_INDEX_PROPERTY).toInt();
   bool isRightOrientation = takeNumberCombo
         ->property(IS_RIGHT_ORIENTATION_PROPERTY).toBool();
   auto &indexMap = isRightOrientation? rightTakesIndexMap : leftTakesIndexMap;
   QComboBox *otherCombo = indexMap[index];
   otherCombo->blockSignals(true);
   otherCombo->setCurrentIndex(previousIndex);
   otherCombo->blockSignals(false);
   indexMap[index]         = takeNumberCombo;
   indexMap[previousIndex] = otherCombo;
   takeNumberCombo->setProperty(PREVIOUS_INDEX_PROPERTY, index);
   otherCombo->setProperty(PREVIOUS_INDEX_PROPERTY, previousIndex);
}

void ExportAnglesDialog::onTakeOrientationChanged(int index) {
   QComboBox *takeOrientationCombo = qobject_cast<QComboBox*>(sender());
   QWidget *parentWidget = takeOrientationCombo->parentWidget();
   TakeInfoFrame *parentFrame = dynamic_cast<TakeInfoFrame*>(parentWidget);
   bool isRightOrientation = index == 0;
   auto &newMap  = isRightOrientation? rightTakesIndexMap : leftTakesIndexMap;
   auto &prevMap = isRightOrientation? leftTakesIndexMap : rightTakesIndexMap;
   int newIndex = newMap.size();
   int prevIndex = parentFrame->takeNumberCombo.property(PREVIOUS_INDEX_PROPERTY).toInt();
   newMap[newIndex] = &parentFrame->takeNumberCombo;
   parentFrame->takeNumberCombo.setProperty(
            PREVIOUS_INDEX_PROPERTY,
            newIndex
   );
   parentFrame->takeNumberCombo.setProperty(
            IS_RIGHT_ORIENTATION_PROPERTY,
            isRightOrientation
   );
   // agregar el índice a todos los combos de la misma orientación
   parentFrame->takeNumberCombo.blockSignals(true);
   parentFrame->takeNumberCombo.clear();
   QStringList nuevosIndices;
   for (int i = 1; i <= newIndex; i++) {
      nuevosIndices << QString::number(i);
   }
   parentFrame->takeNumberCombo.addItems(nuevosIndices);
   parentFrame->takeNumberCombo.blockSignals(false);
   for (int i = 0; i <= newIndex; i++) {
      QComboBox &combo = *newMap[i];
      combo.blockSignals(true);
      combo.addItem(QString::number(newIndex+1));
      combo.blockSignals(false);
   }
   parentFrame->takeNumberCombo.blockSignals(true);
   parentFrame->takeNumberCombo.setCurrentIndex(newIndex);
   parentFrame->takeNumberCombo.blockSignals(false);
   // remover el viejo indice de los combos de la vieja orientación
   int prevMapSize = prevMap.size();
   for (int i = 0; i < prevMapSize; i++) {
      if (i == prevMapSize - 1) {
         prevMap.erase(i);
      } else {
         if (i >= prevIndex) {
            prevMap[i] = prevMap[i+1];
         }
         QComboBox &combo = *prevMap[i];
         combo.blockSignals(true);
         combo.setProperty(PREVIOUS_INDEX_PROPERTY, i);
         combo.setCurrentIndex(i);
         combo.removeItem(prevMapSize-1);
         combo.blockSignals(false);
      }
   }
}

ExportAnglesDialog::TakeInfoFrame::TakeInfoFrame(
      const char *openTakeName,
      QWidget *parent
   ) :
   QFrame(parent),
   takeNameLabel(openTakeName, this),
   takeNumberCombo(this),
   takeOrientationCombo(this) {
   QHBoxLayout *layout = new QHBoxLayout;
   setLayout(layout);
   layout->addWidget(&takeNameLabel);
   layout->addWidget(&takeNumberCombo);
   layout->addWidget(&takeOrientationCombo);
   takeOrientationCombo.addItem("Toma derecha");
   takeOrientationCombo.addItem("Toma Izquierda");
}

} // namespace RatWalkGui
