#include <QtGui/QLayout>
#include <QtGui/QLabel>
#include <QtGui/QIcon>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>
#include <QtGui/QToolTip>
#include <QtGui/QWhatsThis>
#include <QtGui/qlineedit.h>
#include <QtGui/qboxlayout.h>
#include <QtGui/qfiledialog.h>
#include <QtCore/qsettings.h>
#include <QtGui/qformlayout.h>
#include <QtGui/qcombobox.h>
#include <QtGui/qdesktopservices.h>
#include <QtGui/qlistwidget.h>
#include <QtGui/qtreewidget.h>
#include <QtGui/qcheckbox.h>
#include <QtGui/qdesktopservices.h>
#include <QtCore/qdiriterator.h>

#include "dzapp.h"
#include "dzscene.h"
#include "dzstyle.h"
#include "dzmainwindow.h"
#include "dzactionmgr.h"
#include "dzaction.h"
#include "dzskeleton.h"
#include "dzfigure.h"
#include "dzobject.h"
#include "dzshape.h"
#include "dzmodifier.h"
#include "dzpresentation.h"
#include "dzassetmgr.h"
#include "dzproperty.h"
#include "dzsettings.h"
#include "dzmorph.h"
#include "DzUnityAnimatedMorphsDialog.h"
#include "debug.h"

#include "QtGui/qlayout.h"
#include "QtGui/qlineedit.h"

#include <QtCore/QDebug.h>

#include <set>

/*****************************
Local definitions
*****************************/
#define DAZ_TO_UNITY_PLUGIN_NAME "DazToUnity"

DzUnityAnimatedMorphsDialog *DzUnityAnimatedMorphsDialog::singleton = nullptr;

// For sorting the lists
// class SortingListItem : public QListWidgetItem
// {

// public:
//   virtual bool operator<(const QListWidgetItem &otherItem) const
//   {
//     if (this->checkState() != otherItem.checkState())
//     {
//       return (this->checkState() == Qt::Checked);
//     }
//     return QListWidgetItem::operator<(otherItem);
//   }
// };


    DzUnityAnimatedMorphsDialog::DzUnityAnimatedMorphsDialog(QWidget *parent) : DzBasicDialog(parent, DAZ_TO_UNITY_PLUGIN_NAME)
{

  morphListWidget = NULL;
  filterEdit = NULL;
  morphTreeItem = NULL;
  progressBar = NULL;

  // Thread
  // SUCCESS("DAZRuntime - Setting up thread");
  // m_loader.moveToThread(&m_loaderThread);
  // m_loaderThread.start();

  // connect(this, SIGNAL(loadImage(const QString &)), &m_loader, SLOT(loadImage(const QString &)));
  // connect(this, SIGNAL(imageLoaded(const QString &, const QImage &)), this, SLOT(imageAvailable(const QString &, const QImage &)));
  // connect(&m_loader, SIGNAL(imageLoaded(const QString &, const QImage &)), this, SLOT(imageAvailable(const QString &, const QImage &)));

  // Set the dialog title
  setWindowTitle(tr("Animated Morphs"));

  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  morphListWidget = new QListWidget(this);
  morphListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
  // morphListWidget->setViewMode(QListWidget::IconMode);
  morphListWidget->setWordWrap(true);
  // morphListWidget->setIconSize(QSize(150, 150));

  // Quick filter box
  QHBoxLayout *filterLayout = new QHBoxLayout();
  filterLayout->addWidget(new QLabel("filter"));
  filterEdit = new QLineEdit();
  connect(filterEdit, SIGNAL(textChanged(const QString &)), this, SLOT(FilterChanged(const QString &)));
  filterLayout->addWidget(filterEdit);

  // All
  QHBoxLayout *morphsLayout = new QHBoxLayout();

  // Center List of textures based on tree selection
  QVBoxLayout *morphListLayout = new QVBoxLayout();
  morphListLayout->addWidget(new QLabel("Morphs Animated"));
  morphListLayout->addWidget(new QLabel("Select and click Add for Export"));
  morphListLayout->addLayout(filterLayout);
  morphListLayout->addWidget(morphListWidget);

  morphsLayout->addLayout(morphListLayout);
  mainLayout->addLayout(morphsLayout);

  progressBar = new QProgressBar(this);
  mainLayout->addWidget(progressBar);

  this->addLayout(mainLayout);
  resize(QSize(800, 800)); //.expandedTo(minimumSizeHint()));

  connect(morphListWidget, SIGNAL(itemChanged(QListWidgetItem *)),
          this, SLOT(ItemChanged(QListWidgetItem *)));
}

// void DzUnityAnimatedMorphsDialog::imageAvailable(const QString &fichier, const QImage &img)
// {
//   SUCCESS(QString("DAZRuntime - Main Thread received image ").arg(fichier));

//   SortingListItem *item = new SortingListItem();
//   item->setText(fichier);
//   QPixmap map = QPixmap::fromImage(img);
//   item->setData(Qt::DecorationRole, map.scaled(150, 150, Qt::KeepAspectRatio));
//   item->setTextAlignment(Qt::AlignLeft);
  
//   //item->setIcon(QIcon(tex));
//   morphListWidget->addItem(item);
//   progressBar->setValue(progressBar->value() + 1);
//   morphListWidget->repaint();
// }

QSize DzUnityAnimatedMorphsDialog::minimumSizeHint() const
{
  return QSize(800, 800);
}

QStringList DzUnityAnimatedMorphsDialog::GetAvailableMorphs(DzNode *Node)
{
  std::set<QString> morphsList;

  // DzObject *Object = Node->getObject();
  // DzShape *Shape = Object ? Object->getCurrentShape() : NULL;

  // if (Shape)
  // {
  //   Shape->get
  //   QObjectList aMaterials = Shape->getAllMaterials();
  //   for (int x = 0; x < aMaterials.count(); x++)
  //   {
  //     DzMaterial *mat = (DzMaterial *)aMaterials[x];
  //     QObjectList aMaps = mat->getAllMaps();
  //     for (int y = 0; y < aMaps.count(); y++)
  //     {
  //       DzTexture *tex = (DzTexture *)aMaps[y];
  //       morphsList.insert(tex->getFilename());
  //     }
  //   }
  // }
  std::list<QString> asList(morphsList.begin(), morphsList.end());
  return QStringList::fromStdList(asList);
}

void DzUnityAnimatedMorphsDialog::PrepareDialog()
{
  DzNode *Selection = dzScene->getPrimarySelection();

  // For items like clothing, create the morph list from the character
  DzNode *ParentFigureNode = Selection;
  while (ParentFigureNode->getNodeParent())
  {
    ParentFigureNode = ParentFigureNode->getNodeParent();
    if (DzSkeleton *Skeleton = ParentFigureNode->getSkeleton())
    {
      if (DzFigure *Figure = qobject_cast<DzFigure *>(Skeleton))
      {
        Selection = ParentFigureNode;
        break;
      }
    }
  }

  morphs.clear();
  morphList = GetAvailableMorphs(Selection);
  for (int ChildIndex = 0; ChildIndex < Selection->getNumNodeChildren(); ChildIndex++)
  {
    DzNode *ChildNode = Selection->getNodeChild(ChildIndex);
    morphList.append(GetAvailableMorphs(ChildNode));
  }
  UpdateMorphsList();
}

void DzUnityAnimatedMorphsDialog::UpdateMorphsList()
{

  morphListWidget->clear();
  //foreach (TextureInfo texInfo, texsToExport)

  progressBar->setMaximum(morphList.count());
  progressBar->setValue(0);

  foreach (QString tex, morphList)
  {
    SUCCESS(QString("DAZRuntime - Setting up thread to load %1").arg(tex));

    // calling the signal
    // emit loadImage(tex);
    progressBar->setValue(progressBar->value() + 1);
  }
}



#include "moc_DzUnityAnimatedMorphsDialog.cpp"