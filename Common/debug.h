#ifndef DEBUG_H
#define DEBUG_H

#include <QDebug>

QString colorizeFunc(QString name);

#define DEBUG(message)                                                            \
  (                                                                               \
      (qDebug() << colorizeFunc(Q_FUNC_INFO).toStdString().c_str() << ":" << QString(message).toStdString().c_str()), \
      (void)0)

#define SUCCESS(message)                                                                                                                           \
  (                                                                                                                                                \
      (qDebug() << "\033[1;32mSuccess\033[0m: " << colorizeFunc(Q_FUNC_INFO).toStdString().c_str() << ":" << QString(message).toStdString().c_str()), \
      (void)0)

#define WARN(message)                                                               \
  (                                                                                 \
      (qWarning() << colorizeFunc(Q_FUNC_INFO).toStdString().c_str() << ":" << QString(message).toStdString().c_str()), \
      (void)0)

#define CRITICAL(message)                                                            \
  (                                                                                  \
      (qCritical() << colorizeFunc(Q_FUNC_INFO).toStdString().c_str() << ":" << QString(message).toStdString().c_str()), \
      (void)0)

#define FATAL(message)                                                                                                   \
  (                                                                                                                      \
      (qFatal("%s : %s", colorizeFunc(Q_FUNC_INFO).toStdString().c_str(), QString(message).toStdString().c_str())), \
      (void)0)
#endif // DEBUG_H