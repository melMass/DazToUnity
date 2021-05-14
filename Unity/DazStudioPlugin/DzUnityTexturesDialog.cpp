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
#include "DzUnityTexturesDialog.h"
#include "debug.h"

#include "QtGui/qlayout.h"
#include "QtGui/qlineedit.h"

#include <QtCore/QDebug.h>

#include "DzUtils.h"

#include <set>

/*****************************
Local definitions
*****************************/
#define DAZ_TO_UNITY_PLUGIN_NAME "DazToUnity"

DzUnityTexturesDialog *DzUnityTexturesDialog::singleton = nullptr;



    DzUnityTexturesDialog::DzUnityTexturesDialog(QWidget *parent) : DzBasicDialog(parent, DAZ_TO_UNITY_PLUGIN_NAME)
{

  textureListWidget = NULL;
  filterEdit = NULL;
  textureTreeItem = NULL;
  progressBar = NULL;

  // Thread
  SUCCESS("DAZRuntime - Setting up thread");
  m_loader.moveToThread(&m_loaderThread);
  m_loaderThread.start();

  connect(this, SIGNAL(loadImage(const QString &)), &m_loader, SLOT(loadImage(const QString &)));
  connect(this, SIGNAL(imageLoaded(const QString &, const QImage &)), this, SLOT(imageAvailable(const QString &, const QImage &)));
  connect(&m_loader, SIGNAL(imageLoaded(const QString &, const QImage &)), this, SLOT(imageAvailable(const QString &, const QImage &)));

  // Set the dialog title
  setWindowTitle(tr("Collect Maps"));

  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  textureListWidget = new QListWidget(this);
  textureListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
  // textureListWidget->setViewMode(QListWidget::IconMode);
  textureListWidget->setWordWrap(true);
  textureListWidget->setIconSize(QSize(150, 150));

  // Quick filter box
  QHBoxLayout *filterLayout = new QHBoxLayout();
  filterLayout->addWidget(new QLabel("filter"));
  filterEdit = new QLineEdit();
  connect(filterEdit, SIGNAL(textChanged(const QString &)), this, SLOT(FilterChanged(const QString &)));
  filterLayout->addWidget(filterEdit);

  // All
  QHBoxLayout *texturesLayout = new QHBoxLayout();

  // Center List of textures based on tree selection
  QVBoxLayout *textureListLayout = new QVBoxLayout();
  textureListLayout->addWidget(new QLabel("Textures in Group"));
  textureListLayout->addWidget(new QLabel("Select and click Add for Export"));
  textureListLayout->addLayout(filterLayout);
  textureListLayout->addWidget(textureListWidget);

  texturesLayout->addLayout(textureListLayout);
  mainLayout->addLayout(texturesLayout);

  progressBar = new QProgressBar(this);
  mainLayout->addWidget(progressBar);

  this->addLayout(mainLayout);
  resize(QSize(800, 800)); //.expandedTo(minimumSizeHint()));

  connect(textureListWidget, SIGNAL(itemChanged(QListWidgetItem *)),
          this, SLOT(ItemChanged(QListWidgetItem *)));
}

void DzUnityTexturesDialog::imageAvailable(const QString &fichier, const QImage &img)
{
  SUCCESS(QString("DAZRuntime - Main Thread received image %1").arg(fichier));

  SortingListItem *item = new SortingListItem();
  item->setText(fichier);
  QPixmap map = QPixmap::fromImage(img);
  item->setData(Qt::DecorationRole, map.scaled(150, 150, Qt::KeepAspectRatio));
  item->setTextAlignment(Qt::AlignLeft);
  
  //item->setIcon(QIcon(tex));
  textureListWidget->addItem(item);
  progressBar->setValue(progressBar->value() + 1);
  textureListWidget->repaint();
}

QSize DzUnityTexturesDialog::minimumSizeHint() const
{
  return QSize(800, 800);
}

QStringList DzUnityTexturesDialog::GetAvailableTextures(DzNode *Node)
{
  std::set<QString> texturesList;

  DzObject *Object = Node->getObject();
  DzShape *Shape = Object ? Object->getCurrentShape() : NULL;

  if (Shape)
  {
    QObjectList aMaterials = Shape->getAllMaterials();
    for (int x = 0; x < aMaterials.count(); x++)
    {
      DzMaterial *mat = (DzMaterial *)aMaterials[x];
      QObjectList aMaps = mat->getAllMaps();
      for (int y = 0; y < aMaps.count(); y++)
      {
        DzTexture *tex = (DzTexture *)aMaps[y];
        texturesList.insert(tex->getFilename());
      }
    }
  }
  std::list<QString> asList(texturesList.begin(), texturesList.end());
  return QStringList::fromStdList(asList);
}

void DzUnityTexturesDialog::PrepareDialog()
{
  DzNode *Selection = dzScene->getPrimarySelection();

  // DzNode *ParentFigureNode = Selection;
  // while (ParentFigureNode->getNodeParent())
  // {
  //   ParentFigureNode = ParentFigureNode->getNodeParent();
  //   if (DzSkeleton *Skeleton = ParentFigureNode->getSkeleton())
  //   {
  //     if (DzFigure *Figure = qobject_cast<DzFigure *>(Skeleton))
  //     {
  //       Selection = ParentFigureNode;
  //       break;
  //     }
  //   }
  // }

  textures.clear();
  textureList = GetAvailableTextures(Selection);
  for (int ChildIndex = 0; ChildIndex < Selection->getNumNodeChildren(); ChildIndex++)
  {
    DzNode *ChildNode = Selection->getNodeChild(ChildIndex);
    textureList.append(GetAvailableTextures(ChildNode));
  }
  UpdateTexturesList();
}

void DzUnityTexturesDialog::UpdateTexturesList()
{

  textureListWidget->clear();
  //foreach (TextureInfo texInfo, texsToExport)

  progressBar->setMaximum(textureList.count());
  progressBar->setValue(0);

  foreach (QString tex, textureList)
  {
    SUCCESS(QString("DAZRuntime - Setting up thread to load %1").arg(tex));

    // calling the signal
    emit loadImage(tex);
  }
}



#include "moc_DzUnityTexturesDialog.cpp"