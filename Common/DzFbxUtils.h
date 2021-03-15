#include <fbxsdk.h>
#include <map>
#include <iostream>
#include "qstring.h"

class DzFbxUtils
{
  public:
    static void RenameDuplicateBones(FbxNode *RootNode);

  private:
    static void RenameDuplicateBones(FbxNode *RootNode, std::map<std::string, int> &ExistingBones);
};
