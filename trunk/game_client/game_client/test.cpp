//#include <stdio.h>
//#include <sys/types.h>
//#include <Winsock2.h>
//
//void error(char *msg)
//{
//    perror(msg);
//    exit(0);
//}
//
//int main(int argc, char *argv[])
//{
//	WSADATA wsadata;
//
//    int error = WSAStartup(0x0202, &wsadata);
//
//    //Did something happen?
//
//    if (error)
//        return false;
//
//    //Did we get the right Winsock version?
//
//    if (wsadata.wVersion != 0x0202)
//    {
//        WSACleanup(); //Clean up Winsock
//
//        return false;
//    }
//
//    //Fill out the information needed to initialize a socket�
//
//    SOCKADDR_IN target; //Socket address information
//
//
//    target.sin_family = AF_INET; // address family Internet
//
//    target.sin_port = htons (80); //Port to connect on
//
//
//	struct hostent* serverHost = gethostbyname("www.mattrochon.ca");
//
//
//	memcpy((char*)&target.sin_addr.s_addr, (char*)serverHost->h_addr, serverHost->h_length); //Target IP
//
//	SOCKET s;
//    s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
//
//    if (s == INVALID_SOCKET)
//    {
//        return false; //Couldn't create the socket
//
//    }  
//
//    //Try connecting...
//
//
//    if (connect(s, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR) {
//		return -1;
//	}
//
//	char* http = "POST /raknet/DirectoryServer.php?query=upload&uploadPassword=uploader HTTP1.1\nContent-Length:38\n\n__GAME_PORT\00111223\001__GAME_NAME\001testgame\n\n";
//	send(s, http, strlen(http), 0);
//
//	char buf[1000];
//	recv(s, buf, 999, 0);
//
//	printf(buf);
//
//
//	scanf("%s", &buf);
//
//    return 0;
//}
