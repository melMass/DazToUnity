#ifndef DZUTILS_H
#define DZUTILS_H

#include <QStringList>
#include <QListWidgetItem>
#include "dznode.h"
#include <QDir>
#include "DTUConfig.h"
#include "debug.h"
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
  static QMap<QString, MorphInfo> GetAvailableMorphs(DzNode *Node, bool recursive = false);
  static std::string SanitizeName(std::string OriginalName);
  static QString appendPath(const QString &path1, const QString &path2);

  // ...lots of great stuff

private:
  // Remove contructor
  DzUtils() = delete;
};


#endif
