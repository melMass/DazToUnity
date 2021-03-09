#include "DzThread.h"

void DzLoader::loadImage(const QString &fichier)
{
  SUCCESS(QString("DAZRuntime - Thread is loading %1").arg(fichier));
  QImage img(fichier);
  if (!img.isNull())
    emit imageLoaded(fichier, img);
}

#include "moc_DzThread.cpp"