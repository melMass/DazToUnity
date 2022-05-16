#pragma once

/*****************************
	Include files
*****************************/

#include <QtCore/QObject>

/*****************************
	Forward declarations
*****************************/

class DzNode;

/*****************************
	Class definitions
*****************************/

class DzSceneLoader : public QObject {
	Q_OBJECT
public:

	// Constructor
	DzSceneLoader();

	bool	loadScene( const QString &filename );

private:

	void	printSceneStats();
	void	getGeometryStats( int &nVerts, int &nFacets );
	void	getGeometryStats( DzNode* node, int &nVerts, int &nFacets );
};
