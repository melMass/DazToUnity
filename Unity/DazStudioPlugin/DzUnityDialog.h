#pragma once
#include "dzbasicdialog.h"
#include <QtGui/qcombobox.h>
#include <QtCore/qsettings.h>

class QPushButton;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QGroupBox;

class DzUnityDialog : public DzBasicDialog {
	Q_OBJECT
public:

	/** Constructor **/
	 DzUnityDialog(QWidget *parent);

	/** Destructor **/
	virtual ~DzUnityDialog() {}

	QLineEdit* assetNameEdit;
	QLineEdit* projectEdit;
	QPushButton* projectButton;
	QComboBox* assetTypeCombo;
	QLineEdit* assetsFolderEdit;
	QPushButton* assetsFolderButton;
	QPushButton* morphsButton;
	QCheckBox* morphsEnabledCheckBox;
	QPushButton* subdivisionButton;
	QCheckBox* subdivisionEnabledCheckBox;
	QPushButton* textureButton;
	QPushButton *aMorphsButton;
	QGroupBox* advancedSettingsGroupBox;
#ifdef FBXOPTIONS
	QCheckBox* showFbxDialogCheckBox;
#endif
	QCheckBox* installUnityFilesCheckBox;

	// Pass so the DazTRoUnityAction can access it from the morph dialog
	QString GetMorphString();

	// Pass so the DazTRoUnityAction can access it from the morph dialog
	QMap<QString,QString> GetMorphMapping() { return morphMapping; }

	void Accepted();
private slots:
	void HandleSelectAssetsFolderButton();
	void HandleChooseMorphsButton();
	void HandleChooseTexturesButton();
	void HandleChooseAnimatedMorphsButton();
	void HandleMorphsCheckBoxChange(int state);
	void HandleChooseSubdivisionsButton();
	void HandleSubdivisionCheckBoxChange(int state);
#ifdef FBXOPTIONS
	void HandleShowFbxDialogCheckBoxChange(int state);
#endif
	void HandleInstallUnityFilesCheckBoxChange(int state);

private:
	QSettings* settings;

	// These are clumsy leftovers from before the dialog were singletons
	QString morphString;
	QMap<QString,QString> morphMapping;
};
