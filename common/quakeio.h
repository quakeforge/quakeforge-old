#ifndef __quake_io_h
#define __quake_io_h

#include <stdio.h>

#include "config.h"
#ifdef HAS_ZLIB
#include <zlib.h>
#endif

typedef struct {
	FILE *file;
	gzFile *gzfile;
} QFile;

QFile *Qopen(const char *path, const char *mode);
QFile *Qdopen(int fd, const char *mode);
void Qclose(QFile *file);
int Qread(QFile *file, void *buf, int count);
int Qwrite(QFile *file, void *buf, int count);
int Qprintf(QFile *file, const char *fmt, ...);
char *Qgets(QFile *file, char *buf, int count);
int Qgetc(QFile *file);
int Qputc(QFile *file, int c);
int Qseek(QFile *file, long offset, int whence);
long Qtell(QFile *file);
int Qflush(QFile *file);
int Qeof(QFile *file);


#endif /*__quake_io_h*/
