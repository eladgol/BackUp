#pragma once
#pragma region DeviceDiscriptionDirectoryDefinition
#define DeviceDiscriptionDirectory "details.xml"
#define DeviceDiscription "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"\
"<root xmlns=\"urn:schemas-upnp-org:device-1-0\" xmlns:sec=\"http://%s:%d/dlna\" xmlns:dlna=\"urn:schemas-dlna-org:device-1-0\">\r\n" \
"<specVersion>\r\n"\
"<major>1</major>\r\n"\
"<minor>0</minor>\r\n"\
"</specVersion>\r\n"\
"<device>\r\n"\
"<dlna:X_DLNADOC>DMS-1.50</dlna:X_DLNADOC>\r\n"\
"<deviceType>urn:schemas-upnp-org:device:MediaServer:1</deviceType>\r\n"\
"<friendlyName>[Camera]Elad</friendlyName>\r\n"\
"<manufacturer>Elads</manufacturer>\r\n"\
"<manufacturerURL>http://%s:%d/inst</manufacturerURL>\r\n"\
"<modelDescription>EladsCamera</modelDescription>\r\n"\
"<modelName>Elad1</modelName>\r\n"\
"<modelNumber>1.0</modelNumber>\r\n"\
"<modelURL>http://%s:%d/inst</modelURL>\r\n"\
"<serialNumber>S-1-4-05-1405771457-7140577145-140577145-1977</serialNumber>\r\n"\
"<sec:X_ProductCap>a</sec:X_ProductCap>\r\n"\
"<UDN>uuid:%s</UDN>\r\n"\
"<presentationURL>http://%s:d/inst</presentationURL>\r\n"\
"<serviceList>\r\n"\
"<service>\r\n"\
"<serviceType>urn:schemas-upnp-org:service:ContentDirectory:1</serviceType>\r\n"\
"<serviceId>urn:upnp-org:serviceId:ContentDirectory</serviceId>\r\n"\
"<controlURL>/ContentDirectoryControl</controlURL>\r\n"\
"<eventSubURL>/ContentDirectoryEvent</eventSubURL>\r\n"\
"<SCPDURL>/ContentDirectorySCPD</SCPDURL>\r\n"\
"</service>\r\n"\
"<service>\r\n"\
"<serviceType>urn:schemas-upnp-org:service:ConnectionManager:1</serviceType>\r\n"\
"<serviceId>urn:upnp-org:serviceId:ConnectionManager</serviceId>\r\n"\
"<controlURL>/ConnectionManagerControl</controlURL>\r\n"\
"<eventSubURL>/ConnectionManagerEvent</eventSubURL>\r\n"\
"<SCPDURL>/ConnectionManagerSCDP</SCPDURL>\r\n"\
"</service>\r\n"\
"</serviceList>\r\n"\
"<sec:deviceID>\r\n"\
"</sec:deviceID>\r\n"\
"</device>\r\n"\
"</root>"
#pragma endregion DeviceDiscriptionDirectoryDefinition
#pragma region ConnectionManagerDefinition
#define DeviceSCDPDirectory "ConnectionManagerSCDP"
#define DeviceSCDP "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\r\n"\
"<specVersion>\r\n"\
"<major>1</major>\r\n"\
"<minor>0</minor>\r\n"\
"</specVersion>\r\n"\
"<actionList>\r\n"\
"<action>\r\n"\
"<name>GetProtocolInfo</name>\r\n"\
"<argumentList>\r\n"\
"<argument>\r\n"\
"<name>Source</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>SourceProtocolInfo</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>Sink</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>SinkProtocolInfo</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"</argumentList>\r\n"\
"</action>\r\n"\
"<action>\r\n"\
"<name>GetCurrentConnectionIDs</name>\r\n"\
"<argumentList>\r\n"\
"<argument>\r\n"\
"<name>ConnectionIDs</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>CurrentConnectionIDs</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"</argumentList>\r\n"\
"</action>\r\n"\
"<action>\r\n"\
"<name>GetCurrentConnectionInfo</name>\r\n"\
"<argumentList>\r\n"\
"<argument>\r\n"\
"<name>ConnectionID</name>\r\n"\
"<direction>in</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>RcsID</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_RcsID</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>AVTransportID</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_AVTransportID</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>ProtocolInfo</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_ProtocolInfo</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>PeerConnectionManager</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_ConnectionManager</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>PeerConnectionID</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>Direction</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_Direction</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>Status</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_ConnectionStatus</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"</argumentList>\r\n"\
"</action>\r\n"\
"</actionList>\r\n"\
"<serviceStateTable>\r\n"\
"<stateVariable sendEvents=\"yes\">\r\n"\
"<name>SourceProtocolInfo</name>\r\n"\
"<dataType>string</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"yes\">\r\n"\
"<name>SinkProtocolInfo</name>\r\n"\
"<dataType>string</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"yes\">\r\n"\
"<name>CurrentConnectionIDs</name>\r\n"\
"<dataType>string</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>A_ARG_TYPE_ConnectionStatus</name>\r\n"\
"<dataType>string</dataType>\r\n"\
"<allowedValueList>\r\n"\
"<allowedValue>OK</allowedValue>\r\n"\
"<allowedValue>ContentFormatMismatch</allowedValue>\r\n"\
"<allowedValue>InsufficientBandwidth</allowedValue>\r\n"\
"<allowedValue>UnreliableChannel</allowedValue>\r\n"\
"<allowedValue>Unknown</allowedValue>\r\n"\
"</allowedValueList>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>A_ARG_TYPE_ConnectionManager</name>\r\n"\
"<dataType>string</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>A_ARG_TYPE_Direction</name>\r\n"\
"<dataType>string</dataType>\r\n"\
"<allowedValueList>\r\n"\
"<allowedValue>Input</allowedValue>\r\n"\
"<allowedValue>Output</allowedValue>\r\n"\
"</allowedValueList>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>A_ARG_TYPE_ProtocolInfo</name>\r\n"\
"<dataType>string</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>A_ARG_TYPE_ConnectionID</name>\r\n"\
"<dataType>i4</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>A_ARG_TYPE_AVTransportID</name>\r\n"\
"<dataType>i4</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>A_ARG_TYPE_RcsID</name>\r\n"\
"<dataType>i4</dataType>\r\n"\
"</stateVariable>\r\n"\
"</serviceStateTable>\r\n"\
"</scpd>"
#pragma endregion ConnectionManagerDefinition
#pragma region ContentDirectoryDefinitons
#define ContentDirectorySCDPDirectory "ContentDirectorySCPD"
#define ContentDirectorySCDP "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\r\n"\
"<specVersion>\r\n"\
"<major>1</major>\r\n"\
"<minor>0</minor>\r\n"\
"</specVersion>\r\n"\
"<actionList>\r\n"\
"<action>\r\n"\
"<name>GetSearchCapabilities</name>\r\n"\
"<argumentList>\r\n"\
"<argument>\r\n"\
"<name>SearchCaps</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>SearchCapabilities</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"</argumentList>\r\n"\
"</action>\r\n"\
"<action>\r\n"\
"<name>GetSortCapabilities</name>\r\n"\
"<argumentList>\r\n"\
"<argument>\r\n"\
"<name>SortCaps</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>SortCapabilities</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"</argumentList>\r\n"\
"</action>\r\n"\
"<action>\r\n"\
"<name>GetSystemUpdateID</name>\r\n"\
"<argumentList>\r\n"\
"<argument>\r\n"\
"<name>Id</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>SystemUpdateID</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"</argumentList>\r\n"\
"</action>\r\n"\
"<action>\r\n"\
"<name>Browse</name>\r\n"\
"<argumentList>\r\n"\
"<argument>\r\n"\
"<name>ObjectID</name>\r\n"\
"<direction>in</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>BrowseFlag</name>\r\n"\
"<direction>in</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_BrowseFlag</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>Filter</name>\r\n"\
"<direction>in</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_Filter</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>StartingIndex</name>\r\n"\
"<direction>in</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_Index</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>RequestedCount</name>\r\n"\
"<direction>in</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>SortCriteria</name>\r\n"\
"<direction>in</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_SortCriteria</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>Result</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>NumberReturned</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>TotalMatches</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>UpdateID</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_UpdateID</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"</argumentList>\r\n"\
"</action>\r\n"\
"<action>\r\n"\
"<name>Search</name>\r\n"\
"<argumentList>\r\n"\
"<argument>\r\n"\
"<name>ContainerID</name>\r\n"\
"<direction>in</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>SearchCriteria</name>\r\n"\
"<direction>in</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_SearchCriteria</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>Filter</name>\r\n"\
"<direction>in</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_Filter</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>StartingIndex</name>\r\n"\
"<direction>in</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_Index</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>RequestedCount</name>\r\n"\
"<direction>in</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>SortCriteria</name>\r\n"\
"<direction>in</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_SortCriteria</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>Result</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>NumberReturned</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>TotalMatches</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"<argument>\r\n"\
"<name>UpdateID</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>A_ARG_TYPE_UpdateID</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"</argumentList>\r\n"\
"</action>\r\n"\
"<action>\r\n"\
"<name>X_GetRemoteSharingStatus</name>\r\n"\
"<argumentList>\r\n"\
"<argument>\r\n"\
"<name>Status</name>\r\n"\
"<direction>out</direction>\r\n"\
"<relatedStateVariable>X_RemoteSharingEnabled</relatedStateVariable>\r\n"\
"</argument>\r\n"\
"</argumentList>\r\n"\
"</action>\r\n"\
"</actionList>\r\n"\
"<serviceStateTable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>A_ARG_TYPE_ObjectID</name>\r\n"\
"<dataType>string</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>A_ARG_TYPE_Result</name>\r\n"\
"<dataType>string</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>A_ARG_TYPE_SearchCriteria</name>\r\n"\
"<dataType>string</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>A_ARG_TYPE_BrowseFlag</name>\r\n"\
"<dataType>string</dataType>\r\n"\
"<allowedValueList>\r\n"\
"<allowedValue>BrowseMetadata</allowedValue>\r\n"\
"<allowedValue>BrowseDirectChildren</allowedValue>\r\n"\
"</allowedValueList>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>A_ARG_TYPE_Filter</name>\r\n"\
"<dataType>string</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>A_ARG_TYPE_SortCriteria</name>\r\n"\
"<dataType>string</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>A_ARG_TYPE_Index</name>\r\n"\
"<dataType>ui4</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>A_ARG_TYPE_Count</name>\r\n"\
"<dataType>ui4</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>A_ARG_TYPE_UpdateID</name>\r\n"\
"<dataType>ui4</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>SearchCapabilities</name>\r\n"\
"<dataType>string</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"no\">\r\n"\
"<name>SortCapabilities</name>\r\n"\
"<dataType>string</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"yes\">\r\n"\
"<name>SystemUpdateID</name>\r\n"\
"<dataType>ui4</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"yes\">\r\n"\
"<name>ContainerUpdateIDs</name>\r\n"\
"<dataType>string</dataType>\r\n"\
"</stateVariable>\r\n"\
"<stateVariable sendEvents=\"yes\">\r\n"\
"<name>X_RemoteSharingEnabled</name>\r\n"\
"<dataType>boolean</dataType>\r\n"\
"</stateVariable>\r\n"\
"</serviceStateTable>\r\n"\
"</scpd>"
#pragma endregion ContentDirectoryDefinitons
#pragma region GetProtocolDefinitons
#define GetProtocolDirectory "ContentDirectoryControl"
#define GetProtocolXML "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"\
"<s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\">\r\n"\
"<s:Body>\r\n"\
"<u:GetProtocolInfo xmlns:u=\"urn:schemas-upnp-org:service:ConnectionManager:1\"/>\r\n"\
"</s:Body>\r\n"\
"</s:Envelope>"
#define GetProtocolReplyXML "<?xml version=\"1.0\" encoding=\"utf-8\" ?>"\
"<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
"	<s:Body>"\
"		<u:GetProtocolInfoResponse xmlns:u=\"urn:schemas-upnp-org:service:ConnectionManager:1\">"\
"			<Source>rtsp-rtp-udp:*:video/H264</Source>"\
"			<Sink></Sink>"\
"		</u:GetProtocolInfoResponse>"\
"	</s:Body>"\
"</s:Envelope>"
#pragma endregion GetProtocolDefinitons 
#pragma region GetCurrentConnectionIDsResponseDefinitons
#define GetCurrentConnectionIDsResponse "<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
"  <s:Body>"\
"		<u:GetCurrentConnectionIDsResponse xmlns:u=\"urn:schemas-upnp-org:service:ConnectionManager:1\">"\
"			<ConnectionIDs>0</ConnectionIDs>"\
"		</u:GetCurrentConnectionIDsResponse>"\
"	</s:Body>"\
"</s:Envelope>"
#pragma endregion GetCurrentConnectionIResponseDefinitons
#define GetCurrentConnectionInfoResponse "<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
"  <s:Body>"\
"		<u:GetCurrentConnectionInfoResponse xmlns:u=\"urn:schemas-upnp-org:service:ConnectionManager:1\">"\
"			<RcsID>0</RcsID>"\
"			<AVTransportID>-1</RcsID>"\
"			<ProtocolInfo>rtsp-rtp-udp:*:video/H264</ProtocolInfo>"\
"			<PeerConnectionManager>0</PeerConnectionManager>"\
"			<PeerConnectionID>-1</PeerConnectionID>"\
"			<Direction>Output</Direction>"\
"			<Status>OK</Status>"\
"		</u:GetCurrentConnectionInfoResponse>"\
"	</s:Body>"\
"</s:Envelope>"