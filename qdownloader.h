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
public:
	typedef enum {
		WriteOnDownload, WriteOnFinished
	} WriteMode;

	explicit QDownloader(QObject *parent = 0);
	~QDownloader();

	QDownloader::WriteMode writeMode() const;
	void setWriteMoede(QDownloader::WriteMode pWriteMode);

	void setUrls(const QStringList& urls);
	void setSavePath(const QString& savePath);
	void download(const QUrl& url);
	QString defaultSavePath(const QUrl& url);

public slots:
	void start();
	//void pause();
	void cancel();
	//void continueDownloads();

private:
	bool saveToDisk(const QString &savePath, QIODevice *data);

private slots:
	//void slotFinished();
	void slotFinished(QNetworkReply*);
	void slotReadyRead();
	void slotError(QNetworkReply::NetworkError);
	void updateProgress(qint64 byteRead, qint64 total);
	//void slotAuthenticationRequired(QNetworkReply*,QAuthenticator*);

private:
	QDownloader::WriteMode mWriteMode;
	QDownloaderPrivate *d_ptr;
};

#endif // QDOWNLOADER_H
