/* common/config.h.  Generated automatically by configure.  */
/* common/config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if you have a working `mmap' system call.  */
#define HAVE_MMAP 1

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define if your <sys/time.h> declares struct tm.  */
/* #undef TM_IN_SYS_TIME */

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */

/* Define if the X Window System is missing or not being used.  */
/* #undef X_DISPLAY_MISSING */

/* Version strings */
#define QF_VERSION "0.2-devel"
#define VERSION "2.40"

/* Define if you want to use QF-style defaults instead of Id-style */
/* #undef NEWSTYLE */

/* Define this to the subdirectory name of the default game */
#define GAMENAME "id1"

/* Define this if you have OGL */
#define HAS_OGL 1

/* Define if you have the XFree86 DGA extension */
#define HAS_DGA 1

/* If your version of OpenGL uses APIENTRY, define GLAPIENTRY to be APIENTRY*/
/* #undef GLAPIENTRY */

/* Define this to something sane if you don't have stricmp */
#define stricmp strcasecmp

/* Define if you have the dlopen function.  */
#define HAVE_DLOPEN 1

/* Define this if you are using Mesa */
#define XMESA 1

/* Define this if you have GL_COLOR_INDEX8_EXT in GL/gl.h */
#define HAVE_GL_COLOR_INDEX8_EXT 1

/* Define this if C symbols are prefixed with an underscore */
/* #undef HAVE_SYM_PREFIX_UNDERSCORE */

/* Define this if you have a Linux-style CD-ROM API */
#define USE_LINUX_CD 1

/* Define this if you have a BSD-style CD-ROM API */
/* #undef USE_BSD_CD */

/* Define this to use experimental code */
/* #undef _EXPERIMENTAL_ */

/* Define this if your system has zlib */
/* #undef HAS_ZLIB */

/* Define this if your system has socklen_t */
#define HAVE_SOCKLEN_T 1

/* Define this if your system has size_t */
#define HAVE_SIZE_T 1

/* Define if you have the _ftime function.  */
/* #undef HAVE__FTIME */

/* Define if you have the _snprintf function.  */
/* #undef HAVE__SNPRINTF */

/* Define if you have the _vsnprintf function.  */
/* #undef HAVE__VSNPRINTF */

/* Define if you have the connect function.  */
#define HAVE_CONNECT 1

/* Define if you have the dlopen function.  */
#define HAVE_DLOPEN 1

/* Define if you have the fcntl function.  */
#define HAVE_FCNTL 1

/* Define if you have the ftime function.  */
#define HAVE_FTIME 1

/* Define if you have the gethostbyname function.  */
#define HAVE_GETHOSTBYNAME 1

/* Define if you have the gethostname function.  */
#define HAVE_GETHOSTNAME 1

/* Define if you have the getpagesize function.  */
#define HAVE_GETPAGESIZE 1

/* Define if you have the gettimeofday function.  */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the getwd function.  */
#define HAVE_GETWD 1

/* Define if you have the mkdir function.  */
#define HAVE_MKDIR 1

/* Define if you have the putenv function.  */
#define HAVE_PUTENV 1

/* Define if you have the select function.  */
#define HAVE_SELECT 1

/* Define if you have the snprintf function.  */
#define HAVE_SNPRINTF 1

/* Define if you have the socket function.  */
#define HAVE_SOCKET 1

/* Define if you have the stat function.  */
#define HAVE_STAT 1

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the strsep function.  */
#define HAVE_STRSEP 1

/* Define if you have the strstr function.  */
#define HAVE_STRSTR 1

/* Define if you have the vsnprintf function.  */
#define HAVE_VSNPRINTF 1

/* Define if you have the <arpa/inet.h> header file.  */
#define HAVE_ARPA_INET_H 1

/* Define if you have the <asm/io.h> header file.  */
#define HAVE_ASM_IO_H 1

/* Define if you have the <dlfcn.h> header file.  */
#define HAVE_DLFCN_H 1

/* Define if you have the <dsound.h> header file.  */
/* #undef HAVE_DSOUND_H */

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <initguid.h> header file.  */
/* #undef HAVE_INITGUID_H */

/* Define if you have the <linux/soundcard.h> header file.  */
#define HAVE_LINUX_SOUNDCARD_H 1

/* Define if you have the <machine/soundcard.h> header file.  */
/* #undef HAVE_MACHINE_SOUNDCARD_H */

/* Define if you have the <memory.h> header file.  */
#define HAVE_MEMORY_H 1

/* Define if you have the <mme/mme_public.h> header file.  */
/* #undef HAVE_MME_MME_PUBLIC_H */

/* Define if you have the <mme/mmsystem.h> header file.  */
/* #undef HAVE_MME_MMSYSTEM_H */

/* Define if you have the <mmsystem.h> header file.  */
/* #undef HAVE_MMSYSTEM_H */

/* Define if you have the <netdb.h> header file.  */
#define HAVE_NETDB_H 1

/* Define if you have the <netinet/in.h> header file.  */
#define HAVE_NETINET_IN_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <strings.h> header file.  */
#define HAVE_STRINGS_H 1

/* Define if you have the <sys/asoundlib.h> header file.  */
#define HAVE_SYS_ASOUNDLIB_H 1

/* Define if you have the <sys/audioio.h> header file.  */
/* #undef HAVE_SYS_AUDIOIO_H */

/* Define if you have the <sys/filio.h> header file.  */
/* #undef HAVE_SYS_FILIO_H */

/* Define if you have the <sys/io.h> header file.  */
#define HAVE_SYS_IO_H 1

/* Define if you have the <sys/ioctl.h> header file.  */
#define HAVE_SYS_IOCTL_H 1

/* Define if you have the <sys/mman.h> header file.  */
#define HAVE_SYS_MMAN_H 1

/* Define if you have the <sys/socket.h> header file.  */
#define HAVE_SYS_SOCKET_H 1

/* Define if you have the <sys/soundcard.h> header file.  */
#define HAVE_SYS_SOUNDCARD_H 1

/* Define if you have the <sys/stat.h> header file.  */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/timeb.h> header file.  */
#define HAVE_SYS_TIMEB_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the <windows.h> header file.  */
/* #undef HAVE_WINDOWS_H */

/* Define if you have the db library (-ldb).  */
/* #undef HAVE_LIBDB */

/* Define if you have the m library (-lm).  */
#define HAVE_LIBM 1

/* Define if struct ioc_read_toc_single_entry has field entry. */
/* #undef HAVE_STRUCT_IOC_READ_TOC_SINGLE_ENTRY_ENTRY */

