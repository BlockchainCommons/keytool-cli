#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([keytool-cli], [0.8.0])
AC_CONFIG_SRCDIR([src/keytool.cpp])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# on windows/msys2/mingw64:
AS_CASE([$(uname -s)],
  [*MINGW*], [
    LIBS+=-lws2_32
    is_mingw="yes"
  ]
)

# Checks for libraries.
if test "x$is_mingw" == "xyes"; then
  AC_CHECK_LIB([ssp], [__stack_chk_fail], [], [
    echo "### Error! ssp must be installed first"
    exit -1
  ])
fi
AC_CHECK_LIB([bc-crypto-base], [sha256_Raw], [], [
  echo "### Error! libbc-crypto-base must be installed first."
  exit -1
  ])
AC_CHECK_LIB([wallycore], [wally_init], [], [
  echo "### Error! libwally-core must be installed first."
  exit -1
  ], [-lsecp256k1])
AC_CHECK_LIB([argp], [argp_parse], [], [
  echo "### Error! argp must be installed first. Try 'brew install argp-standalone'."
  exit -1
  ])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h memory.h stdint.h stdlib.h string.h strings.h sys/ioctl.h sys/param.h unistd.h stddef.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strerror])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
