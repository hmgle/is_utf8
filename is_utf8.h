#ifndef _IS_UTF8_H
#define _IS_UTF8_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef MAX_LINE
#define MAX_LINE	1024
#endif

enum file_encoding_type {
	UTF8_NO_BOM,
	UTF8_WITH_BOM,
	GBK
};

int is_utf8(unsigned char *str, size_t len);
int detect_file_encoding(FILE *text_fp);

#endif /* _IS_UTF8_H */
