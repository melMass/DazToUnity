#pragma once
#include "dzbasicdialog.h"
#include <QtGui/qlineedit.h>
#include <QtGui/qcombobox.h>
#include <QtCore/qsettings.h>
#include <QProgressBar>
#include "dznode.h"
#include "DzThread.h"

class QListWidget;
class QListWidgetItem;
class QTreeWidget;
class QTreeWidgetItem;
class QLineEdit;
class QComboBox;

struct TextureInfo {
	 QString Name;
	 QString Label;
	 QString Type;
	 QString Path;

	 inline bool operator==(TextureInfo other)
	 {
		  if (Path == other.Path)
		  {
				return true;
		  }
		  return false;
	 }

   TextureInfo()
   {
		  Name = QString();
		  Label = QString();
		  Type = QString();
		  Path = QString();
	 }
};


class DzUnityTexturesDialog : public DzBasicDialog {

	 Q_OBJECT
public:

	 DzUnityTexturesDialog(QWidget* parent);
	 virtual ~DzUnityTexturesDialog() {}

	 // Setup the dialog
	 void PrepareDialog();

	 QProgressBar *progressBar;

	 // Singleton access
	 static DzUnityTexturesDialog* Get(QWidget* Parent)
	 {
		  if (singleton == nullptr)
		  {
				singleton = new DzUnityTexturesDialog(Parent);
		  }
		  singleton->PrepareDialog();
		  return singleton;
	 }

public slots:

	void imageAvailable(const QString &fichier, const QImage &img);

	// void FilterChanged(const QString &filter);

	// void ItemSelectionChanged();
	// void HandleCollectTexturesButton();

signals:
	void loadImage(const QString &fichier);
	void imageLoaded(const QString &fichier, const QImage &img);

private:
	// Recursive function for finding all textures for a node
	QStringList GetAvailableTextures(DzNode *Node);

	void UpdateTexturesList();

  // Textures currently selected in the left tree box
  QList<TextureInfo> selectedInTree;

  static DzUnityTexturesDialog *singleton;

  // A list of all found textures.
  QStringList textureList;

  // Mapping of texture name to TextureInfo
  QMap<QString, TextureInfo> textures;

  // Force the size of the dialog
  QSize minimumSizeHint() const override;

  // Widgets the dialog will access after construction
  QListWidget *textureListWidget;
  QLineEdit *filterEdit;

  QTreeWidgetItem *textureTreeItem;

	DzLoader m_loader;
	DzThread m_loaderThread;
};
