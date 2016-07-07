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

#include "wvdial.h"
#include "ui_wvdial.h"

#include <QMetaObject>
#include <QDebug>

#include <QIcon>
#include <QMenu>

#include <QCoreApplication>

#include <QProcess>

#include <QTimer>
#include <QEventLoop>

wvdial::wvdial() : m_ui( new Ui::wvdial )
{
	m_ui->setupUi( this ) ;

	QIcon icon( ":/off.png" ) ;

	m_trayIcon.setIcon( icon ) ;

	this->setWindowIcon( icon ) ;

	m_ui->pbConnect->setText( tr( "Connect" ) ) ;
}

wvdial::~wvdial()
{
	delete m_ui ;
}

void wvdial::start()
{
	QMetaObject::invokeMethod( this,"run",Qt::QueuedConnection ) ;
}

void wvdial::run()
{
	auto _notConnected = [ this ](){

		auto e = m_ui->pbConnect->text() ;

		e.remove( '&' ) ;

		return e == tr( "Connect" ) ;
	} ;

	connect( m_ui->pbConnect,&QPushButton::clicked,[ this,_notConnected ](){

		if( _notConnected() ){

			m_ui->pbConnect->setEnabled( false ) ;

			m_ui->pbConnect->setText( tr( "Disconnect" ) ) ;

			m_process.start( "wvdial" ) ;

			QIcon icon( ":/on.png" ) ;

			m_trayIcon.setIcon( icon ) ;

			this->setWindowIcon( icon ) ;

			QTimer m ;

			QEventLoop e ;

			connect( &m,&QTimer::timeout,[ & ](){

				m_ui->pbConnect->setEnabled( true ) ;

				m_ui->pbConnect->setFocus() ;

				e.exit() ;
			} ) ;

			m.start( 2000 ) ;

			e.exec() ;			
		}else{
			m_ui->pbConnect->setEnabled( false ) ;

			m_process.terminate() ;
		}
	} ) ;

	connect( m_ui->pbQuit,&QPushButton::clicked,[ this,_notConnected ](){

		if( _notConnected() ){

			QCoreApplication::quit() ;
		}
	} ) ;

	connect( m_ui->pbClear,&QPushButton::clicked,[ this ](){

		m_data.clear() ;

		m_ui->statusOutPut->setText( QString() ) ;
	} ) ;

	connect( &m_process,&QProcess::readyReadStandardError,[ this ](){

		this->hasEvent( m_process.readAllStandardError() ) ;
	} ) ;

	connect( &m_process,&QProcess::readyReadStandardOutput,[ this ](){

		this->hasEvent( m_process.readAllStandardOutput() ) ;
	} ) ;

	connect( &m_process,[](){

		return static_cast< void( QProcess::* )( int,QProcess::ExitStatus )>( &QProcess::finished ) ;

	}(),[ this ]( int e,QProcess::ExitStatus s ){

		Q_UNUSED( e ) ;

		Q_UNUSED( s ) ;

		QIcon icon( ":/off.png" ) ;

		m_trayIcon.setIcon( icon ) ;

		this->setWindowIcon( icon ) ;

		m_ui->pbConnect->setText( tr( "Connect" ) ) ;

		m_ui->pbConnect->setEnabled( true ) ;

		m_ui->pbConnect->setFocus() ;
	} ) ;

	connect( &m_trayIcon,&QSystemTrayIcon::activated,[ this ]( QSystemTrayIcon::ActivationReason e ){

		if( e == QSystemTrayIcon::Trigger ){

			if( this->isVisible() ){

				this->hide() ;
			}else{
				this->show() ;
			}
		}
	} ) ;

	m_trayIcon.show() ;

	this->show() ;
}

void wvdial::hasEvent( const QByteArray& e )
{
	m_data += e ;

	m_ui->statusOutPut->setText( m_data ) ;
}
