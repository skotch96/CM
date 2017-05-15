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

  if (mUsers.size() < 2) {
    // TODO CALL ENTRY
  }
}

ClientInstence* CallEntry::getUser(int index)
{
  return mUsers.at(index);
}

void CallEntry::destroy()
{
  foreach (ClientInstence *user, mUsers) {
    //if (sender == user) continue;
    remove(user);
  }
}

void CallEntry::sendCallDataToEntry(ClientInstence *sender, QDataStream &stream)
{
  uint       length;
  int        size;
  quint64    frameIndex;
  char*      data;
  QByteArray arrBlock;

  stream >> frameIndex;
  stream >> size;

  stream.readBytes(data, length);

  if (length == 0)
    return;

  qDebug () << "sendVoiceDataToEntry " << frameIndex << " " << size;
  QDataStream out(&arrBlock, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_8);

  int type = MessageType::CallFrame;

  out << type;
  out << frameIndex;
  out << size;

  // TODO Add crypto
  out.writeBytes(data, size);

  foreach (ClientInstence *user, mUsers) {
    //if (sender == user) continue;
    user->get()->write(arrBlock);
  }
}
