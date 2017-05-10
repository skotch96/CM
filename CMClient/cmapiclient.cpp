#include "cmapiclient.h"

#include <QDebug>
#include <QDataStream>

CMApiClient::CMApiClient(QObject *parent) : QObject(parent)
{
  initialize();
}

void CMApiClient::initialize()
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
      // show descriptions to user and allow selection
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

void CMApiClient::startCall(int id)
{
  qDebug () << "Start record";
  mAudioRecord->record();
}

void CMApiClient::endCall()
{
  qDebug () << "Stop record";
  mAudioRecord->stop();
}

void CMApiClient::connectToHost(QString host, int port)
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

void CMApiClient::audioBufferProbed(const QAudioBuffer& buffer)
{
  qDebug() << "audioBufferProbed";
  QByteArray  arrBlock;
  QDataStream out(&arrBlock, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_8);

  int type = MessageType::CallFrame;
  out << type;
  out << buffer.byteCount();
  out.writeBytes((const char*)buffer.data(), buffer.byteCount());

  qDebug () << "Write to socket";
  mSocket->write(arrBlock);
}

void CMApiClient::connected()
{
  qDebug() << "Conenct done";
}

void CMApiClient::readyRead()
{
  qDebug() << "readyRead";
  QDataStream in(mSocket);
  in.setVersion(QDataStream::Qt_5_8);

  int type;
  int size;
  char *data;

  in >> type;

  switch (type) {
  case MessageType::CallFrame:
    in >> size;
    uint length;
    in.readBytes(data, length);
    playAudio(data, size);
    break;
  default:
    break;
  }


}

void CMApiClient::slotError(QAbstractSocket::SocketError err)
{
  qDebug() << "Socket error :" << mSocket->errorString();
}

void CMApiClient::playAudio(const char* data, int size)
{
  mAudioOutDevice->write(data, size);
}
