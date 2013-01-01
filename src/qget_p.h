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

#ifndef QDOWNLOADER_P_H
#define QDOWNLOADER_P_H

#include <limits>
#include <QtCore/QFile>
#include <QtCore/QObject>
#include <QtCore/QTime>
#include <QtCore/QUrl>
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtNetwork/QNetworkAccessManager>

#include "ui/progressui.h"

const double kkinv = 1./1000.;
class DownloadStatus
{
public:
	DownloadStatus(const QString& save):progress(0),byte_get(0),byte_total(std::numeric_limits<qint64>::max()) \
	,time_remain(std::numeric_limits<qint64>::max()),speed(0){
		setSavePath(save);
		//time.start();
	}
	~DownloadStatus() {
		if (file->isOpen())
			file->close();
		if (file) {
			delete file;
			file = NULL;
		}
		if (progress) {
			delete progress;
			progress = NULL;
		}
	}

	void estimate(qint64 get, qint64 total) {
		QMutexLocker locker(&mutex);
        if (total != 0)
            byte_total = total;
        byte_get = get;
		time_elapsed = time.elapsed();
		time_remain = (byte_total-byte_get)*time_elapsed/(byte_get+1)*kkinv;
		speed = (byte_get*1000)/(time_elapsed+1);
		time_elapsed /= 1000;
	}

	bool setSavePath(const QString& save) {
		save_path = save;
		if (!save_path.isEmpty()) {
			file = new QFile(save_path);
			return file->open(QIODevice::WriteOnly);
		}
		return true;
	}

	void updateProgress() {
		QMutexLocker locker(&mutex);
		progress->setMaximum(byte_total);
		progress->setValue(byte_get);
		progress->setTimeRemain(time_remain);
		progress->setTimeElapsed(time_elapsed);
		progress->setSpeed(speed);
		progress->update();
	}

	ProgressUI *progress;
	QFile *file;
	QString save_path;
	qint64 byte_get, byte_total;
	int time_remain, time_elapsed, speed;
	QTime time;
	QMutex mutex;
};

class QGet;

class QGetPrivate
{
	Q_DECLARE_PUBLIC(QGet)
public:
	QGetPrivate() :currentReply(0),parallel(true), resumeBrokenTransfer(false),overwrite(true) \
	,succeedDownloads(0),totalDownloads(0),saveDir("."){}
	~QGetPrivate() {
		if (!urls.isEmpty())
			urls.clear();
		if (!downloads.isEmpty()) {
			qDeleteAll(downloads);
			downloads.clear();
		}
	}

	QNetworkReply *currentReply;
	QMap<QNetworkReply*, DownloadStatus*> downloads;
	QNetworkAccessManager manager;
	QList<QUrl> urls;

	bool parallel, resumeBrokenTransfer;
	bool overwrite;
	int succeedDownloads, totalDownloads;
	int numberThreads;
	QString saveDir;

	QGet* q_ptr;
};

#endif // QDOWNLOADER_P_H
