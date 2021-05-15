#pragma once

#include <QMap>

#include <iostream>
#include "qstring.h"
#include "DzUtils.h"
#include "DTUConfig.h"

#ifndef DZFBXUTILS_H
#define DZFBXUTILS_H

#include <fbxsdk.h>

class DzFbxUtils
{
  public:
    static void RenameDuplicateBones(FbxNode *RootNode);
    static void AddWeightsToAllNodes(FbxNode *Parent);
    static void FixClusterTranformLinks(FbxScene *Scene, FbxNode *RootNode);

    static void PostExport(const DTUConfig config);

  private:
    static void RenameDuplicateBones(FbxNode *RootNode, QMap<QString, int> &ExistingBones);
};
#endif
