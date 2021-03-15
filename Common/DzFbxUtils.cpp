#include "DzFbxUtils.h"

void DzFbxUtils::RenameDuplicateBones(FbxNode *RootNode)
{
  std::map<std::string, int> ExistingBones;
  RenameDuplicateBones(RootNode, ExistingBones);
}

void DzFbxUtils::RenameDuplicateBones(FbxNode *RootNode, std::map<std::string, int> &ExistingBones)
{
  if (RootNode == nullptr)
    return;

  FbxNodeAttribute *Attr = RootNode->GetNodeAttribute();
  if (Attr && Attr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
  {
    QString BoneName = RootNode->GetName();
    if (ExistingBones.find(BoneName.toStdString()) != ExistingBones.end())
    {
      ExistingBones[BoneName.toStdString()] += 1;
      BoneName = QString("%s_RENAMED_%d").arg(BoneName).arg(ExistingBones[BoneName.toStdString()]);

      RootNode->SetName(BoneName.toStdString().c_str());
    }
    else
    {
      ExistingBones.insert(make_pair(BoneName.toStdString(), 1));
    }
  }

  for (int ChildIndex = 0; ChildIndex < RootNode->GetChildCount(); ++ChildIndex)
  {
    FbxNode *ChildNode = RootNode->GetChild(ChildIndex);
    RenameDuplicateBones(ChildNode, ExistingBones);
  }
}
