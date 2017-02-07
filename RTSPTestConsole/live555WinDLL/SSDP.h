#pragma once



static const char myGUID[] = { "3d64febc-ae6a-4584-853a-85368ca80800" };
static const char myGUIDWithUUID[] = { "uuid:3d64febc-ae6a-4584-853a-85368ca80800" };
//static const char DescriptionXML[] = { "<?xml version = ""1.0""?><root xmlns = ""urn:schemas-upnp-org:device-1-0""><specVersion><major>1</major><minor>0</minor></specVersion><URLBase>http://www.google.com</URLBase><device><deviceType>urn:schemas-upnp-org:device:MediaServer:1</deviceType><friendlyName>EladsAndroidVirtualCamera</friendlyName><manufacturer>Elad</manufacturer><manufacturerURL>www.Google.Com</manufacturerURL><modelDescription>EladsAndroidVirtualCamera</modelDescription><modelName>1</modelName><modelNumber>1</modelNumber><modelURL>www.Google.Com</modelURL><serialNumber>1</serialNumber><UDN>uuid:3D64FEBC-AE6A-4584-853A-85368CA80800</UDN><UPC>1</UPC><presentationURL>www.google.com</presentationURL><iconList></iconList><serviceList></serviceList></device></root>" };
static const char DescriptionXML[] = { "<root><specVersion><major>1</major><minor>0</minor></specVersion><URLBase>http://www.google.com</URLBase><device><deviceType>urn:schemas-upnp-org:device:MediaServer:1</deviceType><friendlyName>EladsAndroidVirtualCamera</friendlyName><manufacturer>Elad</manufacturer><manufacturerURL>www.Google.Com</manufacturerURL><modelDescription>EladsAndroidVirtualCamera</modelDescription><modelName>1</modelName><modelNumber>1</modelNumber><modelURL>www.Google.Com</modelURL><serialNumber>1</serialNumber><UDN>uuid:3D64FEBC-AE6A-4584-853A-85368CA80800</UDN><UPC>1</UPC><presentationURL>www.google.com</presentationURL><iconList></iconList><serviceList></serviceList></device></root>" };
#include <mutex>
#include <condition_variable>
#include <thread>
#include <Groupsock.hh>
#include <BasicUsageEnvironment.hh>
#include "HTTPSmallServer.h"
#include "DeviceDiscription.h"
#include <map>
class SSDPServer
{
public:
	SSDPServer(int rtspPort, int detailsPort);
	virtual ~SSDPServer();
	int a;
private:
	bool bContinue = true;
	bool bSending = false;
	std::mutex m;
	std::condition_variable cv;
	std::thread tRunSSDPAlive;
	std::thread tRunHTTPSServer;
	std::thread tReplySSDPMSearch;
	unsigned char fRequestBuffer[2048];
	unsigned char fRequestBuffer1[2048];
	unsigned char fRequestBuffer2[2048];
	int sizeofRequestBuffer;
	int sizeofRequestBuffer1;
	int sizeofRequestBuffer2;
	Groupsock* fOutSocket;
	BasicUsageEnvironment* env;
	HTTPSmallServer* smallServer;
	Groupsock* fHTTPOutSocket;
//	std::map<std::string, std::string> mapURLToContent;
	unsigned char bufferHTTP[2048];
	void RunHTTPServer(char sIP[16], int detailsPort);
	void RunSSDPMSearchReply(int detailsPort);
	void SendSSDPAliveMessages(char sIP[16], int detailsPort);
	char sIP[16] = "";
};

class SSDPClient
{
public:
	SSDPClient();
	virtual ~SSDPClient();
	int GetIPAndPort(std::string& IP, int& port) { IP = _IP; port = _port; return 0; };
private:
	//HTTPSmallClient httpSmallClient;
	std::string _IP;
	int _port = 0;
	Groupsock* fHTTPOutSocket;
	char bufferHTTP[2048];
};