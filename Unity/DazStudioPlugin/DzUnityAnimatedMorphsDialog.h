#pragma once
#include "dzbasicdialog.h"
#include <QtGui/qlineedit.h>
#include <QtGui/qcombobox.h>
#include <QtCore/qsettings.h>

#include <QProgressBar>
#include "dznode.h"
#include "DzThread.h"

#include "DzUtils.h"

class QListWidget;
class QListWidgetItem;
class QTreeWidget;
class QTreeWidgetItem;
class QLineEdit;
class QComboBox;

// struct MorphInfo {
// 	 QString Name;
// 	 QString Label;
// 	 QString Type;
// 	 QString Path;

// 	 inline bool operator==(MorphInfo other)
// 	 {
// 		  if (Path == other.Path)
// 		  {
// 				return true;
// 		  }
// 		  return false;
// 	 }

//    MorphInfo()
//    {
// 		  Name = QString();
// 		  Label = QString();
// 		  Type = QString();
// 		  Path = QString();
// 	 }
// };


class DzUnityAnimatedMorphsDialog : public DzBasicDialog {

	 Q_OBJECT
public:

	 DzUnityAnimatedMorphsDialog(QWidget* parent);
	 virtual ~DzUnityAnimatedMorphsDialog() {}

	 // Setup the dialog
	 void PrepareDialog();

	 QProgressBar *progressBar;

	 // Singleton access
	 static DzUnityAnimatedMorphsDialog* Get(QWidget* Parent)
	 {
		  if (singleton == nullptr)
		  {
				singleton = new DzUnityAnimatedMorphsDialog(Parent);
		  }
		  singleton->PrepareDialog();
		  return singleton;
	 }

public slots:

	// void imageAvailable(const QString &fichier, const QImage &img);

	// void FilterChanged(const QString &filter);

	// void ItemSelectionChanged();
	// void HandleCollectmorphsButton();

// signals:
// 	void loadImage(const QString &fichier);
// 	void imageLoaded(const QString &fichier, const QImage &img);

private:
	// Recursive function for finding all morphs for a node
	QStringList GetAvailableMorphs(DzNode *Node);

	void UpdateMorphsList();

  // morphs currently selected in the left tree box
  QList<MorphInfo> selectedInTree;

  static DzUnityAnimatedMorphsDialog *singleton;

  // A list of all found morphs.
  QStringList morphList;

  // Mapping of morph name to MorphInfo
  QMap<QString, MorphInfo> morphs;

  // Force the size of the dialog
  QSize minimumSizeHint() const override;

  // Widgets the dialog will access after construction
  QListWidget *morphListWidget;
  QLineEdit *filterEdit;

  QTreeWidgetItem *morphTreeItem;

	DzLoader m_loader;
	DzThread m_loaderThread;
};
