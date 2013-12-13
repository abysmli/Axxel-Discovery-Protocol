/**
 * LocalNetworkInformation.cpp
 * Project: Axxel Discovery Protocol
 * Created on: 2012-8-4
 * Author: Li, Yuan
 *
 * collect the informations of local network,
 * and save these in LocalNetworkInformation.dat,
 * the informations include host name, host port, network address, mask, interface
 * it can also automatic delete the same records
 */

#include "axxel_discovery_protokoll.h"

/**
 *@brief Constructor
 */
LocalNetworkInformation::LocalNetworkInformation()
{
	// vRoutetable is a vector, which is based on stRoutetable struct declared
	// vRoutetable store the records: Host Names, Host Ports, Network Address, Masks, Interfaces
	// reserve enough memory for vRoutetable
	vRoutetable.reserve(1000);
}

/**
 *@brief get Host Names and return it
 *@brief use gethostname function and getdomainname function to get host name and domain name
 *@return sHostname [return "Host Name" + "." + "Domain Name"]
 */
std::string LocalNetworkInformation::getHostName()
{
	std::string sHostName = "";
	char cHostName[50];
	char cDomainName[50];
	gethostname(cHostName, sizeof(cHostName));
	getdomainname(cDomainName, sizeof(cDomainName));
	sHostName = cHostName;
	sHostName += ".";
	sHostName += cDomainName;
	try
	{
		if (!strcmp(cHostName, ""))
		{
			throw Exception(6,
					"Get Host Name ERROR! No HostName! " + getTime());
		}
		else
		{
		}
		if (!strcmp(cDomainName, ""))
		{
			throw Exception(6,
					"Get Host Domain ERROR! No HostDomain! " + getTime());
		}
		else
		{
		}

	} catch (Exception &Exception)
	{
		Exception.dispose();
	}
	return sHostName;
}

/**
 *@brief get ipv6 network informations: Network Address, Mask, Interface and store them in vRoutetable
 *@param iFlag [the position of vRoutetable]
 *@return vRoutetable [return all datum from LocalNetworkInformation.dat]
 */
void LocalNetworkInformation::getLocalNetworkInformation_v6(int iFlag)
		throw (Exception)
{
	// get datum from /proc/net/route
	std::ifstream infile("/proc/net/ipv6_route");
	// /proc/net/route can not be opened
	if (!infile)
	{
		throw Exception(2,
				"Open File '/proc/net/route' by LocalNetworkInformation::getLocalNetworkInformation_v6() ERROR! "
						+ getTime());
	}
	// loop until the file pointer point to the end of file
	while (!infile.eof())
	{

		char cLine[500];
		// get records from the file line by line
		infile.getline(cLine, 500);
		stRoutetable stRoutetable;
		const char *cSeparator = "	 ";
		char *cBuffer;
		bool bFlag = false;
		// use strtok function to separate the line
		cBuffer = strtok(cLine, cSeparator);

		int iSeparator = 0;
		std::string sInterface = "";
		std::string sDestinationNetwork = "";
		std::string sDestinationPrefixLength = "";
		// iteration for separating of informations
		while (cBuffer)
		{
			iSeparator++;
			std::string sBuffer = cBuffer;
			// the first rank of /proc/net/route is ipv6 destination address
			if (iSeparator == 1)
			{
				// rule out the ipv6 address which began with "0000", "ff02", "fe08", "ff00" or "fc02"
				if (sBuffer.substr(0, 4) != "0000"
						&& sBuffer.substr(0, 4) != "ff02"
						&& sBuffer.substr(0, 4) != "fe80"
						&& sBuffer.substr(0, 4) != "ff00"
						&& sBuffer.substr(0, 4) != "fc02")
				{
					bFlag = true;
					sDestinationNetwork = sBuffer;
				}

			}
			else if (iSeparator == 10 && bFlag)
			{
				sInterface = sBuffer;
			}
			else if (iSeparator == 2 && bFlag)
			{
				sDestinationPrefixLength = sBuffer;
			}
			cBuffer = strtok(NULL, cSeparator);
		}

		if (bFlag)
		{
			if (sInterface != "lo")
			{
				std::string sNetworkAddress = "";
				std::string sNetmask = "";

				int iDestinationPrefixLength = 0;
				// calculate the network address and netmask from destination address
				iDestinationPrefixLength = ((sDestinationPrefixLength[0] - 48)
						* 16 + sDestinationPrefixLength[1] - 48) / 4;
				sNetworkAddress = sDestinationNetwork.substr(0,
						iDestinationPrefixLength);
				sNetmask = sDestinationNetwork.substr(iDestinationPrefixLength,
						32 - iDestinationPrefixLength);
				std::string sNetworkAddressBuffer;
				std::string sNetmaskBuffer;

				// the abbreviation of ipv6 address
				for (unsigned int i = 0; i < sNetworkAddress.length() / 4; i++)
				{
					if (i == sNetworkAddress.length() / 4 - 1)
					{
						if (sNetworkAddress.substr(i * 4, 4) == "0000")
						{
							sNetworkAddressBuffer += "0";
						}
						else if (sNetworkAddress.substr(i * 4, 3) == "000")
						{
							sNetworkAddressBuffer += sNetworkAddress.substr(
									i * 4 + 3, 1);
						}
						else if (sNetworkAddress.substr(i * 4, 2) == "00")
						{
							sNetworkAddressBuffer += sNetworkAddress.substr(
									i * 4 + 2, 2);
						}
						else if (sNetworkAddress.substr(i * 4, 1) == "0")
						{
							sNetworkAddressBuffer += sNetworkAddress.substr(
									i * 4 + 1, 3);
						}
						else
						{
							sNetworkAddressBuffer += sNetworkAddress.substr(
									i * 4, 4);
						}
					}
					else
					{
						if (sNetworkAddress.substr(i * 4, 4) == "0000")
						{
							sNetworkAddressBuffer += "0:";
						}
						else if (sNetworkAddress.substr(i * 4, 3) == "000")
						{
							sNetworkAddressBuffer += sNetworkAddress.substr(
									i * 4 + 3, 1) + ":";
						}
						else if (sNetworkAddress.substr(i * 4, 2) == "00")
						{
							sNetworkAddressBuffer += sNetworkAddress.substr(
									i * 4 + 2, 2) + ":";
						}
						else if (sNetworkAddress.substr(i * 4, 1) == "0")
						{
							sNetworkAddressBuffer += sNetworkAddress.substr(
									i * 4 + 1, 3) + ":";
						}
						else
						{
							sNetworkAddressBuffer += sNetworkAddress.substr(
									i * 4, 4) + ":";
						}

					}

				}

				for (unsigned int i = 0; i < sNetmask.length() / 4; i++)
				{
					if (i == sNetmask.length() / 4 - 1)
					{
						if (sNetmask.substr(i * 4, 4) == "0000")
						{
							sNetmaskBuffer += "0";
						}
						else if (sNetmask.substr(i * 4, 3) == "000")
						{
							sNetmaskBuffer += sNetmask.substr(i * 4 + 3, 1);
						}
						else if (sNetmask.substr(i * 4, 2) == "00")
						{
							sNetmaskBuffer += sNetmask.substr(i * 4 + 2, 2);
						}
						else if (sNetmask.substr(i * 4, 1) == "0")
						{
							sNetmaskBuffer += sNetmask.substr(i * 4 + 1, 3);
						}
						else
						{
							sNetmaskBuffer += sNetmask.substr(i * 4, 4);
						}
					}
					else
					{
						if (sNetmask.substr(i * 4, 4) == "0000")
						{
							sNetmaskBuffer += "0:";
						}
						else if (sNetmask.substr(i * 4, 3) == "000")
						{
							sNetmaskBuffer += sNetmask.substr(i * 4 + 3, 1)
									+ ":";
						}
						else if (sNetmask.substr(i * 4, 2) == "00")
						{
							sNetmaskBuffer += sNetmask.substr(i * 4 + 2, 2)
									+ ":";
						}
						else if (sNetmask.substr(i * 4, 1) == "0")
						{
							sNetmaskBuffer += sNetmask.substr(i * 4 + 1, 3)
									+ ":";
						}
						else
						{
							sNetmaskBuffer += sNetmask.substr(i * 4, 4) + ":";
						}

					}

				}
				sNetworkAddress = sNetworkAddressBuffer;
				sNetmask = sNetmaskBuffer;
				stRoutetable.sNetworkAddress = sNetworkAddress;
				stRoutetable.sHostIP = getIPAddress(sInterface, 6);
				stRoutetable.sMask = sNetmask;
				stRoutetable.sInterface = sInterface;
				stRoutetable.sHostPort = convertIntToString(
						iServerSidePortIPv6);
				stRoutetable.sHostName = getHostName();
				vRoutetable.push_back(stRoutetable);
			}

		}
		else
		{
		}
	}
	for (unsigned int i = iFlag; i < vRoutetable.size(); i++)
	{
		stListContainer stList;
		stList.sRemoteABLocalIP = getIPAddress(vRoutetable.at(i).sInterface, 6);
		stList.sRemoteABLocalPort = convertIntToString(iServerSidePortIPv6);
		stList.sDesIP = vRoutetable.at(i).sNetworkAddress;
		stList.sDesNWAdress = vRoutetable.at(i).sNetworkAddress;
		stList.sTime = getTime();
		if (sExternIPAddress == "")
		{
			stList.sRemoteABIP = stList.sRemoteABLocalIP;
		}
		else
		{
			stList.sRemoteABIP = sExternIPAddress;
		}
		if (sExternPort == "")
		{
			stList.sRemoteABPort = stList.sRemoteABLocalPort;
		}
		else
		{
			stList.sRemoteABPort = sExternPort;
		}
		vList.push_back(stList);
	}
	// call writeInformation to write the datum in files
	writeInformation();
}

/**
 *@brief get ipv4 network informations: Network Address, Mask, Interface and store them in vRoutetable
 *@return vRoutetable [return all datum from LocalNetworkInformation.dat]
 */
void LocalNetworkInformation::getLocalNetworkInformation_v4()
{
	try
	{
		// get datum from /proc/net/route
		std::ifstream infile("/proc/net/route");
		if (!infile)
		{
			throw Exception(2,
					"Open File '/proc/net/route' by LocalNetworkInformation::getLocalNetworkInformation_v4() ERROR! "
							+ getTime());
		}
		int iRowNum = 0;
		// loop until the file pointer point to the end of file
		while (!infile.eof() && ++iRowNum)
		{
			char cLine[500];
			// get records from the file line by line
			infile.getline(cLine, 500);
			if (iRowNum != 1)
			{
				stRoutetable stRoutetable;
				const char *cSeparator = "	 ";
				char *cBuffer;
				bool bFlag = false;
				// use strtok function to separate the line
				cBuffer = strtok(cLine, cSeparator);
				int iSeparator = 0;
				std::string sInterface = "";
				// iteration for separating of informations
				while (cBuffer)
				{
					iSeparator++;
					std::string sBuffer = cBuffer;
					// the first rank is interface
					if (iSeparator == 1)
					{
						sInterface = sBuffer;
					}
					// the second rank is destination address
					// the eighth rank is netmask
					if (iSeparator == 2 || iSeparator == 8)
					{
						int iPartValue[4];
						for (int i = 0; i < 4; i++)
						{
							std::stringstream convert(sBuffer.substr(i * 2, 2));
							convert >> std::hex >> iPartValue[i];
						}
						if (iSeparator == 2 && iPartValue[3] != 0
								&& iPartValue[3] != 127 && iPartValue[3] != 169)
						{
							bFlag = true;
						}
						std::string sConvertBuffer = "";
						for (int i = 3; i >= 0; i--)
						{
							if (i == 0)
							{
								sConvertBuffer += convertIntToString(
										iPartValue[i]);
							}
							else
							{
								sConvertBuffer += convertIntToString(
										iPartValue[i]) + ".";
							}
						}
						if (iSeparator == 2 && bFlag)
						{
							stRoutetable.sNetworkAddress = sConvertBuffer;
						}
						else if (iSeparator == 8 && bFlag)
						{
							stRoutetable.sMask = sConvertBuffer;
						}

					}

					cBuffer = strtok(NULL, cSeparator);
				}
				if (bFlag)
				{
					stRoutetable.sHostIP = getIPAddress(sInterface, 4);
					stRoutetable.sInterface = sInterface;
					stRoutetable.sHostPort = convertIntToString(
							iServerSidePortIPv4);
					stRoutetable.sHostName = getHostName();

					vRoutetable.push_back(stRoutetable);
				}
			}
		}
		int iFlag = 0;
		for (unsigned int i = 0; i < vRoutetable.size(); i++)
		{
			stListContainer stList;
			stList.sRemoteABLocalIP = getIPAddress(vRoutetable.at(i).sInterface,
					4);
			stList.sRemoteABLocalPort = convertIntToString(iServerSidePortIPv4);
			stList.sDesIP = vRoutetable.at(i).sNetworkAddress;
			stList.sDesNWAdress = vRoutetable.at(i).sNetworkAddress;
			stList.sTime = getTime();
			if (sExternIPAddress == "")
			{
				stList.sRemoteABIP = stList.sRemoteABLocalIP;
			}
			else
			{
				stList.sRemoteABIP = sExternIPAddress;
			}
			if (sExternPort == "")
			{
				stList.sRemoteABPort = stList.sRemoteABLocalPort;
			}
			else
			{
				stList.sRemoteABPort = sExternPort;
			}
			vList.push_back(stList);
			iFlag = i;
		}

		getLocalNetworkInformation_v6(iFlag + 1);
		checkLocalNetworkInformation();
	} catch (Exception &Exception)
	{
		Exception.dispose();
	} catch (...)
	{
		Exception Exception(7, "Iterator Error! " + getTime());
		Exception.dispose();
	}
}

/**
 *@brief save all records of vRoutetable in LocalNetworkInformation.dat and in List.dat
 */
void LocalNetworkInformation::writeInformation()
{
	// save the Host Name, Host IP, Host Port, Network Address, Mask and Interface in LocalNetworkInforamtion.dat
	std::ofstream outfile("LocalNetworkInformation.dat", std::ios::app);
	if (!outfile)
	{
		throw Exception(2,
				"Open File 'LocalNetworkInformation.dat' by LocalNetworkInformation::writeInformation() ERROR! "
						+ getTime());
	}
	outfile.setf(std::ios::left);
	/*outfile << "#LocalNetworkInformation" << std::endl << std::setw(30)
			<< "#HostName" << std::setw(30) << "Host IP Address"
			<< std::setw(15) << "HostPort" << std::setw(25) << "NetworkAddress"
			<< std::setw(25) << "Mask" << std::setw(15) << "Interface"
			<< std::endl;*/
	for (unsigned int i = 0; i < vRoutetable.size(); i++)
	{
		outfile << std::setw(30) << vRoutetable.at(i).sHostName << std::setw(30)
				<< vRoutetable.at(i).sHostIP << std::setw(15)
				<< vRoutetable.at(i).sHostPort << std::setw(25)
				<< vRoutetable.at(i).sNetworkAddress << std::setw(25)
				<< vRoutetable.at(i).sMask << std::setw(15)
				<< vRoutetable.at(i).sInterface << std::endl;
	}
	outfile.close();

	// save the Remote AxxelBox IP, Remote AxxelBox Port, Destination IP, Destination Network Address and Time stamp in List.dat
	std::ofstream outfile_List("List.dat", std::ios::app);
	if (!outfile_List)
	{
		throw Exception(2,
				"Open File 'List.dat' by LocalNetworkInformation::writeInformation() ERROR! "
						+ getTime());
	}
	outfile_List.setf(std::ios::left);
	/*outfile_List << "#SystemInformation" << std::endl << std::setw(30)
			<< "#Remote AxxelBox IP" << std::setw(25) << "Remote AxxelBox Port"
			<< std::setw(30) << "Remote AxxelBox Local IP" << std::setw(30)
			<< "Remote AxxelBox Local Port" << std::setw(30)
			<< "Destination IP" << std::setw(30)
			<< "Destination Network Address" << std::setw(20) << "Time"
			<< std::endl;*/
	for (unsigned int i = 0; i < vRoutetable.size(); i++)
	{

		outfile_List << std::setw(30) << vList.at(i).sRemoteABIP
				<< std::setw(25) << vList.at(i).sRemoteABPort << std::setw(30)
				<< vList.at(i).sRemoteABLocalIP << std::setw(30)
				<< vList.at(i).sRemoteABLocalPort << std::setw(30)
				<< vList.at(i).sDesIP << std::setw(30)
				<< vList.at(i).sDesNWAdress << std::setw(20)
				<< vList.at(i).sTime << std::endl;
	}
	outfile_List.close();
	vRoutetable.clear();
	vList.clear();
}

/**
 *@brief check the records of LocalNetworkInformation.dat and delete the same records
 */
void LocalNetworkInformation::checkLocalNetworkInformation()
{
	// get all records from LocalNetworkInformation.dat and store them in vLocalNetworkInformation
	std::ifstream infile("LocalNetworkInformation.dat");
	std::vector<std::string> vLocalNetworkInformation;
	vLocalNetworkInformation.reserve(1000);
	std::string sBufferRow;

	// read LocalNetworkInformation.dat in sBufferRow line by line
	while (getline(infile, sBufferRow))
	{
		// store sBufferRow in vLocalNetworkInformation
		vLocalNetworkInformation.push_back(sBufferRow);
	}
	infile.close();

	// use iterator to delete the same records
	for (std::vector<std::string>::iterator i =
			vLocalNetworkInformation.begin();
			i != vLocalNetworkInformation.end();)
	{
		for (std::vector<std::string>::iterator j = i + 1;
				j != vLocalNetworkInformation.end();)
		{
			if (*i == *j)
			{
				// when *i is the same as *j, vLocalNetworkInformation(j) will be deleted
				j = vLocalNetworkInformation.erase(j);
			}
			else
			{
				j++;
			}
		}
		i++;
	}

	// save vLocalNetworkInformation in LocalNetworkInformation.dat
	std::ofstream outfile("LocalNetworkInformation.dat", std::ios::out);
	if (!outfile)
	{
		throw Exception(2,
				"Open File 'LocalNetworkInformation.dat' by LocalNetworkInformation::checkLocalNetworkInformation() ERROR! "
						+ getTime());
	}
	for (unsigned int i = 0; i < vLocalNetworkInformation.size(); i++)
	{
		outfile << vLocalNetworkInformation.at(i) << std::endl;
	}
	outfile.close();
	vLocalNetworkInformation.clear();
}

/**
 *@brief Destructor
 */
LocalNetworkInformation::~LocalNetworkInformation()
{
	// release memory
	vRoutetable.clear();
	vList.clear();
}
