#include "cmserver.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>

#include <QDebug>

CMServer::CMServer(QObject *parent) : QObject(parent)
{
  mServer = NULL;
}

void CMServer::start(QString host, int port)
{
  if (mServer != NULL)
    delete mServer;

  mServer = new QTcpServer(this);
  if (! mServer->listen(QHostAddress(host), port)) {
    QMessageBox::critical(NULL, "Critical error", "Cant run server on " + host + ":" + QString::number(port));
    return;
  }

  connect(mServer, SIGNAL(newConnection()),
          this,    SLOT(newConnection()));

  qDebug() << "Serever start";

  entry = new CallEntry(this);
}

void CMServer::newConnection()
{
  qDebug() << "New Connection";
  QTcpSocket *socket     = mServer->nextPendingConnection();
  ClientInstence *client = new ClientInstence(socket);

  mConnections.insert(socket, client);

  connect(socket, SIGNAL(disconnected()),
          this,   SLOT(disconnect()));

  entry->add(client);
}

void CMServer::disconnect()
{
  QTcpSocket *socket = dynamic_cast<QTcpSocket*>(sender());

  if (socket == NULL)
    return;

  ClientInstence *client = mConnections.value(socket, NULL);

  if (!client)
    return;

  mConnections.remove(socket);
  client->disconect();

  delete client;
}
