#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QString>
#include <QObject>
#include <QDebug>
#include <QTemporaryFile>
#include <QThread>

class XMLerLoadUrlThread : public QThread
{
  Q_OBJECT
public:
  explicit XMLerLoadUrlThread(QObject *parent = 0);
  ~XMLerLoadUrlThread();
  void setUrl(QString s);
  QUrl getUrl();
  QString getTempFileName();
  void start();

signals:
  void urlDownloaded(int c, QString fileName, QString msg);

private slots:
  void onNetworkReply(QNetworkReply* reply);
  void printProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
  QUrl url;
  qint64 _bytesTotal;
  void run();
  QTemporaryFile* tempFile;
  QNetworkAccessManager* mNetworkManager;
};
