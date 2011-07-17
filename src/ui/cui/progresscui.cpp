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

#include "progresscui.h"
#include <cstdio>
#include "convert.h"

ProgressCUI::ProgressCUI()
{
}

void ProgressCUI::updateDisplay()
{
	if (mMessage.isEmpty()) {
		//convert value, max, time, speed, byte to string
		static char get_str[11], total_str[11], speed_str[11], left_str[9], elapsed_str[9];
		memcpy(get_str, size2str(mValue), sizeof(get_str));
		memcpy(total_str, size2str(mMax), sizeof(total_str));
		memcpy(speed_str, size2str(mSpeed), sizeof(speed_str));
		memcpy(left_str, sec2str(mTimeRemain), sizeof(left_str));
		memcpy(elapsed_str, sec2str(mTimeElapsed), sizeof(elapsed_str));

		mMessage = QString("\r %1/%2  %3/s  %4/%5  (%6%%)").arg(get_str, 11).arg(total_str, -11)
					.arg(speed_str, 11).arg(left_str, 7).arg(elapsed_str).arg(100*mValue/mMax);
		//qint64 -> %lld
		//printf("\r %lld/%lld   %d/s   %ds/%ds   (%d%%)",
		//		mValue, mMax, mSpeed, mTimeRemain, mTimeElapsed, (100*mValue)/mMax);

		//printf("\r %11s/%-11s   %11s/s   %s/%s   (%d%%)" \
		//, get_str, total_str, speed_str, left_str, sec2str(mTimeElapsed) \
		//, (100*mByteRead)/mByteTotal); //\b:back \r:本行开始处
	}

	printf(qPrintable(mMessage));
	mMessage.clear();
}

void ProgressCUI::setText(const QString &text)
{
	mMessage = text;
}

void ProgressCUI::setValue(qint64 value)
{
	mValue = value;
}

void ProgressCUI::setMaximum(qint64 max)
{
	mMax = max;
}

void ProgressCUI::setProgress(qint64 byteRead, qint64 byteTotal)
{
	mValue = byteRead;
	mMax = byteTotal;
}

void ProgressCUI::setTimeRemain(int sec)
{
	mTimeRemain = sec;
}

void ProgressCUI::setTimeElapsed(int sec)
{
	mTimeElapsed = sec;
}

void ProgressCUI::setSpeed(int speed)
{
	mSpeed = speed;
}




