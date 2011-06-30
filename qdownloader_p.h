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

#ifndef QDOWNLOADER_P_H
#define QDOWNLOADER_P_H

#include <QtCore/QFile>
#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QMap>
#include <QtNetwork/QNetworkAccessManager>

class QDownloader;

class QDownloaderPrivate
{
	Q_DECLARE_PUBLIC(QDownloader)
public:
	QDownloaderPrivate() :currentReply(0),resumeBrokenTransfer(false){}
	~QDownloaderPrivate() {
		if (!urls.isEmpty())
			urls.clear();
		if (!downloads.isEmpty()) {
			qDeleteAll(downloads);
			downloads.clear();
		}
	}

	QNetworkReply *currentReply;
	QMap<QNetworkReply*, QFile*> downloads;
	QNetworkAccessManager manager;
	QList<QUrl> urls;

	bool resumeBrokenTransfer;
	int failedDownloads, totalDownloads;
	int numberThreads;

	QDownloader* q_ptr;
};

#endif // QDOWNLOADER_P_H
