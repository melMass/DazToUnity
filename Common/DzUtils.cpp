#include "DzUtils.h"

#include "dzscene.h"
#include "dzskeleton.h"
#include "dzfigure.h"
#include "dzobject.h"
#include "dzshape.h"
#include "dzmorph.h"
#include "dzproperty.h"
#include "dzpresentation.h"

QMap<QString, MorphInfo> DzUtils::GetAvailableMorphs(DzNode *Node, bool recursive )
{
  QMap<QString, MorphInfo> morphs;

  DzObject *Object = Node->getObject();
  DzShape *Shape = Object ? Object->getCurrentShape() : NULL;

  for (int index = 0; index < Node->getNumProperties(); index++)
  {
    DzProperty *property = Node->getProperty(index);
    QString propName = property->getName();
    QString propLabel = property->getLabel();
    DzPresentation *presentation = property->getPresentation();

    if (presentation)
    {
      MorphInfo morphInfo;
      morphInfo.Name = propName;
      morphInfo.Label = propLabel;
      morphInfo.Path = Node->getLabel() + "/" + property->getPath();
      morphInfo.Type = presentation->getType();
      if (!morphs.contains(morphInfo.Name))
      {
        morphs.insert(morphInfo.Name, morphInfo);
      }
      //qDebug() << "Property Name " << propName << " Label " << propLabel << " Presentation Type:" << presentation->getType() << "Path: " << property->getPath();
      //qDebug() << "Path " << property->getGroupOnlyPath();
    }
    // if (presentation && presentation->getType() == "Modifier/Shape")
    // {
    //   SortingListItem *item = new SortingListItem(); // modLabel, morphListWidget);
    //   item->setText(propLabel);
    //   item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    //   if (morphList.contains(propLabel))
    //   {
    //     item->setCheckState(Qt::Checked);
    //     newMorphList.append(propName);
    //   }
    //   else
    //   {
    //     item->setCheckState(Qt::Unchecked);
    //   }
    //   item->setData(Qt::UserRole, propName);
    //   morphNameMapping.insert(propName, propLabel);
    // }
  }

  if (Object)
  {
    for (int index = 0; index < Object->getNumModifiers(); index++)
    {
      DzModifier *modifier = Object->getModifier(index);
      QString modName = modifier->getName();
      QString modLabel = modifier->getLabel();
      DzMorph *mod = qobject_cast<DzMorph *>(modifier);
      if (mod)
      {
        for (int propindex = 0; propindex < modifier->getNumProperties(); propindex++)
        {
          DzProperty *property = modifier->getProperty(propindex);
          QString propName = property->getName();
          QString propLabel = property->getLabel();
          DzPresentation *presentation = property->getPresentation();
          if (presentation)
          {
            MorphInfo morphInfoProp;
            morphInfoProp.Name = modName;
            morphInfoProp.Label = propLabel;
            morphInfoProp.Path = Node->getLabel() + "/" + property->getPath();
            morphInfoProp.Type = presentation->getType();
            if (!morphs.contains(morphInfoProp.Name))
            {
              morphs.insert(morphInfoProp.Name, morphInfoProp);
            }
            //qDebug() << "Modifier Name " << modName << " Label " << propLabel << " Presentation Type:" << presentation->getType() << " Path: " << property->getPath();
            //qDebug() << "Path " << property->getGroupOnlyPath();
          }
        }
      }
    }

    if (recursive)
    {
      for (int ChildIndex = 0; ChildIndex < Node->getNumNodeChildren(); ChildIndex++)
      {
      }
    }
  }

  return morphs;
}