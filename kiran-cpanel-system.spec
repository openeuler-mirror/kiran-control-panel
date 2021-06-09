Name:		kiran-cpanel-system
Version:	2.0.1
Release:	1%{?dist}.kb1
Summary:	This program displays the hardware information and license information of the system. Users can active the system through this program

Group:		System Environment/Base
License:	LGPLv2+
URL:		http://gitlab.kylinos.com.cn/os/kiran-cpanel-system
Source0:	%{name}-%{version}.tar.gz

BuildRequires:	qt5-qtsvg-devel
BuildRequires:	cmake
BuildRequires:	gettext
BuildRequires:	gcc-c++
BuildRequires:	qt5-qtbase-devel
BuildRequires:	qt5-linguist
BuildRequires:	zint-qt-devel >= 2.6.2
BuildRequires:	kiranwidgets-qt5-devel >= 2.0.0
BuildRequires:	kiran-cc-daemon-devel
BuildRequires:	kylin-license-devel
BuildRequires:	libstdc++-devel
BuildRequires:	zlog-devel

Requires:	gcc
Requires:	kylin-license-client
Requires:	qt5-qtbase
Requires:	qt5-qtsvg
Requires:	kiranwidgets-qt5
Requires:	kiran-system-daemon >= 2.0.2
Requires:	zlog

%description
Displays the hardware information and license information of the system.Provide the system authorization services.

%prep
%autosetup -p1

%build
%{__mkdir} -p %{buildroot}
%cmake -DCMAKE_INSTALL_PREFIX=/usr
make %{?_smp_mflags}

%install
%make_install

%files
%doc
%{_bindir}/%{name}
%{_datadir}/%{name}/icons/*
%{_datadir}/%{name}/translations/*
%{_datadir}/applications/%{name}.desktop

%clean
rm -rf %{buildroot}

%changelog
* Tue Jun 1 2021 yuanxing <yuanxing@kylinos.com.cn> - 2.0.1.kb1
- KYOS-F: change the package name to kiran-cpanel-system and modified the term

