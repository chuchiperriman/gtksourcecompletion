# gtksourcecompletion.spec
#
# Copyright (c) 2007 chuchiperriman chuchiperriman@gmail.com
#
%define name gtksourcecompletion
%define version 0.2.2
%define release 1
%define manifest %{_builddir}/%{name}-%{version}-%{release}.manifest

# required items
Name: %{name}
Version: %{version}
Release: %{release}
Copyright: GPL
Group: Application/Misc

# optional items
#Vendor: chuchiperriman
#Distribution:
#Icon:
#URL:
#Packager: chuchiperriman chuchiperriman@gmail.com

# source + patches
Source: %{name}-%{version}.tar.gz
#Source1:
#Patch:
#Patch1:

# RPM info
#Provides:
#Requires:
#Conflicts:
#Prereq:

#Prefix: /usr
BuildRoot: /var/tmp/%{name}-%{version}

Summary: Adds Completion support to GtkSourceView

%description
Adds Completion support to GtkSourceView
gtksourcecompletion.spec.  Generated from gtksourcecompletion.spec.in by configure.
Please edit gtksourcecompletion.spec.in to add several more lines of description
here if appropriate, and to delete these instructions.

%prep
%setup -q
#%patch0 -p1

%build
%configure
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
%makeinstall

# __os_install_post is implicitly expanded after the
# %install section... do it now, and then disable it,
# so all work is done before building manifest.

%{?__os_install_post}
%define __os_install_post %{nil}

# build the file list automagically into %{manifest}

cd $RPM_BUILD_ROOT
rm -f %{manifest}
find . -type d \
        | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' >> %{manifest}
find . -type f \
        | sed 's,^\.,\%attr(-\,root\,root) ,' >> %{manifest}
find . -type l \
        | sed 's,^\.,\%attr(-\,root\,root) ,' >> %{manifest}

#%pre
#%post
#%preun
#%postun

%clean
rm -f %{manifest}
rm -rf $RPM_BUILD_ROOT

%files -f %{manifest}
%defattr(-,root,root)
#%doc README
#%docdir
#%config

%changelog
