#ifndef CMSERVER_H
#define CMSERVER_H

#include <QObject>
#include <QMap>

#include "clientinstence.h"
#include "callentry.h"

class QTcpSocket;
class QTcpServer;

typedef QMap<QTcpSocket *, ClientInstence *> ConenctionMap;

class CMServer : public QObject
{
  Q_OBJECT
private:
  QTcpServer *mServer;
  ConenctionMap mConnections;

  CallEntry *entry;
public:
  explicit CMServer(QObject *parent = 0);
  void start(QString host, int port);
signals:

private slots:
  void newConnection();

  void disconnect();
};

#endif // CMSERVER_H
