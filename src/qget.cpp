/******************************************************************************
	QGet: A QtNetwork based downloader.
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

#include "qget.h"
#include "qget_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "convert.h"
#include "ui/cui/progresscui.h"

QGet::QGet(QObject *parent) :
	QObject(parent),d_ptr(new QGetPrivate)
{
	mWriteMode = QGet::WriteOnDownload;
	mUi = QGet::Cui;
	Q_D(QGet);
	connect(&d->manager, SIGNAL(finished(QNetworkReply*)), SLOT(slotFinished(QNetworkReply*)));
}

QGet::~QGet()
{
	if (d_ptr!=NULL) {
		delete d_ptr;
		d_ptr = NULL;
	}
}

void QGet::setUiType(UiType ui)
{
	mUi = ui;
}

QGet::WriteMode QGet::writeMode() const
{
	return mWriteMode;
}

void QGet::setWriteMoede(QGet::WriteMode pWriteMode)
{
	mWriteMode = pWriteMode;
}

bool QGet::isOverwrite() const
{
	Q_D(const QGet);
	return d->overwrite;
}

void QGet::setOverwrite(bool pOverwrite)
{
	Q_D(QGet);
	d->overwrite = pOverwrite;
}

bool QGet::isParallel() const
{
	Q_D(const QGet);
	return d->parallel;
}

void QGet::setParallel(bool parallel)
{
	Q_D(QGet);
	d->parallel = parallel;
}

void QGet::setUrls(const QStringList &urls)
{
	Q_D(QGet);
	if (!d->urls.isEmpty())
		d->urls.clear();
	foreach(const QString& url, urls) {
		d->urls.append(QUrl::fromEncoded(url.toAscii())); //toLocal8Bit()
	}
}


void QGet::setSaveDir(const QString &pSaveDir)
{
	Q_D(QGet);
	d->saveDir = pSaveDir;
	if (!QDir(d->saveDir).exists())
		if (!QDir().mkdir(d->saveDir))
			qDebug(qPrintable(tr("Failed to create dir").arg(d->saveDir)));
}

void QGet::download(const QUrl &url)
{
	Q_D(QGet);
	QNetworkReply *reply = d->manager.get(QNetworkRequest(url));
	connect(reply, SIGNAL(metaDataChanged()), SLOT(slotTransferStarted()));
	connect(reply, SIGNAL(readyRead()), SLOT(slotReadyRead()));
	connect(reply, SIGNAL(downloadProgress(qint64,qint64)), SLOT(updateProgress(qint64,qint64)));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(slotError(QNetworkReply::NetworkError)));

	DownloadStatus* status = new DownloadStatus(defaultSavePath(url));
	if (mUi==QGet::Cui)
		status->progress = new ProgressUI(new ProgressCUI);
	d->downloads.insert(reply, status);
}

QString QGet::defaultSavePath(const QUrl &url)
{
	Q_D(QGet);
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

void QGet::start()
{
	Q_D(QGet);
	foreach(const QUrl& url, d->urls) {
		qDebug("Starting: %s", qPrintable(url.toString()));
		download(url);
		++d->totalDownloads;
	}
	startTimer(1000);
}

void QGet::cancel()
{
	Q_D(QGet);
	foreach (QNetworkReply *reply, d->downloads.keys()) {
		cancelReply(reply);
	}
	//emit canceled?
}

void QGet::quitApp(int exitCode)
{
	qDebug("exit code: %d", exitCode);
	QCoreApplication::instance()->exit(exitCode);
}

void QGet::timerEvent(QTimerEvent *)
{
	Q_D(QGet);
	foreach(DownloadStatus *status, d->downloads.values()) {
		if (d->downloads.key(status)->isRunning()) //do not update unconnected status
			status->updateProgress();
	}
}

bool QGet::saveToDisk(const QString &savePath, QIODevice *data)
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


void QGet::cancelReply(QNetworkReply *reply)
{
	Q_D(QGet);
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

void QGet::slotTransferStarted()
{
	Q_D(QGet);
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	QString fileName = reply->url().toString();
	qDebug("Connected to %s. Start transfering data...", qPrintable(fileName));
	d->downloads[reply]->time.start();
}

void QGet::slotFinished(QNetworkReply* reply)
{
	qDebug(" ");
	Q_D(QGet);
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
		if (mWriteMode == QGet::WriteOnFinished) {
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

void QGet::slotReadyRead()
{
	if (mWriteMode == QGet::WriteOnFinished)
		return;

	Q_D(QGet);
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

void QGet::slotError(QNetworkReply::NetworkError)
{
	QNetworkReply *reply = (QNetworkReply*)sender();
	qWarning(qPrintable(tr("Download error: %1").arg(reply->errorString())));
	cancelReply(reply);
}

void QGet::updateProgress(qint64 byteRead, qint64 total)
{
	Q_D(QGet);
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	QString fileName = reply->url().toString();
	if (reply!=d->currentReply) {
		d->currentReply = reply;
		qDebug("\nUrl: %s", qPrintable(fileName));
	}

	DownloadStatus *ds = d->downloads.value(reply);
	ds->estimate(byteRead, total);
	ds->updateProgress();
}
