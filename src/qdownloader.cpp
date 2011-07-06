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
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "convert.h"

QDownloader::QDownloader(QObject *parent) :
	QObject(parent),d_ptr(new QDownloaderPrivate)
{
	mWriteMode = QDownloader::WriteOnDownload;
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


QDownloader::WriteMode QDownloader::writeMode() const
{
	return mWriteMode;
}

void QDownloader::setWriteMoede(QDownloader::WriteMode pWriteMode)
{
	mWriteMode = pWriteMode;
}

bool QDownloader::isOverwrite() const
{
	Q_D(const QDownloader);
	return d->overwrite;
}

void QDownloader::setOverwrite(bool pOverwrite)
{
	Q_D(QDownloader);
	d->overwrite = pOverwrite;
}

void QDownloader::setUrls(const QStringList &urls)
{
	Q_D(QDownloader);
	if (!d->urls.isEmpty())
		d->urls.clear();
	foreach(const QString& url, urls) {
		d->urls.append(QUrl::fromEncoded(url.toAscii())); //toLocal8Bit()
	}
}


void QDownloader::setSaveDir(const QString &pSaveDir)
{
	Q_D(QDownloader);
	d->saveDir = pSaveDir;
	if (!QDir(d->saveDir).exists())
		if (!QDir().mkdir(d->saveDir))
			qDebug(qPrintable(tr("Failed to create dir").arg(d->saveDir)));
}

void QDownloader::download(const QUrl &url)
{
	Q_D(QDownloader);
	QNetworkReply *reply = d->manager.get(QNetworkRequest(url));
	connect(reply, SIGNAL(readyRead()), SLOT(slotReadyRead()));
	connect(reply, SIGNAL(downloadProgress(qint64,qint64)), SLOT(updateProgress(qint64,qint64)));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(slotError(QNetworkReply::NetworkError)));

	d->downloads.insert(reply, new QDownloadStatus(defaultSavePath(url)));
}

QString QDownloader::defaultSavePath(const QUrl &url)
{
	Q_D(QDownloader);
	QString path = url.path();
	QString basename = QFileInfo(path).fileName();

	if (basename.isEmpty())
		basename = "download";

	path = d->saveDir+"/"+basename;

	if (QFile::exists(path) && !d->overwrite) {
		int i = 0;
		basename += '.';
		while (QFile::exists(basename + QString::number(i))) ++i;
		basename += QString::number(i);
		path = d->saveDir+"/"+basename;
	}
	return path;
}

void QDownloader::start()
{
	Q_D(QDownloader);
	foreach(const QUrl& url, d->urls) {
		qDebug("Starting: %s", qPrintable(url.toString()));
		download(url);
		++d->totalDownloads;
	}
}

void QDownloader::cancel()
{
	Q_D(QDownloader);
	foreach (QNetworkReply *reply, d->downloads.keys()) {
		cancelReply(reply);
	}
	//emit canceled?
}

void QDownloader::quitApp(int exitCode)
{
	qDebug("exit code: %d", exitCode);
	QCoreApplication::instance()->exit(exitCode);
}

bool QDownloader::saveToDisk(const QString &savePath, QIODevice *data)
{
	QFile file(savePath);
	if (!file.open(QIODevice::WriteOnly)) {
		qWarning(qPrintable(tr("Could not open %1 for writing: %2\n").arg(QFileInfo(file).absoluteFilePath(), file.errorString())));
		return false;
	}
	if (file.write(data->readAll())==-1) {
		qWarning(qPrintable(tr("Could not write to %1:\n").arg(QFileInfo(file).absoluteFilePath(), file.errorString())));
		return false;
	}
	file.close();
	return true;
}


void QDownloader::cancelReply(QNetworkReply *reply)
{
	Q_D(QDownloader);
	if(reply->isRunning()) {
		//reply->abort();  //FUCK!
		QFile *tmpFile = d->downloads[reply]->file;
		//Remove it or not, that's a problem!
		if (tmpFile->exists())
			if (!tmpFile->remove())
				qWarning(qPrintable(tr("Failed to remove %1").arg(tmpFile->fileName())));
		qDebug(qPrintable(tr("Reply canceled: %1").arg(reply->url().toString())));
		delete d->downloads.take(reply);
		reply->deleteLater();
	}
	qDebug("replies remain: %d", d->downloads.size());
}

void QDownloader::slotFinished(QNetworkReply* reply)
{
	qDebug(" ");
	Q_D(QDownloader);
	QUrl url = reply->url();
	QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	if (reply->error()) {
		qDebug(qPrintable(tr("Download failed: %1.").arg(reply->errorString())));
	} else if (!redirectionTarget.isNull()) {
		QUrl newUrl = url.resolved(redirectionTarget.toUrl());
		qDebug(qPrintable(tr("Redirect to %1").arg(newUrl.toString())));
		cancelReply(reply);
		QFile *file = d->downloads.take(reply)->file;
		if (file->isOpen())
			file->remove();
		file->deleteLater(); //?
		reply->deleteLater();//?
		download(newUrl);
		return;
	} else {
		if (mWriteMode == QDownloader::WriteOnFinished) {
			QString savePath = defaultSavePath(url);
			if (saveToDisk(savePath, reply)) {
				qDebug(qPrintable(tr("Download succeeded!\n[%1 ==> %2]").arg(url.toString(), QFileInfo(savePath).absoluteFilePath())));
				++d->succeedDownloads;
			}
		} else {
			QFile *saveFile =d->downloads.value(reply)->file;
			saveFile->close();
			qDebug(qPrintable(tr("Download succeeded!\n[%1 ==> %2]").arg(url.toString(), QFileInfo(*saveFile).absoluteFilePath())));
			++d->succeedDownloads;
		}
	}
	delete d->downloads.take(reply);
	reply->deleteLater();			//?

	if (d->downloads.isEmpty()) {
		qDebug(qPrintable(tr("Download finished. %1 succeeded, %2 failed").arg(d->succeedDownloads).arg(d->totalDownloads-d->succeedDownloads)));
		emit finished(d->succeedDownloads!=d->totalDownloads);
	}
}

void QDownloader::slotReadyRead()
{
	if (mWriteMode == QDownloader::WriteOnFinished)
		return;

	Q_D(QDownloader);
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	QFile *saveFile = d->downloads.value(reply)->file;

	if (!saveFile->isOpen()) {
		if (!saveFile->open(QIODevice::WriteOnly)) {
			qWarning(qPrintable(tr("Could not open %1 for writing: %2\n").arg(saveFile->fileName(), saveFile->errorString())));
			return;
		}
	}

	if (!saveFile->atEnd())
		saveFile->seek(saveFile->size());

	if (saveFile->write(reply->readAll())==-1)
		qWarning(qPrintable(tr("Could not write to %1:\n").arg(saveFile->fileName(), saveFile->errorString())));
}

void QDownloader::slotError(QNetworkReply::NetworkError)
{
	QNetworkReply *reply = (QNetworkReply*)sender();
	qWarning(qPrintable(tr("Download error: %1").arg(reply->errorString())));
	cancelReply(reply);
}

void QDownloader::updateProgress(qint64 byteRead, qint64 total)
{
	Q_D(QDownloader);
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	QString fileName = reply->url().toString();
	if (reply!=d->currentReply) {
		d->currentReply = reply;
		qDebug("\n%s", qPrintable(fileName));
	}

	QDownloadStatus *ds = d->downloads.value(reply);
	ds->estimate(byteRead, total);

	static char get_str[11], total_str[11], speed_str[11], left_str[9];
	memcpy(get_str, size2str(byteRead), sizeof(get_str));
	memcpy(total_str, size2str(total), sizeof(total_str));
	memcpy(speed_str, size2str(ds->speed), sizeof(speed_str));
	memcpy(left_str, sec2str(ds->time_left), sizeof(left_str));
	//qint64 -> %lld
	//printf("\r %11s/%-11s %11s/s %7ds/%ds (%d%%)"
	printf("\r %11s/%-11s %11s/s %s/%s (%d%%)" \
	, get_str, total_str, speed_str, left_str, sec2str(ds->time_elapsed) \
	, (100*byteRead)/total); //\b:back \r:本行开始处
}
