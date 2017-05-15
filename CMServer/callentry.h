#ifndef CALLENTRY_H
#define CALLENTRY_H

#include <QList>

#include "clientinstence.h"

class CallEntry : public QObject
{
  Q_OBJECT
private:
  QList<ClientInstence*> mUsers;

public:
  explicit CallEntry(QObject *parent = 0);

  void add(ClientInstence *user);
  void remove(ClientInstence *user);

  ClientInstence *getUser(int index);

  void destroy();

  void sendCallDataToEntry(ClientInstence *sender, QDataStream &stream);
signals:

public slots:
};

#endif // CALLENTRY_H
