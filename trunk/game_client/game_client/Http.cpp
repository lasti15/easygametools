
#include "Http.h"
#include <stdlib.h>

using namespace ROG;


const int Http::getStatus(char* response) {
	
	static const char* HTTP_RESPONSE_PREFIX = "HTTP/1.1 ";
	static const int prefix_len = strlen(HTTP_RESPONSE_PREFIX);

	if (strncmp(HTTP_RESPONSE_PREFIX, response, prefix_len) == 0) {

		const int CODE_BUF_LEN = 10;
		char codeBuf[CODE_BUF_LEN];
		char* statusStart = response + prefix_len;
		int code_buf_ptr = 0;

		while (isdigit(*statusStart) && code_buf_ptr < CODE_BUF_LEN-1) {
			codeBuf[code_buf_ptr] = *statusStart;
			code_buf_ptr++;
			statusStart++;
		}

		codeBuf[code_buf_ptr] = 0;

		int result = atoi(codeBuf);
		return result;
	}

	return HTTP_ERR;
}
	
const char* Http::getData(char* response) {
	const static char* NEWLINES[] = {"\r\n\r\n", "\n\n", "\r\r"};
	//scan until we find 2 of the same newlines in a row
	for (int i=0; i < sizeof(NEWLINES); i++) {
		char* newLine = strstr(response, NEWLINES[i]);
		if (newLine != NULL) {
			//skip the 2 newlines
			return newLine+strlen(NEWLINES[i]);
		}
	}
	return NULL;
}