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

#include "qdownloader.h"
#include "qdownloader_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

QDownloader::QDownloader(QObject *parent) :
	QObject(parent),d_ptr(new QDownloaderPrivate)
{
	Q_D(QDownloader);
	connect(&d->manager, SIGNAL(finished(QNetworkReply*)), SLOT(slotFinished(QNetworkReply*)));
}

QDownloader::~QDownloader()
{
	if (d_ptr!=NULL) {
		delete d_ptr;
		d_ptr = NULL;
	}
}

void QDownloader::setUrls(const QStringList &urls)
{
	Q_D(QDownloader);
	if (!d->urls.isEmpty())
		d->urls.clear();
	foreach(QString url, urls) {
		d->urls.append(QUrl::fromEncoded(url.toAscii())); //toLocal8Bit()
	}
}


void QDownloader::setSavePath(const QString &savePath)
{
	Q_D(QDownloader);
	d->savePath = savePath;
}
/*
int QDownloader::download(const QString &pUrl, const QString &savePath)
{
	static QDownloader downloader;
	downloader.setUrls(pUrl);
	downloader.setSavePath(savePath);
	downloader.start();

	return 0;
}
*/

void QDownloader::download(const QUrl &url)
{
	Q_D(QDownloader);
	QNetworkReply *reply = d->manager.get(QNetworkRequest(url));
	//connect(reply, SIGNAL(finished()), SLOT(slotFinished()));
	//connect(reply, SIGNAL(readyRead()), SLOT(slotReadyRead()));
	connect(reply, SIGNAL(downloadProgress(qint64,qint64)), SLOT(updateProgress(qint64,qint64)));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(slotError(QNetworkReply::NetworkError)));

	d->downloads.append(reply);
}

QString QDownloader::defaultSavePath(const QUrl &url)
{
	QString path = url.path();
	QString basename = QFileInfo(path).fileName();

	if (basename.isEmpty())
		basename = "download";

	if (QFile::exists(basename)) {
		// already exists, don't overwrite
		int i = 0;
		basename += '.';
		while (QFile::exists(basename + QString::number(i)))
			++i;

		basename += QString::number(i);
	}

	return basename;
}

void QDownloader::start()
{
	Q_D(QDownloader);

	foreach(QUrl url, d->urls) {
		download(url);
	}
}

void QDownloader::cancel()
{
	Q_D(QDownloader);
	//emit canceled?
	foreach (QNetworkReply *reply, d->downloads) {
		if(reply->isRunning())
			reply->abort();
	}
}

bool QDownloader::saveToDisk(const QString &savePath, QIODevice *data)
{
	QFile file(savePath);
	if (!file.open(QIODevice::WriteOnly)) {
		qWarning(qPrintable(tr("Could not open %1 for writing: %2\n").arg(savePath, file.errorString())));
		return false;
	}
	if (file.write(data->readAll())==-1) {
		qWarning(qPrintable(tr("Could not write to %1:\n").arg(savePath, file.errorString())));
		return false;
	}
	file.close();
	return true;
}

void QDownloader::slotFinished(QNetworkReply* reply)
{
	//QNetworkReply *reply = ((QNetworkReply*)sender());
	if (reply->canReadLine()) {
		QUrl url = reply->url();
		QString saveName = defaultSavePath(url);
		if (saveToDisk(saveName, reply))
			qDebug(qPrintable(tr("Download of %1 succeeded (saved to %2)").arg(url.toEncoded(), saveName)));
	} else {
		return;
	}
	Q_D(QDownloader);
	d->downloads.removeAll(reply);	//?
	reply->deleteLater();			//?

	if (d->downloads.isEmpty())
		QCoreApplication::instance()->quit();
}

void QDownloader::slotError(QNetworkReply::NetworkError)
{
	QNetworkReply *reply = ((QNetworkReply*)sender());
	QUrl url = reply->url();
	//TODO: convert QNetworkReply::NetworkError to string
	qDebug(qPrintable(tr("Download from %1 error: %2").arg(url.toEncoded(), reply->errorString())));
}

void QDownloader::updateProgress(qint64 byteRead, qint64 total)
{
	QNetworkReply *reply = ((QNetworkReply*)sender());
	QUrl url = reply->url();
	QByteArray address = url.toEncoded();

	//printf \r \b
	printf("\r %s %d/%d", address.right(address.length()-address.lastIndexOf('/')-1).constData(), byteRead, total); //\b: back \r  本行开始处
}
