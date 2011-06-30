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

#include <QtCore/QCoreApplication>
#include <QtCore/QTranslator>

#include "qdownloader.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	if (argc<2) {
		qDebug("%s - downloads all URLs in parallel\n"
			"Usage: download url1 [url2... urlN]\n"
			"\n"
			"Downloads the URLs passed in the command-line to the local directory\n"
			"If the target file already exists, a .0, .1, .2, etc. is appended to\n"
			"differentiate.\n", qPrintable(a.applicationFilePath()));

		return 0;
	}

	QTranslator appTranslator;
	appTranslator.load("QDownloader-zh_CN", "i18n");
	//a.installTranslator(&appTranslator);;
	QTranslator qtTranslator;
	qtTranslator.load("zh_CN");
	a.installTranslator(&qtTranslator);

	QDownloader qdown;
	QStringList urls = a.arguments();
	urls.takeAt(0);
	qdown.setUrls(urls);
	qdown.start();
	return a.exec();
}
