/**
 * Client.cpp
 * Project: Axxel Discovery Protocol
 * Created on: 2012-8-4
 * Author: Li, Yuan
 *
 * running as client thread
 * first collect the datum from List.dat and LocalNetworkInformation.dat
 * extract IPs of remote AxxelBoxs
 * then send these datum to remote AxxelBoxs
 */

#include "axxel_discovery_protokoll.h"
#include "adp_List.nsmap"

/**
 *@brief Constructor
 */
Client::Client()
{
	// vList is a vector, which is based on stListContainer struct declared
	// vList store the records: Remote AxxelBox IP, Remote AxxelBox Ports, Destination IP, Destination Network Address, Time
	// reserve enough memory for vList
	vList.reserve(1000);
}

/**
 *@brief send List.dat and LocalNetworkInformation.dat to remote AxxelBoxs
 */
void Client::listTransfer()
{
	// always send datum to server side
	while (1)
	{
		try
		{
			// call getList function and get remote AxxelBox IP Addresses from List.dat
			getList();
			// call getFullText function and get datum from List.dat
			getFullText();
			// call getLocalNetworkInformation function and get datum from LocalNetworkInformation.dat
			getLocalNetworkInformation();

			struct soap soap;
			// initializes a stack-allocated gSOAP context
			soap_init(&soap);
			// set timeout = 5s
			soap.connect_timeout = 5;
			std::ofstream outfile("Sendlog.dat", std::ios::app);
			if (!outfile)
			{
				throw Exception(2,
						"Open File 'Sendlog.dat' by Client Side function listTransfer ERROR! "
								+ getTime());
			}

			for (unsigned int i = 0; i < vList.size() - 1; i++)
			{
				// equal 1 means that, informations have already received by server side
				// equal 0 means that, informations can not be received by server side
				int iResult = 0;
				// store remote AxxelBox IP Addresses temporary
				std::string sAddressPuffer = "";
				std::string sSelbstIPAddress = "";
				if (vList.at(i).sRemoteABIP[1] == '.'
						|| vList.at(i).sRemoteABIP[2] == '.'
						|| vList.at(i).sRemoteABIP[3] == '.')
				{
					if (bWITHIPV6ONLY)
					{
						continue;
					}
					else
					{
						// endpoint char array of IPv4 must in form of "xxx.xxx.xxx.xxx:yyyy"
						sAddressPuffer = vList.at(i).sRemoteABIP + ":"
								+ vList.at(i).sRemoteABPort;
						sSelbstIPAddress = getIPAddress(sINTERFACE, 4);
					}
				}
				else if (bWITHIPV6 || bWITHIPV6ONLY)
				{
					// endpoint char array of IPv6 must in form of "[xxx.xxx.xxx.xxx]:yyyy"
					sAddressPuffer = "[" + vList.at(i).sRemoteABIP + "]" + ":"
							+ vList.at(i).sRemoteABPort;
					sSelbstIPAddress = getIPAddress(sINTERFACE, 6);
				}
				else
				{
					continue;
				}
				// endpoint must be a char array
				const char* chAddress = sAddressPuffer.c_str();
				soap.connect_timeout = 5;
				// sFullText is the datum from List.dat
				// sLocalNetworkInformation is the datum from LocalNetworkInformation.dat
				// soap_call_app_List__tauschen is be created by GSOAP
				// sFullText and sLocalNetworkInformation will be sent to server side
				// iResult will be received
				soap_call_adp_List__tauschen(&soap, chAddress, NULL, sFullText,
						sLocalNetworkInformation, sSelbstIPAddress, &iResult);

				// equal 0 means that, informations can not be received by server side
				if (iResult == 0)
				{
					// save the logs in Sendlog.dat
					outfile << "Error in Transfer to " << chAddress
							<< " Paket_Size: "
							<< sFullText.length()
									+ sLocalNetworkInformation.length()
							<< "Byte" << " Time: " << getTime() << std::endl;
					// when bShowInformation = true, show the running's informations
					// when bShowInformation = false, show nothing
					if (bShowInformation)
					{
						std::cerr << "Error in Transfer to " << chAddress
								<< " Paket_Size: "
								<< sFullText.length()
										+ sLocalNetworkInformation.length()
								<< "Byte" << " Time: " << getTime()
								<< std::endl;
					}
				}
				// iResult == 1
				// equal 1 means that, informations have already received by server side
				else
				{
					// save the logs in Sendlog.dat
					outfile << "Completed Transfer to " << chAddress
							<< " Paket_Size: "
							<< sFullText.length()
									+ sLocalNetworkInformation.length()
							<< "Byte" << " Time: " << getTime() << std::endl;
					// when bShowInformation = true, show the running's informations
					// when bShowInformation = false, show nothing
					if (bShowInformation)
					{
						std::cout << "Completed Transfer to " << chAddress
								<< " Paket_Size: "
								<< sFullText.length()
										+ sLocalNetworkInformation.length()
								<< "Byte" << " Time: " << getTime()
								<< std::endl;
					}
				}

			}

			outfile.close();
			// clean up class instances

			soap_destroy(&soap);
			// clean up everything and close socket
			soap_end(&soap);
			// close master socket and detach context
			// soap_done(&soap);
			// release the memory which be used by vList
			vList.clear();
			// init sFullText
			sFullText = "";
			// init sLocalNetworkInformation
			sLocalNetworkInformation = "";
			// sleep [iDelayTime] seconds
			// C++11 standard

			std::this_thread::sleep_for(std::chrono::seconds(iDelayTime));
		} catch (Exception &Exception)
		{
			Exception.dispose();
		} catch (...)
		{
			Exception Exception(-14,
					"Unexpected Error by Client Side IPV4 GSOAP" + getTime());
		}
	}
}

/**
 *@brief get datum from LocalNetworkInformation.dat
 */
void Client::getLocalNetworkInformation() throw (Exception)
{
	std::ifstream infile("LocalNetworkInformation.dat");
	char chListBuffer[100000];
	if (!infile)
	{
		throw Exception(2,
				"Open File 'LocalNetworkInformation' by Client Side function getLocalNetworkInformation Error "
						+ getTime());
	}
	// store all the datum in chListBuffer
	infile.get(chListBuffer, 100000, '\0');
	infile.close();
	sLocalNetworkInformation = chListBuffer;
}

/**
 *@brief get datum from List.dat
 */
void Client::getFullText() throw (Exception)
{
	// call getFullText function and get datum from List.dat
	std::ifstream infile("List.dat");
	char chListBuffer[100000];
	if (!infile)
	{
		throw Exception(2,
				"opening File 'List.dat' by Client Side function getFullText ERROR! "
						+ getTime());
	}
	// store all the datum in chListBuffer
	infile.get(chListBuffer, 100000, '\0');
	infile.close();
	sFullText = chListBuffer;
}

/**
 *@brief get remote AxxelBox IP Addresses from List.dat
 */
void Client::getList() throw (Exception)
{
	// get datum from list.dat
	std::ifstream infile("List.dat");
	// List.dat can not be opened
	if (!infile)
	{
		throw Exception(2,
				"Open File 'List.dat' by Client Side function getList() ERROR! "
						+ getTime());
	}
	// loop until the file pointer point to the end of file
	while (!infile.eof())
	{

		char cLine[500];
		// get records from the file line by line
		infile.getline(cLine, 500);
		// the first char of line is '#' means that, this line is a commentary
		if (cLine[0] != '#')
		{
			stListContainer stListContainer;
			const char *cSeparator = "	 ";
			char *cBuffer;
			int iSeparator = 0;
			// use strtok function to separate the line
			cBuffer = strtok(cLine, cSeparator);
			// iteration for separating of informations
			while (cBuffer)
			{
				iSeparator++;
				// the first rank is Remote Axxelbox IP address
				if (iSeparator == 1)
				{
					stListContainer.sRemoteABIP = cBuffer;
				}
				// the second rank is Remote Axxelbox Port
				else if (iSeparator == 2)
				{
					stListContainer.sRemoteABPort = cBuffer;
				}
				// the third rank is Remote Axxelbox extern IP address
				else if (iSeparator == 3)
				{
					stListContainer.sRemoteABLocalIP = cBuffer;
				}
				// the fourth rank is Remote Axxelbox extern Port
				else if (iSeparator == 4)
				{
					stListContainer.sRemoteABLocalPort = cBuffer;
				}
				// the fifth rank is Destination IP Address
				else if (iSeparator == 5)
				{
					stListContainer.sDesIP = cBuffer;
				}
				// the sixth rank is Destination Network Address
				else if (iSeparator == 6)
				{
					stListContainer.sDesNWAdress = cBuffer;
				}
				// the seventh rank is Time
				else
				{
					stListContainer.sTime = cBuffer;
				}
				cBuffer = strtok(NULL, cSeparator);
			}
			if (!(stListContainer.sRemoteABIP == ""
					&& stListContainer.sRemoteABPort == ""
					&& stListContainer.sRemoteABLocalIP == ""
					&& stListContainer.sRemoteABLocalPort == ""
					&& stListContainer.sDesIP == ""
					&& stListContainer.sDesNWAdress == ""
					&& stListContainer.sTime == ""))
			{
				vList.push_back(stListContainer);
			}
			vList.push_back(stListContainer);
		}
		else
		{

		}
	}
	infile.close();

	for (std::vector<stListContainer>::iterator i = vList.end() - 1;
			i != vList.begin();)
	{
		// iterator j traverse from begin to i
		for (std::vector<stListContainer>::iterator j = vList.begin(); j < i;)
		{

			if ((i->sRemoteABIP == j->sRemoteABIP)
					&& (i->sRemoteABPort == j->sRemoteABPort))
			{
				// deleted the same records
				j = vList.erase(j);
			}
			else
			{
				j++;
			}
		}
		i--;
	}
}

/**
 *@brief Destructor
 */
Client::~Client()
{
	vList.clear();
}
