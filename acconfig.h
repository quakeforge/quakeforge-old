/* Version string */
#define        VERSION         "0.1.0-pre"

/* Define if you have the XFree86 DGA extension */
#undef HAS_DGA

/* Define if you have an old Mesa that defines APIENTRY instead
   of GLAPIENTRY */
#undef GLAPIENTRY_IS_APIENTRY

#ifdef GLAPIENTRY_IS_APIENTRY
#  define GLAPIENTRY   APIENTRY
#endif

/* Define this if you have GL_COLOR_INDEX8_EXT in GL/gl.h */
#undef HAVE_GL_COLOR_INDEX8_EXT

