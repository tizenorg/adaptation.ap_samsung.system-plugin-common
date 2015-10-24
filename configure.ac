#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(system-plugin-common, 0.0.01, [BUG-REPORT-ADDRESS])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE
AC_PREFIX_DEFAULT([/usr])
AM_INIT_AUTOMAKE([foreign])

LT_PREREQ(2.2)
LT_INIT([disable-static])

# Checks for programs.
AC_PROG_MKDIR_P
AC_PROG_LN_S
AC_PROG_SED
AC_PROG_GREP
AC_PROG_AWK
#AC_PROG_INSTALL

AC_PROG_CC
AC_PROG_CC_C99
AM_PROG_CC_C_O
AC_PROG_GCC_TRADITIONAL
AC_PATH_PROG([M4], [m4])
M4_DEFINES=

# ------------------------------------------------------------------------------
AC_ARG_WITH([rootprefix],
        AS_HELP_STRING([--with-rootprefix=DIR],
                [rootfs directory prefix for config files and kernel modules]),
                [], [with_rootprefix=${ac_default_prefix}])
AC_SUBST([rootprefix], [$with_rootprefix])

# ------------------------------------------------------------------------------
AC_ARG_ENABLE([engmode],
        AS_HELP_STRING([--enable-engmode], [disable engineer mode]),
                [case "${enableval}" in
                        yes) with_engmode=yes ;;
                        no) with_engmode=no ;;
                        *) AC_MSG_ERROR(bad value ${enableval} for --disable-engmode) ;;
                esac],
                with_engmode=yes)
if test "x$with_engmode" != "xno"; then
        M4_DEFINES="$M4_DEFINES -DWITH_ENGMODE"
fi
AC_SUBST(WITH_ENGMODE)
AM_CONDITIONAL([WITH_ENGMODE], [test "x$with_engmode" != xno])

# ------------------------------------------------------------------------------
AC_ARG_WITH(initial-boot,
            AS_HELP_STRING([--with-initialbootdone=PATH],
                           [tizen system initial boot done]),
                [INITAILBOOT_DONE="$withval"],
                [INITAILBOOT_DONE="/opt/etc/.initialboot_done"])
AC_SUBST(INITAILBOOT_DONE)

AC_ARG_WITH(need-initialized,
        AS_HELP_STRING([--with-initializedone=PATH],
                       [tizen system need initialized done]),
                [INITIALIZE_DONE="$withval"],
                [INITIALIZE_DONE="/opt/etc/.initialize_done"])
AC_SUBST(INITIALIZE_DONE)

# ------------------------------------------------------------------------------
AC_ARG_WITH(tizen-readahead,
            AS_HELP_STRING([--with-tizenreadaheaddir=PATH],
                           [tizen readahead dir]),
                [READAHEAD_DIR="$withval"],
                [READAHEAD_DIR="/opt/etc"])
AC_SUBST(READAHEAD_DIR)

# ------------------------------------------------------------------------------
AC_ARG_ENABLE([engmode2],
        AS_HELP_STRING([--enable-engmode2], [disable engineer mode2]),
                [case "${enableval}" in
                        yes) with_engmode2=yes ;;
                        no) with_engmode2=no ;;
                        *) AC_MSG_ERROR(bad value ${enableval} for --disable-engmode2) ;;
                esac],
                with_engmode2=yes)
if test "x$with_engmode2" != "xno"; then
        M4_DEFINES="$M4_DEFINES -DWITH_ENGMODE2"
fi
AC_SUBST(WITH_ENGMODE2)
AM_CONDITIONAL([WITH_ENGMODE2], [test "x$with_engmode2" != xno])

# ------------------------------------------------------------------------------
AC_ARG_ENABLE([udevd-killer],
        AS_HELP_STRING([--enable-udevd-killer],
                       [install udevd killer service]),
                [case "${enableval}" in
                        yes) enable_udevd_killer=yes ;;
                        no) enable_udevd_killer=no ;;
                        *) AC_MSG_ERROR(bad value ${enableval} for --enable-udevd-killer) ;;
                esac],
                enable_udevd_killer=no)
AC_SUBST(WITH_UDEVD_KILLER)
AM_CONDITIONAL([WITH_UDEVD_KILLER], [test "x$enable_udevd_killer" != xno])

# ------------------------------------------------------------------------------
AC_ARG_ENABLE([mobile],
        AS_HELP_STRING([--enable-mobile],
                       [enable mobile configuration]),
                [case "${enableval}" in
                        yes) enable_mobile=yes ;;
                        no) enable_mobile=no ;;
                        *) AC_MSG_ERROR(bad value ${enableval} for --enable-mobile) ;;
                esac],
                enable_mobile=no)
AC_SUBST(MOBILE)
AM_CONDITIONAL([MOBILE], [test "x$enable_mobile" != xno])

# ------------------------------------------------------------------------------
AC_ARG_ENABLE([wearable],
        AS_HELP_STRING([--enable-wearable],
                       [enable wearable configuration]),
                [case "${enableval}" in
                        yes) enable_wearable=yes ;;
                        no) enable_wearable=no ;;
                        *) AC_MSG_ERROR(bad value ${enableval} for --enable-wearable) ;;
                esac],
                enable_wearable=no)
AC_SUBST(WEARABLE)
AM_CONDITIONAL([WEARABLE], [test "x$enable_wearable" != xno])

# ------------------------------------------------------------------------------
AC_ARG_ENABLE([tv],
        AS_HELP_STRING([--enable-tv],
                       [enable tv configuration]),
                [case "${enableval}" in
                        yes) enable_tv=yes ;;
                        no) enable_tv=no ;;
                        *) AC_MSG_ERROR(bad value ${enableval} for --enable-tv) ;;
                esac],
                enable_tv=no)
AC_SUBST(TV)
AM_CONDITIONAL([TV], [test "x$enable_tv" != xno])

if test "x$enable_tv" == "xyes"; then
        M4_DEFINES="$M4_DEFINES -DTV"
fi

# ------------------------------------------------------------------------------
AC_ARG_ENABLE([smack-label],
        AS_HELP_STRING([--enable-smack-label],
                       [enable smack process label configuration]),
                [case "${enableval}" in
                        yes) enable_smack_label=yes ;;
                        no) enable_smack_label=no ;;
                        *) AC_MSG_ERROR(bad value ${enableval} for --enable-smack-label) ;;
                esac],
                enable_smack-label=no)
AC_SUBST(SMACK_LABEL)
AM_CONDITIONAL([SMACK_LABEL], [test "x$enable_smack_label" != xno])

if test "x$enable_smack_label" == "xyes"; then
        M4_DEFINES="$M4_DEFINES -DSMACK_LABEL"
fi

# ------------------------------------------------------------------------------
AC_ARG_ENABLE([wmready],
        AS_HELP_STRING([--disable-wmready], [without window manager waiting]),
                [case "${enableval}" in
                        yes) have_winmgr=yes ;;
                        no) have_winmgr=no ;;
                        *) AC_MSG_ERROR(bad value ${enableval} for --disable-wmready) ;;
                esac],
                have_winmgr=yes)
AC_SUBST(WITH_WMREADY)
AM_CONDITIONAL([WITH_WMREADY], [test "x$have_winmgr" != xno])

# ------------------------------------------------------------------------------
AC_ARG_ENABLE([frequent-fstrim],
        AS_HELP_STRING([--enable-frequent-fstrim],
                       [use more frequently fstrim timer]),
                [case "${enableval}" in
                        yes) enable_frequent_fstrim=yes ;;
                        no) enable_frequent_fstrim=no ;;
                        *) AC_MSG_ERROR(bad value ${enableval} for --enable-frequent-fstrim) ;;
                esac],
                enable_frequent_fstrim=no)
if test "x$enable_frequent_fstrim" == "xyes"; then
        M4_DEFINES="$M4_DEFINES -DWITH_FREQUENT_FSTRIM"
fi

AC_SUBST(M4_DEFINES)
AC_SUBST(WITH_FREQUENT_FSTRIM)
AM_CONDITIONAL([WITH_FREQUENT_FSTRIM], [test "x$enable_frequent_fstrim" != xno])

# ------------------------------------------------------------------------------
AC_ARG_WITH(dirty-writeback-centisecs,
            AS_HELP_STRING([--with-dirty-writeback-centisecs=NUMBER],
                           [Path to /etc/rc.local]),
                           [DIRTY_WRITEBACK_CENTISECS="$withval"],
                           [DIRTY_WRITEBACK_CENTISECS="500"])

AC_DEFINE_UNQUOTED(DIRTY_WRITEBACK_CENTISECS, ["$DIRTY_WRITEBACK_CENTISECS"], [Path of /etc/rc.local script])
AC_SUBST(DIRTY_WRITEBACK_CENTISECS)

# ------------------------------------------------------------------------------
PKG_CHECK_MODULES(DBUS, [dbus-1 >= 1.3.2])
PKG_CHECK_MODULES(GLIB, [glib-2.0])
PKG_CHECK_MODULES(GIO, [gio-2.0])

# ------------------------------------------------------------------------------
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
AC_MSG_RESULT([
        $PACKAGE_NAME $VERSION

        prefix:                  ${prefix}
        rootprefix:              ${with_rootprefix}
        sysconf dir:             ${sysconfdir}
        datarootdir:             ${datarootdir}
        lib dir:                 ${libdir}
        rootlib dir:             ${with_rootlibdir}
        initial boot done flag:  ${INITAILBOOT_DONE}
        inialized done flag:     ${INITIALIZE_DONE}

        XATTR:                   ${have_xattr}
        SMACK:                   ${have_smack}

        engineer mode:           ${with_engmode}
        window manager:          ${have_winmgr}
        udevd killer:            ${enable_udevd_killer}
        frequent fstrim:         ${enable_frequent_fstrim}
        enable mobile:           ${enable_mobile}
        enable wearable:         ${enable_wearable}
        enable tv:               ${enable_tv}
])
