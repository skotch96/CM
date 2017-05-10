#include "callentry.h"

#include <QDebug>

CallEntry::CallEntry(QObject *parent) : QObject(parent)
{

}

void CallEntry::add(ClientInstence *user)
{
  if (user == NULL)
    return;

  mUsers.append(user);
  user->setCall(this);
}

void CallEntry::remove(ClientInstence *user)
{
  mUsers.removeOne(user);
  user->setCall(NULL);
}

void CallEntry::sendCallDataToEntry(ClientInstence *sender, QDataStream &stream)
{
  qDebug () << "sendCallDataToEntry";

  int size;
  char *data;

  stream >> size;
  uint length;
  stream.readBytes(data, length);

  QByteArray arrBlock;
  QDataStream out(&arrBlock, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_8);

  int type = ClientInstence::CallFrame;
  out << type;
  out << size;
  out.writeBytes(data, size);

  foreach (ClientInstence *user, mUsers) {
    if (sender == user)
      continue;
    user->get()->write(arrBlock);
  }
}
