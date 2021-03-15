#ifndef DZUTILS_H
#define DZUTILS_H

#include <QStringList>
#include <QListWidgetItem>
#include "dznode.h"

struct MorphInfo
{
  QString Name;
  QString Label;
  QString Type;
  QString Path;

  inline bool operator==(MorphInfo other)
  {
    if (Name == other.Name)
    {
      return true;
    }
    return false;
  }

  MorphInfo()
  {
    Name = QString();
    Label = QString();
    Type = QString();
    Path = QString();
  }
};

// For sorting the lists
class SortingListItem : public QListWidgetItem
{

public:
  virtual bool operator<(const QListWidgetItem &otherItem) const
  {
    if (this->checkState() != otherItem.checkState())
    {
      return (this->checkState() == Qt::Checked);
    }
    return QListWidgetItem::operator<(otherItem);
  }
};

class DzUtils
{
public:
  static QStringList GetAvailableMorphs(DzNode *Node, QMap<QString, MorphInfo> &morphs);

  // ...lots of great stuff

private:
  // Remove contructor
  DzUtils() = delete;
};


#endif