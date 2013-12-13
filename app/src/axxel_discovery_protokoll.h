/**
 * axxel_discovery_protokoll.h
 * Project: Axxel Discovery Protocol
 * Created on: 2012-8-4
 * Author: Li, Yuan
 */

#ifndef AXXEL_DISCOVERY_PROTOKOLL_H_
#define AXXEL_DISCOVERY_PROTOKOLL_H_

#include "soapH.h"
#include "soapStub.h"
#include <string.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <ctime>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <syslog.h>
#include <iomanip>

/**
 *@struct stListContainer
 *@brief save the data from List.dat temporary
 */
struct stListContainer
{
	///declare variable sRemoteABIP [store remote AxxelBox IP Address]
	std::string sRemoteABIP;
	///declare variable sRemoteABPort [store remote AxxelBox Ports]
	std::string sRemoteABPort;
	///declare variable sRemoteABLocalIP [store remote AxxelBox IP Local Address]
	std::string sRemoteABLocalIP;
	///declare variable sRemoteABLocalPort [store remote AxxelBox Local Ports]
	std::string sRemoteABLocalPort;
	///declare variable sDesIP [store Destination IP Address]
	std::string sDesIP;
	///declare variable sDesNWAdress [store Destination Network Address]
	std::string sDesNWAdress;
	///declare variable sTime [store Time stamp]
	std::string sTime;
};

/**
 *@struct stRoutetable
 *@brief save the data from LocalNetworkInformation.dat temporary
 */
struct stRoutetable
{
	///declare variable sHostName [store Host Name]
	std::string sHostName;
	///declare variable sHostIP [store Host IP Address]
	std::string sHostIP;
	///declare variable sHostPort [store Host Port]
	std::string sHostPort;
	///declare variable sNetworkAddress [store Network Address]
	std::string sNetworkAddress;
	///declare variable sMask [store Mask]
	std::string sMask;
	///declare variable sInterface [store Interface]
	std::string sInterface;
};

/**
 *@class Exception
 *@brief Exception system for Axxel Discovery Protocol
 *@brief Show the Fault Message and save them in Syslog
 */
class Exception
{
private:
	// Error Level
	int iFaultCode;
	// Fault Message
	std::string sFaultMessage;

public:
	// Constructor
	Exception(int, std::string);
	// save the Fault in syslog
	void saveInLog();
	// dispose the exception
	void dispose();
};

/**
 *@class LocalNetworkInformation
 *@brief collect the informations of local network,
 *@brief and save these in LocalNetworkInformation.dat,
 *@brief the informations include host name, host port, network address, mask, interface
 *@brief it can also automatic delete the same records
 */
class LocalNetworkInformation
{
private:
	// vRoutetable store the records: Host Names, Host Ports, Network Address, Masks, Interfaces
	std::vector<stRoutetable> vRoutetable;
	// vList store the records: Remote AxxelBox IP, Remote AxxelBox Ports, Destination IP, Destination Network Address, Time
	std::vector<stListContainer> vList;
public:
	// Constructor
	LocalNetworkInformation(void);
	// get Host Names and Host Ports
	std::string getHostName(void);
	// save all records in LocalNetworkInformation.dat
	void writeInformation(void);
	// check the records and delete the same records
	void checkLocalNetworkInformation(void);
	// get ipv6 network informations: Network Address, Mask, Interface and store them in vRoutetable
	void getLocalNetworkInformation_v6(int) throw (Exception);
	// get ipv4 network informations: Network Address, Mask, Interface and store them in vRoutetable
	void getLocalNetworkInformation_v4(void);
	// Destructor
	~LocalNetworkInformation(void);

};

/**
 *@class Server
 *@brief running as server thread
 *@brief collect and manage datum, which received by server side
 *@brief save these datum in List.dat and automatic delete same records
 *@brief automatic delete disconnective remote AxxelBox records
 */
class Server
{
private:
	// vDiffList store the records: Remote AxxelBox IP, Remote AxxelBox Ports, Destination IP, Destination Network Address, Time
	std::vector<stListContainer> vDiffList;

public:
	// Constructor
	Server();
	// get remote AxxelBox IP, remote AxxelBox Port, Destination IP, Destination Network Address and Time
	std::vector<stListContainer> getList(const std::string);
	// compare the received list with the local list, delete the some records
	void compareList(std::vector<stListContainer>, std::vector<stListContainer>,
			std::string);
	// save the compared List in List.dat
	void writeList(std::string);
	// find out the same and the wrong records and deleted them
	void checkElement(std::vector<stListContainer>);
	// start the server side in an alone thread
	void startServer(void);
	// start the ipv6 server side in an alone thread
	void startServer_WITHIPV6(void);
	// find out the remote AxxelBox, which more than 3 hours can not be connected
	bool checkTime(std::string);
	// Destructor
	~Server();

};

/**
 *@class Client
 *@brief running as client thread
 *@brief first collect the datum from List.dat and LocalNetworkInformation.dat
 *@brief extract IPs of remote AxxelBoxs
 *@brief then send these datum to remote AxxelBoxs
 */
class Client
{
private:
	// vList store the records: Remote AxxelBox IP, Remote AxxelBox Ports, Destination IP, Destination Network Address, Time
	std::vector<stListContainer> vList;
	// store all the Informations from List.dat
	std::string sFullText;
	// store all the Informations from LocalNetworkInformation.dat
	std::string sLocalNetworkInformation;
public:
	//Constructor
	Client();
	// get datum from List.dat
	void getFullText(void) throw(Exception);;
	// get remote AxxelBox IP Addresses from List.dat
	void getList(void) throw(Exception);;
	// send List.dat and LocalNetworkInformation.dat to remote AxxelBoxs
	void listTransfer(void);
	// get datum from LocalNetworkInformation.dat
	void getLocalNetworkInformation(void) throw(Exception);;
	// Destructor
	~Client();
};

/**
 *@class ControlCenter
 *@brief controls this adp system
 *@brief kernel of exchange process
 */
class ControlCenter
{
private:
	// the switch of daemon mode
	bool bDaemonSwitch;
public:
	// Constructor
	ControlCenter(bool);
	// start the Program
	void beginControl();
	// show help informations
	void showHelp(void);
	// check out which parameter has been inputed
	void checkParameter(int ARGC, char **ARGV);
	// the menu of ControlCenter
	void menu(void);
	// configure Program
	void configureProgram(void);
	// create setting file adp.ini
	void createSettingFile(void);
};

/**
 *@fn getTime
 *@brief get current Time and return a string
 */
std::string getTime(void);

/**
 *@fn getIPAddress
 *@brief get Host IP Address
 */
std::string getIPAddress(std::string, int) throw(Exception);

/**
 *@fn getDefaultInterface
 *@brief get Default Interface
 */
std::string getDefaultInterface(void) throw(Exception);

/**
 *@fn convertIntToString
 *@brief convert Integer variable to String variable
 */
std::string convertIntToString(int);

/**
 *@var iDelayTime
 *@brief iDelayTime interval of sending data
 */
extern int iDelayTime;

/**
 *@var bShowInformation
 *@brief bShowInformation = true  : shows the informations of connections and transfer states
 *@brief bShowInformation = false : shows nothing during the running program
 */
extern bool bShowInformation;

/**
 *@var sINTERFACE
 *@brief choose the INTERFACE, which will be used
 */
extern std::string sINTERFACE;

/**
 *@var bWITHIPV6
 *@brief the Switch of IPv6
 *@brief bWITHIPV6 = true means, running with IPv6
 *@brief bWITHIPV6 = false means, running without IPv6
 */
extern bool bWITHIPV6;

/**
 *@var bWITHIPV6ONLY
 *@brief only use IPv6
 *@brief bWITHIPV6 = true means, running only with IPv6
 *@brief bWITHIPV6 = false means, running with IPv6 and IPv4
 *@brief initializes value of bWITHIPV6ONLY 'false'
 */
extern bool bWITHIPV6ONLY;

/**
 *@var iLostTime
 *@brief the records which more than iLostTime hours can not be connected, will be deleted
 */
extern int iLostTime;

/**
 *@var iServerSidePortIPv4
 *@brief Server Port by ipv4
 */
extern int iServerSidePortIPv4;

/**
 *@var iServerSidePortIPv6
 *@brief Server Port by ipv6
 */
extern int iServerSidePortIPv6;

/**
 *@var sExternIPAddress
 *@brief extern IP address
 */
extern std::string sExternIPAddress;

/**
 *@var sExternPort
 *@brief extern port
 */
extern std::string sExternPort;

#endif /* AXXEL_DISCOVERY_PROTOKOLL_H_ */

