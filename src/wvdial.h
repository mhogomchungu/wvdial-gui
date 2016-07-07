/*
 *
 *  Copyright (c) 2016
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WVDIAL_H
#define WVDIAL_H

#include <QMainWindow>
#include <QSystemTrayIcon>

#include <QAction>
#include <QProcess>

#include <QByteArray>

namespace Ui {
class wvdial;
}

class wvdial : public QMainWindow
{
	Q_OBJECT
public:
	explicit wvdial() ;
	~wvdial();

	void start() ;
private slots:
	void run() ;
	void hasEvent( const QByteArray& ) ;
private:
	Ui::wvdial * m_ui ;

	QSystemTrayIcon m_trayIcon ;

	QProcess m_process ;

	QByteArray m_data ;
};

#endif // WVDIAL_H
