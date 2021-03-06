#pragma once
#include <dzaction.h>
#include <dznode.h>
#include <dzjsonwriter.h>
#include <QtCore/qfile.h>
#include <QtCore/qtextstream.h>
#include <DzRuntimePluginAction.h>
#include "DzUnitySubdivisionDialog.h"

class DzUnityAction : public DzRuntimePluginAction {
	 Q_OBJECT
public:
	 DzUnityAction();

protected:
	 DzUnitySubdivisionDialog* SubdivisionDialog;
	 bool InstallUnityFiles;

	 void executeAction();
	 void WriteMaterials(DzNode* Node, DzJsonWriter& Stream);
	 void WriteConfiguration();
	 void SetExportOptions(DzFileIOSettings& ExportOptions);
	 void CreateUnityFiles(bool replace = true);

	 bool CopyFile(QFile *file, QString *dst, bool replace = true, bool compareFiles = true);
	 QString GetMD5(const QString &path);
	 QString appendPath(const QString &path1, const QString &path2);
};