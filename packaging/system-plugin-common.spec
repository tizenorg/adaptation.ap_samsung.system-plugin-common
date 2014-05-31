###########################
# Default feature config. #
###########################
# SMACK
%define WITH_SMACK 1
# udev daemon killer unit
%define WITH_UDEVD_KILLER 0
# If window manager exist then waiting unit will be installed.
%define WITH_WMREADY 1

%define WITH_FREQUENT_FSTRIM 0

%if "%{_repository}" == "wearable"
%define WITH_FREQUENT_FSTRIM 1
%endif

%define _prefix_devel /opt/usr/devel

Name: system-plugin-common
Summary: system common file of system
Version: 0.0.01
Release: 1
License: Apache License v2
Group: System/Base
ExclusiveArch: %arm
Source: %{name}-%{version}.tar.gz
Source1001: %{name}.manifest

BuildRequires: autoconf
BuildRequires: automake
%if %{WITH_SMACK}
BuildRequires: libacl-devel
BuildRequires: smack-devel
%endif

Requires: e2fsprogs
Requires: /bin/grep
Requires: /usr/bin/awk
Requires: psmisc
Requires(post): coreutils

%description
Startup files

%prep
%setup -q

%build
cp %{SOURCE1001} .

aclocal
automake --add-missing
autoconf
%configure \
        --prefix=%{_prefix} \
%if 0%{?tizen_build_binary_release_type_eng:1}
        --enable-engmode \
%endif
%if %{WITH_SMACK}
        --enable-smack \
%endif
%if ! %{WITH_WMREADY}
        --disable-wmready \
%endif
%if %{WITH_UDEVD_KILLER}
        --enable-udevd-killer \
%endif
%if %{WITH_FREQUENT_FSTRIM}
        --enable-frequent-fstrim \
%endif

make %{?_smp_mflags}

%install
%make_install

mkdir -p $RPM_BUILD_ROOT%{_datadir}/license
cat LICENSE > $RPM_BUILD_ROOT%{_datadir}/license/%{name}

%post
touch %{_sysconfdir}/ld.so.nohwcap

%files
%defattr(-,root,root,-)
%{_datadir}/license/%{name}
%{_sysconfdir}/systemd/default-extra-dependencies/ignore-units
%{_bindir}/change-booting-mode.sh
%{_bindir}/tizen-boot.sh

# systemd service units
%{_libdir}/systemd/system/tizen-generate-env.service
%{_libdir}/systemd/system/basic.target.wants/tizen-generate-env.service
%if %{WITH_WMREADY}
%{_libdir}/systemd/system/wm_ready.service
%{_libdir}/systemd/system/tizen-boot.target.wants/wm_ready.service
%endif
%{_libdir}/systemd/system/check-mount.service
%{_libdir}/systemd/system/tizen-system.target.wants/check-mount.service

# system initialize units
%{_libdir}/systemd/system/tizen-init.target
%{_libdir}/systemd/system/tizen-init-check.service
%{_libdir}/systemd/system/basic.target.wants/tizen-init-check.service
%{_libdir}/systemd/system/tizen-init-done.service
%{_libdir}/systemd/system/tizen-init.target.wants/tizen-init-done.service
%{_libdir}/systemd/system/tizen-initial-boot-done.service
%{_libdir}/systemd/system/default.target.wants/tizen-initial-boot-done.service

# fstrim units
%{_bindir}/tizen-fstrim-on-charge.sh
%{_libdir}/systemd/system/default.target.wants/tizen-fstrim-user.timer
%{_libdir}/systemd/system/tizen-fstrim-user.service
%{_libdir}/systemd/system/tizen-fstrim-user.timer

# readahead units
%{_libdir}/systemd/system/tizen-readahead-collect.service
%{_libdir}/systemd/system/tizen-readahead-collect-stop.service
%{_libdir}/systemd/system/tizen-init.target.wants/tizen-readahead-collect.service
%{_libdir}/systemd/system/tizen-readahead-replay.service
%{_libdir}/systemd/system/multi-user.target.wants/tizen-readahead-replay.service

# udev daemon killer
%if %{WITH_UDEVD_KILLER}
%{_libdir}/systemd/system/default.target.wants/systemd-udevd-kill.timer
%{_libdir}/systemd/system/systemd-udevd-kill.service
%{_libdir}/systemd/system/systemd-udevd-kill.timer
%endif
%manifest %{name}.manifest
