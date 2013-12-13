/**
 * Server.cpp
 * Project: Axxel Discovery Protocol
 * Created on: 2012-8-4
 * Author: Li, Yuan
 *
 * running as server thread
 * collect and manage datum, which received by server side
 * save these datum in List.dat and automatic delete same records
 * automatic delete disconnective remote AxxelBox records
 */

#include "axxel_discovery_protokoll.h"
/**
 *@brief Constructor
 */
Server::Server()
{
	// vDiffList is a vector, which is based on stListContainer struct declared
	// vDiffList store the records: Remote AxxelBox IP, Remote AxxelBox Ports, Destination IP, Destination Network Address, Time
	// reserve enough memory for vDiffList
	vDiffList.reserve(1000);
}

/**
 *@brief start the ipv6 server side in an alone thread
 *@brief endless loop and never end
 */
void Server::startServer_WITHIPV6()
{
	try
	{
		struct soap soap_ipv6;

		// open Recvlog.dat and save the logs in it
		std::ofstream outfile("Recvlog.dat", std::ios::app);
		if (!outfile)
		{
			throw Exception(2,
					"Open File 'Recvlog.dat' by Server Side startServer_WITHIPV6() ERROR! "
							+ getTime());
		}
		// master socket and Slave socket
		int iMasterSocket_ipv6, iSlaveSocket_ipv6;
		// initializes a stack-allocated gSOAP context
		soap_init(&soap_ipv6);

		// returns master socket (backlog = max. queue size for requests).
		// when host == NULL : host is the machine on which the service runs
		iMasterSocket_ipv6 = soap_bind(&soap_ipv6,
				getIPAddress(sINTERFACE, 6).c_str(), iServerSidePortIPv6, 100);

		// fault to start the Server_ipv6
		if (iMasterSocket_ipv6 < 0)
		{
			// show the fault information
			soap_print_fault(&soap_ipv6, stderr);
			throw Exception(-11,
					"Gsoap by Server Side IPV6 soap_bind() ERROR! "
							+ getTime());
		}
		else
		{
			// save the logs in Recvlog.dat
			outfile << "Socket connection successful: master socket = "
					<< iMasterSocket_ipv6 << " Time: " << getTime()
					<< std::endl;
			// when bShowInformation = true, show the running's informations
			// when bShowInformation = false, show nothing
			if (bShowInformation)
			{
				std::cout << "Socket connection successful: master socket = "
						<< iMasterSocket_ipv6 << " Time: " << getTime()
						<< std::endl;
			}
			while (1)
			{
				// returns slave socket
				iSlaveSocket_ipv6 = soap_accept(&soap_ipv6);

				// fault to start the Client
				if (iSlaveSocket_ipv6 < 0)
				{
					soap_print_fault(&soap_ipv6, stderr);
					throw Exception(-12,
							"Gsoap by Server Side IPV6 soap_accept() ERROR! "
									+ getTime());
					break;
				}

				// save the logs in Recvlog.dat
				char cIPAddress[50];

				struct sockaddr_in6 *sockaddr_in6 =
						(struct sockaddr_in6 *) &soap_ipv6.peer;
				int iPort = ntohs(sockaddr_in6->sin6_port);
				inet_ntop(AF_INET6, &sockaddr_in6->sin6_addr, cIPAddress,
						sizeof(cIPAddress));

				outfile << "Accepted connection from IP = " << cIPAddress
						<< " port= " << iPort << " socket= "
						<< iSlaveSocket_ipv6 << " Time: " << getTime()
						<< std::endl;
				// when bShowInformation = true, show the running's informations
				// when bShowInformation = false, show nothing
				if (bShowInformation)
				{
					std::cout << "Accepted connection from IP = " << cIPAddress
							<< " port= " << iPort << " socket= "
							<< iSlaveSocket_ipv6 << " Time: " << getTime()
							<< std::endl;
				}

				// process RPC request
				if (soap_serve(&soap_ipv6) != SOAP_OK)
				{
					// print error
					soap_print_fault(&soap_ipv6, stderr);
					throw Exception(-13,
							"Gsoap by Server Side IPV6 ERROR! SOAP_OK not returned! "
									+ getTime());
				}
				// clean up class instances
				soap_destroy(&soap_ipv6);
				// clean up everything and close socket
				soap_end(&soap_ipv6);
			}
		}
		// close master socket and detach context
		soap_done(&soap_ipv6);
	} catch (Exception &Exception)
	{
		Exception.dispose();
	} catch (...)
	{
		Exception Exception(-14,
				"Unexpected Error by Server Side IPV6 GSOAP " + getTime());
		Exception.dispose();
	}
}

/**
 *@brief start the server side in an alone thread
 *@brief endless loop and never end
 */
void Server::startServer()
{
	try
	{
		struct soap soap;

		// open Recvlog.dat and save the logs in it
		std::ofstream outfile("Recvlog.dat", std::ios::app);
		if (!outfile)
		{
			throw Exception(2,
					"Open File 'Recvlog.dat' by Server Side startServer_WITHIPV4() ERROR! "
							+ getTime());
		}
		// master socket and Slave socket
		int iMasterSocket, iSlaveSocket;

		// initializes a stack-allocated gSOAP context
		soap_init(&soap);

		// returns master socket (backlog = max. queue size for requests).
		// when host == NULL : host is the machine on which the service runs
		iMasterSocket = soap_bind(&soap, getIPAddress(sINTERFACE, 4).c_str(),
				iServerSidePortIPv4, 100);

		// fault to start the Server
		if (iMasterSocket < 0)
		{
			// show the fault information
			soap_print_fault(&soap, stderr);
			throw Exception(-11,
					"Gsoap by Server Side IPV4 soap_bind() ERROR! "
							+ getTime());
		}

		else
		{
			// save the logs in Recvlog.dat
			outfile << "Socket connection successful: master socket = "
					<< iMasterSocket << " Time: " << getTime() << std::endl;
			// when bShowInformation = true, show the running's informations
			// when bShowInformation = false, show nothing
			if (bShowInformation)
			{
				std::cout << "Socket connection successful: master socket = "
						<< iMasterSocket << " Time: " << getTime() << std::endl;
			}
			while (1)
			{
				// returns slave socket
				iSlaveSocket = soap_accept(&soap);
				// fault to start the Client
				if (iSlaveSocket < 0)
				{
					soap_print_fault(&soap, stderr);
					throw Exception(-12,
							"Gsoap by Server Side IPV4 soap_accept() ERROR! "
									+ getTime());
					break;
				}
				// save the logs in Recvlog.dat
				outfile << "Accepted connection from IP = "
						<< ((soap.ip >> 24) & 0xFF) << "."
						<< ((soap.ip >> 16) & 0xFF) << "."
						<< ((soap.ip >> 8) & 0xFF) << "." << (soap.ip & 0xFF)
						<< " port= " << soap.port << " socket= " << iSlaveSocket
						<< " Time: " << getTime() << std::endl;
				// when bShowInformation = true, show the running's informations
				// when bShowInformation = false, show nothing
				if (bShowInformation)
				{
					fprintf(stderr,
							"Server: Accepted connection from IP=%ld.%ld.%ld.%ld port=%d socket=%d\n",
							(soap.ip >> 24) & 0xFF, (soap.ip >> 16) & 0xFF,
							(soap.ip >> 8) & 0xFF, soap.ip & 0xFF, soap.port,
							iSlaveSocket);
				}

				// process RPC request
				if (soap_serve(&soap) != SOAP_OK)
				{
					// print error
					soap_print_fault(&soap, stderr);
					throw Exception(-13,
							"Gsoap by Server Side IPV4 ERROR! SOAP_OK not returned! "
									+ getTime());
				}
				// clean up class instances
				soap_destroy(&soap);
				// clean up everything and close socket
				soap_end(&soap);
			}
		}
		// close master socket and detach context
		soap_done(&soap);
	} catch (Exception &Exception)
	{
		Exception.dispose();
	} catch (...)
	{
		Exception Exception(-14,
				"Unexpected Error by Server Side IPV4 GSOAP " + getTime());
		Exception.dispose();
	}
}

/**
 *@brief find out the remote AxxelBox, which more than 3 hours can not be connected
 *@param sRecordTime [Record's Time]
 *@return false [means this record should not be deleted]
 *@return true [means this record should be deleted]
 */
bool Server::checkTime(std::string sRecordTime)
{
// no time record, wait for time stamp, should not be deleted
	if (sRecordTime == "")
	{
		return false;
	}
	else
	{
		// get current Time
		std::string sNowTime = getTime();
		// store current time's hour in iNowTimeHour in integer
		int iNowTimeHour = (sNowTime[11] - 48) * 10 + sNowTime[12] - 48;
		// store record's time's hour in iRecordTimeHour in integer
		int iRecordTimeHour = (sRecordTime[11] - 48) * 10 + sRecordTime[12]
				- 48;
		// difference between iNowTimeHour and iRecordTimeHour more than iLostTime hours -> this record should be deleted
		if (((sNowTime[1] == sRecordTime[1])
				&& (abs(iNowTimeHour - iRecordTimeHour) <= iLostTime))
				|| ((sNowTime[1] - sRecordTime[1] == 1)
						&& (((24 - iRecordTimeHour) + iNowTimeHour) <= iLostTime)))
		{
			return false;
		}
		else
		{

			return true;
		}
	}
}

/**
 *@brief find out the same records and deleted them
 *@brief find out the wrong records and deleted them
 *@brief find out the remote AxxelBox, which more than 3 hours can not be connected
 *@param vList [all datum from List.dat]
 */
void Server::checkElement(std::vector<stListContainer> vList)
{
// find out the same records and deleted them
// iterator i traverse from end to begin
	for (std::vector<stListContainer>::iterator i = vList.end() - 1;
			i != vList.begin();)
	{
		// iterator j traverse from begin to i
		for (std::vector<stListContainer>::iterator j = vList.begin(); j < i;)
		{

			if ((i->sRemoteABIP == j->sRemoteABIP
					&& i->sRemoteABPort == j->sRemoteABPort
					&& i->sDesIP == j->sDesIP
					&& i->sDesNWAdress == j->sDesNWAdress))
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

// find out the wrong records and deleted them
// find out the remote AxxelBox, which more than 3 hours can not be connected
// iterator i traverse from begin to end
	for (std::vector<stListContainer>::iterator i = vList.begin();
			i != vList.end();)
	{
		// sRemoteABPort is empty means this record wrong
		// call checkTime in order to determine whether this remote AxxelBox can be connected or not
		if (i->sRemoteABPort == "" || i->sRemoteABIP == "" || i->sDesIP == ""
				|| i->sDesNWAdress == "" || checkTime(i->sTime))
		{
			i = vList.erase(i);
			if (checkTime(i->sTime))
			{
				try
				{
					throw Exception(8,
							"Warning! " + i->sRemoteABIP
									+ " can not connected already "
									+ convertIntToString(iLostTime)
									+ " hours! It will be deleted! "
									+ getTime());

				} catch (Exception &Exception)
				{
					Exception.dispose();
				}
			}
		}
		else
		{
			i++;
		}
	}
	// keep the original commentary not to change
	std::ifstream infile("List.dat");
	if (!infile)
	{
		throw Exception(2,
				"Open File 'List.dat' by Server::checkElement() ERROR! "
						+ getTime());
	}
	std::string sList[50];
	int iRowNum = 0;
	while (getline(infile, sList[iRowNum++]))
	{
		if (sList[iRowNum - 1][0] != 35)
		{
			break;
		}
		else
		{
		}
	}
	infile.close();
	// save vList in List.dat
	std::ofstream outfile("List.dat", std::ios::out);
	if (!infile)
	{
		throw Exception(2,
				"Open File 'List.dat' by Server::checkElement() ERROR! "
						+ getTime());
	}
	for (int i = 0; i < iRowNum - 1; i++)
	{
		outfile << sList[i] << std::endl;
	}
	outfile.setf(std::ios::left);
	for (unsigned int i = 0; i < vList.size(); i++)
	{
		outfile << std::setw(30) << vList.at(i).sRemoteABIP << std::setw(25)
				<< vList.at(i).sRemoteABPort << std::setw(30)
				<< vList.at(i).sRemoteABLocalIP << std::setw(30)
				<< vList.at(i).sRemoteABLocalPort << std::setw(30)
				<< vList.at(i).sDesIP << std::setw(30)
				<< vList.at(i).sDesNWAdress << std::setw(20)
				<< vList.at(i).sTime << std::endl;
	}
	vList.clear();
	outfile.close();
}

/**
 *@brief save the compared List in List.dat
 *@param sIP [client side's IP address]
 */
void Server::writeList(std::string sIP)
{
	compareList(getList("Temp"), getList("List.dat"), sIP);
	std::ofstream outfile("List.dat", std::ios::app);
	if (!outfile)
	{
		throw Exception(2,
				"Open File 'List.dat' by Server::writeList() ERROR! "
						+ getTime());
	}
	outfile.setf(std::ios::left);
	for (unsigned int i = 0; i < vDiffList.size(); i++)
	{
		outfile << std::setw(30) << vDiffList.at(i).sRemoteABIP << std::setw(25)
				<< vDiffList.at(i).sRemoteABPort << std::setw(30)
				<< vDiffList.at(i).sRemoteABLocalIP << std::setw(30)
				<< vDiffList.at(i).sRemoteABLocalPort << std::setw(30)
				<< vDiffList.at(i).sDesIP << std::setw(30)
				<< vDiffList.at(i).sDesNWAdress << std::setw(20)
				<< vDiffList.at(i).sTime << std::endl;
	}
	outfile.close();
	vDiffList.clear();
}

/**
 *@brief List contrast
 *@brief find out the difference between local List.dat and received List.dat
 *@brief update time stamp of records
 *@param vTempList [all datum from the file "Temp"]
 *@param vLocalList [all datum from the local file "List.dat"]
 *@param sIP [client side's IP address]
 */
void Server::compareList(std::vector<stListContainer> vTempList,
		std::vector<stListContainer> vLocalList, std::string sIP)
{
// update time stamp of remote AxxelBox which sent this List in received List.dat
	for (unsigned int i = 0; i < vTempList.size(); i++)
	{
		if (vTempList.at(i).sRemoteABIP == sIP)
		{
			vTempList.at(i).sTime = getTime();
			vDiffList.push_back(vTempList.at(i));
		}
		else
		{
		}
	}
// update time stamp of remote AxxelBox which sent this List in local List.dat
	for (unsigned int i = 0; i < vLocalList.size(); i++)
	{
		if (vLocalList.at(i).sRemoteABIP == sIP)
		{
			vLocalList.at(i).sTime = getTime();
			vDiffList.push_back(vLocalList.at(i));
		}
		else
		{

		}
	}
// find out the difference between local List.dat and received List.dat
	for (unsigned int i = 0; i < vTempList.size(); i++)
	{
		// bJudgeFlag = true means the records is the same
		bool bJudgeFlag = false;
		for (unsigned int j = 0; j <= vLocalList.size() - 1; j++)
		{
			if (vTempList.at(i).sRemoteABIP == vLocalList.at(j).sRemoteABIP
					&& vTempList.at(i).sRemoteABPort
							== vLocalList.at(j).sRemoteABPort
					&& vTempList.at(i).sDesIP == vLocalList.at(j).sDesIP
					&& vTempList.at(i).sDesNWAdress
							== vLocalList.at(j).sDesNWAdress)
			{
				bJudgeFlag = true;
			}
			else
			{
			}
		}
		// when the received record is different, store it in vDiffList
		if (!bJudgeFlag)
		{
			vDiffList.push_back(vTempList.at(i));
		}
		else
		{

		}
	}
	vTempList.clear();
	vLocalList.clear();
}

/**
 *@details get remote AxxelBox IP, remote AxxelBox Port, Destination IP, Destination Network Address and Time from List.dat
 *@details and save them in vList
 *@param sFileName [file name, from which the datum will be got]
 *@return vList [return vList with all datum of List.dat]
 */
std::vector<stListContainer> Server::getList(const std::string sFileName)
{
	const char* chFileName = sFileName.c_str();
	// get datum from chFileName
	std::fstream infile(chFileName);
	// chFileName can not be opened
	if (!infile)
	{
		throw Exception(2,
				"Open File " + sFileName + " by Server::getList() ERROR! "
						+ getTime());
	}
	std::vector<stListContainer> vList;
	// loop until the file pointer point to the end of file
	while (!infile.eof())
	{
		char cLine[500];
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

		}
		else
		{

		}
	}
	infile.close();
	return vList;
}

/**
 *@brief Destructor
 */
Server::~Server()
{
	// release memory
	vDiffList.clear();
}
