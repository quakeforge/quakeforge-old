/* common/config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define to empty if the keyword does not work.  */
#undef const

/* Define if you don't have vprintf but do have _doprnt.  */
#undef HAVE_DOPRNT

/* Define if you have a working `mmap' system call.  */
#undef HAVE_MMAP

/* Define if you have the vprintf function.  */
#undef HAVE_VPRINTF

/* Define as __inline if that's what the C compiler calls it.  */
#undef inline

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#undef TIME_WITH_SYS_TIME

/* Define if your <sys/time.h> declares struct tm.  */
#undef TM_IN_SYS_TIME

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
#undef WORDS_BIGENDIAN

/* Define if the X Window System is missing or not being used.  */
#define X_DISPLAY_MISSING

/* Version string */
#define VERSION "2.40"
#define QF_VERSION "0.2.0-pre"

/* Define if you want to use QF-style defaults instead of Id-style */
#undef NEWSTYLE

/* Define this to the subdirectory name of the default game */
#ifdef NEWSTYLE
#	define GAMENAME "base"
#else
#	define GAMENAME "id1"
#endif

/* Define if you have the XFree86 DGA extension */
#undef HAS_DGA

/* If your version of OpenGL uses APIENTRY, define GLAPIENTRY to be APIENTRY*/
#define GLAPIENTRY APIENTRY

/* Define this to something sane if you don't have stricmp */
#undef stricmp

/* Define this if you are using Mesa */
#undef XMESA

/* Define this if you have GL_COLOR_INDEX8_EXT in GL/gl.h */
#undef HAVE_GL_COLOR_INDEX8_EXT

/* Define this if C symbols are prefixed with an underscore */
#define HAVE_SYM_PREFIX_UNDERSCORE

/* Define this if you have a Linux-style CD-ROM API */
#undef USE_LINUX_CD

/* Define this if you have a BSD-style CD-ROM API */
#undef USE_BSD_CD

/* Define if you have the _ftime function.  */
#define HAVE__FTIME

/* Define if you have the _snprintf function.  */
#define HAVE__SNPRINTF

/* Define if you have the _vsnprintf function.  */
#define HAVE__VSNPRINTF

/* Define if you have the connect function.  */
#define HAVE_CONNECT

/* Define if you have the fcntl function.  */
#undef HAVE_FCNTL

/* Define if you have the ftime function.  */
#define HAVE_FTIME

/* Define if you have the gethostbyname function.  */
#define HAVE_GETHOSTBYNAME

/* Define if you have the gethostname function.  */
#define HAVE_GETHOSTNAME

/* Define if you have the getpagesize function.  */
#undef HAVE_GETPAGESIZE

/* Define if you have the gettimeofday function.  */
#undef HAVE_GETTIMEOFDAY

/* Define if you have the getwd function.  */
#undef HAVE_GETWD

/* Define if you have the mkdir function.  */
#define HAVE_MKDIR

/* Define if you have the putenv function.  */
#undef HAVE_PUTENV

/* Define if you have the select function.  */
#define HAVE_SELECT

/* Define if you have the snprintf function.  */
#undef HAVE_SNPRINTF

/* Define if you have the socket function.  */
#define HAVE_SOCKET

/* Define if you have the stat function.  */
#undef HAVE_STAT

/* Define if you have the strerror function.  */
#undef HAVE_STRERROR

/* Define if you have the strstr function.  */
#undef HAVE_STRSTR

/* Define if you have the vsnprintf function.  */
#undef HAVE_VSNPRINTF

/* Define if you have the <arpa/inet.h> header file.  */
#undef HAVE_ARPA_INET_H

/* Define if you have the <dsound.h> header file.  */
#define HAVE_DSOUND_H

/* Define if you have the <fcntl.h> header file.  */
#undef HAVE_FCNTL_H

/* Define if you have the <initguid.h> header file.  */
#undef HAVE_INITGUID_H

/* Define if you have the <linux/soundcard.h> header file.  */
#undef HAVE_LINUX_SOUNDCARD_H

/* Define if you have the <machine/soundcard.h> header file.  */
#undef HAVE_MACHINE_SOUNDCARD_H

/* Define if you have the <mme/mme_public.h> header file.  */
#undef HAVE_MME_MME_PUBLIC_H

/* Define if you have the <mme/mmsystem.h> header file.  */
#undef HAVE_MME_MMSYSTEM_H

/* Define if you have the <mmsystem.h> header file.  */
#define HAVE_MMSYSTEM_H

/* Define if you have the <netdb.h> header file.  */
#undef HAVE_NETDB_H

/* Define if you have the <netinet/in.h> header file.  */
#undef HAVE_NETINET_IN_H

/* Define if you have the <sys/asoundlib.h> header file.  */
#undef HAVE_SYS_ASOUNDLIB_H

/* Define if you have the <sys/audioio.h> header file.  */
#undef HAVE_SYS_AUDIOIO_H

/* Define if you have the <sys/filio.h> header file.  */
#undef HAVE_SYS_FILIO_H

/* Define if you have the <sys/ioctl.h> header file.  */
#undef HAVE_SYS_IOCTL_H

/* Define if you have the <sys/mman.h> header file.  */
#undef HAVE_SYS_MMAN_H

/* Define if you have the <sys/socket.h> header file.  */
#undef HAVE_SYS_SOCKET_H

/* Define if you have the <sys/soundcard.h> header file.  */
#undef HAVE_SYS_SOUNDCARD_H

/* Define if you have the <sys/stat.h> header file.  */
#undef HAVE_SYS_STAT_H

/* Define if you have the <sys/time.h> header file.  */
#undef HAVE_SYS_TIME_H

/* Define if you have the <sys/timeb.h> header file.  */
#define HAVE_SYS_TIMEB_H

/* Define if you have the <unistd.h> header file.  */
#undef HAVE_UNISTD_H

/* Define if you have the <windows.h> header file.  */
#define HAVE_WINDOWS_H

/* Define if you have the Xxf86dga library (-lXxf86dga).  */
#undef HAVE_LIBXXF86DGA

/* Define if you have the Xxf86vm library (-lXxf86vm).  */
#undef HAVE_LIBXXF86VM

/* Define if you have the db library (-ldb).  */
#undef HAVE_LIBDB

/* Define if you have the m library (-lm).  */
#undef HAVE_LIBM

/* Define if you want the QSG standards */
#define QSG_STANDARD "1"
