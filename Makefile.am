ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS = .

# legacy rc script dir
rcdir=$(sysconfdir)/rc.d

# Inherit from systemd
systemdsysconfdir=$(sysconfdir)/systemd
systemconfigunitdir=$(systemdsysconfdir)/system
userconfigunitdir=$(systemdsysconfdir)/user
systemdignoreunitdir=$(systemdsysconfdir)/default-extra-dependencies
systemunitdir=$(rootprefix)/lib/systemd/system
userunitdir=$(prefix)/lib/systemd/user
udevlibexecdir=$(rootprefix)/lib/udev
udevhomedir=$(udevlibexecdir)
udevrulesdir=$(udevlibexecdir)/rules.d

# And these are the special ones for /
rootprefix=@rootprefix@
rootbindir=$(rootprefix)/bin
rootlibexecdir=$(rootprefix)/lib/systemd

if WITH_ENGMODE
engbindir=/opt/usr/devel/$(rootbindir)
endif

bin_SCRIPTS =
rc_SCRIPTS =
SCRIPT_IN_FILES =
sysconf_DATA =
systemdsysconf_DATA =
systemdignoreunit_DATA =
systemconfigunit_DATA =
systemunit_DATA =
udevrules_DATA =

if WITH_ENGMODE
engbin_PROGRAMS =
endif

INSTALL_EXEC_HOOKS =
UNINSTALL_EXEC_HOOKS =

SHUTDOWN_TARGET_WANTS =
LOCAL_FS_TARGET_WANTS =
BASIC_TARGET_WANTS =
SYSINIT_TARGET_WANTS =
SOCKETS_TARGET_WANTS =
TIMERS_TARGET_WANTS =
TIZEN_INIT_TARGET_WANTS =
TIZEN_BOOT_TARGET_WANTS =
TIZEN_SYSTEM_TARGET_WANTS =
TIZEN_RUNTIME_TARGET_WANTS =
MULTI_USER_TARGET_WANTS =
DEFAULT_TARGET_WANTS =
SYSCONF_LOCAL_FS_TARGET_WANTS =

install-target-wants-hook:
	where=$(systemunitdir) && what="$(SHUTDOWN_TARGET_WANTS)" && wants=shutdown.target && $(add-wants)
	where=$(systemunitdir) && what="$(LOCAL_FS_TARGET_WANTS)" && wants=local-fs.target && $(add-wants)
	where=$(systemunitdir) && what="$(BASIC_TARGET_WANTS)" && wants=basic.target && $(add-wants)
	where=$(systemunitdir) && what="$(MULTI_USER_TARGET_WANTS)" && wants=multi-user.target && $(add-wants)
	where=$(systemunitdir) && what="$(SYSINIT_TARGET_WANTS)" && wants=sysinit.target && $(add-wants)
	where=$(systemunitdir) && what="$(SOCKETS_TARGET_WANTS)" && wants=sockets.target && $(add-wants)
	where=$(systemunitdir) && what="$(TIMERS_TARGET_WANTS)" && wants=timers.target && $(add-wants)
	where=$(systemunitdir) && what="$(SLICES_TARGET_WANTS)" && wants=slices.target && $(add-wants)
	where=$(systemunitdir) && what="$(TIZEN_INIT_TARGET_WANTS)" && wants=tizen-init.target && $(add-wants)
	where=$(systemunitdir) && what="$(TIZEN_BOOT_TARGET_WANTS)" && wants=tizen-boot.target && $(add-wants)
	where=$(systemunitdir) && what="$(TIZEN_SYSTEM_TARGET_WANTS)" && wants=tizen-system.target && $(add-wants)
	where=$(systemunitdir) && what="$(TIZEN_RUNTIME_TARGET_WANTS)" && wants=tizen-runtime.target && $(add-wants)
	where=$(systemunitdir) && what="$(TIZEN_RUNTIME_TARGET_WANTS)" && wants=tizen-runtime.target && $(add-wants)
	where=$(systemunitdir) && what="$(DEFAULT_TARGET_WANTS)" && wants=default.target && $(add-wants)
	where=$(systemconfigunitdir) && what="$(SYSCONF_LOCAL_FS_TARGET_WANTS)" && wants=local-fs.target && $(add-wants)

define add-wants
[ -z "$$what" -o -z "$$where" ] || ( \
	dir=$(DESTDIR)$$where/$$wants.wants && \
	$(MKDIR_P) -m 0755 $$dir && \
	cd $$dir && \
	rm -f $$what && \
	for i in $$what; do $(LN_S) ../$$i . || exit $$? ; done )
endef

INSTALL_EXEC_HOOKS += \
	install-target-wants-hook

# ------------------------------------------------------------------------------
#  AM_V_M4 = $(AM_V_M4_$(V))
#  AM_V_M4_ = $(AM_V_M4_$(AM_DEFAULT_VERBOSITY))
#  AM_V_M4_0 = @echo "  M4      " $@;
#
#  AM_V_XSLT = $(AM_V_XSLT_$(V))
#  AM_V_XSLT_ = $(AM_V_XSLT_$(AM_DEFAULT_VERBOSITY))
#  AM_V_XSLT_0 = @echo "  XSLT    " $@;
#
#  AM_V_GPERF = $(AM_V_GPERF_$(V))
#  AM_V_GPERF_ = $(AM_V_GPERF_$(AM_DEFAULT_VERBOSITY))
#  AM_V_GPERF_0 = @echo "  GPERF   " $@;
#
#  AM_V_LN = $(AM_V_LN_$(V))
#  AM_V_LN_ = $(AM_V_LN_$(AM_DEFAULT_VERBOSITY))
#  AM_V_LN_0 = @echo "  LN      " $@;

# ------------------------------------------------------------------------------
bin_SCRIPTS += \
	scripts/change-booting-mode.sh \
	scripts/tizen-boot.sh \
	scripts/tizen-fstrim-on-charge.sh

systemunit_DATA += \
	units/check-mount.service \
	units/tizen-generate-env.service \
	units/tizen-readahead-collect.service \
	units/tizen-readahead-collect-stop.service \
	units/tizen-readahead-replay.service \
	units/tizen-fstrim-user.service \
	units/tizen-fstrim-user.timer \
	units/tizen-init.target \
	units/tizen-init-check.service \
	units/tizen-initial-boot-done.service \
	units/tizen-init-done.service

systemdignoreunit_DATA += \
	conf/systemd/ignore-units

BASIC_TARGET_WANTS += \
	tizen-generate-env.service \
	tizen-init-check.service

TIZEN_INIT_TARGET_WANTS += \
	tizen-readahead-collect.service \
	tizen-init-done.service

TIZEN_SYSTEM_TARGET_WANTS += \
	check-mount.service

MULTI_USER_TARGET_WANTS += \
	tizen-readahead-replay.service

DEFAULT_TARGET_WANTS += \
	tizen-fstrim-user.timer \
	tizen-initial-boot-done.service

if WITH_UDEVD_KILLER
systemunit_DATA += \
	units/systemd-udevd-kill.service \
	units/systemd-udevd-kill.timer

DEFAULT_TARGET_WANTS += \
	systemd-udevd-kill.timer
endif

if WITH_WMREADY
systemunit_DATA += \
	units/wm_ready.service

TIZEN_BOOT_TARGET_WANTS += \
	wm_ready.service
endif

# ------------------------------------------------------------------------------
substitutions = \
       '|rootlibexecdir=$(rootlibexecdir)|' \
       '|rootbindir=$(rootbindir)|' \
       '|bindir=$(bindir)|' \
       '|SYSTEMCTL=$(rootbindir)/systemctl|' \
       '|SYSTEMD_NOTIFY=$(rootbindir)/systemd-notify|' \
       '|systemdsysconfdir=$(systemdsysconfdir)|' \
       '|SYSTEM_CONFIG_UNIT_PATH=$(systemdsysconfdir)/system|' \
       '|USER_CONFIG_UNIT_PATH=$(systemdsysconfdir)/user|' \
       '|pkgdatadir=$(pkgdatadir)|' \
       '|systemunitdir=$(systemunitdir)|' \
       '|userunitdir=$(userunitdir)|' \
       '|systempresetdir=$(systempresetdir)|' \
       '|userpresetdir=$(userpresetdir)|' \
       '|udevhwdbdir=$(udevhwdbdir)|' \
       '|udevrulesdir=$(udevrulesdir)|' \
       '|catalogdir=$(catalogdir)|' \
       '|tmpfilesdir=$(tmpfilesdir)|' \
       '|sysctldir=$(sysctldir)|' \
       '|PACKAGE_VERSION=$(PACKAGE_VERSION)|' \
       '|PACKAGE_NAME=$(PACKAGE_NAME)|' \
       '|PACKAGE_URL=$(PACKAGE_URL)|' \
       '|RANDOM_SEED=$(localstatedir)/lib/random-seed|' \
       '|prefix=$(prefix)|' \
       '|exec_prefix=$(exec_prefix)|' \
       '|libdir=$(libdir)|' \
       '|includedir=$(includedir)|' \
       '|VERSION=$(VERSION)|' \
       '|rootprefix=$(rootprefix)|' \
       '|udevlibexecdir=$(udevlibexecdir)|' \
       '|SUSHELL=$(SUSHELL)|' \
       '|DEBUGTTY=$(DEBUGTTY)|' \
       '|KILL=$(KILL)|' \
       '|KMOD=$(KMOD)|' \
       '|MKDIR_P=$(MKDIR_P)|' \
       '|QUOTAON=$(QUOTAON)|' \
       '|QUOTACHECK=$(QUOTACHECK)|' \
       '|SYSTEM_SYSVINIT_PATH=$(sysvinitdir)|' \
       '|VARLOGDIR=$(varlogdir)|' \
       '|RC_LOCAL_SCRIPT_PATH_START=$(RC_LOCAL_SCRIPT_PATH_START)|' \
       '|RC_LOCAL_SCRIPT_PATH_STOP=$(RC_LOCAL_SCRIPT_PATH_STOP)|' \
       '|PYTHON=$(PYTHON)|' \
       '|PYTHON_BINARY=$(PYTHON_BINARY)|' \
       '|INITAILBOOT_DONE=$(INITAILBOOT_DONE)|' \
       '|INITIALIZE_DONE=$(INITIALIZE_DONE)|' \
       '|READAHEAD_DIR=$(READAHEAD_DIR)|'

SED_PROCESS = \
	$(AM_V_GEN)$(MKDIR_P) $(dir $@) && \
	$(SED) $(subst '|,-e 's|@,$(subst =,\@|,$(subst |',|g',$(substitutions)))) \
		< $< > $@

units/%: units/%.in Makefile
	$(SED_PROCESS)

%.rules: %.rules.in Makefile
	$(SED_PROCESS)

%.sh: %.sh.in Makefile
	$(SED_PROCESS)
	$(AM_V_GEN)chmod +x $@

units/%: units/%.m4 Makefile
	$(AM_V_M4)$(MKDIR_P) $(dir $@)
	$(AM_V_M4)$(M4) -P $(M4_DEFINES) -DFOR_SYSTEM=1 < $< > $@

install-exec-hook: $(INSTALL_EXEC_HOOKS)
