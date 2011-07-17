/******************************************************************************
	ProgressCUI: Commandline Progress User Interface
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

#ifndef PROGRESSCUI_H
#define PROGRESSCUI_H

#include <QtCore/QString>

class ProgressCUI
{
public:
	ProgressCUI();

	void updateDisplay();
	void setText(const QString &text);
	void setValue(qint64 value);
	void setMaximum(qint64 max);
	void setProgress(qint64 byteRead, qint64 byteTotal);
	void setTimeRemain(int sec);
	void setTimeElapsed(int sec);
	void setSpeed(int speed);

private:
	int mTimeRemain, mTimeElapsed, mSpeed;
	qint64 mValue, mMax;
	QString mMessage;
};

#endif // PROGRESSCUI_H
