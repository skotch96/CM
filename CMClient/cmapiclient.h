#ifndef CMAPICLIENT_H
#define CMAPICLIENT_H

#include <QObject>
#include <QAudioRecorder>
#include <QMediaPlayer>
#include <QAudioProbe>
#include <QAudioOutput>
#include <QTcpSocket>

class CMApiClient : public QObject
{
  Q_OBJECT
private:
  QAudioRecorder *mAudioRecord;
  QAudioRecorder *mRecord;
  QAudioProbe    *mProbe;
  QAudioOutput   *mAudioOut;
  QIODevice      *mAudioOutDevice;
  QTcpSocket     *mSocket;

  quint64 mLastVoiceFrameIndex;
  quint64 mExpectedVoiceFrameIndex;

  void playAudio(const char* data, int size);
public:
  enum MessageType {
    Undefined = -1,
    CallFrame = 0,
    StartCall,
    SuccessCall,
    CanselCall,
    EndCall,
    TextMessage
  };

  explicit CMApiClient(QObject *parent = 0);

  void initialize();

  void startCall(int id);
  void endCall();

  void connectToHost(QString host, int port);

  void finilize();
signals:
  void error(QString msg);
  void newTextMessage(QString from, QString message);
  void signalEndCall();
  void signalCall();

private slots:
  void audioBufferProbed(const QAudioBuffer &buffer);

  void connected();
  void readyRead();
  void slotError(QAbstractSocket::SocketError err);
};

#endif // CMAPICLIENT_H
