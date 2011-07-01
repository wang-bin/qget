/******************************************************************************
	Name: description
	Copyright (C) 2011 Wang Bin <wbsecg1@gmail.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
******************************************************************************/

#ifndef QDOWNLOADER_H
#define QDOWNLOADER_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtNetwork/QNetworkReply>

class QDownloaderPrivate;
class QNetworkReply;
class QAuthenticator;

class QDownloader : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(QDownloader)
	Q_PROPERTY(WriteMode writeMode READ writeMode WRITE setWriteMoede)
	Q_PROPERTY(bool overwite READ isOverwrite WRITE setOverwrite)
	Q_ENUMS(WriteMode)
public:
	// WriteOnDownload: use less RAM than when reading it at the finished() signal of the QNetworkReply
	typedef enum {
		WriteOnDownload, WriteOnFinished
	} WriteMode;

	explicit QDownloader(QObject *parent = 0);
	~QDownloader();

	QDownloader::WriteMode writeMode() const;
	void setWriteMoede(QDownloader::WriteMode pWriteMode);
	bool isOverwrite() const;
	void setOverwrite(bool pOverwrite);

	//void append(const QUrl& url);
	//void append(const QStringList& urls);
	void setUrls(const QStringList& urls);
	void setSaveDir(const QString& pSaveDir);
	void download(const QUrl& url);					//rename statRequest
	QString defaultSavePath(const QUrl& url);

signals:
	void finished(int); //0, 1

public slots:
	void start();
	//void pause();
	//void resume();
	void cancel();									//rename cancelAll
	void quitApp(int);

private:
	bool saveToDisk(const QString &savePath, QIODevice *data);
	void cancelReply(QNetworkReply*);

private slots:
	void slotFinished(QNetworkReply*);
	void slotReadyRead();
	void slotError(QNetworkReply::NetworkError);
	void updateProgress(qint64 byteRead, qint64 total);
	//void slotAuthenticationRequired(QNetworkReply*,QAuthenticator*);

protected:
	QDownloaderPrivate *d_ptr; //QDownloaderPrivate *const d_ptr;
private:
	QDownloader::WriteMode mWriteMode;

};

#endif // QDOWNLOADER_H
