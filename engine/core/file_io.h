#ifndef FILE_IO_H_
#define FILE_IO_H_

#include "core_inc.h"

static u8 *fio_read_all(const char *src_filepath, u32 *byte_count) {
	u8 *buffer = NULL;
	*byte_count = 0;
	FILE *file = fopen(src_filepath, "rb");
	if (file) {
		fseek(file,0,SEEK_END);
		u32 size = ftell(file);
		rewind(file);
		buffer = (u8*)ALLOC(size+1);
		if (buffer) {
			fread(buffer, 1, size,file);
			buffer[size] = '\0'; // do we always need the null termination? i think not
			*byte_count = size;
		}
		fclose(file);
	}
	return buffer;
}
static b32 fio_write_all(const char *dst_filepath, u8 *buffer, u32 byte_count) {
	b32 res = 0;
	FILE *file = fopen(dst_filepath, "wb");
	if (file){
		u32 wsize = fwrite(buffer, 1, byte_count, file);
		res = (wsize == byte_count);
		fclose(file);
	}
	return res;
}
static void fio_dealloc_all(u8 *buffer) {
	FREE(buffer);
	buffer = NULL;
}
#endif