#ifndef DZFBXUTILS_H
#define DZFBXUTILS_H
#include <fbxsdk.h>
#include <map>
#include <iostream>
#include "qstring.h"

class DzFbxUtils
{
  public:
    static void RenameDuplicateBones(FbxNode *RootNode);
    static void AddWeightsToAllNodes(FbxNode *Parent);
    static void FixClusterTranformLinks(FbxScene *Scene, FbxNode *RootNode);

    static void PostExport(std::string path);

  private:
    static void RenameDuplicateBones(FbxNode *RootNode, std::map<std::string, int> &ExistingBones);
};
#endif
