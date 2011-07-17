/******************************************************************************
	ProgressUI: Progress User Interface
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

#ifndef PROGRESSUI_H
#define PROGRESSUI_H

#include <algorithm>
#include <QtCore/QString>

class ProgressUI
{
public:
    ProgressUI();
	template<typename T>
	ProgressUI(T* ui):ui(new IProgressUIHolder<T>(ui)){}

	ProgressUI(const ProgressUI& other);

	~ProgressUI();

	template<typename T>
	ProgressUI& operator=(const ProgressUI& that)
	{
		std::swap(ProgressUI(that).ui, (*this).ui);
		return *this;
	}

	ProgressUI& operator=(ProgressUI that)
	{
		std::swap(that.ui, (*this).ui);
		return *this;
	}

	void update() {ui->update();}
	void setText(const QString& text){ui->setText(text);}
	void setValue(qint64 value) {ui->setValue(value);}
	void setMaximum(qint64 max) {ui->setMaximum(max);}
	void setProgress(qint64 byteRead, qint64 byteTotal) {ui->setProgress(byteRead, byteTotal);}
	void setTimeRemain(int sec) {ui->setTimeRemain(sec);}
	void setTimeElapsed(int sec) {ui->setTimeElapsed(sec);}
	void setSpeed(int speed) {ui->setSpeed(speed);}

	class IProgressUI {
	public:
		virtual ~IProgressUI(){}
		virtual IProgressUI* clone() const = 0;

		virtual void update() = 0;
		virtual void setText(const QString &text) = 0;
		virtual void setValue(qint64 value) = 0;
		virtual void setMaximum(qint64 max) = 0;
		virtual void setProgress(qint64 byteRead, qint64 byteTotal) = 0;
		virtual void setTimeRemain(int sec) = 0;
		virtual void setTimeElapsed(int sec) = 0;
		virtual void setSpeed(int speed) = 0;

	};
	template<typename T>
	class IProgressUIHolder :public IProgressUI
	{
	public:
		IProgressUIHolder(T* value):held(value){}
		virtual IProgressUIHolder* clone() const {return new IProgressUIHolder(held);}

		virtual void update() {held->updateDisplay();}
		virtual void setText(const QString &text) {held->setText(text);}
		virtual void setValue(qint64 value) {held->setValue(value);}
		virtual void setMaximum(qint64 max) {held->setMaximum(max);}
		virtual void setProgress(qint64 byteRead, qint64 byteTotal) {held->setProgress(byteRead, byteTotal);}
		virtual void setTimeRemain(int sec) {held->setTimeRemain(sec);}
		virtual void setTimeElapsed(int sec) {held->setTimeElapsed(sec);}
		virtual void setSpeed(int speed) {held->setSpeed(speed);}

		T *held;
	private:
		IProgressUIHolder& operator=(const IProgressUIHolder&);
	};

//private:
	IProgressUI *ui;
private:
	//QString message;

};

#endif // PROGRESSUI_H
