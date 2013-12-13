/**
 * axxel_discovery_protokoll.cpp
 * Project: Axxel Discovery Protocol
 * Created on: 2012-8-4
 * Author: Li, Yuan
 */

#include "axxel_discovery_protokoll.h"

/**
 *@var iDelayTime
 *@brief initializes Value of delay time '15s'
 *@brief that means the client side send the datum every 15 seconds
 */
int iDelayTime = 1;

/**
 *@var bShowInformation
 *@brief initializes value of bShowInformation 'false'
 *@brief that means nothing will be displayed by starting of the Program
 */
bool bShowInformation = false;

/**
 *@var bWITHIPV6
 *@brief the Switch of IPv6
 *@brief bWITHIPV6 = true means, running with IPv6
 *@brief bWITHIPV6 = false means, running without IPv6
 *@brief initializes value of bWITHIPV6 'false'
 */
bool bWITHIPV6 = false;

/**
 *@var bWITHIPV6ONLY
 *@brief only use IPv6
 *@brief bWITHIPV6 = true means, running only with IPv6
 *@brief bWITHIPV6 = false means, running with IPv6 and IPv4
 *@brief initializes value of bWITHIPV6ONLY 'false'
 */
bool bWITHIPV6ONLY=false;


/**
 *@var sINTERFACE
 *@brief choose the INTERFACE, which will be used
 *@brief default use the DefaultInterface
 */
std::string sINTERFACE = getDefaultInterface();

/**
 *@var iLostTime
 *@brief the records which more than iLostTime hours can not be connected, will be deleted
 */
int iLostTime = 3;

/**
 *@var iServerSidePortIPv4
 *@brief Server Port by ipv4
 */
int iServerSidePortIPv4 = 5000;

/**
 *@var iServerSidePortIPv6
 *@brief Server Port by ipv6
 */
int iServerSidePortIPv6 = 8000;

/**
 *@var sExternIPAddress
 *@brief extern IP address
 */
std::string sExternIPAddress = "";

/**
 *@var sExternPort
 *@brief extern port
 */
std::string sExternPort = "";

/**
 *@fn main
 *@brief choose the method to get into the ControlCenter
 */
int main(int argc, char **argv)
{

	// argc == 2 means there is parameters following ./adp
	if (argc == 2)
	{
		// check which parameter is be used
		ControlCenter ControlCenter(false);
		ControlCenter.checkParameter(argc, argv);
	}

	// Parameter Fault!
	else if (argc > 2)
	{
		std::cerr << "Parameter Fault!" << std::endl;
		ControlCenter ControlCenter(false);
		ControlCenter.showHelp();
		exit(1);
	}

	// no parameter
	// normal starting
	else
	{
		// start Daemon
		try
		{
			daemon(1, 0);
		} catch (...)
		{
			Exception Exception(1, "Daemon Start Error! " + getTime());
			Exception.dispose();
		}
		// start the Programm
		ControlCenter ControlCenter(true);
		ControlCenter.beginControl();
	}
	return 0;
}

/**
 *@fn adp_List__tauschen
 *@brief this function is be created by GSOAP
 *@brief it is used for receiving the informations, which sent by client sides
 *@param *soap [pointer of soap, connection's informations can be extracted by *soap]
 *@param sList [string sList =  List.dat of remote AxxelBoxs]
 *@param sLocalNetworkInformation [string sLocalNetworkInformation = LocalNetworkInformation.dat of remote AxxelBoxs]
 *@param *iResult [*iResult will be sent back, equal 1 means that, informations have already received by server side]
 *@return SOAP_OK [tell SOAP, every running in right way]
 */
int adp_List__tauschen(struct soap *soap, std::string sList,
		std::string sLocalNetworkInformation, std::string sIP, int *iResult)
{
	try
	{
		// open file "Temp", erase everything, ready to write
		std::ofstream outfile("Temp", std::ios::out);
		if (!outfile)
		{
			throw Exception(2, "Open File 'Temp' ERROR! " + getTime());
		}
		// save the List.dat of remote AxxelBoxs in "Temp" in local
		outfile << sList;
		// close file
		outfile.close();

		// open File "LocalNetworkInformation.dat", add mode, ready to write
		std::ofstream outfile_Local("LocalNetworkInformation.dat",
				std::ios::app);
		// save the LocalNetworkInformation.dat of remote AxxelBoxs in LocalNetworkInformation.dat in local
		if (!outfile_Local)
		{
			throw Exception(2,
					"Open File 'LocalNetworkInformation.dat' by function adp_List__tauschen ERROR! "
							+ getTime());
		}
		outfile_Local << sLocalNetworkInformation;
		// close file
		outfile_Local.close();

		// equal 1 means that, informations have already received by server side
		*iResult = 1;
		Server ListControl;
		// save the file "Temp" into "List.dat", at the same time extract the informations from Temp
		ListControl.writeList(sIP);
		// check the file "List.dat", delete the same records
		ListControl.checkElement(ListControl.getList("List.dat"));
		// create a LocalNetworkInformation object
		LocalNetworkInformation LocalNetworkInformationControl;
		// delete the same records
		LocalNetworkInformationControl.checkLocalNetworkInformation();
	} catch (Exception &Exception)
	{
		Exception.dispose();
	} catch (...)
	{
		Exception Exception(9,
				"Unexpected Error by function adp_List__tauschen() "
						+ getTime());
		Exception.dispose();
	}
	return SOAP_OK;
}

/**
 *@fn getTime
 *@brief get current time and return a string of time
 *@return sTime [return a string of current time]
 */
std::string getTime()
{

	time_t now_time;
	// initializes object now_time
	now_time = time(NULL);

	// declare a string to return the current time
	std::string sTime;
	// create a char array to store the time
	char chTime[40];

	// initializes chTime with '\0'
	memset(chTime, 0, sizeof(chTime));

	// save the current time in form of "day/month/year/time" in chTime
	strftime(chTime, sizeof(chTime), "%d/%m/%Y/%X", localtime(&now_time));

	sTime = chTime;
	return sTime;
}

/**
 *@fn getIPAddress
 *@brief get Host IP Address
 *@param sInterface [this interface's IP address will be returned
 *@param iNET [iNET = 4 means ipv4 IP address ,INET = 6 means ipv6 IP address]
 *@return sIPAddress [return string ip address]
 */
std::string getIPAddress(std::string sInterface, int iINET) throw (Exception)
{
	std::string sIPAddress = "";
	std::string sInterfaceBuffer = "";

	struct ifaddrs * ifAddrStruct = NULL;
	struct ifaddrs * ifa = NULL;
	void * tmpAddrPtr = NULL;

	getifaddrs(&ifAddrStruct);
	// check it is IP4
	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr->sa_family == AF_INET && iINET == 4)
		{
			// is a valid IP4 Address
			tmpAddrPtr = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			sInterfaceBuffer = ifa->ifa_name;
			if (sInterfaceBuffer == sInterface)
			{

				sIPAddress = addressBuffer;
			}
		}
		// check it is IP6
		else if (ifa->ifa_addr->sa_family == AF_INET6 && iINET == 6)
		{
			// is a valid IP6 Address
			tmpAddrPtr = &((struct sockaddr_in6 *) ifa->ifa_addr)->sin6_addr;
			char addressBuffer[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
			sInterfaceBuffer = ifa->ifa_name;
			if (sInterfaceBuffer == sInterface)
			{
				std::string saddressBuffer = addressBuffer;
				if (saddressBuffer.substr(0, 4) != "0000"
						&& saddressBuffer.substr(0, 4) != "ff02"
						&& saddressBuffer.substr(0, 4) != "fe80"
						&& saddressBuffer.substr(0, 4) != "ff00"
						&& saddressBuffer.substr(0, 4) != "fc02"
						&& saddressBuffer.substr(0, 2) != "::")
					sIPAddress = addressBuffer;
			}
		}
	}

	// release memory
	if (ifAddrStruct != NULL)
		freeifaddrs(ifAddrStruct);
	if (sIPAddress == "" && iINET == 4)
	{
		throw Exception(4, "Get IPv4 Address Error! " + getTime());
	}
	else if (sIPAddress == "" && iINET == 6)
	{
		throw Exception(4, "Get IPv6 Address Error! " + getTime());
	}
	else
	{
	}
	return sIPAddress;
}

/**
 *@fn getDefaultInterface
 *@brief get Default Interface
 *@return sDefaultInterface [return default interface]
 */
std::string getDefaultInterface() throw (Exception)
{
	std::string sDefaultInterface = "";
	// get datum from /proc/net/route
	std::ifstream infile("/proc/net/route");
	// /proc/net/route can not be opened
	if (!infile)
	{
		throw Exception(2,
				"Open File '/proc/net/route' by function getDefaultInterface() ERROR! "
						+ getTime());
	}
	// loop until the file pointer point to the end of file
	while (!infile.eof())
	{

		char cLine[500];
		// get records from the file line by line
		infile.getline(cLine, 500);
		const char *cSeparator = "	 ";
		char *cBuffer;
		char *cInterfaceBuffer;
		int iSeparator = 0;
		// use strtok function to separate the line
		cBuffer = strtok(cLine, cSeparator);
		// iteration for separating of informations
		while (cBuffer)
		{
			iSeparator++;
			// the first rank is interface
			if (iSeparator == 1)
			{
				cInterfaceBuffer = cBuffer;
			}
			// the second rank is destination address
			else if (iSeparator == 2)
			{
				// destination address 00000000 means default route
				if (!strcmp(cBuffer, "00000000"))
				{
					sDefaultInterface = cInterfaceBuffer;
					break;
				}
			}
			else
			{

			}
			cBuffer = strtok(NULL, cSeparator);
		}

	}
	infile.close();
	if (sDefaultInterface == "")
	{
		throw Exception(5, "Get Default Interface ERROR! " + getTime());
	}
	return sDefaultInterface;
}

/**
 *@brief convert Integer variable to String variable
 *@param iNumber [iNumber will convert to String]
 *@return sReturnValue [sting value of iNumber]
 */
std::string convertIntToString(int iNumber)
{
	if (iNumber == 0)
		return "0";
	std::string sTemp = "";
	std::string sReturnValue = "";
	while (iNumber > 0)
	{
		sTemp += iNumber % 10 + 48;
		iNumber /= 10;
	}
	for (unsigned int i = 0; i < sTemp.length(); i++)
		sReturnValue += sTemp[sTemp.length() - i - 1];
	return sReturnValue;
}
