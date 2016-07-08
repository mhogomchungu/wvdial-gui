#
# Spec file for package wvdial-gui
#
# Copyright © 2016 Francis Banyikwa <mhogomchungu@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

Name:           wvdial-gui
Version:        1.0.0
Release:        0
Summary:        Qt GUI front end to wvdial
License:        GPL-2.0+
Group:          Productivity/Security
Source:         %{name}-%{version}.tar.xz
Source100:      wvdial-gui-rpmlint
URL:            https://github.com/mhogomchungu/wvdial-gui

BuildRequires: cmake
BuildRequires: gcc-c++
BuildRequires: glibc-devel
BuildRequires: libgcrypt-devel
BuildRequires: libsecret-devel

%if 0%{?fedora}
BuildRequires: qt5-qtbase-devel
%else
#This package maybe named differently in your distribution.
#What is required here is a package or packages that provides development packages for Qt5Core,Qt5GUI
#BuildRequires: libqt5-qtbase-devel
BuildRequires: lib64qt5widgets-devel
BuildRequires: lib64qt5core-devel
%endif

%description
wvdial-gui is a Qt/C++ front end to wvdial.

%prep
%setup -q

%build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RELEASE ..

%install
cd build
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf %{buildroot}
rm -rf $RPM_BUILD_DIR/wvdial-gui

%files
%defattr(0755,root,root)
%{_bindir}/wvdial-gui
%{_datadir}/applications/wvdial-gui.desktop

%defattr(0644,root,root)
%{_datadir}/icons/hicolor/48x48/apps/wvdial-gui.png
%{_datadir}/pixmaps/wvdial-gui.png
%defattr(0644,root,root)

%changelog
