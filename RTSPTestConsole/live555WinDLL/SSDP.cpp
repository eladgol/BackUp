
#include "SSDP.h"
#include <stdio.h>
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include "HTTPServer.h"
#include <algorithm>
#include <cctype>
#include <map>
#include <stack>
#define SSDP_ADDR "239.255.255.250"
#define SSDP_PORT 1900
#define SSDP_PORT_STR "1900"

#define SSDP_DISCOVERY_REQUEST                      \
        "M-SEARCH * HTTP/1.1\r\n"                   \
        "Host: " SSDP_ADDR ":" SSDP_PORT_STR "\r\n" \
        "Man: \"ssdp:discover\"\r\n"                \
        "ST: %s\r\n"                                \
        "MX: %d\r\n"                                \
        "User-Agent: %s GSSDP/" VERSION "\r\n\r\n"  \

#define SSDP_DISCOVERY_RESPONSE                     \
        "HTTP/1.1 200 OK\r\n"                       \
		"CACHE-CONTROL: max-age=%d\r\n"             \
		"DATE: %s\r\n"								\
        "EXT:\r\n"                                  \
		"LOCATION: %s\r\n"                          \
        "SERVER: %s\r\n"                            \
        "ST: %s\r\n"                   \
		"USN: uuid:%s%s\r\n"						\
        "Content-Length: 0\r\n\r\n"

#define SSDP_ALIVE_MESSAGE                          \
        "NOTIFY * HTTP/1.1\r\n"                     \
        "Host: " SSDP_ADDR ":" SSDP_PORT_STR "\r\n" \
        "Cache-Control: max-age=%d\r\n"             \
        "Location: %s\r\n"                          \
        "Server: %s\r\n"                            \
        "NTS: ssdp:alive\r\n"                       \
        "NT: %s\r\n"                                \
        "USN: uuid:%s:%s\r\n\r\n"

#define SSDP_BYEBYE_MESSAGE                         \
        "NOTIFY * HTTP/1.1\r\n"                     \
        "Host: " SSDP_ADDR ":" SSDP_PORT_STR "\r\n" \
        "NTS: ssdp:byebye\r\n"                     \
        "NT: %s\r\n"                                \
        "USN: %s\r\n\r\n"

#define SSDP_SEARCH_METHOD "M-SEARCH"
#define GENA_NOTIFY_METHOD "NOTIFY"

#define SSDP_ALIVE_NTS  "ssdp:alive"
#define SSDP_BYEBYE_NTS "ssdp:byebye"

#define SSDP_DEFAULT_MAX_AGE 1810
#define SSDP_DEFAULT_MX      3
void GetOurIPAddress(char sIP[16], BasicUsageEnvironment* env)
{
	struct in_addr destinationAddress;
	destinationAddress.s_addr = inet_addr(SSDP_ADDR);
	auto addr = ourIPAddress(*env);
	netAddressBits addrNBO = htonl(addr); // make sure we have a value in a known byte order: big endian
	sprintf(sIP, "%u.%u.%u.%u", (addrNBO >> 24) & 0xFF, (addrNBO >> 16) & 0xFF, (addrNBO >> 8) & 0xFF, addrNBO & 0xFF);
}

SSDPServer::SSDPServer(int rtspPort, int detailsPort)
{
	// Begin by setting up our usage environment:
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);
	GetOurIPAddress(sIP, env);	
	SendSSDPAliveMessages(sIP, detailsPort);
	RunHTTPServer(sIP, detailsPort);
	RunSSDPMSearchReply(detailsPort);
}

void SSDPServer::SendSSDPAliveMessages(char _sIP[16], int detailsPort)
{
	char location[1024];
	sprintf(location, "http://%s:%d/%s", sIP, detailsPort, DeviceDiscriptionDirectory);
	sizeofRequestBuffer = sprintf((char*)fRequestBuffer, SSDP_ALIVE_MESSAGE, SSDP_DEFAULT_MAX_AGE,
		(char *)location,
		"Android/10.0 UPnP/1.0 UPnP-Device-Host/1.0", myGUIDWithUUID,
		myGUID, "");
	sizeofRequestBuffer1 = sprintf((char*)fRequestBuffer1, SSDP_ALIVE_MESSAGE, SSDP_DEFAULT_MAX_AGE,
		(char *)location,
		"Android/10.0 UPnP/1.0 UPnP-Device-Host/1.0", "upnp:rootdevice",
		myGUID, ":upnp:rootdevice");

	sizeofRequestBuffer2 = sprintf((char*)fRequestBuffer2, SSDP_ALIVE_MESSAGE, SSDP_DEFAULT_MAX_AGE,
		(char *)location,
		"Android/10.0 UPnP/1.0 UPnP-Device-Host/1.0", "urn:schemas-upnp-org:device:MediaServer:1",
		myGUID, ":urn:schemas-upnp-org:device:MediaServer:1");

	//smallServer = new HTTPSmallServer(sIP, detailsPort);
	//char details[2048];
	////sprintf(details, DescriptionXML);
	//sprintf(details, DeviceDiscription, sIP, detailsPort, sIP, detailsPort, sIP, detailsPort);
	////		<VirtualCameraDiscription><presentationURL>www.google.com</presentationURL><IP>%s</IP><Port>%d</Port>" \
		////		"</VirtualCameraDiscription>", sIP, rtspPort);

//smallServer->BindHTMLToDirectory("/details.xml", details);
//tRunHTTPSServer = std::thread([&]()
//{
//	smallServer->RunServer();
//});
//tRunHTTPSServer.detach();

	struct in_addr destinationAddress;
	destinationAddress.s_addr = inet_addr(sIP);
	fOutSocket = new Groupsock(*env, destinationAddress, SSDP_PORT, 255);
	tRunSSDPAlive = std::thread([&]()
	{

		while (bContinue)
		{
			bSending = true;
			auto success = fOutSocket->output(*env, fOutSocket->ttl(), fRequestBuffer, sizeofRequestBuffer);
			success = fOutSocket->output(*env, fOutSocket->ttl(), fRequestBuffer1, sizeofRequestBuffer1);
			success = fOutSocket->output(*env, fOutSocket->ttl(), fRequestBuffer2, sizeofRequestBuffer2);
			bSending = false;
			cv.notify_all();
			//fOutSocket->handleRead()
			std::this_thread::sleep_for(std::chrono::seconds((int)15));


		}
		//std::unique_lock<std::mutex> lk(lock);
		//cv.wait(lk);
	});
	tRunSSDPAlive.detach();
}
void SSDPServer::RunSSDPMSearchReply(int detailsPort)
{
	tReplySSDPMSearch = std::thread([&, detailsPort]()
	{
		std::string sResponse;
		std::string sResponseUpperCase;
		unsigned char fResponseBuffer[65536];
		struct in_addr destinationAddress;
		destinationAddress.s_addr = inet_addr(SSDP_ADDR);
		Groupsock* fOutSocket = new Groupsock(*env, destinationAddress, SSDP_PORT, 255);
		unsigned int bytesRead = 0;
		sockaddr_in fromAddr;
		*env << "SSDPServer::RunSSDPMSearchReply START\n";
		while (bContinue)
		{
			bool bSuccess = fOutSocket->handleRead(fResponseBuffer, 2048, bytesRead, fromAddr);
			*env << "SSDPServer::RunSSDPMSearchReply bSuccess " << (bSuccess ? "TRUE" : "FALSE") << " bytes read " << bytesRead << "\n";
			if (bSuccess && bytesRead > 0)
			{
				
				sResponse = ((char*)fResponseBuffer);
				sResponseUpperCase = sResponse;
				std::transform(sResponseUpperCase.begin(), sResponseUpperCase.end(), sResponseUpperCase.begin(), [](unsigned char c) { return std::toupper(c); });
				if (sResponseUpperCase.find("M-SEARCH") != -1)
				{
					*env << "SSDPServer::RunSSDPMSearchReply Encountered M-SEARCH\n";
					time_t now = time(0);   // get time now
					struct tm  tstruct;
					char       dateBuf[80];
					tstruct = *localtime(&now);
					// DATE:  Thu, 26 Jan 2017 23:09:07 GMT
					strftime(dateBuf, sizeof(dateBuf), "%a, %e %b %Y %T GMT", &tstruct);
					*env << "SSDPServer::RunSSDPMSearchReply M-SEARCH 2 \n";
					*env << "SSDPServer::RunSSDPMSearchReply M-SEARCH 2 \n";
					char discoveryReply[2048];
					auto addr = ourIPAddress(*env);
					char sIP[16];
					char location[1024];
					netAddressBits addrNBO = htonl(addr); // make sure we have a value in a known byte order: big endian
					sprintf(sIP, "%u.%u.%u.%u", (addrNBO >> 24) & 0xFF, (addrNBO >> 16) & 0xFF, (addrNBO >> 8) & 0xFF, addrNBO & 0xFF);
					sprintf(location, "http://%s:%d/%s", sIP, detailsPort, DeviceDiscriptionDirectory);
					unsigned short port = ntohs(fromAddr.sin_port);
					Groupsock fReplySocket(*env, fromAddr.sin_addr, 53359, 255);
					fReplySocket.changeDestinationParameters(fromAddr.sin_addr, Port(port), fReplySocket.ttl());
					int sizeofReply;
					
					//sizeofReply = sprintf((char*)discoveryReply, SSDP_DISCOVERY_RESPONSE, 
					//	SSDP_DEFAULT_MAX_AGE,
					//	dateBuf,
					//	(char *)location,
					//	"Linux/2.x.x, UPnP/1.0, pvConnect UPnP SDK/1.0, TwonkyMedia UPnP SDK/1.1",
					//	myGUIDWithUUID,
					//	myGUID,
					//	"");
					//bool bSuccess = fReplySocket.output(*env, fReplySocket.ttl(), (unsigned char*)discoveryReply, sizeofReply);
					//*env << "SSDPServer::RunSSDPMSearchReply M-SEARCH Sending " << (bSuccess ? "OK " : "Failed ") << discoveryReply << "\n";

					sizeofReply = sprintf((char*)discoveryReply, SSDP_DISCOVERY_RESPONSE,
						SSDP_DEFAULT_MAX_AGE,
						dateBuf,
						(char *)location,
						"Linux/2.x.x, UPnP/1.0, pvConnect UPnP SDK/1.0, TwonkyMedia UPnP SDK/1.1",
						"upnp:rootdevice",
						myGUID,
						"::upnp:rootdevice");
					for (int i = 0; i < 10; i++)
					{
						bSuccess = fReplySocket.output(*env, fReplySocket.ttl(), (unsigned char*)discoveryReply, sizeofReply);
						std::this_thread::sleep_for(std::chrono::milliseconds((int)100));
						*env << "SSDPServer::RunSSDPMSearchReply M-SEARCH Sending " << (bSuccess ? "OK " : "Failed ") << discoveryReply << "\n";
					}
				//	sizeofReply = sprintf((char*)discoveryReply, SSDP_DISCOVERY_RESPONSE,
				//		SSDP_DEFAULT_MAX_AGE,
				//		dateBuf,
				//		(char *)location,
				//		"Linux/2.x.x, UPnP/1.0, pvConnect UPnP SDK/1.0, TwonkyMedia UPnP SDK/1.1",
				//		"urn:schemas-upnp-org:device:MediaServer:1",
				//		myGUID,
				//		":urn:schemas-upnp-org:device:MediaServer:1");
				//	bSuccess = fReplySocket.output(*env, fReplySocket.ttl(), (unsigned char*)discoveryReply, sizeofReply);
					*env << "SSDPServer::RunSSDPMSearchReply M-SEARCH Sending " << (bSuccess ? "OK " : "Failed ") << discoveryReply << "\n";

				}
			}
		}
	});
	tReplySSDPMSearch.detach();
}
void SSDPServer::RunHTTPServer(char _sIP[16], int detailsPort)
{
	// init server
//	std::string sContent(csContent);
//	mapURLToContent.insert(make_pair(std::string(DeviceDiscriptionDirectory), sContent));
	int rtspPort = -1;
	std::copy(_sIP, &_sIP[16], sIP);
	tRunHTTPSServer = std::thread([&, detailsPort]()
	{
		HTTPServer* rtspServerHTTP = HTTPServer::createNew(*env, 8080, NULL);
		rtspServerHTTP->setUpTunnelingOverHTTP(Port(detailsPort));
		
		char csContent[65536];
		sprintf(csContent, DeviceDiscription, sIP, detailsPort, sIP, detailsPort, sIP, detailsPort, myGUID, sIP, detailsPort);
		rtspServerHTTP->CreatePage(DeviceDiscriptionDirectory, csContent);
		sprintf(csContent, DeviceSCDP);
		rtspServerHTTP->CreatePage(DeviceSCDPDirectory, csContent);
		sprintf(csContent, ContentDirectorySCDP);
		rtspServerHTTP->CreatePage(ContentDirectorySCDPDirectory, csContent);
		const char* ConnectionManagerMethods[] = { "GetCurrentConnectionIDs", "GetCurrentConnectionInfo", "GetProtocolInfo" };
		const char* ConnectionManagerMethodsReply[] = { GetCurrentConnectionIDsResponse, GetCurrentConnectionInfoResponse, GetProtocolReplyXML };
		rtspServerHTTP->CreateSOAPMethod("ConnectionManager", ConnectionManagerMethods, ConnectionManagerMethodsReply, 3);
		//const char* ContentDirectoryServices[] = { "Browse", "GetSearchCapabilities", "GetSortCapabilities", "GetSystemUpdate", "Search", "X_GetRemoteSharingStatus" };
		//rtspServerHTTP->CreateSOAPMethod("ContentDirectory", ContentDirectoryServices, 6);

		env->taskScheduler().doEventLoop(); // does not return
	//}

	//in_addr ourIPAddr;
	//ourIPAddr.s_addr = inet_addr(sIP);
	//fHTTPOutSocket = new Groupsock(*env, ourIPAddr, detailsPort, 255);
	//unsigned int bytesRead;
	//sockaddr_in fromAddress;
	//tRunHTTPSServer = std::thread([&, detailsPort]()
	//{
	//	int sock = setupDatagramSocket(*env, detailsPort);
	//	//if (sock < 0) break;
	//	TCPStreamSink* fTCPSink = TCPStreamSink::createNew(*env, sock);
	//	char* buf = new char[2048];
	//	auto fSource = ByteStreamMemoryBufferSource::createNew(*env, (u_int8_t*)buf, 2048);

	//	fTCPSink->startPlaying(*fSource, afterStreaming, fTCPSink);
	//	
		//int fHTTPServerSocket = setUpOurSocket(envir(), detailsPort);



		//while (bContinue)
		//{
		//	fHTTPOutSocket->handleRead(bufferHTTP, 2048, bytesRead, fromAddress);
		//	std::string sHTTP((char*)bufferHTTP);
		//	std::string sHTTPRequest((char*)bufferHTTP);
		//	int iSpace = sHTTP.find_first_of(" ");

		//	if (iSpace == -1)
		//	{
		//		//error
		//	}
		//	else
		//	{
		//		sHTTPRequest = sHTTPRequest.substr(0, iSpace);
		//		if (sHTTPRequest != "GET")
		//		{
		//			// error
		//		}
		//		else
		//		{
		//			sHTTP = sHTTP.substr(iSpace + 1);
		//			int iSecondSpace = sHTTP.find_first_of(" ");
		//			std::string sURL = sHTTP.substr(0, iSecondSpace);
		//			std::string sResponse = "";
		//			auto sContent = mapURLToContent.find(sURL);
		//			if (sContent != mapURLToContent.end())
		//			{
		//				char csHeaders[2048];
		//				sprintf(csHeaders, "HTTP/1.0 200 OK\nContent-Type:text/html\nContentLength:%d", sContent->second.length());
		//				sResponse = sResponse + std::string(csHeaders);
		//			}
		//			else
		//			{
		//				sResponse = "HTTP/1.0 404 Not Found\n";
		//			}
		//			fHTTPOutSocket->output(*env, fHTTPOutSocket->ttl(), (unsigned char*)sResponse.c_str(), sResponse.length());

		//		}
		//	}
		//}
	});
	tRunHTTPSServer.detach();
}

SSDPServer::~SSDPServer()
{
	bContinue = false;
	std::unique_lock<std::mutex> lk(m);
	cv.wait(lk, [&] {return !bSending; });
	tRunSSDPAlive.join();
}
typedef std::string sParentType;
typedef std::string sValType;
typedef std::string sFieldType;
typedef std::pair<sValType, sParentType> xmlElement;
typedef std::map<sFieldType, xmlElement> xmlMapType;
int ParseXML(std::string sDetailsXMLUpperCase, std::shared_ptr<xmlMapType>& pXmlMap)
{
	int err = 0;
	pXmlMap = std::make_shared<xmlMapType>(xmlMapType()); // element and parent
	int iStartXMLElement = 0;
	int iEndXMLElement = 0;
	int iStartNextXMLElement = 0;
	int iEndNextXMLElement = 0;
	
	std::stack<std::string> parentsElements;
	while (iStartXMLElement < sDetailsXMLUpperCase.length())
	{
		iStartXMLElement = sDetailsXMLUpperCase.find_first_of("<");

		if (iStartXMLElement == -1)
		{
			iStartXMLElement = sDetailsXMLUpperCase.length();
			break;
		}

		iEndXMLElement = sDetailsXMLUpperCase.find_first_of(">");
		if (iEndXMLElement == -1)
		{
			iStartXMLElement = sDetailsXMLUpperCase.length();
			break;
		}

		std::string sField = sDetailsXMLUpperCase.substr(iStartXMLElement + 1, iEndXMLElement - iStartXMLElement - 1);
		std::string sParent = (parentsElements.size() > 0) ? parentsElements.top() : "";
		parentsElements.push(sField);

		sDetailsXMLUpperCase = sDetailsXMLUpperCase.substr(iEndXMLElement + 1);

		iStartNextXMLElement = sDetailsXMLUpperCase.find_first_of("<");
		if (iStartNextXMLElement == -1)
		{
			iStartXMLElement = sDetailsXMLUpperCase.length();
			break;
		}
		std::string sVal = sDetailsXMLUpperCase.substr(0, iStartNextXMLElement);
		iEndNextXMLElement = sDetailsXMLUpperCase.find_first_of(">");

		pXmlMap->insert(std::make_pair(sField, std::make_pair(sVal, sParent)));

		std::string sNextField = sDetailsXMLUpperCase.substr(iStartNextXMLElement + 1, iEndNextXMLElement - iStartNextXMLElement - 1);
		int iClosingField = sNextField.find_first_of("//");
		if (iClosingField != -1)
		{
			std::string sEndField = parentsElements.top();
			sNextField = sNextField.substr(1);
			parentsElements.pop();
			if (sEndField != sNextField)
			{
				// not good
				err = -1;
			}	
			iEndNextXMLElement++;
		}
		else
		{
			iEndNextXMLElement = iStartNextXMLElement;
		}	
		sDetailsXMLUpperCase = sDetailsXMLUpperCase.substr(iEndNextXMLElement);
		iStartXMLElement = 0;
	}
	return err;
}
SSDPClient::SSDPClient()
{
	// Begin by setting up our usage environment:
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	auto env = BasicUsageEnvironment::createNew(*scheduler);
	unsigned char fResponseBuffer[2048];
	struct in_addr destinationAddress;
	destinationAddress.s_addr = inet_addr(SSDP_ADDR);

	Groupsock* fOutSocket = new Groupsock(*env, destinationAddress, SSDP_PORT, 255);
	unsigned int bytesRead;
	struct sockaddr_in fromAddr;

	bool bFoundDevice = false;
	int iLocation = 0;
	std::string sResponse;
	std::string sResponseUpperCase;
	while (!bFoundDevice)
	{
		fOutSocket->handleRead(fResponseBuffer, 2048, bytesRead, fromAddr);
		sResponse = ((char*)fResponseBuffer);
		sResponseUpperCase = sResponse;
		std::transform(sResponseUpperCase.begin(), sResponseUpperCase.end(), sResponseUpperCase.begin(), [](unsigned char c) { return std::toupper(c); });
		int iDevice = sResponseUpperCase.find(myGUID, 0);
		if (iDevice == -1)
		{
			bFoundDevice = false;
		}
		else
		{
			bFoundDevice = true;
			iLocation = sResponseUpperCase.find("LOCATION:"/* HTTP:////"*/, 0);
			if (iLocation == -1)
			{
				*env << "Can't find location in response\n";
				return;
			}
		}
		
	}
	sResponse = sResponse.substr(iLocation);
	iLocation = sResponse.find("http://");
	iLocation += 7; // jump over http://
	sResponse = sResponse.substr(iLocation);
	
	
	auto xEnd = sResponse.find_first_of(" \r\n");
	if (xEnd == -1)
	{
		*env << "Can't find location end in location in response\n";
		return;
	}
	sResponse = sResponse.substr(0, xEnd);
	auto ipEnd = sResponse.find_first_of("\\ :/");
	if (ipEnd == -1)
	{
		*env << "Can't find IP end in location in response\n";
		return;
	}
	std::string sIP = sResponse.substr(0, ipEnd);
	sResponse = sResponse.substr(ipEnd + 1);
	auto portEnd = sResponse.find_first_of("\\ /");
	if (portEnd == -1)
	{
		*env << "Can't find port end in location in response\n";
		return;
	}
	std::string sPort = sResponse.substr(0, portEnd);
	char* pEnd;
	int port = strtol(sPort.c_str(), &pEnd, 10);
	if (port <= 0)
	{
		*env << "port is invalid in location in response\n";
		return;
	}
	sResponse = sResponse.substr(portEnd);
	std::string sDirectory = sResponse;
	
	/// need to access http://sIP:sPort/sDirectory using a HTTPSimpleClient
	
	char cStrIP[16];
	strcpy(cStrIP, sIP.c_str());
	char cDirectory[2048];
	strcpy(cDirectory, sDirectory.c_str());
	//httpSmallClient.SendRequest(cStrIP, port, cDirectory);
	//int szResponse = 0;
	//httpSmallClient.GetResponse(&((char*)fResponseBuffer)[0], 2048, &szResponse);

	in_addr ourIPAddr;
	ourIPAddr.s_addr = inet_addr(cStrIP);
	fHTTPOutSocket = new Groupsock(*env, ourIPAddr, port, 255);
	sprintf(bufferHTTP, "GET http://%s:%d%s HTTP/1.0", cStrIP, port, cDirectory);
	auto szBuffer = strlen(bufferHTTP);
	fHTTPOutSocket->output(*env, 255, (unsigned char*)bufferHTTP, szBuffer);



	//std::string sDetailsXML(&((char*)fResponseBuffer)[0]);
	//std::string sDetailsXMLUpperCase = sDetailsXML;
	//std::transform(sDetailsXMLUpperCase.begin(), sDetailsXMLUpperCase.end(), sDetailsXMLUpperCase.begin(), [](unsigned char c) { return std::toupper(c); });
	//std::shared_ptr<xmlMapType> pXml;
	//int err = ParseXML(sDetailsXMLUpperCase, pXml);
	//if (err != 0)
	//{
	//	*env << "XML is invalid " << sDetailsXMLUpperCase.c_str() << "\n";
	//}
	//auto IPForRTSP = (*pXml).find(std::string("IP"));
	//if (IPForRTSP != (*pXml).end())
	//{
	//	_IP = IPForRTSP->second.first;
	//}
	//auto PortForRTSP = (*pXml).find(std::string("PORT"));
	//if (PortForRTSP != (*pXml).end())
	//{
	//	char* pEnd;
	//	_port = strtol(PortForRTSP->second.first.c_str(), &pEnd, 10);
	//}
	_port = 8080;
	_IP = "192.168.1.28";
}

SSDPClient::~SSDPClient()
{

}
