/** web.cpp
  * Implements classes declared in web.h
  * Author: Vijay Mathew Pandyalakal
  * Date: 18/10/03
**/

#include <string>
#include <vector>
using namespace std;

#include <windows.h>
#include <wininet.h>
#include <tchar.h>

#include "Web.h"
#include "FatalException.h"
using namespace openutils;






static LPCTSTR ErrorMessage( DWORD error ) 

// Routine Description:
//      Retrieve the system error message for the last-error code.
{ 

    LPVOID lpMsgBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    return((LPCTSTR)lpMsgBuf);
}




string URLEncoder::encode(string str) {
    int len = str.length();
    char* buff = new char[len + 1];
    strcpy(buff,str.c_str());
    string ret = "";
    for(int i=0;i<len;i++) {
        if(isOrdinaryChar(buff[i])) {
            ret = ret + buff[i];
        }else if(buff[i] == ' ') {
            ret = ret + "+";
        }else {
            char tmp[6];
            sprintf(tmp,"%%%x",buff[i]);
            ret = ret + tmp;
        }
    }
    delete[] buff;
    return ret;
}

bool URLEncoder::isOrdinaryChar(char c) {
    char ch = tolower(c);
    if(ch == 'a' || ch == 'b' || ch == 'c' || ch == 'd' || ch == 'e' 
        || ch == 'f' || ch == 'g' || ch == 'h' || ch == 'i' || ch == 'j' 
        || ch == 'k' || ch == 'l' || ch == 'm' || ch == 'n' || ch == 'o' 
        || ch == 'p' || ch == 'q' || ch == 'r' || ch == 's' || ch == 't' 
        || ch == 'u' || ch == 'v' || ch == 'w' || ch == 'x' || ch == 'y' 
        || ch == 'z' || ch == '0' || ch == '1' || ch == '2' || ch == '3' 
        || ch == '4' || ch == '5' || ch == '6' || ch == '7' || ch == '8' 
        || ch == '9') {
        return true;
    }
    return false;
}

string URLDecoder::decode(string str) {
    int len = str.length();
    char* buff = new char[len + 1];
    strcpy(buff,str.c_str());
    string ret = "";
    for(int i=0;i<len;i++) {
        if(buff[i] == '+') {
            ret = ret + " ";
        }else if(buff[i] == '%') {
            char tmp[4];
            char hex[4];            
            hex[0] = buff[++i];
            hex[1] = buff[++i];
            hex[2] = '\0';      
            //int hex_i = atoi(hex);
            sprintf(tmp,"%c",convertToDec(hex));
            ret = ret + tmp;
        }else {
            ret = ret + buff[i];
        }
    }
    delete[] buff;
    return ret;
}

int URLDecoder::convertToDec(const char* hex) {
    char buff[12];
    sprintf(buff,"%s",hex);
    int ret = 0;
    int len = strlen(buff);
    for(int i=0;i<len;i++) {
        char tmp[4];
        tmp[0] = buff[i];
        tmp[1] = '\0';
        getAsDec(tmp);
        int tmp_i = atoi(tmp);
        int rs = 1;
        for(int j=i;j<(len-1);j++) {
            rs *= 16;
        }
        ret += (rs * tmp_i);
    }
    return ret;
}

void URLDecoder::getAsDec(char* hex) {
    char tmp = tolower(hex[0]);
    if(tmp == 'a') {
        strcpy(hex,"10");
    }else if(tmp == 'b') {
        strcpy(hex,"11");
    }else if(tmp == 'c') {
        strcpy(hex,"12");
    }else if(tmp == 'd') {
        strcpy(hex,"13");
    }else if(tmp == 'e') {
        strcpy(hex,"14");
    }else if(tmp == 'f') {
        strcpy(hex,"15");
    }else if(tmp == 'g') {
        strcpy(hex,"16");
    }
}

WebForm::WebForm() {
    string m_strContentType = "Content-Type: application/x-www-form-urlencoded";
    string m_strHostName = "localhost:8080";
    string m_strScriptFile = "";
    HINTERNET m_hSession = NULL;
    HINTERNET m_hConnect = NULL;
    HINTERNET m_hRequest = NULL;
    HINTERNET m_hResponse = NULL;
	this->cookies = NULL;
	this->data = NULL;
	this->port = 80;
}
void WebForm::putVariable(const char* var_name,const char* value) {
    if(isDuplicateVar(var_name)) {
        string str = "Duplicate variable name - ";
        str = str + var_name;
        throw WebFormException(str.c_str());
    }else {
        m_vctVars.push_back(var_name);
        m_vctVals.push_back(value);
    }
}

string WebForm::getVariable(const char* var_name) const {
    for(unsigned int i=0;i<m_vctVars.size();i++) {
        if(strcmpi(var_name,m_vctVars[i].c_str()) == 0) {
            return m_vctVals[i];
        }
    }
    string str = "Variable not found - ";
    str = str + var_name;
    throw WebFormException(str.c_str());
}

void WebForm::clearVariables() {
	m_vctVars.clear();
	m_vctVals.clear();
}

void WebForm::setHost(const char* host) {
    m_strHostName = host;
}

void WebForm::setScriptFile(const char* sf) {
    m_strScriptFile = sf;
}

void WebForm::setScriptMethod(const char* sm) {
    m_strScriptMethod = sm;
}

string WebForm::getHost() const {
    return m_strHostName;
}

string WebForm::getScriptFile() const {
    return m_strScriptFile;
}

string WebForm::getScriptMethod() const {
    return m_strScriptMethod;
}

void WebForm::sendRequest(const char* agent, const char* sessionId) {


	m_hSession = InternetOpen(agent,
                                PRE_CONFIG_INTERNET_ACCESS,
                                NULL,
                                INTERNET_INVALID_PORT_NUMBER,
                                0);
    if(m_hSession == NULL) {
        throw new ROG::FatalException("Error:- InternetOpen()");
    }

	LPCTSTR lpszServerName = m_strHostName.c_str();
	INTERNET_PORT nServerPort = port;
	LPCTSTR lpszUserName = NULL;
	LPCTSTR lpszPassword = NULL;
	DWORD dwConnectFlags = 0;
	DWORD dwConnectContext = 0;
	HINTERNET hConnect = InternetConnect(m_hSession,
					lpszServerName, nServerPort,
					lpszUserName, lpszPassword,
					INTERNET_SERVICE_HTTP,
					dwConnectFlags, dwConnectContext);



	string method = m_strScriptMethod;
	if (!(method == "GET") && !(method=="POST")) {
		method = "GET";
	}


	string host = m_strHostName;
    string form_action;
    int sz = m_vctVars.size();
    for(int i=0;i<sz;i++) {
        string var = m_vctVars[i];
        string enc_var = URLEncoder::encode(var);
        string val = m_vctVals[i];
        string enc_val = URLEncoder::encode(val);
        form_action = form_action + enc_var;
        form_action = form_action + "=";
        form_action = form_action + enc_val;
        if(i != (sz-1)) {
            form_action = form_action + "&";
        }
    }

	if (method == "GET" && form_action.length() > 0) {
		m_strScriptFile += "?" + form_action;
	}


	LPCTSTR lpszVerb = method.c_str();
	LPCTSTR lpszObjectName = m_strScriptFile.c_str();
	LPCTSTR lpszVersion = NULL;			// Use default.
	LPCTSTR lpszReferrer = NULL;		// No referrer.
	LPCTSTR *lplpszAcceptTypes = NULL;	// Whatever the server wants to give us.
	DWORD dwOpenRequestFlags = INTERNET_FLAG_RELOAD;
	DWORD dwOpenRequestContext = 0;
	HINTERNET hRequest = HttpOpenRequest(hConnect, lpszVerb, lpszObjectName, lpszVersion,
			lpszReferrer, lplpszAcceptTypes,
			dwOpenRequestFlags, dwOpenRequestContext);


	BOOL bResult = false;

	if (method == "GET") {
		bResult = HttpSendRequest(hRequest, NULL, 0, NULL, 0);
	} else {
		static const char* contentType = "Content-Type: application/x-www-form-urlencoded\n";

		char headers[1024];
		_snprintf(headers, 1023, "%s", contentType);

		if (sessionId != NULL) {
			//add the cookie header
			_snprintf(headers, 1023, "%sCookie: %s\n", contentType, sessionId);
		}

		bResult = HttpSendRequest(hRequest, headers, strlen(headers), (void*)form_action.c_str(), form_action.length());
	}

	if (bResult == FALSE) {
		printf("ERROR: %d", GetLastError());
		throw new ROG::FatalException("WebForm failed1");
	}

	static char httpResponse[1024];
	DWORD resultSize = 1023;

	bResult = HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE, httpResponse, &resultSize, NULL);
	if (bResult == FALSE) {
		throw new ROG::FatalException("WebForm failed2");
	}

	int result = atoi(httpResponse);
	this->responseStatus = result;

	resultSize = 1023;
	bResult = HttpQueryInfo(hRequest, HTTP_QUERY_SET_COOKIE, httpResponse, &resultSize, NULL);
	if (bResult == TRUE) {
		if (this->cookies != NULL) {
			delete [] this->cookies;
		}
		this->cookies = strdup(httpResponse);
		this->cookieLength =  strlen(this->cookies);
	}

	unsigned int bufferPtr = 0;
	char tempBuffer[8192];
	DWORD dwBytesAvailable;
	while (InternetQueryDataAvailable(hRequest, &dwBytesAvailable, 0, 0) == TRUE)
	{
		char *pMessageBody = tempBuffer + bufferPtr;
		if (bufferPtr + dwBytesAvailable > 8191) {
			dwBytesAvailable = 8192 - bufferPtr - 1;
		}

		DWORD dwBytesRead;
		BOOL bResult = InternetReadFile(hRequest, pMessageBody,	dwBytesAvailable, &dwBytesRead);
		if (!bResult)
		{
			throw new ROG::FatalException("WebForm failed3");
		}
		bufferPtr += dwBytesRead;

		if (dwBytesRead == 0)
			break;	// End of File.

		//make sure were not going to overflow the buffer
		if (bufferPtr >= 8191) {
			break;
		}
	}

	tempBuffer[bufferPtr] = 0;
	this->data = strdup(tempBuffer);
	this->dataLength = strlen(this->data);

	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(m_hSession);
}

char* WebForm::getResponseData() {
    return data;    
}

int WebForm::getResponseStatus() {
	return responseStatus;
}

char* WebForm::getResponseCookies() {
	return cookies; 
}

bool WebForm::isDuplicateVar(const char* var_name) {
    for(unsigned int i=0;i<m_vctVars.size();i++) {
        if(strcmpi(var_name,m_vctVars[i].c_str()) == 0) {
            return true;
        }
    }
    return false;
}

WebForm::~WebForm() {
    if(m_hSession != NULL) InternetCloseHandle(m_hSession);
    if(m_hRequest != NULL) InternetCloseHandle(m_hRequest); 
};
