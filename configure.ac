#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(system-plugin-common, 0.0.01, [BUG-REPORT-ADDRESS])

AC_CONFIG_MACRO_DIR([m4])
AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE
AC_PREFIX_DEFAULT([/usr])
AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_MKDIR_P
AC_PROG_LN_S
AC_PROG_SED
AC_PROG_GREP
AC_PROG_AWK
#AC_PROG_INSTALL
AC_PATH_PROG([M4], [m4])

# ------------------------------------------------------------------------------
AC_ARG_ENABLE([xattr],
        AS_HELP_STRING([--disable-xattr],[Disable optional XATTR support]),
                [case "${enableval}" in
                        yes) have_xattr=yes ;;
                        no) have_xattr=no ;;
                        *) AC_MSG_ERROR(bad value ${enableval} for --disable-xattr) ;;
                esac],
                [have_xattr=auto])

if test "x${have_xattr}" != xno ; then
        AC_CHECK_HEADERS(
                [attr/xattr.h],
                [have_xattr=yes],
                [if test "x$have_xattr" = xyes ; then
                        AC_MSG_ERROR([*** XATTR headers not found.])
                fi])

        AC_CHECK_LIB(
                [attr],
                [fsetxattr],
                [have_xattr=yes],
                [if test "x$have_xattr" = xyes ; then
                        AC_MSG_ERROR([*** libattr not found.])
                fi])

        if test "x$have_xattr" = xyes ; then
                XATTR_LIBS="-lattr"
                AC_DEFINE(HAVE_XATTR, 1, [XATTR available])
        else
                have_xattr=no
        fi
else
        XATTR_LIBS=
fi
AC_SUBST(XATTR_LIBS)
AM_CONDITIONAL([HAVE_XATTR], [test "x$have_xattr" != xno])

# ------------------------------------------------------------------------------
AC_ARG_ENABLE([smack], AS_HELP_STRING([--disable-smack],[Disable optional SMACK support]),
                [case "${enableval}" in
                        yes) have_smack=yes ;;
                        no) have_smack=no ;;
                        *) AC_MSG_ERROR(bad value ${enableval} for --disable-smack) ;;
                esac],
                [have_smack=auto])

if test "x${have_xattr}" = xno; then
        if test "x${have_smack}" = xyes; then
                AC_MSG_ERROR(SMACK requires xattr support)
        else
                have_smack=no
        fi
else
        if test "x${have_smack}" = xauto; then
                have_smack=yes
        fi
fi

if test "x${have_smack}" = xyes ; then
        AC_DEFINE(HAVE_SMACK, 1, [Define if SMACK is available])
fi

# ------------------------------------------------------------------------------
AC_ARG_WITH([rootprefix],
        AS_HELP_STRING([--with-rootprefix=DIR],
                [rootfs directory prefix for config files and kernel modules]),
                [], [with_rootprefix=${ac_default_prefix}])
AC_SUBST([rootprefix], [$with_rootprefix])

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
AC_ARG_ENABLE([engmode],
        AS_HELP_STRING([--disable-engmode], [disable engineer mode]),
                [case "${enableval}" in
                        yes) with_engmode=yes ;;
                        no) with_engmode=no ;;
                        *) AC_MSG_ERROR(bad value ${enableval} for --disable-engmode) ;;
                esac],
                with_engmode=no)
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
M4_DEFINES=
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
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
AC_MSG_RESULT([
        $PACKAGE_NAME $VERSION

        engineer mode:           ${with_engmode}

        XATTR:                   ${have_xattr}
        SMACK:                   ${have_smack}

        window manager:          ${have_winmgr}
        udevd killer:            ${enable_udevd_killer}
        frequent fstrim:         ${enable_frequent_fstrim}

        prefix:                  ${prefix}
        rootprefix:              ${with_rootprefix}
        sysconf dir:             ${sysconfdir}
        datarootdir:             ${datarootdir}
        lib dir:                 ${libdir}
        rootlib dir:             ${with_rootlibdir}
        initial boot done flag:  ${INITAILBOOT_DONE}
        inialized done flag:     ${INITIALIZE_DONE}
])
