/* Version string */
#undef VERSION

/* Define if you have the XFree86 DGA extension */
#undef HAS_DGA

/* Define if you have an old Mesa that defines APIENTRY instead
   of GLAPIENTRY */
#undef GLAPIENTRY_IS_APIENTRY

#ifdef GLAPIENTRY_IS_APIENTRY
#  define GLAPIENTRY   APIENTRY
#endif

/* Define this to something sane if you don't have stricmp */
#undef stricmp

/* Define this if you have GL_COLOR_INDEX8_EXT in GL/gl.h */
#undef HAVE_GL_COLOR_INDEX8_EXT

#if defined(X11)
#   define OUTPUT_STR	"X11"
#elif defined(GGI)
#   define OUTPUT_STR	"GGI"
#else
#   define OUTPUT_STR	"Unknown"
#endif

