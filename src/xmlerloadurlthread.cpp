#include "xmlerloadurlthread.h"

XMLerLoadUrlThread::XMLerLoadUrlThread (QObject *parent):
  QThread(parent)
{
	mNetworkManager = new QNetworkAccessManager(this);
	connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkReply(QNetworkReply*)));
};
XMLerLoadUrlThread::~XMLerLoadUrlThread ()
{
  delete mNetworkManager;
  this->deleteLater();
}
void XMLerLoadUrlThread::setUrl(QString s)
{
	url = QUrl(s);
};
QUrl XMLerLoadUrlThread::getUrl()
{
	return url;
};
QString XMLerLoadUrlThread::getTempFileName()
{
	return tempFile->fileName();
};

void XMLerLoadUrlThread::start()
{
	this->run();
}

void XMLerLoadUrlThread::run()
{
	QNetworkRequest request(url);
	QNetworkReply* rep = mNetworkManager->get(request);
	connect(rep, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(printProgress(qint64, qint64)));
	disconnect(rep, SIGNAL(downloadProgress(qint64, qint64)));
}

void XMLerLoadUrlThread::printProgress(qint64 bytesReceived, qint64 bytesTotal )
{
	_bytesTotal = bytesTotal;
}

void XMLerLoadUrlThread::onNetworkReply(QNetworkReply* reply)
{
	QString replyString;
	QString fileTemplate;
	fileTemplate.append(QDir::tempPath());
	fileTemplate.append(QDir::separator());
	fileTemplate.append("XMLer");
	tempFile = new QTemporaryFile(this);
	tempFile->setFileTemplate(fileTemplate);
	tempFile->setAutoRemove(true);
	int httpstatuscode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
	switch(httpstatuscode)
		{
			case 200:
				if (reply->isReadable())
					{
						if (!tempFile->open())
							{
								replyString = "fail";
								break;
							};
						tempFile->write(reply->readAll());
						tempFile->close();
						replyString = QString("complete.\nTemporary file: %1\n(%2)").arg(tempFile->fileName()).arg(_bytesTotal);
					}
				else replyString = "fail";
				break;
			default:
				if (reply->isReadable())
					{
						replyString = QString("fail.\nReply: %1\n(%2)").\
							arg(reply->readAll().data()).arg(_bytesTotal);
					}
				else replyString = "fail";
				break;
		};
	QString msg = QString("%1 | %2: download %3.").arg(reply->error()).arg(httpstatuscode).arg(replyString);
	emit urlDownloaded(reply->error(), tempFile->fileName(), msg);
	reply->deleteLater();
}
