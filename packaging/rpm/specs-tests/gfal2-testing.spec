%global checkout_tag 20120828svn
%define debug_package %{nil}

Name:				gfal2-testing
Version:			2.0.3
Release:			1.%{checkout_tag}%{?dist}
Summary:			Grid file access library 2.0
Group:				Applications/Internet
License:			ASL 2.0
URL:				https://svnweb.cern.ch/trac/lcgutil/wiki/gfal2
# svn export http://svn.cern.ch/guest/lcgutil/gfal2/trunk gfal2
Source0:			http://grid-deployment.web.cern.ch/grid-deployment/dms/lcgutil/tar/%{name}/%{name}-%{version}-%{checkout_tag}.tar.gz
BuildRoot:			%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
AutoReqProv:                    no

#main lib dependencies
Requires:		cmake%{?_isa}
Requires:		gfal2-devel%{?_isa}
Requires:		gfal2-all%{?_isa}
Requires:		voms-clients%{?_isa}

%description
test suite for gfal 2.0

%clean
rm -rf %{buildroot};

%prep
%setup -q

%build

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/src/gfal2
cp -r * %{buildroot}/usr/src/gfal2/

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr (-,root,root)
/usr/src/gfal2/*


%changelog
* Fri Jul 20 2012 Adrien Devresse <adevress at cern.ch> - 2.0.0-1
 - initial test set 