#include "clientinstence.h"
#include "callentry.h"

#include <QDebug>

Account *ClientInstence::getAccount() const
{
  return mAccount;
}

void ClientInstence::setAccount(Account *account)
{
  mAccount = account;
}

ClientInstence::ClientInstence(QTcpSocket *socket, QObject *parent)
  : QObject(parent)
{
  mSocket    = socket;
  mCallEntry = NULL;
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

 /* if (mSocket->isOpen())
    mSocket->close();
  delete mSocket;*/
}

void ClientInstence::readyRead()
{

}
