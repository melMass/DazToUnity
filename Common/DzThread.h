#ifndef DZTHREAD_H
#define DZTHREAD_H

#include <QtCore>
#include <QImage>

#include "debug.h"

class DzThread final : public QThread
{
public:
  ~DzThread()
  {
    WARN("DAZRuntime - Thread Killed");
    quit();
    wait();
  }
};

class DzLoader : public QObject
{
  Q_OBJECT
public:
  explicit DzLoader(QObject *parent = nullptr) : QObject(parent) {}
signals:
  void imageLoaded(const QString &fichier, const QImage &);
public slots:
  void loadImage(const QString &fichier);
  
};
#endif