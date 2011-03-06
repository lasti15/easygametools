/** web.h
  * 1.Declares a class to encode strings converting a String 
  * into a MIME format called "x-www-form-urlencoded" format. 
  * To convert a String, each character is examined in turn: 
  *     1) The ASCII characters 'a' through 'z', 'A' through 'Z', and '0' through '9' remain the same. 
  *     2) The space character ' ' is converted into a plus sign '+'. 
  *     3) All other characters are converted into the 3-character string "%xy", where xy is the two-digit hexadecimal representation of the lower 8-bits of the character. 
  * 2.Declares a class to decode such strings
  * 3. Declares the WebForm class to wrap win32 HTTP calls
  * Author: Vijay Mathew Pandyalakal
  * Date: 18/10/03
**/

#ifndef WEB_H_
#define WEB_H_

#include <string>
#include <vector>
#include <iostream>
using namespace std;

#include <windows.h>
#include <wininet.h>

namespace openutils {

class URLEncoder {
public:
    static string encode(string str);
private:
    static bool isOrdinaryChar(char c);
};

class URLDecoder {
public:
    static string decode(string str);
private:
    static int convertToDec(const char* hex);
    static void getAsDec(char* hex);
};

class WebForm {
private:
    vector<string> m_vctVars;
    vector<string> m_vctVals; 
    string m_strHostName;
    string m_strScriptFile;
	string m_strScriptMethod;
    HINTERNET m_hSession;   
    HINTERNET m_hRequest;
    unsigned long m_lBytesRead;

	unsigned int responseStatus;
	char* cookies;
	unsigned int cookieLength;
	char* data;
	unsigned int dataLength;

	unsigned int port;

public:
    WebForm();
    void putVariable(const char* var_name,const char* value);
    string getVariable(const char* var_name) const;
    void setHost(const char* host);
	void setPort(const int port) { this->port = port; }
    void setScriptFile(const char* sf);
	void setScriptMethod(const char* sm);
	int getPort() { return port; }
    string getHost() const;
    string getScriptFile() const;
	string getScriptMethod() const;

	void clearVariables();

    void sendRequest(const char* agent, const char* sessionId = NULL);

    char* getResponseData();
	int getResponseStatus();
	char* getResponseCookies();

	int getResponseLength() { return dataLength; }

    unsigned long getBytesRead() { return m_lBytesRead; }
    ~WebForm();
private:
    bool isDuplicateVar(const char* var_name);
};

class WebFormException {
private:
    string err;
public:
    WebFormException(const char* str) {
        err = str;
    }
    string getMessage() {
        return err;
    }
};
} // openutils

#endif
