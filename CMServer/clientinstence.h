#ifndef CLIENTINSTENCE_H
#define CLIENTINSTENCE_H

#include <QTcpSocket>
#include <QObject>

class CallEntry;

class ClientInstence : public QObject
{
  Q_OBJECT
private:
  QTcpSocket *mSocket;
  CallEntry  *mCallEntry;
public:
  enum MessageType {
    Undefined = -1,
    CallFrame = 0
  };

  ClientInstence(QTcpSocket *socket, QObject *parent = NULL);

  QTcpSocket *get();

  CallEntry *getCall();
  void setCall(CallEntry *entry);

  void disconect();

private slots:
  void readyRead();
};

#endif // CLIENTINSTENCE_H
