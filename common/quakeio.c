#include <malloc.h>
#include <stdarg.h>
#include "quakeio.h"

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
	if (zip) {
		file->gzfile=gzopen(path,m);
		if (!file->gzfile) {
			free(file);
			return 0;
		}
	} else {
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
	if (zip) {
		file->gzfile=gzdopen(fd,m);
		if (!file->gzfile) {
			free(file);
			return 0;
		}
	} else {
		file->file=fdopen(fd,m);
		if (!file->file) {
			free(file);
			return 0;
		}
	}
	return file;
}

void Qclose(QFile *file)
{
	if (file->file)
		fclose(file->file);
	else
		gzclose(file->gzfile);
	free(file);
}

int Qread(QFile *file, void *buf, int count)
{
	if (file->file)
		return fread(buf, 1, count, file->file);
	else
		return gzread(file->gzfile,buf,count);
}

int Qwrite(QFile *file, void *buf, int count)
{
	if (file->file)
		return fwrite(buf, 1, count, file->file);
	else
		return gzwrite(file->gzfile,buf,count);
}

int Qprintf(QFile *file, const char *fmt, ...)
{
	va_list args;
	int ret;

	va_start(args,fmt);
	if (file->file)
		ret=vfprintf(file->file, fmt, args);
	else {
		char buf[4096];
		va_start(args, fmt);
#ifdef HAVE_VSNPRINTF
		(void)vsnprintf(buf, sizeof(buf), fmt, args);
#else
		(void)vsprintf(buf, fmt, args);
#endif
		va_end(va);
		ret = strlen(buf); /* some *sprintf don't return the nb of bytes written */
		if (ret>0)
			ret=gzwrite(file, buf, (unsigned)ret);
	}
	va_end(args);
	return ret;
}

char *Qgets(QFile *file, char *buf, int count)
{
	if (file->file)
		return fgets(buf, count, file->file);
	else
		return gzgets(file->gzfile,buf,count);
}

int Qgetc(QFile *file)
{
	if (file->file)
		return fgetc(file->file);
	else
		return gzgetc(file->gzfile);
}

int Qputc(QFile *file, int c)
{
	if (file->file)
		return fputc(c, file->file);
	else
		return gzputc(file->gzfile,c);
}

int Qseek(QFile *file, long offset, int whence)
{
	if (file->file)
		return fseek(file->file, offset, whence);
	else
		return gzseek(file->gzfile,offset,whence);
}

long Qtell(QFile *file)
{
	if (file->file)
		return ftell(file->file);
	else
		return gztell(file->gzfile);
}

int Qflush(QFile *file)
{
	if (file->file)
		return fflush(file->file);
	else
		return gzflush(file->gzfile,Z_SYNC_FLUSH);
}

int Qeof(QFile *file)
{
	if (file->file)
		return feof(file->file);
	else
		return gzeof(file->gzfile);
}
