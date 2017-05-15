#include "cmclientengen.h"

#include <QDebug>
#include <QDataStream>

CMClientEngene::CMClientEngene(QObject *parent) : QObject(parent)
{
  initialize();
  mAccount = NULL;
}

void CMClientEngene::initialize()
{
  QAudioEncoderSettings setting;
  setting.setCodec("audio/PCM");
  setting.setQuality(QMultimedia::HighQuality);

  mAudioRecord = new QAudioRecorder(this);
  mAudioRecord->setEncodingSettings(setting);

  mProbe = new QAudioProbe(this);

  QStringList inputs = mAudioRecord->audioInputs();
  QString selectedInput;
  foreach (QString input, inputs) {
    QString description = mAudioRecord->audioInputDescription(input);
    selectedInput = input;
    qDebug() << selectedInput << description << "\n";
  }

  mAudioRecord->setAudioInput(selectedInput);
  mProbe->setSource(mAudioRecord);

  connect(mProbe, SIGNAL(audioBufferProbed(QAudioBuffer)),
          this,   SLOT(audioBufferProbed(QAudioBuffer)));

  QAudioFormat format;
  format.setChannelCount(1);
  format.setCodec("audio/pcm");
  format.setSampleRate(48000);
  format.setSampleSize(16);
  format.setByteOrder(QAudioFormat::LittleEndian);
  format.setSampleType(QAudioFormat::SignedInt);

  mAudioOut       = new QAudioOutput(format, this);
  mAudioOutDevice = mAudioOut->start();
}

void CMClientEngene::successCall()
{
  qDebug () << "Start record";

  QByteArray  arr;
  QDataStream out(&arr, QIODevice::WriteOnly);

  out << (int) SuccessCall;
  mSocket->write(out);
}

void CMClientEngene::canselCall()
{
  qDebug () << "Start record";

  QByteArray  arr;
  QDataStream out(&arr, QIODevice::WriteOnly);

  out << (int) CanselCall;
  out << mAccount->name();

  mSocket->write(out);
}

void CMClientEngene::startCall(const QString &recipient)
{
  qDebug () << "Start record";

  QByteArray  arr;
  QDataStream out(&arr, QIODevice::WriteOnly);

  out << (int) StartCall;
  out << recipient;

  mSocket->write(out);
}

void CMClientEngene::endCall()
{
  qDebug () << "Stop record";
  mAudioRecord->stop();

  QByteArray arr;
  QDataStream out(&arr, QIODevice::WriteOnly);

  out << (int) EndCall;

  mSocket->write(out);
}

void CMClientEngene::connectToHost(QString host, int port)
{
  mSocket = new QTcpSocket(this);
  mSocket->connectToHost(host, port);

  connect(mSocket, SIGNAL(connected()),
          this, SLOT(connected()));

  connect(mSocket, SIGNAL(readyRead()),
          this, SLOT(readyRead()));

  connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)),
          this, SLOT(slotError(QAbstractSocket::SocketError)));
}

void CMClientEngene::finilize()
{
  mAudioRecord->stop();
  mAudioOut->stop();

  if (!mSocket)
    return;

  /* if (mSocket->isOpen())
    mSocket->close();*/

  if (mAccount)
    delete mAccount;

  delete mSocket;
}

void CMClientEngene::loadAccountList()
{
  qDebug() << "loadAccountList";
  QByteArray  arrBlock;
  QDataStream out(&arrBlock, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_8);

  int type = MessageType::GetUserList;
  out << type;

  mSocket->write(arrBlock);
}

void CMClientEngene::readCallFrame(QDataStream &stream)
{
  uint    lengthRead;
  quint64 voiceFrameIndex;
  int     lengthExpected;
  char*   data;

  stream >> voiceFrameIndex;
  stream >> lengthExpected;

  if (mExpectedVoiceFrameIndex <= voiceFrameIndex) {
    stream.readBytes(data, lengthRead);
    qDebug() << "LengthRead: " << lengthRead << " " << lengthExpected;
    playAudio(data, lengthExpected);
  } else {
    qDebug () << "old frame";
  }

  mExpectedVoiceFrameIndex = voiceFrameIndex++;

}

void CMClientEngene::audioBufferProbed(const QAudioBuffer& buffer)
{
  qDebug() << "audioBufferProbed" << buffer.byteCount();

  int count = buffer.byteCount();
  if (count == 0)
    return;

  QByteArray  arrBlock;
  QDataStream out(&arrBlock, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_8);

  int type = MessageType::CallFrame;

  out << type;
  out << mLastVoiceFrameIndex;
  out << count;

  qDebug () << "Write to socket" << count;
  out.writeBytes((const char*)buffer.data(), count);

  mLastVoiceFrameIndex ++;
  mSocket->write(arrBlock);
}

void CMClientEngene::connected()
{
  qDebug() << "Conenct done";
  emit connectedDone();
}

void CMClientEngene::readyRead()
{
  qDebug() << "readyRead";
  QDataStream stream(mSocket);
  stream.setVersion(QDataStream::Qt_5_8);

  int type;

  try {
    stream >> type;
    qDebug () << "Type" << type;
    switch (type) {
    case CallFrame: {
      readCallFrame(stream);
    } break;
    case Auth: {
      bool isSuccess;
      stream >> isSuccess;
      qDebug() << "Auth resualt :" << isSuccess;
      if (!isSuccess)
        delete mAccount;

      emit authResualt(isSuccess);
    } break;
    case GetUserList: {
      int         count;
      QString     buffer;
      QStringList list;

      stream >> count;

      for(; count != 0; count --) {
        stream >> buffer;
        list << buffer;
      }
      qDebug() << "ACCOUNTS:" << list;
      emit accountList(list);
    } break;
    case TextMessage: {
      MessageInformation msg(stream);
      emit newTextMessage(msg);
    } break;
    case StartCall: {
      QString fromName;
      stream >> fromName;

      emit signalStartCall(fromName);
    } break;
    case SuccessCall: {
      QString fromName;
      stream >> fromName;

      mLastVoiceFrameIndex     = 0;
      mExpectedVoiceFrameIndex = 0;
      mAudioRecord->record();

      emit signalSuccessCall(fromName);
    } break;
    case EndCall: {
      mAudioRecord->stop();
      emit signalEndCall();
    }
    case CanselCall: {
      QString fromName;
      stream >> fromName;
      emit signalCanselCall(fromName);
    }
    default:
      break;
    }
  } catch (...) {
    qDebug () << "Error read chunck";
  }

}

void CMClientEngene::slotError(QAbstractSocket::SocketError err)
{
  qDebug() << "Socket error :" << mSocket->errorString();
}

void CMClientEngene::auth(const QString &strName, const QString &strPassword)
{
  QByteArray  arrBlock;
  QDataStream out(&arrBlock, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_8);

  out << (int)Auth;
  out << strName;
  out << strPassword;

  mAccount = new Account(strName, strPassword);
  mSocket->write(arrBlock);
}

void CMClientEngene::sendMessage(const QString &recipient, const QString &text)
{
  QByteArray arr;
  QDataStream stream(&arr, QIODevice::WriteOnly);

  QDate date = QDate::currentDate();
  QTime time = QTime::currentTime();

  if (mAccount) {
    MessageInformation msg(recipient, mAccount->name(), text, date, time);
    stream << (int) MessageType::TextMessage;
    msg.saveToStream(stream);

    mSocket->write(arr);
  } else {
    emit error("Auth error try auth");
  }
}

Account *CMClientEngene::account() const
{
  return mAccount;
}

void CMClientEngene::playAudio(const char* data, int size)
{
  mAudioOutDevice->write(data, size);
}
