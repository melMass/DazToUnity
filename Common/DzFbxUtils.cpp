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

void DzFbxUtils::FixClusterTranformLinks(FbxScene *Scene, FbxNode *RootNode)
{
  FbxGeometry *NodeGeometry = static_cast<FbxGeometry *>(RootNode->GetMesh());

  // Create missing weights
  if (NodeGeometry)
  {

    for (int DeformerIndex = 0; DeformerIndex < NodeGeometry->GetDeformerCount(); ++DeformerIndex)
    {
      FbxSkin *Skin = static_cast<FbxSkin *>(NodeGeometry->GetDeformer(DeformerIndex));
      if (Skin)
      {
        for (int ClusterIndex = 0; ClusterIndex < Skin->GetClusterCount(); ++ClusterIndex)
        {
          // Get the current tranform
          FbxAMatrix Matrix;
          FbxCluster *Cluster = Skin->GetCluster(ClusterIndex);
          Cluster->GetTransformLinkMatrix(Matrix);

          // Update the rotation
          FbxDouble3 Rotation = Cluster->GetLink()->PostRotation.Get();
          Matrix.SetR(Rotation);
          Cluster->SetTransformLinkMatrix(Matrix);
        }
      }
    }
  }

  for (int ChildIndex = 0; ChildIndex < RootNode->GetChildCount(); ++ChildIndex)
  {
    FbxNode *ChildNode = RootNode->GetChild(ChildIndex);
    FixClusterTranformLinks(Scene, ChildNode);
  }
}

void DzFbxUtils::AddWeightsToAllNodes(FbxNode *Parent)
{
  //for (int ChildIndex = Parent->GetChildCount() - 1; ChildIndex >= 0; --ChildIndex)
  {
    FbxNode *ChildNode = Parent; //Parent->GetChild(ChildIndex);
    //RootBone->AddChild(ChildNode);
    const char* ChildName = ChildNode->GetName();
    //UE_LOG(LogTemp, Warning, TEXT("ChildNode %s Checking Weights"), *ChildName);

    if (FbxGeometry *NodeGeometry = static_cast<FbxGeometry *>(ChildNode->GetMesh()))
    {
      //UE_LOG(LogTemp, Warning, TEXT("  No Deformers"), *ChildName);
      if (NodeGeometry->GetDeformerCount() == 0)
      {
        FbxCluster *Cluster = FbxCluster::Create(Parent->GetScene(), "");
        Cluster->SetLink(Parent);
        Cluster->SetLinkMode(FbxCluster::eTotalOne);

        FbxSkin *Skin = FbxSkin::Create(Parent->GetScene(), "");
        Skin->AddCluster(Cluster);
        NodeGeometry->AddDeformer(Skin);

        for (int PolygonIndex = 0; PolygonIndex < ChildNode->GetMesh()->GetPolygonCount(); ++PolygonIndex)
        {
          for (int VertexIndex = 0; VertexIndex < ChildNode->GetMesh()->GetPolygonSize(PolygonIndex); ++VertexIndex)
          {
            int Vertex = ChildNode->GetMesh()->GetPolygonVertex(PolygonIndex, VertexIndex);
            Cluster->AddControlPointIndex(Vertex, 1.0f);
          }
        }
      }
    }

    //AddWeightsToAllNodes(ChildNode);
  }
}