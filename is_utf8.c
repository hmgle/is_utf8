#include "is_utf8.h"

/*
  Check if the given unsigned char * is a valid utf-8 sequence.

  Return value :
  If the string is valid utf-8, 0 is returned.
  Else the position, starting from 1, is returned.

  Valid utf-8 sequences look like this :
  0xxxxxxx
  110xxxxx 10xxxxxx
  1110xxxx 10xxxxxx 10xxxxxx
  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
int is_utf8(unsigned char *str, size_t len)
{
    size_t i = 0;
    size_t continuation_bytes = 0;

    while (i < len)
    {
        if (str[i] <= 0x7F)
            continuation_bytes = 0;
        else if (str[i] >= 0xC0 /*11000000*/ && str[i] <= 0xDF /*11011111*/)
            continuation_bytes = 1;
        else if (str[i] >= 0xE0 /*11100000*/ && str[i] <= 0xEF /*11101111*/)
            continuation_bytes = 2;
        else if (str[i] >= 0xF0 /*11110000*/ && str[i] <= 0xF4 /* Cause of RFC 3629 */)
            continuation_bytes = 3;
        else
            return i + 1;
        i += 1;
        while (i < len && continuation_bytes > 0
               && str[i] >= 0x80
               && str[i] <= 0xBF)
        {
            i += 1;
            continuation_bytes -= 1;
        }
        if (continuation_bytes != 0)
            return i + 1;
    }
    return 0;
}

int
detect_file_encoding(FILE *text_fp)
{
	int ret;
	fpos_t pos;
	uint8_t buf[MAX_LINE];
	size_t len;
	enum file_encoding_type encoding_type;

	ret = fgetpos(text_fp, &pos);
	assert(!ret);

	rewind(text_fp);
	if (fgets((char *)buf, sizeof(buf), text_fp) == NULL)
		return -1;
	len = strlen((char *)buf);
	if (len >= 3 && buf[0] == 0xef && buf[1] == 0xbb && buf[2] == 0xbf) {
		encoding_type = UTF8_WITH_BOM;
		goto end;
	}
	else {
		ret = is_utf8(buf, len);
		if (ret > 0) {
			encoding_type = GBK;
			goto end;
		} else if (ret == -1) {
			encoding_type = UTF8_NO_BOM;
			goto end;
		}
	}

	while (fgets((char *)buf, sizeof(buf), text_fp)) {
		len = strlen((char *)buf);
		ret = is_utf8(buf, len);
		if (ret > 0) {
			encoding_type = GBK;
			goto end;
		} else if (ret == -1) {
			encoding_type = UTF8_NO_BOM;
			goto end;
		}
	}
	encoding_type = UTF8_NO_BOM;
end:
	ret = fsetpos(text_fp, &pos);
	assert(!ret);
	return encoding_type;
}
