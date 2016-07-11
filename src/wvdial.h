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
#include <QSettings>

#include <QTimer>

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
	void setIcon( const QString& ) ;
	void run() ;
private:
	Ui::wvdial * m_ui ;

	QSystemTrayIcon m_trayIcon ;

	QProcess m_process ;

	QProcess m_process_0 ;

	QString m_exe ;
	QString m_interface ;

	int m_interval ;

	QSettings m_settings ;

	QRect m_dimensions ;

	QTimer m_timer ;

	quint64 m_sent = 0 ;
	quint64 m_received = 0 ;
	quint64 m_sent_old = 0 ;
	quint64 m_received_old = 0 ;

	bool m_quit = false ;
};

#endif // WVDIAL_H
