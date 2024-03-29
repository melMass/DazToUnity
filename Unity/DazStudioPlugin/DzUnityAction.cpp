#include <QtGui/qcheckbox.h>
#include <QtGui/QMessageBox>
#include <QtNetwork/qudpsocket.h>
#include <QtNetwork/qabstractsocket.h>
#include <QCryptographicHash>
#include <QtCore/qdir.h>

#include <dzapp.h>
#include <dzscene.h>
#include <dzmainwindow.h>
#include <dzshape.h>
#include <dzproperty.h>
#include <dzobject.h>
#include <dzpresentation.h>
#include <dznumericproperty.h>
#include <dzimageproperty.h>
#include <dzcolorproperty.h>
#include <dpcimages.h>

#include "DzUnityDialog.h"
#include "DzUnityAction.h"

DzUnityAction::DzUnityAction() :
	 DzRuntimePluginAction(tr("&Daz to Unity"), tr("Send the selected node to Unity."))
{
	 SubdivisionDialog = nullptr;
	 QAction::setIcon(QIcon(":/Images/icon"));
}

void DzUnityAction::executeAction()
{
	 // Check if the main window has been created yet.
	 // If it hasn't, alert the user and exit early.
	 DzMainWindow* mw = dzApp->getInterface();
	 if (!mw)
	 {
		  QMessageBox::warning(0, tr("Error"),
				tr("The main window has not been created yet."), QMessageBox::Ok);

		  return;
	 }

	 // Create and show the dialog. If the user cancels, exit early,
	 // otherwise continue on and do the thing that required modal
	 // input from the user.

	 if (dzScene->getNumSelectedNodes() != 1)
	 {
		  QMessageBox::warning(0, tr("Error"),
				tr("Please select one Character or Prop to send."), QMessageBox::Ok);
		  return;
	 }

	 // Create the dialog
	 DzUnityDialog* dlg = new DzUnityDialog(mw);

	 // If the Accept button was pressed, start the export
	 if (dlg->exec() == QDialog::Accepted)
	 {
		  //Create Daz3D folder if it doesn't exist
		  QDir dir;
		  ImportFolder = appendPath(dlg->assetsFolderEdit->text(),"Daz3D");
		  dir.mkpath(ImportFolder);

		  // Collect the values from the dialog fields
		  CharacterName = dlg->assetNameEdit->text();
		  CharacterFolder = appendPath(ImportFolder,CharacterName);
		  CharacterFBX = appendPath(CharacterFolder, CharacterName + ".fbx");
		  AssetType = dlg->assetTypeCombo->currentText().replace(" ", "");
		  MorphString = dlg->GetMorphString();
		  ExportMorphs = dlg->morphsEnabledCheckBox->isChecked();
		  ExportSubdivisions = dlg->subdivisionEnabledCheckBox->isChecked();
		  MorphMapping = dlg->GetMorphMapping();
#ifdef FBXOPTIONS
		  ShowFbxDialog = dlg->showFbxDialogCheckBox->isChecked();
#endif
		  InstallUnityFiles = dlg->installUnityFilesCheckBox->isChecked();

		  CreateUnityFiles(true);

		  SubdivisionDialog = DzUnitySubdivisionDialog::Get(dlg);
		  SubdivisionDialog->LockSubdivisionProperties(ExportSubdivisions);
		  FBXVersion = QString("FBX 2014 -- Binary");

		  Export();

		  //Rename the textures folder
		  QDir textureDir(appendPath(CharacterFolder , CharacterName + ".images"));
		  textureDir.rename(appendPath(CharacterFolder , CharacterName + ".images"), appendPath(CharacterFolder, "Textures"));
	 }
}

QString DzUnityAction::GetMD5(const QString &path)
{
	auto algo = QCryptographicHash::Md5;
	QFile sourceFile(path);
    qint64 fileSize = sourceFile.size();
    const qint64 bufferSize = 10240;

    if (sourceFile.open(QIODevice::ReadOnly))
    {
        char buffer[bufferSize];
        int bytesRead;
        int readSize = qMin(fileSize, bufferSize);

        QCryptographicHash hash(algo);
        while (readSize > 0 && (bytesRead = sourceFile.read(buffer, readSize)) > 0)
        {
            fileSize -= bytesRead;
            hash.addData(buffer, bytesRead);
            readSize = qMin(fileSize, bufferSize);
        }

        sourceFile.close();
        return QString(hash.result().toHex());
    }
    return QString();
}

QString DzUnityAction::appendPath(const QString &path1, const QString &path2)
{
	return QDir::cleanPath(path1 + QDir::separator() + path2);
	//return path1 + QDir::separator() + path2;
}

bool DzUnityAction::CopyFile(QFile *file, QString *dst, bool replace, bool compareFiles)
{
	bool dstExists = QFile::exists(*dst);

	if(replace)
	{
		if(compareFiles && dstExists)
		{
			auto srcFileMD5 = GetMD5(file->fileName());
			auto dstFileMD5 = GetMD5(*dst);

			if(srcFileMD5.length() > 0 && dstFileMD5.length() > 0 && srcFileMD5.compare(dstFileMD5) == 0)
			{
				return false;
			}
		}

		if(dstExists)
		{
			QFile::remove(*dst);
		}
	}

	/*
	if(dstExists)
	{
		QFile::setPermissions(QFile::ReadOther | QFile::WriteOther);
	}
	*/

	//ensure our output destination file has the correct file permissions
	//file->setPermissions(QFile::ReadOther | QFile::WriteOther);

	auto result = file->copy(*dst);

	if(QFile::exists(*dst))
	{
		QFile::setPermissions(*dst, QFile::ReadOther | QFile::WriteOther);
	}

	return result;
}

void DzUnityAction::CreateUnityFiles(bool replace)
{
	 if (!InstallUnityFiles)
		  return;

		// This approach is actually pretty good, but for now it's out of sync. This is not a user project anyway.
		return;
	 //Create shader folder if it doesn't exist
	 QDir dir;
	 QString scriptsFolder = appendPath(ImportFolder, "Scripts");
	 dir.mkpath(scriptsFolder);

	 QStringList scripts = QDir(":/Scripts/").entryList();
	 for (int i = 0; i < scripts.size(); i++)
	 {
		  QString script = appendPath(scriptsFolder, scripts[i]);
		  QFile file(":/Scripts/" + scripts[i]);
		  CopyFile(&file, &script, replace);
		  file.close();
	 }

	 //Create editor folder if it doesn't exist
	 QString editorFolder = appendPath(appendPath(ImportFolder, "Scripts"), "Editor");
	 dir.mkpath(editorFolder);

	 QStringList editorScripts = QDir(":/Editor/").entryList();
	 for (int i = 0; i < editorScripts.size(); i++)
	 {
		  QString script = appendPath(editorFolder, editorScripts[i]);
		  QFile file(":/Editor/" + editorScripts[i]);
		  CopyFile(&file, &script, replace);
		  file.close();
	 }

	 //Create shader folder if it doesn't exist
	 QString shaderFolder = appendPath(ImportFolder, "Shaders");
	 dir.mkpath(shaderFolder);

	 QStringList shaders = QDir(":/Shaders/").entryList();
	 for (int i = 0; i < shaders.size(); i++)
	 {
		  QString shader = appendPath(shaderFolder, shaders[i]);
		  QFile file(":/Shaders/" + shaders[i]);
		  CopyFile(&file, &shader, replace);
		  file.close();
	 }

	 //Create shader helpers folder if it doesn't exist
	 QString shaderHelperFolder = appendPath(appendPath(ImportFolder, "Shaders"), "Helpers");
	 dir.mkpath(shaderHelperFolder);

	 QStringList shaderHelpers = QDir(":/ShaderHelpers/").entryList();
	 for (int i = 0; i < shaderHelpers.size(); i++)
	 {
		  QString shaderHelper = appendPath(shaderHelperFolder, shaderHelpers[i]);
		  QFile file(":/ShaderHelpers/" + shaderHelpers[i]);
		  CopyFile(&file, &shaderHelper, replace);
		  file.close();
	 }

	 //Create vendors folder if it doesn't exist
	 QString vendorsFolder = appendPath(ImportFolder,"Vendors");
	 dir.mkpath(vendorsFolder);

	 QStringList vendors = QDir(":/Vendors/").entryList();
	 for (int i = 0; i < vendors.size(); i++)
	 {
		  QString vendor = appendPath(vendorsFolder, vendors[i]);
		  QFile file(":/Vendors/" + vendors[i]);
		  CopyFile(&file, &vendor, replace);
		  file.close();
	 }

	 //Create DiffusionProfiles folder if it doesn't exist
	 QString profilesFolder = appendPath(ImportFolder,"DiffusionProfiles");
	 dir.mkpath(profilesFolder);

	 QStringList profiles = QDir(":/DiffusionProfiles/").entryList();
	 for (int i = 0; i < profiles.size(); i++)
	 {
		  QString profile = appendPath(profilesFolder , profiles[i]);
		  QFile file(":/DiffusionProfiles/" + profiles[i]);
		  CopyFile(&file, &profile, replace);
		  file.close();
	 }

	 //Create Resources folder if it doesn't exist
	 QString resourcesFolder = appendPath(ImportFolder, "Resources");
	 dir.mkpath(resourcesFolder);

	 QStringList resources = QDir(":/Resources/").entryList();
	 for (int i = 0; i < resources.size(); i++)
	 {
		  QString resource = appendPath(resourcesFolder, resources[i]);
		  QFile file(":/Resources/" + resources[i]);
		  CopyFile(&file, &resource, replace);
		  file.close();
	 }
}

void DzUnityAction::WriteConfiguration()
{
	 QString DTUfilename = appendPath(CharacterFolder, CharacterName + ".dtu");
	 QFile DTUfile(DTUfilename);
	 DTUfile.open(QIODevice::WriteOnly);
	 DzJsonWriter writer(&DTUfile);
	 writer.startObject(true);
	 writer.addMember("AssetId", Selection->getAssetId());
	 writer.addMember("AssetName", CharacterName);
	 writer.addMember("AssetType", AssetType);
	 writer.addMember("FBXFile", CharacterFBX);
	 writer.addMember("ImportFolder", CharacterFolder);

	 writer.startMemberArray("Materials", true);
	 WriteMaterials(Selection, writer);
	 writer.finishArray();

	 writer.startMemberArray("Morphs", true);
	 if (ExportMorphs)
	 {
		  for (QMap<QString, QString>::iterator i = MorphMapping.begin(); i != MorphMapping.end(); ++i)
		  {
				writer.startObject(true);
				writer.addMember("Name", i.key());
				writer.addMember("Label", i.value());
				writer.finishObject();
		  }
	 }
	 writer.finishArray();

	 writer.startMemberArray("Subdivisions", true);
	 if (ExportSubdivisions)
		  SubdivisionDialog->WriteSubdivisions(writer);

	 writer.finishArray();
	 writer.finishObject();

	 DTUfile.close();
}

// Setup custom FBX export options
void DzUnityAction::SetExportOptions(DzFileIOSettings& ExportOptions)
{
	 ExportOptions.setBoolValue("doSelected", true);
	 ExportOptions.setBoolValue("doLights", true);
	 ExportOptions.setBoolValue("doCameras", true);

	 ExportOptions.setBoolValue("doAnims", AssetType == "Animation");


	 if ((AssetType == "SkeletalMesh" || AssetType == "Animation") && ExportMorphs && MorphString != "")
	 {
		  ExportOptions.setBoolValue("doMorphs", true);
		  ExportOptions.setStringValue("rules", MorphString);
	 }
	 else
	 {
		  ExportOptions.setBoolValue("doMorphs", false);
		  ExportOptions.setStringValue("rules", "");
	 }

	 ExportOptions.setIntValue("RunSilent", !ShowFbxDialog);

	 ExportOptions.setBoolValue("doEmbed", true);
	 ExportOptions.setBoolValue("doDiffuseOpacity", true);
	 ExportOptions.setBoolValue("doMergeClothing", true);
	 ExportOptions.setBoolValue("doCopyTextures", true);
	 ExportOptions.setBoolValue("doLocks", true);
	 ExportOptions.setBoolValue("doLimits", true);
}

// Write out all the surface properties
void DzUnityAction::WriteMaterials(DzNode* Node, DzJsonWriter& Writer)
{
	 DzObject* Object = Node->getObject();
	 DzShape* Shape = Object ? Object->getCurrentShape() : NULL;

	 if (Shape)
	 {
		  for (int i = 0; i < Shape->getNumMaterials(); i++)
		  {
				DzMaterial* Material = Shape->getMaterial(i);
				if (Material)
				{
					 Writer.startObject(true);
					 Writer.addMember("Version", 2);
					 Writer.addMember("AssetName", Node->getLabel());
					 Writer.addMember("MaterialName", Material->getName());
					 Writer.addMember("MaterialType", Material->getMaterialName());

					 DzPresentation* presentation = Node->getPresentation();
					 if (presentation != nullptr)
					 {
						  const QString presentationType = presentation->getType();
						  Writer.addMember("Value", presentationType);
					 }
					 else
					 {
						  Writer.addMember("Value", QString("Unknown"));
					 }

					 Writer.startMemberArray("Properties", true);
					 for (int propertyIndex = 0; propertyIndex < Material->getNumProperties(); propertyIndex++)
					 {
						  DzProperty* Property = Material->getProperty(propertyIndex);
						  DzImageProperty* ImageProperty = qobject_cast<DzImageProperty*>(Property);
						  if (ImageProperty)
						  {
								QString Name = Property->getName();
								QString TextureName = "";

								if (ImageProperty->getValue())
								{
									 TextureName = ImageProperty->getValue()->getFilename();
								}

								Writer.startObject(true);
								Writer.addMember("Name", Name);
								Writer.addMember("Value", Material->getDiffuseColor().name());
								Writer.addMember("DataType", QString("Texture"));
								Writer.addMember("Texture", TextureName);
								Writer.finishObject();
								continue;
						  }

						  DzColorProperty* ColorProperty = qobject_cast<DzColorProperty*>(Property);
						  if (ColorProperty)
						  {
								QString Name = Property->getName();
								QString TextureName = "";

								if (ColorProperty->getMapValue())
								{
									 TextureName = ColorProperty->getMapValue()->getFilename();
								}

								Writer.startObject(true);
								Writer.addMember("Name", Name);
								Writer.addMember("Value", ColorProperty->getColorValue().name());
								Writer.addMember("DataType", QString("Color"));
								Writer.addMember("Texture", TextureName);
								Writer.finishObject();
								continue;
						  }

						  DzNumericProperty* NumericProperty = qobject_cast<DzNumericProperty*>(Property);
						  if (NumericProperty)
						  {
								QString Name = Property->getName();
								QString TextureName = "";

								if (NumericProperty->getMapValue())
								{
									 TextureName = NumericProperty->getMapValue()->getFilename();
								}

								Writer.startObject(true);
								Writer.addMember("Name", Name);
								Writer.addMember("Value", QString::number(NumericProperty->getDoubleValue()));
								Writer.addMember("DataType", QString("Double"));
								Writer.addMember("Texture", TextureName);
								Writer.finishObject();
						  }
					 }
					 Writer.finishArray();

					 Writer.finishObject();
				}
		  }
	 }

	 DzNodeListIterator Iterator = Node->nodeChildrenIterator();
	 while (Iterator.hasNext())
	 {
		  DzNode* Child = Iterator.next();
		  WriteMaterials(Child, Writer);
	 }
}


#include "moc_DzUnityAction.cpp"
