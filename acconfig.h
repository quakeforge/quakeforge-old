/* Version string */
#undef QF_VERSION
#undef VERSION

/* Define if you have the XFree86 DGA extension */
#undef HAS_DGA

/* If your version of OpenGL uses APIENTRY, define GLAPIENTRY to be APIENTRY*/
#undef GLAPIENTRY

/* Define this to something sane if you don't have stricmp */
#undef stricmp

/* Define this if you are using Mesa */
#undef XMESA

/* Define this if you have GL_COLOR_INDEX8_EXT in GL/gl.h */
#undef HAVE_GL_COLOR_INDEX8_EXT

/* Define this if C symbols are prefixed with an underscore */
#undef HAVE_SYM_PREFIX_UNDERSCORE

/* Define this if you have a Linux-style CD-ROM API */
#undef USE_LINUX_CD

/* Define this if you have a BSD-style CD-ROM API */
#undef USE_BSD_CD

/* Define this to use experimental code */
#undef _EXPERIMENTAL_

#if defined(X11)
#   define OUTPUT_STR	"X11"
#elif defined(GGI)
#   define OUTPUT_STR	"GGI"
#else
#   define OUTPUT_STR	"Unknown"
#endif

