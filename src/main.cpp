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
#include <QtCore/QLocale>
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

		return 1;
	}

	QString save_dir = "downloads";
	QStringList args = a.arguments();
	args.takeFirst();
	QStringList::iterator it=args.begin();
	while(it!=args.end()) {
		if((*it).startsWith("-P")) {
			if((*it)=="-P") {
				it = args.erase(it);
				save_dir = *it;
				it = args.erase(it);
			} else {
				save_dir = (*it).mid(2);
				it = args.erase(it);
			}
		} else {
			++it;
		}
	}

	QString locale = QLocale::system().name();
	qDebug("locale: %s", qPrintable(locale));
	QTranslator appTranslator;
	appTranslator.load("QDownloader-"+locale, "i18n");
	a.installTranslator(&appTranslator);;
	QTranslator qtTranslator;
	qtTranslator.load("qt_zh_CN");
	a.installTranslator(&qtTranslator);

	QDownloader qdown;
	QObject::connect(&qdown, SIGNAL(finished(int)), &qdown, SLOT(quitApp(int)));
	qdown.setOverwrite(true);
	qdown.setSaveDir(save_dir);
	QStringList urls = args;
	urls.takeAt(0);
	qdown.setUrls(urls);
	qdown.start();
	return a.exec();
}
