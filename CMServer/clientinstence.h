#ifndef CLIENTINSTENCE_H
#define CLIENTINSTENCE_H

#include "account.h"
#include <QTcpSocket>
#include <QObject>
#include "protocol.h"

class CallEntry;

class ClientInstence : public QObject
{
  Q_OBJECT
private:
  QTcpSocket *mSocket;
  CallEntry  *mCallEntry;
  Account    *mAccount;
public:
  ClientInstence(QTcpSocket *socket, QObject *parent = NULL);

  QTcpSocket *get();

  CallEntry *getCall();
  void setCall(CallEntry *entry);

  void disconect();

  Account *getAccount() const;
  void setAccount(Account *account);
private slots:
  void readyRead();
};

#endif // CLIENTINSTENCE_H
