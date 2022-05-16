/**********************************************************************
	Copyright (c) 2002-2020 Daz 3D, Inc. All Rights Reserved.

	This file is part of the Daz Studio SDK.

	This file may be used only in accordance with the Daz Studio SDK
	license provided with the Daz Studio SDK.

	The contents of this file may not be disclosed to third parties,
	copied or duplicated in any form, in whole or in part, without the
	prior written permission of Daz 3D, Inc, except as explicitly
	allowed in the Daz Studio SDK license.

	See http://www.daz3d.com to contact Daz 3D, Inc or for more
	information about the Daz Studio SDK.
**********************************************************************/
/*****************************
	Include files
*****************************/
#include "dzsceneloader.h"

#include "dzfacetmesh.h"
#include "dznode.h"
#include "dzobject.h"
#include "dzscene.h"
#include "dzshape.h"

///////////////////////////////////////////////////////////////////////
// DzSceneLoader
///////////////////////////////////////////////////////////////////////

/**
	Default Constructor.
**/
DzSceneLoader::DzSceneLoader()
{
}

/**
	Loads the scene from the given file, and prints file statistics to the console.

	@param filename		An absolute path to the scene file to load.
**/
bool DzSceneLoader::loadScene( const QString &filename )
{
	QByteArray bytes = filename.toLatin1();

	printf( "Loading Scene %s...\n", bytes.data() );

	const DzError err = dzScene->loadScene( filename, DzScene::OpenNew );
	if ( err != DZ_NO_ERROR )
	{
		bytes = getErrorMessage( err ).toLatin1();
		printf( "Error Loading Scene: %s\n", bytes.data() );
		return false;
	}

	printf( "Scene loaded successfully.\n" );

	printSceneStats();

	return true;
}

/**
	Prints the statistics of the current scene to the console.
**/
void DzSceneLoader::printSceneStats()
{
	int nVerts;
	int nFacets;
	getGeometryStats( nVerts, nFacets );

	printf( "Scene Statistics:\n" );
	printf( "\tTotal Number of Nodes in Scene: %d\n", dzScene->getNumNodes() );
	printf( "\tFigures in Scene: %d\n", dzScene->getNumSkeletons() );
	printf( "\tCameras in Scene: %d\n", dzScene->getNumCameras() );
	printf( "\tLights in Scene: %d\n", dzScene->getNumLights() );
	printf( "\tTotal Vertices in Scene: %d\n", nVerts );
	printf( "\tTotal Facets in Scene: %d\n", nFacets );
}

/**
	Collects geometry statistics from the nodes in the current scene.

	@param nVerts		Set to the total number of vertices in all geometries in the scene.
	@param nFacets		Set to the total number of facets in all geometries in the scene.
**/
void DzSceneLoader::getGeometryStats( int &nVerts, int &nFacets )
{
	nVerts = 0;
	nFacets = 0;

	DzNodeListIterator nodeIt( dzScene->nodeListIterator() );
	while ( nodeIt.hasNext() )
	{
		int nodeVerts;
		int nodeFacets;

		DzNode* node = nodeIt.next();
		getGeometryStats( node, nodeVerts, nodeFacets );

		nVerts += nodeVerts;
		nFacets += nodeFacets;
	}
}

/**
	Collects geometry statistics for the given node.

	@param node			The node to get the geometry statistics for.
	@param nVerts		Set to the number of vertices for any geometry on the node.
	@param nFacets		Set to the number of facets for any geometry on the node.
**/
void DzSceneLoader::getGeometryStats( DzNode* node, int &nVerts, int &nFacets )
{
	nVerts = 0;
	nFacets = 0;

	assert( node );
	if ( !node )
	{
		return;
	}

	DzObject* obj = node->getObject();
	if ( !obj )
	{
		return;
	}

	DzShape* shape = obj->getCurrentShape();
	if ( !shape )
	{
		return;
	}

	DzGeometry* geom = shape->getGeometry();
	if ( !geom )
	{
		return;
	}

	nVerts = geom->getNumVertices();

	if ( DzFacetMesh* mesh = qobject_cast<DzFacetMesh*>( geom ) )
	{
		nFacets = mesh->getNumFacets();
	}
}

#include "moc_dzsceneloader.cpp"
