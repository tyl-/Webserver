#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <unistd.h>
#include <iostream>

#include <time.h>
#include <cstring>
#include <string>

#include <fstream>
#include <algorithm>
#include <dirent.h>

#include <sys/stat.h>

#include <sstream>

std::string createHeader(int code, time_t tN, time_t tLM, std::string contentType, std::string contentLength){
	char buffer[256];
	char bufferB[256];
	struct tm *timeNow = localtime(&tN);
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", timeNow);
	std::string dateStringNow(buffer);
	struct tm *timeLM = localtime(&tLM);
	strftime(bufferB, sizeof(bufferB), "%a, %d %b %Y %H:%M:%S %Z", timeLM);
	std::string dateStringLM(bufferB);
	std::string header = "";														//String variable that will store the HTTP header, this will be sent as the return value.
	switch(code){
		case(200):																	//HTTP code is 200, which is an OK response
			header += "HTTP/1.1 200 OK\r\n";
			header += "Date: " + dateStringNow + "\r\n";
			header += "Last-Modified: " + dateStringLM + "\r\n";
			header += "Content-Length: " + contentLength + "\r\n";
			header += "Content-Type: " + contentType + "\r\n";
			header += "Connection: close\r\n\r\n";
			break;
		case(400):
			header += "HTTP/1.1 400 BAD REQUEST\r\n";								//HTTP code is 400, which is a BAD REQUEST response
			header += "Date: " + dateStringNow + "\r\n";
			header += "Content-Length: 15\r\n";
			header += "Content-Type: text/plain\r\n";
			header += "Connection: close\r\n\r\n";
			header += "400 BAD REQUEST";
			break;
		case(403):																	//HTTP code is 403, which is a FORBIDDEN response
			header += "HTTP/1.1 403 FORBIDDEN\r\n";
			header += "Date: " + dateStringNow + "\r\n";
			header += "Content-Length: 13\r\n";
			header += "Content-Type: text/plain\r\n";
			header += "Connection: close\r\n\r\n";
			header += "403 FORBIDDEN";
			break;
		case(404):																	//HTTP code is 404, which is a NOT FOUND response
			header += "HTTP/1.1 404 NOT FOUND\r\n";
			header += "Date: " + dateStringNow + "\r\n";
			header += "Content-Length: 13\r\n";
			header += "Content-Type: text/plain\r\n";
			header += "Connection: close\r\n\r\n";
			header += "404 NOT FOUND";
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

std::string addNumbers(std::string raw){
	int num1;
	int num2;
	std::string requestVars;
	std::string firstName;
	std::string stringNumOne;
	std::string stringNumTwo;
	std::stringstream ss;
	std::string stringSum;
	std::string htmlText;
	requestVars.append(raw, raw.find_last_of("?") + 1, raw.length());
	firstName = requestVars.substr(requestVars.find("PERSON=") + 7);
	firstName = firstName.substr(0, firstName.find("NUM1=") - 1);
	requestVars = requestVars.substr(requestVars.find("NUM1="));
	stringNumOne = requestVars.substr(requestVars.find("NUM1=") + 5);
	stringNumOne = stringNumOne.substr(0, stringNumOne.find("NUM2=") - 1);
	requestVars = requestVars.substr(requestVars.find("NUM2="));
	stringNumTwo = requestVars.substr(requestVars.find("NUM2=") + 5);
	std::istringstream(stringNumOne) >> num1;
	std::istringstream(stringNumTwo) >> num2;
	ss << (num1 + num2);
	stringSum = ss.str();
	htmlText.append("<html>\r\n<head><TITLE>SAMPLE FORM for AddNum</title></head>\r\n<BODY>\r\n<H1> AddNum </H1>\r\n\r\n");
	htmlText.append("<FORM method=\"GET\" action=\"http://localhost:2540/cgi/addnums.fake-cgi\">\r\n\r\n");
	htmlText.append("Enter your name and two numbers: <br>\r\n\r\n");
	htmlText.append("<INPUT TYPE=\"text\" NAME=\"person\" size=20 value=\"" + firstName + "\"><P>\r\n\r\n");
	htmlText.append("<INPUT TYPE=\"text\" NAME=\"num1\" size=5 value=\"" + stringNumOne + "\"> <br>\r\n");
	htmlText.append("<INPUT TYPE=\"text\" NAME=\"num2\" size=5 value=\"" + stringNumTwo + "\"> <br>\r\n\r\n");
	htmlText.append("<INPUT TYPE=\"submit\" VALUE=\"Submit Numbers\"> <br>\r\n\r\n");
	htmlText.append("Hello, " + firstName + ". The sum of " + stringNumOne + " and " + stringNumTwo + " is " + stringSum + "\r\n\r\n");
	htmlText.append("</FORM> </BODY></html>");
	return htmlText;
}

std::string processRequest(char *requestPath, time_t tN){
	std::string pathString(requestPath, 0, strlen(requestPath));
	transform(pathString.begin(), pathString.end(), pathString.begin(),::toupper);
	std::ifstream readFile;
	size_t found;
	bool is_a_file = false;
	bool is_a_directory = false;
	if(pathString.find("..") != std::string::npos && pathString.find("/.ht") != std::string::npos && pathString.find("~") != std::string::npos) {
		return createHeader(400, tN, tN, "", "");
	}
	else if(pathString.find("/CGI/ADDNUMS.FAKE-CGI?") != std::string::npos){
		std::string htmlText;
		htmlText = addNumbers(pathString);
		std::stringstream ss;
		ss << htmlText.length();
		std::string stringLength;
		stringLength = ss.str();
		return createHeader(200, tN, tN, "text/html", stringLength) + htmlText;
	}else if(pathString.find("/") == 0){
		struct stat buf;
		std::string parentDir = "./";
		std::string tempPath = ".";
		if(pathString.find_last_of("/") == pathString.length() - 1 && pathString.length() > 1){
			tempPath.append(pathString.begin(), pathString.end()-1);
			pathString.append(pathString.begin(), pathString.end()-1);
		}else{
			tempPath.append(pathString);
		}
		stat(tempPath.c_str(), &buf);
		if(stat(tempPath.c_str(), &buf) == 0){
			is_a_file = S_ISREG(buf.st_mode);
			is_a_directory = S_ISDIR(buf.st_mode);
			time_t tLM = buf.st_mtime;
			if(is_a_file){
				std::string fileExtension;
				fileExtension.append(tempPath, tempPath.find_last_of(".") + 1, tempPath.length());
				if(fileExtension == "HTML" || fileExtension == "TXT"){
					std::ifstream fin(tempPath.c_str());
					std::stringstream ss;
					std::string dataText;
					if (fin){
						ss << fin.rdbuf();
						fin.close();
					}
					dataText = ss.str();
					std::stringstream ssB;
					ssB << dataText.length();
					std::string stringLength = ssB.str();
					if (fileExtension == "HTML"){
						return createHeader(200, tN, tLM, "text/html", stringLength) + dataText;
					}else if (fileExtension == "TXT"){
						return createHeader(200, tN, tLM, "text/plain", stringLength) + dataText;
					}
				}else{
					return createHeader(403, tN, tN, "", "");
				}
			}else if(is_a_directory || pathString.find_last_of("/") == pathString.length() -1){
				DIR* tempDir = opendir(tempPath.c_str());
				dirent* tempDE;
				std::string htmlText = "<html><body>\r\n<h1> Index of ";
				htmlText.append(pathString, pathString.find_last_of("/"), pathString.length());
				htmlText.append("</h1>\r\n\r\n");
				while(true){
					tempDE = readdir(tempDir);
					if (tempDE == NULL){
						break;
					}
					if(std::string(tempDE->d_name).compare(".") == 0 ){
						//skip
					}
					else if (std::string(tempDE->d_name).compare("..") == 0){
						if(tempPath.compare(parentDir) != 0){
							htmlText.append("<a href='");
							htmlText.append(pathString, 0, pathString.find_last_of("/"));
							htmlText.append("/'>Parent Directory</a><br>\r\n");
						}
					}else{
						htmlText.append("<a href='");
						htmlText.append(pathString);
						if(pathString.length() != 1){
							htmlText.append("/");
						}
						htmlText.append(std::string(tempDE->d_name));
						htmlText.append("'>");
						htmlText.append(std::string(tempDE->d_name));
						htmlText.append("</a><br>\r\n");
					}
				}
				htmlText.append("\r\n</body></html>");
				std::stringstream ss;
				ss << htmlText.length();
				std::string strLength = ss.str();
				return createHeader(200, tN, tLM, "text/html", strLength) + htmlText;
			}
			else{
				return createHeader(404, tN, tN, "", "");
			}
		}
		else{
			return createHeader(404, tN, tN, "", "");
		}
	}
	else{
		return createHeader(400, tN, tN, "", "");
	}
}

void sendResponse(int socket, char* request){
	char *firstLine = strtok(request, "\n");
	char *httpRequest = strtok(firstLine, " \r\n");
	char *requestPath = strtok(NULL, " \r\n");
	char *httpVersion = strtok(NULL, " \r\n");
	time_t tN = time(0);
	if(checkRequestHeader(httpRequest, requestPath, httpVersion) == true){
		std::string tempString = processRequest(requestPath, tN);
		char *httpResponse = new char[tempString.length()];
		std::strcpy(httpResponse, tempString.c_str());
		write(socket, httpResponse, strlen(httpResponse));
		//std::cout << tempString << std::endl;
	}else{
		std::string tempString = createHeader(400, tN, tN, "", "");
		char *httpResponse = new char[tempString.length()];
		write(socket, httpResponse, strlen(httpResponse));
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