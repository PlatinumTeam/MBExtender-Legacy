#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([MBExtender], [1.0.0], [amd@xboxchaos.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIRS([build-aux])
AM_CONFIG_HEADER(config.h)
AC_CONFIG_SRCDIR([src/MBExtender/linux/main-linux.cpp])
AM_INIT_AUTOMAKE([subdir-objects])
AC_DISABLE_STATIC
LT_INIT

# Check system type
AC_CANONICAL_HOST

OS=""
case $host_os in
	linux*)
		OS="linux"
		;;
	mingw*)
		AC_MSG_ERROR([Please use Visual Studio to compile on Windows])
		;;
	*)
		AC_MSG_ERROR([Only Linux and Windows (Visual Studio) are supported right now])
		ll
esac
AM_CONDITIONAL([LINUX], [test "x$OS" = xlinux])
AM_CONDITIONAL([WINDOWS], [test "x$OS" = xwindows]) ## This will never happen right now, but it's needed so make dist doesn't break

CPU=""
case $host_cpu in
	i[3456]86* | x86*)
		CPU="x86"
		;;
	*)
		AC_MSG_ERROR([Only x86-compatible hosts are supported right now])
		;;
esac
AM_CONDITIONAL([X86], [test "x$CPU" = xx86])

: ${CXXFLAGS="-g -O2"}
CXXFLAGS="-m32 $CXXFLAGS"
CFLAGS="-m32 $CFLAGS"
CCASFLAGS="-m32 $CCASFLAGS"
LDFLAGS="-m32 $LDFLAGS"

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AM_PROG_AS

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lboost_filesystem':
AC_CHECK_LIB([boost_filesystem], [main], [], [AC_MSG_ERROR([boost_filesystem is required])])
AC_CHECK_LIB([boost_system], [main], [], [AC_MSG_ERROR([boost_filesystem is required])])
AC_CHECK_LIB([dl], [dlopen])
AC_CHECK_LIB([ws2_32], [WSAStartup])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h unistd.h Windows.h])
AC_CHECK_HEADERS(
	[cpuid.h],
	[],
	[AC_CHECK_HEADERS(
		[intrin.h],
		[],
		[AC_MSG_ERROR([Either cpuid.h or intrin.h is required])]
	)]
)

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])
AX_CXX_COMPILE_STDCXX_11([noext])

# Checks for library functions.
#AC_FUNC_MMAP
#AC_CHECK_FUNCS([memmove memset mmap munmap mprotect])

AC_CONFIG_FILES([Makefile include/Makefile])
AC_OUTPUT
