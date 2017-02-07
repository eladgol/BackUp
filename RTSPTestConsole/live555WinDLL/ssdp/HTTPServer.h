/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** HTTPServer.h
** 
** V4L2 RTSP streamer                                                                 
**        
** HTTP server that serves HLS & MPEG-DASH playlist and segments
**                                                                                    
** -------------------------------------------------------------------------*/


#include "RTSPServer.hh"
#include "RTSPCommon.hh"
#include <vector>
#include <algorithm>
#define HTTPXMLResponse "HTTP/1.0 200 OK\r\n"\
					"%s"\
					"Server: EladsCamera Streaming Media \r\n"\
					"Content-Length: %d\r\n"\
					"Content-Type: text/xml; charset=\"utf-8\"\r\n"\
					"\r\n"
#define HTTPEventResponse "HTTP/1.0 200 OK\r\n"\
					"%s"\
					"Server: EladsCamera Streaming Media \r\n"\
					"Timeout: Second - 300\r\n"\
					"SID: uuid:%s\r\n"\
					"Content-Length: 0\r\n"\
					"\r\n"
void afterStreaming(void* clientData)
{
	TCPStreamSink* sink = (TCPStreamSink*)clientData;
}
// ---------------------------------------------------------
//  Extend RTSP server to add support for HLS and MPEG-DASH
// ---------------------------------------------------------
class HTTPServer : public RTSPServer
{
public:
	static HTTPServer* createNew(UsageEnvironment& env, Port ourPort = 554,
		UserAuthenticationDatabase* authDatabase = NULL,
		unsigned reclamationTestSeconds = 65)
	{
		int ourSocket = setUpOurSocket(env, ourPort);
		if (ourSocket == -1) return NULL;

		return new HTTPServer(env, ourSocket, ourPort, authDatabase, reclamationTestSeconds);
	}
	HTTPServer(UsageEnvironment& env,
		int ourSocket, Port ourPort,
		UserAuthenticationDatabase* authDatabase,
		unsigned reclamationTestSeconds) : RTSPServer(env,
			ourSocket, ourPort,
			authDatabase,
			reclamationTestSeconds)
	{

	}
	int CreateSOAPMethod(const char* csService, const char* csMethods[], const char* csMethodReply[], const int nMethods)
	{
		std::string sService(csService);
		soap_methods vecService;
		for (int i = 0; i < nMethods; i++)
			vecService.push_back(make_pair(std::string(csMethods[i]), std::string(csMethodReply[i])));
		mapSerivceToMethods.insert(std::make_pair(sService, vecService));
		//mapSerivceToMethods.insert(std::make_pair(sUri, sContent));
		if (!clientConnection)
		{

		}
		else
		{
			clientConnection->mapSerivceToMethods.insert(std::make_pair(sService, vecService));
		}
		return 0;
	}
	int CreatePage(const char* csUri, const char* csContent)
	{
		std::string sUri(csUri);
		std::string sContent(csContent);
		mapDirectoryToHTML.insert(std::make_pair(sUri, sContent));
		if (!clientConnection)
		{
			
		}
		else
		{
			clientConnection->mapDirectoryToHTML.insert(std::make_pair(sUri, sContent));
		}
		return 0;
	}
	virtual ClientConnection* createNewClientConnection(int clientSocket, struct sockaddr_in clientAddr)
	{
		clientConnection =  new HTTPClientConnection(*this, clientSocket, clientAddr, mapDirectoryToHTML, mapSerivceToMethods);
		return (clientConnection);
	}
	std::unordered_map<std::string, std::string> mapDirectoryToHTML;
	typedef std::string soap_method;
	typedef std::string soap_service;
	typedef std::string soap_methodReply;
	typedef std::vector<std::pair<soap_method, soap_methodReply>> soap_methods;
	std::map<soap_service, soap_methods> mapSerivceToMethods;
private:
	class HTTPClientConnection : public RTSPServer::RTSPClientConnection
	{
	protected:
		virtual void handleRequestBytes(int newBytesRead)
		{
			if (newBytesRead)
			{
				fRequestBuffer[newBytesRead] = 0;
				std::string sRequestBuffer((char*)fRequestBuffer);
				if (sRequestBuffer.find("GET ") == 0)
				{
					sRequestBuffer = sRequestBuffer.substr(5); // "GET \"
					int iSpace = sRequestBuffer.find(" ");
					std::string sURL = sRequestBuffer.substr(0, iSpace);
					auto entry = mapDirectoryToHTML.find(std::string(sURL));
					if (entry != mapDirectoryToHTML.end())
					{
						RTSPServer::RTSPClientConnection::handleRequestBytes(newBytesRead);
					}
				}
				else if (sRequestBuffer.find("POST ") == 0)
				{
					envir() << "HTTPClientConnection::handleRequestBytes got POST command " << sRequestBuffer.c_str() << "\n";
					int iSoapAction;
					if ((iSoapAction = sRequestBuffer.find("SOAPACTION:")) != -1)
					{
						sRequestBuffer = sRequestBuffer.substr(iSoapAction + strlen("SOAPACTION:"));
						envir() << "HTTPClientConnection::handleRequestBytes found SOAPACTION" << sRequestBuffer.c_str() << "\n";
						while (sRequestBuffer[0] == '\"' || sRequestBuffer[0] == ' ')
						{
							sRequestBuffer = sRequestBuffer.substr(1);
						}
						int iEndSoapAction = sRequestBuffer.find_first_of("\n\r");
						sRequestBuffer = sRequestBuffer.substr(0, iEndSoapAction);
						while (sRequestBuffer[sRequestBuffer.length() - 1] == '\"' || sRequestBuffer[sRequestBuffer.length() - 1] == ' ')
						{
							sRequestBuffer = sRequestBuffer.substr(0, sRequestBuffer.length() - 1);
						}
						std::vector<std::string> sSoapActionDelimited;
						while ((iEndSoapAction = sRequestBuffer.find_first_of(":#")) != -1)
						{
							std::string sDelimited = sRequestBuffer.substr(0, iEndSoapAction);
							sSoapActionDelimited.push_back(sDelimited);
							sRequestBuffer = sRequestBuffer.substr(iEndSoapAction + 1);
						}
						int iPrefix =  sRequestBuffer.find("#");
						soap_method sMethod = (iPrefix > 0) ? sRequestBuffer.substr(iPrefix) : sRequestBuffer;
						sSoapActionDelimited.push_back(sRequestBuffer);
						soap_service sService;
						if (sSoapActionDelimited.size() >= 3)
							sService = sSoapActionDelimited[3];
						else
						{
							envir() << "HTTPClientConnection::handleRequestBytes ERROR! SOAPACTION invalid\n";
							return;
						}

						auto iterMapSerivceToMethods = mapSerivceToMethods.find(sService);
						if (iterMapSerivceToMethods == mapSerivceToMethods.end())
						{
							envir() << "HTTPClientConnection::handleRequestBytes could not find service " << sService.c_str() << "\n";
							return;
						}
						soap_methods vecMethods = iterMapSerivceToMethods->second;
						auto itMethod = std::find_if(vecMethods.begin(), vecMethods.end(),
							[sMethod](const std::pair<std::string, std::string>& element) { return element.first == sMethod; });
						if (itMethod != vecMethods.end())
						{
							envir() << "HTTPClientConnection::handleRequestBytes Sending reply to " << itMethod->first.c_str() << " - " << itMethod->second.c_str() << "\n";
							sendResponse(itMethod->second);
						}
						else
						{
							envir() << "HTTPClientConnection::handleRequestBytes could not find method " << itMethod->second.c_str() << " \n";
						}
					}
				}
				if (sRequestBuffer.find("SUBSCRIBE") == 0)
				{
					envir() << "HTTPClientConnection::handleRequestBytes SUBSCRIBE - " << sRequestBuffer.c_str() << " \n";
					sendResponse(std::string("455ed78c-a6be-46d2-9ae1-63e83549deB5"), true);
				}
			}
		}
		void sendResponse(std::string sXmlResponse, bool bEvent = false)
		{
			// Construct our response:
			if (!bEvent)
			{
				sprintf((char*)fResponseBuffer,	HTTPXMLResponse, dateHeader(), sXmlResponse.length());
			}
			else
			{

				sprintf((char*)fResponseBuffer, HTTPEventResponse, dateHeader(), sXmlResponse.c_str());
			}
			ByteStreamMemoryBufferSource* fBuf = ByteStreamMemoryBufferSource::createNew(envir(), (u_int8_t*)fResponseBuffer, strlen((char*)fResponseBuffer));
			ByteStreamMemoryBufferSource* fBuf2 = ByteStreamMemoryBufferSource::createNew(envir(), (u_int8_t*)sXmlResponse.c_str(), sXmlResponse.length());

			TCPStreamSink* fTCPSink = TCPStreamSink::createNew(envir(), fClientOutputSocket);
			fTCPSink->startPlaying(*fBuf, afterStreaming, fTCPSink);
			fTCPSink->stopPlaying();
			//TCPStreamSink::close(fTCPSink);
			fTCPSink = TCPStreamSink::createNew(envir(), fClientOutputSocket);
			fTCPSink->startPlaying(*fBuf2, afterStreaming, fTCPSink);
			fTCPSink->stopPlaying();
		}
	public:
		std::unordered_map<std::string, std::string> mapDirectoryToHTML;
		std::map<soap_service, soap_methods> mapSerivceToMethods;

		HTTPClientConnection(RTSPServer& ourServer, int clientSocket, struct sockaddr_in clientAddr, std::unordered_map<std::string, std::string> _mapDirectoryToHTML, std::map<soap_service, soap_methods>_mapSerivceToMethods) :
			RTSPClientConnection(ourServer, clientSocket, clientAddr)
		{
			mapDirectoryToHTML = _mapDirectoryToHTML;
			mapSerivceToMethods = _mapSerivceToMethods;
		}
		//virtual void handleRequestBytes(int newBytesRead)
		//{

		//}
		virtual ~HTTPClientConnection() {};
		virtual void handleCmd_GET_PARAMETER(char const* fullRequestStr)
		{
		}
		virtual void handleHTTPCmd_StreamingGET(char const* urlSuffix, char const* fullRequestStr)
		{
			envir() << "\r\nhandleHTTPCmd_StreamingGET urlSuffix" << urlSuffix << "\r\n";
			auto entry = mapDirectoryToHTML.find(std::string(urlSuffix));
			if (entry != mapDirectoryToHTML.end())
			{
				sendResponse(entry->second);
			}
			
		}

		

		
	};
private:
	HTTPClientConnection* clientConnection = nullptr;
};