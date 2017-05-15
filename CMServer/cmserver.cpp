#include "cmserver.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>

#include <QDebug>
#include "messageinformation.h"

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

  //entry = new CallEntry(this);
}

void CMServer::newConnection()
{
  qDebug() << "New Connection";
  QTcpSocket *socket     = mServer->nextPendingConnection();
  ClientInstence *client = new ClientInstence(socket);

  mConnections.insert(socket, client);

  connect(socket, SIGNAL(disconnected()),
          this,   SLOT(disconnect()));

  connect(socket, SIGNAL(readyRead()),
          this,    SLOT(readyRead()));

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
  mClients.remove(client->getAccount()->name());

  client->disconect();
  delete client;
}
// TODO CREATE COMMAND FACTORY
void CMServer::readyRead()
{
  QTcpSocket *socket = dynamic_cast<QTcpSocket*>(sender());

  if (socket == NULL)
    return;

  ClientInstence *client = mConnections.value(socket, NULL);

  QDataStream stream(socket);
  int type = MessageType::Undefined;

  //while(!stream.atEnd()) {
    try {
      stream >> type;
      qDebug() << "readyRead:" << type << " " << socket->bytesAvailable() ;
      switch (type) {
      case MessageType::Auth: {
        QString name;
        QString pass;

        stream >> name;
        stream >> pass;

        QByteArray arr;
        QDataStream out(&arr, QIODevice::WriteOnly);
        out << (int) MessageType::Auth;

        if (mClients.value(name, NULL) == NULL) {
          out << true;
          client->setAccount(new Account(name, pass));
          mClients.insert(name, client);
          qDebug() << "Auth:" << name;
        } else {
          out << false;
        }

        socket->write(arr);
      } break;
      case MessageType::CallFrame: {
        CallEntry* call = client->getCall();
        if (call) {
          call->sendCallDataToEntry(client, stream);
        }
      } break;
      case MessageType::TextMessage: {
        MessageInformation msg(stream);
        ClientInstence *rec = mClients.value(msg.getRecipient(), NULL);
        if (rec) {
          QByteArray arr;
          QDataStream out(&arr, QIODevice::WriteOnly);
          out.setVersion(QDataStream::Qt_5_8);
          out << (int) MessageType::TextMessage;

          msg.saveToStream(out);
          rec->get()->write(arr);
        } else {
          // TODO SAVE HISTORY
        }

      } break;
      case MessageType::GetUserList: {
        QList<QString> list = mClients.keys();
        QByteArray arr;
        QDataStream out(&arr, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_8);

        out << (int) MessageType::GetUserList;
        out << list.size();

        foreach (QString buff, list) {
          out << buff;
        }

        socket->write(arr);
        qDebug() << "GetUserList " << mClients.keys();
      } break;
      case MessageType::StartCall: {
        QString recipient;
        stream >> recipient;

        QByteArray arr;
        QDataStream out(&arr, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_8);

        out << (int) MessageType::StartCall;
        out << client->getAccount()->name();

        ClientInstence* rec = mClients.value(recipient, NULL);
        if (rec != NULL) {
          rec->get()->write(arr);

          CallEntry *entry = new CallEntry(this);

          entry->add(client);
          entry->add(rec);

          mCalls.append(entry);
        } else {
          // TODO SEND ERROR
        }
      } break;
      case MessageType::SuccessCall: {
        CallEntry *entry = client->getCall();
        if (entry != NULL) {
          QByteArray arr;
          QDataStream out(&arr, QIODevice::WriteOnly);
          out.setVersion(QDataStream::Qt_5_8);
          out << (int) MessageType::SuccessCall;
          out << client->getAccount()->name();

          entry->getUser(0)->get()->write(arr);
        }
      } break;
      case MessageType::CanselCall: {
        CallEntry *entry = client->getCall();
        if (entry != NULL) {
          QByteArray arr;
          QDataStream out(&arr, QIODevice::WriteOnly);
          out.setVersion(QDataStream::Qt_5_8);
          out << (int) MessageType::CanselCall;
          out << client->getAccount()->name();

          entry->getUser(0)->get()->write(arr);

          mCalls.removeOne(entry);

          entry->destroy();
          delete entry;
        }
      } break;
      case MessageType::EndCall: {
         CallEntry *entry = client->getCall();
         if (entry != NULL) {
           QByteArray arr;
           QDataStream out(&arr, QIODevice::WriteOnly);
           out.setVersion(QDataStream::Qt_5_8);

           entry->getUser(0)->get()->write(arr);
           entry->getUser(1)->get()->write(arr);

           mCalls.removeOne(entry);

           entry->destroy();
           delete entry;
         } else {

         }
      } break;
      default:
        // TODO SEND MESSAGE
        break;
      }
      qDebug () << "readyRead end :" << socket->bytesAvailable();
    } catch(...) {
      qDebug () << "Error read package";
    }
 // }
}
