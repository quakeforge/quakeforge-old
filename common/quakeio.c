#include <malloc.h>
#include <stdarg.h>
#include <stdlib.h>
#include "quakeio.h"
#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

QFile *Qopen(const char *path, const char *mode)
{
	QFile *file;
	char m[80],*p;
	int zip=0;

	for (p=m; *mode && p-m<(sizeof(m)-1); mode++) {
		if (*mode=='z') {
			zip=1;
			continue;
		}
		*p++=*mode;
	}
	*p=0;

	file=calloc(sizeof(*file),1);
	if (!file)
		return 0;
#ifdef HAS_ZLIB
	if (zip) {
		file->gzfile=gzopen(path,m);
		if (!file->gzfile) {
			free(file);
			return 0;
		}
	} else
#endif
	{
		file->file=fopen(path,m);
		if (!file->file) {
			free(file);
			return 0;
		}
	}
	return file;
}

QFile *Qdopen(int fd, const char *mode)
{
	QFile *file;
	char m[80],*p;
	int zip=0;

	for (p=m; *mode && p-m<(sizeof(m)-1); mode++) {
		if (*mode=='z') {
			zip=1;
			continue;
		}
		*p++=*mode;
	}

	*p=0;

	file=calloc(sizeof(*file),1);
	if (!file)
		return 0;
#ifdef HAS_ZLIB
	if (zip) {
		file->gzfile=gzdopen(fd,m);
		if (!file->gzfile) {
			free(file);
			return 0;
		}
	} else
#endif
	{
		file->file=fdopen(fd,m);
		if (!file->file) {
			free(file);
			return 0;
		}
	}
#ifdef WIN32
	_setmode(_fileno(file->file),_O_BINARY);
#endif
	return file;
}

void Qclose(QFile *file)
{
	if (file->file)
		fclose(file->file);
#ifdef HAS_ZLIB
	else
		gzclose(file->gzfile);
#endif
	free(file);
}

int Qread(QFile *file, void *buf, int count)
{
	if (file->file)
		return fread(buf, 1, count, file->file);
#ifdef HAS_ZLIB
	else
		return gzread(file->gzfile,buf,count);
#else
	return -1;
#endif
}

int Qwrite(QFile *file, void *buf, int count)
{
	if (file->file)
		return fwrite(buf, 1, count, file->file);
#ifdef HAS_ZLIB
	else
		return gzwrite(file->gzfile,buf,count);
#else
	return -1;
#endif
}

int Qprintf(QFile *file, const char *fmt, ...)
{
	va_list args;
	int ret=-1;

	va_start(args,fmt);
	if (file->file)
		ret=vfprintf(file->file, fmt, args);
#ifdef HAS_ZLIB
	else {
		char buf[4096];
		va_start(args, fmt);
#ifdef HAVE_VSNPRINTF
		(void)vsnprintf(buf, sizeof(buf), fmt, args);
#else
		(void)vsprintf(buf, fmt, args);
#endif
		va_end(args);
		ret = strlen(buf); /* some *sprintf don't return the nb of bytes written */
		if (ret>0)
			ret=gzwrite(file, buf, (unsigned)ret);
	}
#endif
	va_end(args);
	return ret;
}

char *Qgets(QFile *file, char *buf, int count)
{
	if (file->file)
		return fgets(buf, count, file->file);
#ifdef HAS_ZLIB
	else
		return gzgets(file->gzfile,buf,count);
#else
	return 0;
#endif
}

int Qgetc(QFile *file)
{
	if (file->file)
		return fgetc(file->file);
#ifdef HAS_ZLIB
	else
		return gzgetc(file->gzfile);
#else
	return -1;
#endif
}

int Qputc(QFile *file, int c)
{
	if (file->file)
		return fputc(c, file->file);
#ifdef HAS_ZLIB
	else
		return gzputc(file->gzfile,c);
#else
	return -1;
#endif
}

int Qseek(QFile *file, long offset, int whence)
{
	if (file->file)
		return fseek(file->file, offset, whence);
#ifdef HAS_ZLIB
	else
		return gzseek(file->gzfile,offset,whence);
#else
	return -1;
#endif
}

long Qtell(QFile *file)
{
	if (file->file)
		return ftell(file->file);
#ifdef HAS_ZLIB
	else
		return gztell(file->gzfile);
#else
	return -1;
#endif
}

int Qflush(QFile *file)
{
	if (file->file)
		return fflush(file->file);
#ifdef HAS_ZLIB
	else
		return gzflush(file->gzfile,Z_SYNC_FLUSH);
#else
	return -1;
#endif
}

int Qeof(QFile *file)
{
	if (file->file)
		return feof(file->file);
#ifdef HAS_ZLIB
	else
		return gzeof(file->gzfile);
#else
	return -1;
#endif
}
