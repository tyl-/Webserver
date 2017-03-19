#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <unistd.h>
#include <iostream>

#include <time.h>
#include <string>

#include <fstream>
#include <algorithm>
#include <dirent.h>

std::string createHeader(int code, tm* timeLM, std::string contentType, std::string contentLength){
	time_t t;
	time(&t);
	struct tm* timeNow = localtime(&t);
	char buffer[256];
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", timeNow);
	std::string dateStringNow(buffer);
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", timeLM);
	std::string dateStringLM(buffer);
	std::string header = "";																//String variable that will store the HTTP header, this will be sent as the return value.
	switch(code){
		case(200):																	//HTTP code is 200, which is an OK response
			header += "HTTP/1.1 200 OK\r\n";
			header += "Date: " + dateStringNow + "\r\n";
			header += "Last-Modified: " + dateStringLM + "\r\n";
			header += "Content-Length: " + contentLength + "\r\n";
			header += "Content-Type: " + contentType + "\r\n\r\n";
			break;
		case(400):
			header += "HTTP/1.1 400 BAD REQUEST\r\n";								//HTTP code is 400, which is a BAD REQUEST response
			header += "Date: " + dateStringNow + "\r\n";
			header += "Content-Length: 0\r\n";
			header += "Connection: close\r\n\r\n\0";
			break;
		case(403):																	//HTTP code is 403, which is a FORBIDDEN response
			header += "HTTP/1.1 403 FORBIDDEN\r\n";
			header += "Date: " + dateStringNow + "\r\n";
			header += "Content-Length: 0\r\n";
			header += "Connection: close\r\n\r\n";
			break;
		case(404):																	//HTTP code is 404, which is a NOT FOUND response
			header += "HTTP/1.1 404 NOT FOUND\r\n";
			header += "Date: " + dateStringNow + "\r\n";
			header += "Content-Length: 0\r\n";
			header += "Connection: close\r\n\r\n";
			break;
		default:																	//default case, shouldn't happen.
			break;
	}
	return header;			
}

bool checkRequestHeader(char* httpRequest, char* requestPath, char *httpVersion){
	if(httpRequest != NULL && strlen(httpRequest) > 0 && requestPath != NULL && strlen(requestPath) > 0 && httpVersion != NULL && strlen(httpVersion) > 0){
		if (strcmp(httpRequest, "GET") == 0 && (strcmp(httpVersion, "HTTP/1.0") == 0 || strcmp(httpVersion, "HTTP/1.0\r") == 0 || strcmp(httpVersion, "HTTP/1.0\r\n") == 0 || strcmp(httpVersion, "HTTP/1.1") == 0 || strcmp(httpVersion, "HTTP/1.1\r") == 0 || strcmp(httpVersion, "HTTP/1.1\r\n") == 0)){
			return true;
		}
	}
	return false;
}

void addNumbers(){
	
}

std::string processRequest(char *requestPath, tm* timeNow){
	std::string pathString(requestPath, 0, strlen(requestPath));
	transform(pathString.begin(), pathString.end(), pathString.begin(),::toupper);
	std::ifstream readFile;
	size_t found;
	if(pathString.find("..") != std::string::npos && pathString.find("/.ht") != std::string::npos && pathString.find("~") != std::string::npos) {
		return createHeader(403, timeNow, "", "");
	}
	else if(pathString.find("/CGI/ADDNUMS.FAKE-CGI?") != std::string::npos){
		addNumbers();
	}
	else if(pathString.find(".HTML") == pathString.length() -5){
		return "Found an HTML request";
	}
	else if(pathString.find(".TXT") == pathString.length() -4){
		return "Found an TXT request";
	}
	else if(pathString.find_last_of("/") == pathString.length() -1){
		std::string tempPath = "." + pathString;
		std::string responseString;
		if(opendir(tempPath.c_str()) != NULL){
			DIR* tempDir = opendir(tempPath.c_str());
			dirent* tempDE;
			while(true){
				tempDE = readdir(tempDir);
				if (tempDE == NULL){
					break;
				}
				std::cout << std::string(tempDE->d_name) << std::endl;
			}
			return "FOUND A DIRECTORY";
		}
	}else{
		return createHeader(403, timeNow, "", "");
	}
}

void sendResponse(int socket, char* request){
	char *firstLine = strtok(request, "\n");
	char *httpRequest = strtok(firstLine, " \r\n");
	char *requestPath = strtok(NULL, " \r\n");
	char *httpVersion = strtok(NULL, " \r\n");
	time_t t;
	time(&t);
	struct tm* timeNow = localtime(&t);
	if(checkRequestHeader(httpRequest, requestPath, httpVersion) == true){
		std::cout << processRequest(requestPath, timeNow);
	}else{
		std::cout << createHeader(400, timeNow, "", "");
	}
}

void worker(int socket){
	std::cout << "Handling client\n";
	
	char *sendbuf;
	char recvbuf[1000];//Variables												

	memset(recvbuf, '\0', 1000);																			//Clears buffer
	int recvMsgSize;
	recvMsgSize = recv(socket, recvbuf, 1000, 0);
	if (recvMsgSize < 0){
		perror("ERROR on message received");
	} else if (recvMsgSize == 0){
		perror("ERROR client disconnected");
	}else{
		printf("%s", recvbuf);
		sendResponse(socket, recvbuf);
	}
	exit(1);
}

int main( int argc, char *argv[] ) {
   int sockfd, portno, clilen, n, pid, newsockfd;
   int* newersockfd;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   pthread_t t;
   
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 2540;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
   
   listen(sockfd,6);
   clilen = sizeof(cli_addr);
   
   while (1) {
	   newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	   if(newsockfd < 0){
		   perror("ERROR on accept");
	   }
	   pid = fork();
	   if (pid < 0){
		   perror("ERROR on fork");
	   }
	   if (pid ==0){
		   close(sockfd);
		   worker(newsockfd);
		   close(newsockfd);
		   exit(0);
	   }
   } /* end of while */
}