###########################
# Default feature config. #
###########################

# udev daemon killer unit
%define WITH_UDEVD_KILLER 0
# If window manager exist then waiting unit will be installed.
%define WITH_WMREADY 1

%define WITH_FREQUENT_FSTRIM 0

%if "%{?tizen_profile_name}" == "wearable"
%define WITH_FREQUENT_FSTRIM 1
%define dirty_writeback_centisecs 1000
%endif

Name: system-plugin-common
Summary: system common file of system
Version: 0.0.01
Release: 1
License: Apache-2.0
Group: System/Base
ExclusiveArch: %arm
Source: %{name}-%{version}.tar.gz
Source1001: %{name}.manifest

BuildRequires: autoconf
BuildRequires: automake
BuildRequires: libtool
BuildRequires: kernel-headers
BuildRequires: pkgconfig(dbus-1)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(gio-2.0)

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

./autogen.sh
%configure CFLAGS='-g -O0 -Werror' \
        --prefix=%{_prefix} \
%if ! %{WITH_WMREADY}
        --disable-wmready \
%endif
%if %{WITH_UDEVD_KILLER}
        --enable-udevd-killer \
%endif
%if %{WITH_FREQUENT_FSTRIM}
        --enable-frequent-fstrim \
%endif
%if ("%{?dirty_writeback_centisecs}" != "")
        --with-dirty-writeback-centisecs=%{dirty_writeback_centisecs} \
%endif
%if "%{?tizen_profile_name}" == "mobile"
        --enable-mobile
%endif
%if "%{?tizen_profile_name}" == "wearable"
        --enable-wearable
%endif
%if "%{?tizen_profile_name}" == "tv"
        --enable-tv
%endif

make %{?_smp_mflags}

%install
%make_install

mkdir -p $RPM_BUILD_ROOT%{_datadir}/license
cat LICENSE > $RPM_BUILD_ROOT%{_datadir}/license/%{name}

%if "%{?tizen_profile_name}" == "mobile" || "%{?tizen_profile_name}" == "tv"
mkdir -p %{buildroot}%{_libdir}/sysctl.d
mkdir -p %{buildroot}%{_sysconfdir}
touch %{buildroot}%{_sysconfdir}/machine-id
%endif

%post
touch %{_sysconfdir}/ld.so.nohwcap

%files
%defattr(-,root,root,-)
%{_datadir}/license/%{name}
%{_sysconfdir}/systemd/default-extra-dependencies/ignore-units
%{_bindir}/change-booting-mode.sh
%{_bindir}/check-mount.sh

# systemd service units
%{_libdir}/systemd/system/tizen-generate-env.service
%{_libdir}/systemd/system/basic.target.wants/tizen-generate-env.service
%if %{WITH_WMREADY}
%{_libdir}/systemd/system/wm_ready.service
%{_libdir}/systemd/system/tizen-boot.target.wants/wm_ready.service
%endif
%{_libdir}/systemd/system/check-mount.service
%{_libdir}/systemd/system/tizen-system.target.wants/check-mount.service
%{_libdir}/systemd/system/remount-rootfs.service
%{_libdir}/systemd/system/local-fs.target.wants/remount-rootfs.service

# system initialize units
%{_libdir}/systemd/system/tizen-init.target
%{_libdir}/systemd/system/tizen-init-check.service
%{_libdir}/systemd/system/basic.target.wants/tizen-init-check.service
%{_libdir}/systemd/system/tizen-init-done.service
%{_libdir}/systemd/system/tizen-init.target.wants/tizen-init-done.service
%{_libdir}/systemd/system/tizen-initial-boot-done.service

# fstrim units
%{_bindir}/tizen-fstrim-on-charge.sh
%{_libdir}/systemd/system/tizen-fstrim-user.service
%{_libdir}/systemd/system/tizen-fstrim-user.timer

# readahead units
%{_libdir}/systemd/system/tizen-readahead-collect.service
%{_libdir}/systemd/system/tizen-readahead-collect-stop.service
%{_libdir}/systemd/system/tizen-init.target.wants/tizen-readahead-collect.service
%{_libdir}/systemd/system/tizen-readahead-replay.service

# udev daemon killer
%if %{WITH_UDEVD_KILLER}
%{_libdir}/systemd/system/systemd-udevd-kill.service
%{_libdir}/systemd/system/systemd-udevd-kill.timer

%if "%{?tizen_profile_name}" == "mobile" || "%{?tizen_profile_name}" == "tv"
%{_libdir}/systemd/system/graphical.target.wants/systemd-udevd-kill.timer
%else
%if "%{?tizen_profile_name}" == "wearable"
%{_libdir}/systemd/system/default.target.wants/systemd-udevd-kill.timer
%endif
%endif

%endif
%manifest %{name}.manifest

# mobile & wearable & tv difference
%if "%{?tizen_profile_name}" == "mobile" || "%{?tizen_profile_name}" == "tv"
%{_libdir}/systemd/system/graphical.target.wants/tizen-initial-boot-done.service
%{_libdir}/systemd/system/graphical.target.wants/tizen-fstrim-user.timer
%{_libdir}/systemd/system/graphical.target.wants/tizen-readahead-replay.service
%else
%if "%{?tizen_profile_name}" == "wearable"
%{_libdir}/systemd/system/default.target.wants/tizen-initial-boot-done.service
%{_libdir}/systemd/system/default.target.wants/tizen-fstrim-user.timer
%{_libdir}/systemd/system/multi-user.target.wants/tizen-readahead-replay.service
%endif
%endif

%if "%{?tizen_profile_name}" == "mobile" || "%{?tizen_profile_name}" == "tv"
%ghost %config(noreplace) %{_sysconfdir}/machine-id

%{_libdir}/udev/rules.d/51-tizen-udev-default.rules
%{_libdir}/systemd/system/tizen-boot.target
%{_libdir}/systemd/system/multi-user.target.wants/tizen-boot.target
%{_libdir}/systemd/system/tizen-system.target
%{_libdir}/systemd/system/multi-user.target.wants/tizen-system.target
%{_libdir}/systemd/system/tizen-runtime.target
%{_libdir}/systemd/system/multi-user.target.wants/tizen-runtime.target

# sysctl
%{_libdir}/sysctl.d/50-tizen-default.conf

# cleanup storage
%{_bindir}/cleanup-storage.sh
%{_libdir}/systemd/system/basic.target.wants/cleanup-storage.service
%{_libdir}/systemd/system/timers.target.wants/cleanup-storage.timer
%{_libdir}/systemd/system/cleanup-storage.service
%{_libdir}/systemd/system/cleanup-storage.timer

%{_libdir}/systemd/system/tizen-journal-flush.service
%{_libdir}/systemd/system/graphical.target.wants/tizen-journal-flush.service

%{_libdir}/systemd/system/init-conf.service
%{_libdir}/systemd/system/sysinit.target.wants/init-conf.service
%else
%if "%{?tizen_profile_name}" == "wearable"
%{_libdir}/udev/rules.d/51-tizen-udev-default.rules
%endif
%endif
