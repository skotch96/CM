#include "clientinstence.h"
#include "callentry.h"

#include <QDebug>

ClientInstence::ClientInstence(QTcpSocket *socket, QObject *parent)
  : QObject(parent)
{
  mSocket    = socket;
  mCallEntry = NULL;

  connect(mSocket, SIGNAL(readyRead()),
          this,    SLOT(readyRead()));
}

QTcpSocket *ClientInstence::get()
{
  return mSocket;
}

CallEntry *ClientInstence::getCall()
{
  return mCallEntry;
}

void ClientInstence::setCall(CallEntry *entry)
{
  mCallEntry = entry;
}

void ClientInstence::disconect()
{
  if (mCallEntry) {
    mCallEntry->remove(this);
  }

  mSocket->close();
  delete mSocket;
}

void ClientInstence::readyRead()
{
  qDebug() << "readyRead";

  QDataStream stream(mSocket);

  int type = MessageType::Undefined;

  stream >> type;
  qDebug () << "Read type :" << type;
  switch (type) {
  case MessageType::CallFrame: {
    if (mCallEntry) {
      mCallEntry->sendCallDataToEntry(this, stream);
    }
   } break;
  default:
    // TODO SEND MESSAGE
    break;
  }
  qDebug () << "readyRead end :" << type;
}
