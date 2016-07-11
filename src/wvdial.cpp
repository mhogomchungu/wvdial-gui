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
#include <QFile>

#include <iostream>

wvdial::wvdial() : m_ui( new Ui::wvdial ),m_settings( "wvdial-gui","wvdial-gui" )
{
	m_ui->setupUi( this ) ;

	auto _has_wvdial = [ this ](){

		auto l = { "/usr/local/bin/wvdial",
			   "/usr/local/sbin/wvdial",
			   "/usr/bin/wvdial",
			   "/usr/sbin/wvdial",
			   "/bin/wvdial",
			   "/sbin/wvdial" } ;

		for( const auto& it : l ){

			if( QFile::exists( it ) ){

				m_exe = it ;

				return true ;
			}
		}

		return false ;
	} ;

	if( m_settings.contains( "dimensions" ) ){

		auto e = m_settings.value( "dimensions" ).toString().split( ' ' ) ;

		if( e.size() >= 4 ){

			m_dimensions.setX( e.at( 0 ).toInt() ) ;
			m_dimensions.setY( e.at( 1 ).toInt() ) ;
			m_dimensions.setWidth( e.at( 2 ).toInt() ) ;
			m_dimensions.setHeight( e.at( 3 ).toInt() ) ;

			this->window()->setGeometry( m_dimensions ) ;
		}
	}

	if( m_settings.contains( "interface" ) ){

		m_interface = m_settings.value( "interface" ).toString() ;
	}else{
		m_interface = "ppp0" ;

		m_settings.setValue( "interface",m_interface ) ;
	}

	if( m_settings.contains( "interval" ) ){

		m_interval = 1000 * m_settings.value( "interval" ).toString().toInt() ;
	}else{
		m_interval = 2 ;

		m_settings.setValue( "interval",QString::number( m_interval / 1000 ) ) ;
	}

	if( _has_wvdial() ){

		this->setIcon( "off" ) ;

		m_ui->pbConnect->setText( tr( "&Connect" ) ) ;
	}else{
		m_ui->statusOutPut->setText( tr( "Error: Failed To Find wvdial Executable" ) ) ;
		m_ui->pbConnect->setEnabled( false ) ;
	}

	m_process.setProcessChannelMode( QProcess::MergedChannels ) ;

	m_process_0.setProcessChannelMode( QProcess::MergedChannels ) ;

	m_ui->statusOutPut->ensureCursorVisible() ;
}

wvdial::~wvdial()
{
	m_settings.setValue( "dimensions",[ this ](){

		auto _number = []( int e ){ return QString::number( e ) ; } ;

		auto e = this->window()->geometry() ;

		return QString( "%1 %2 %3 %4" ).arg( _number( e.x() ),
						     _number( e.y() ),
						     _number( e.width() ),
						     _number( e.height() ) ) ;
	}() ) ;

	delete m_ui ;
}

void wvdial::start()
{
	QMetaObject::invokeMethod( this,"run",Qt::QueuedConnection ) ;
}

void wvdial::setIcon( const QString& iconName )
{
	QIcon icon( ":/" + iconName + ".png" ) ;

	m_trayIcon.setIcon( icon ) ;

	this->setWindowIcon( icon ) ;
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

			m_ui->pbQuit->setEnabled( false ) ;

			m_ui->pbConnect->setText( tr( "&Disconnect" ) ) ;

			m_process.start( m_exe ) ;

			this->setIcon( "on") ;

			QTimer m ;

			QEventLoop e ;

			connect( &m,&QTimer::timeout,[ & ](){

				m_ui->pbConnect->setEnabled( true ) ;

				m_ui->pbConnect->setFocus() ;

				e.exit() ;
			} ) ;

			m.start( 2000 ) ;

			e.exec() ;

			m_timer.start( m_interval ) ;
		}else{
			m_ui->pbConnect->setEnabled( false ) ;

			m_process.terminate() ;
		}
	} ) ;

	connect( m_ui->pbQuit,&QPushButton::clicked,[ this,_notConnected ](){

		m_timer.stop() ;

		if( m_process_0.state() == QProcess::NotRunning ){

			QCoreApplication::quit() ;
		}else{
			m_process_0.terminate() ;

			m_quit = true ;
		}
	} ) ;

	connect( m_ui->pbClear,&QPushButton::clicked,[ this ](){

		m_ui->statusOutPut->setText( QString() ) ;
	} ) ;

	connect( &m_process,&QProcess::readyReadStandardOutput,[ this ](){

		m_ui->statusOutPut->append( m_process.readAllStandardOutput() ) ;
	} ) ;

	connect( &m_process,[](){

		using type_t = void( QProcess::* )( int,QProcess::ExitStatus ) ;

		return static_cast< type_t >( &QProcess::finished ) ;

	}(),[ this ]( int e,QProcess::ExitStatus s ){

		Q_UNUSED( e ) ;

		Q_UNUSED( s ) ;

		this->setIcon( "off" ) ;

		m_ui->pbConnect->setText( tr( "&Connect" ) ) ;

		m_ui->pbConnect->setEnabled( true ) ;

		m_ui->pbQuit->setEnabled( true ) ;

		m_ui->pbConnect->setFocus() ;

		m_timer.stop() ;
	} ) ;

	connect( &m_trayIcon,&QSystemTrayIcon::activated,
		 [ this ]( QSystemTrayIcon::ActivationReason e ){

		if( e == QSystemTrayIcon::Trigger ){

			if( this->isVisible() ){

				m_dimensions = this->window()->geometry() ;

				this->hide() ;
			}else{
				this->window()->setGeometry( m_dimensions ) ;

				this->show() ;
			}
		}
	} ) ;

	connect( &m_timer,&QTimer::timeout,[ this ](){

		m_process_0.start( "ifconfig" ) ;
	} ) ;

	connect( &m_process_0,[](){

		using type_t = void( QProcess::* )( int,QProcess::ExitStatus ) ;

		return static_cast< type_t >( &QProcess::finished ) ;

	}(),[ this ]( int e,QProcess::ExitStatus s ){

		Q_UNUSED( e ) ;

		Q_UNUSED( s ) ;

		if( m_quit ){

			QCoreApplication::quit() ;
		}else{
			m_timer.start( m_interval ) ;
		}
	} ) ;

	connect( &m_process_0,&QProcess::readyReadStandardOutput,[ this ](){

		auto _prettify = []( quint64 s ){

			auto _convert = [ & ]( const char * p,double q ){

				auto e = QString::number( double( s ) / q,'f',2 ) ;

				e.remove( ".00" ) ;

				return QString( "%1 %2" ).arg( e,p ) ;
			} ;

			switch( QString::number( s ).size() ){

			case 0 :
			case 1 : case 2 : case 3 :

				return QString( "%1 B" ).arg( QString::number( s ) ) ;

			case 4 : case 5 : case 6 :

				return _convert( "KB",1024 ) ;

			case 7 : case 8 : case 9 :

				return _convert( "MB",1048576 ) ;

			case 10: case 11 : case 12 :

				return _convert( "GB",1073741824 ) ;

			default:
				return _convert( "TB",1024.0 * 1073741824 ) ;
			}
		} ;

		auto _split = []( const QString& e,char s ){

			return QString( e ).split( s,QString::SkipEmptyParts ) ;
		} ;

		auto _manage_data = []( quint64 New,quint64 * old,quint64 * sum ){

			if( New > *old ){

				*sum += New - *old ;

			}else if( New < *old ){

				*sum += New ;
			}

			*old = New ;
		} ;

		auto _sent = [ this,&_prettify,&_manage_data ]( const QStringList& l ){

			auto e = l.at( 5 ) ;

			e.remove( "bytes:" ) ;

			_manage_data( e.toULongLong(),&m_sent_old,&m_sent ) ;

			auto a = tr( "Data Sent: %1 " ).arg( _prettify( m_sent ) ) ;

			m_ui->sent->setText( a ) ;
		} ;

		auto _received = [ this,&_prettify,&_manage_data ]( const QStringList& l ){

			auto e = l.at( 1 ) ;

			e.remove( "bytes:" ) ;

			_manage_data( e.toULongLong(),&m_received_old,&m_received ) ;

			auto a = tr( "Data Received: %1 " ).arg( _prettify( m_received ) ) ;

			m_ui->received->setText( a ) ;
		} ;

		auto e = _split( m_process_0.readAllStandardOutput(),'\n' ) ;

		auto s = e.size() ;

		for( int i = 0 ; i < s ; i++ ){

			if( e.at( i ).startsWith( m_interface ) ){

				if( i + 6 < s ){

					const auto& z = e.at( i + 6 ) ;

					if( z.contains( "RX bytes:" ) ){

						auto q = _split( z,' ' ) ;

						_received( q ) ;

						_sent( q ) ;
					}
				}

				break ;
			}
		}		
	} ) ;

	m_trayIcon.show() ;

	this->show() ;
}
