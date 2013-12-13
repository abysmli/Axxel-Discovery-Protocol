/**
 * ControlCenter.cpp
 * Project: Axxel Discovery Protocol
 * Created on: 2012-8-4
 * Author: Li, Yuan
 *
 * controls this adp system
 * kernel of exchange process
 */

#include "axxel_discovery_protokoll.h"

/**
 *@brief Constructor
 *@param bSwitch [bSwitch = true means, the program will running as daemon]
 */
ControlCenter::ControlCenter(bool bSwitch)
{
	bDaemonSwitch = bSwitch;
}

/**
 *@brief start the Program
 *@brief start the Server side and Client side
 */
void ControlCenter::beginControl()
{
	// create adp.ini file
	createSettingFile();

	// get and refresh LocalNetworkInformation
	std::thread thRefreshLocalNetworkInformation([]()
	{
		LocalNetworkInformation getInformation;
		while(1)
		{
			getInformation.getLocalNetworkInformation_v4();
			std::this_thread::sleep_for(std::chrono::seconds(300));
		}

	});
	thRefreshLocalNetworkInformation.detach();

	// start server side in another thread
	// here use the lambda expression (c++11) is easier, if not, we need a function
	if (!bWITHIPV6ONLY)
	{
		std::thread thServerThread([]()
		{
			Server serverThread;
			serverThread.startServer();
		});
		// let server side thread works alone
		thServerThread.detach();
	}
	else
	{

	}
	// when bWITHIPV6 = true start one more thread for ipv6 server side
	if (bWITHIPV6||bWITHIPV6ONLY)
	{
		std::thread thServerThread_WITHIPv6([]()
		{
			Server serverThread;
			serverThread.startServer_WITHIPV6();
		});
		// let ipv6 server side thread works alone
		thServerThread_WITHIPv6.detach();
	}
	else
	{

	}
	// only for tests
	// sleep one second
	// wait for starting of server side
	// one second is enough for starting of server side
	std::this_thread::sleep_for(std::chrono::seconds(1));

	// start client side in another thread
	std::thread thClientThread([]()
	{
		Client clientThread;
		clientThread.listTransfer();
	});

	// let client side thread works alone
	thClientThread.detach();

	// running as daemon
	// the Program should always running in background
	std::ofstream outfile("Log", std::ios::app);
	outfile << "start Axxel Discovery Protocol at " << getTime() << std::endl;
	outfile.close();
	if (bDaemonSwitch)
	{
		while (1)
		{
			// check the apd.ini and change the environment variable every 10 seconds
			configureProgram();
			std::this_thread::sleep_for(std::chrono::seconds(10));
		}
	}

}

/**
 *@brief check out which parameter has been inputed
 *@param argc [number of parameters]
 *@param argv [parameters]
 */
void ControlCenter::checkParameter(int argc, char **argv)
{
	// when "-m" has been inputed
	if (!strcmp(argv[1], "-m"))
	{
		// start the program
		// show the menu
		menu();
	}
	// when "-h" has been inputed
	else if ((!strcmp(argv[1], "-h")) || (!strcmp(argv[1], "-?")))
	{
		// showHelp...
		showHelp();
	}
	// when "-rl" has been inputed
	else if (!strcmp(argv[1], "-rl"))
	{
		// reset LocalNetworkInformation.dat
		// LocalNetworkInformation.dat saves records, for example: Host Names, Host Ports, Network Address, Masks, Interfaces
		// all these things are about local network
		std::ofstream outfile("LocalNetworkInformation.dat", std::ios::out);
		outfile.setf(std::ios::left);
		outfile << "#LocalNetworkInformation" << std::endl << std::setw(30)
				<< "#HostName" << std::setw(30) << "Host IP Address"
				<< std::setw(15) << "HostPort" << std::setw(25)
				<< "NetworkAddress" << std::setw(25) << "Mask" << std::setw(15)
				<< "Interface" << std::endl;
		outfile.close();
		std::cout << "LocalNetworkInformation.dat reseted" << std::endl;
	}
	// when "-rm" has been inputed
	else if (!strcmp(argv[1], "-rm"))
	{
		// reset List.dat
		// List.dat saves records, for example: Remote AxxelBox IPs, Remote AxxelBox Ports, Destination IPs, Destination Network Address,Time
		std::ofstream outfile("List.dat", std::ios::out);
		outfile.setf(std::ios::left);
		outfile << "#SystemInformation" << std::endl << std::setw(30)
				<< "#Remote AxxelBox IP" << std::setw(25)
				<< "Remote AxxelBox Port" << std::setw(30)
				<< "Remote AxxelBox Local IP" << std::setw(30)
				<< "Remote AxxelBox Local Port" << std::setw(30)
				<< "Destination IP" << std::setw(30)
				<< "Destination Network Address" << std::setw(20) << "Time"
				<< std::endl;
		outfile.close();
		std::cout << "List.dat reseted" << std::endl;
	}
	// when "-rs" has been inputed
	else if (!strcmp(argv[1], "-rs"))
	{
		// reset Recvlog.dat
		// Recvlog.dat saves logs of server side
		std::ofstream outfile("Recvlog.dat", std::ios::out);
		outfile.close();
		std::cout << "Recvlog.dat" << std::endl;
	}
	// when "-rc" has been inputed
	else if (!strcmp(argv[1], "-rc"))
	{
		// reset Sendlog.dat
		// Sendlog.dat saves logs of client side
		std::ofstream outfile("Sendlog.dat", std::ios::out);
		outfile.close();
		std::cout << "Sendlog.dat reseted" << std::endl;
	}
	// when "-sl" has been inputed
	else if (!strcmp(argv[1], "-sl"))
	{
		// edit LocalNetworkInformation.dat by vi
		system("vi LocalNetworkInformation.dat");
	}
	// when "-sm" has been inputed
	else if (!strcmp(argv[1], "-sm"))
	{
		// edit List.dat by vi
		system("vi List.dat");
	}
	// when "-ss" has been inputed
	else if (!strcmp(argv[1], "-ss"))
	{
		// edit Recvlog.dat by vi
		system("vi Recvlog.dat");
	}
	// when "-sc" has been inputed
	else if (!strcmp(argv[1], "-sc"))
	{
		// edit Sendlog.dat by vi
		system("vi Sendlog.dat");
	}
	else if (!strcmp(argv[1], "-ri"))
	{
		// reset adp.ini
		// adp.ini saves all settings of adp
		std::ofstream outfile("adp.ini", std::ios::out);
		// set default DelayTime = 15
		outfile << "DelayTime = 15" << std::endl;
		// set default WITHIPV6 = False, that means show nothing in screen
		outfile << "WITHIPV6 = False" << std::endl;
		// set default Interface = Default, that means use default interface
		outfile << "Interface = Default" << std::endl;
		// set LostTime = 3, that means the records which more than 3 hours can not be connected, will be deleted
		outfile << "LostTime = 3" << std::endl;
		// set ServerSidePortIPv4 = 5000
		outfile << "ServerSidePortIPv4 = 5000" << std::endl;
		// set ServerSidePortIPv6 = 8000
		outfile << "ServerSidePortIPv6 = 8000" << std::endl;
		// set Extern IP = "" means
		outfile << "ExternIP = " << std::endl;
		outfile.close();
	}
	// when "-ra" has been inputed
	else if (!strcmp(argv[1], "-ra"))
	{
		// reset Recvlog.dat
		// Recvlog.dat saves logs of server side
		std::ofstream outfilers("Recvlog.dat", std::ios::out);
		outfilers.close();

		// reset List.dat
		// List.dat saves records, for example: Remote AxxelBox IPs, Remote AxxelBox Ports, Destination IPs, Destination Network Address, Time
		std::ofstream outfilerm("List.dat", std::ios::out);
		outfilerm.setf(std::ios::left);
		outfilerm << "#SystemInformation" << std::endl << std::setw(30)
				<< "#Remote AxxelBox IP" << std::setw(25)
				<< "Remote AxxelBox Port" << std::setw(30)
				<< "Remote AxxelBox Local IP" << std::setw(30)
				<< "Remote AxxelBox Local Port" << std::setw(30)
				<< "Destination IP" << std::setw(30)
				<< "Destination Network Address" << std::setw(20) << "Time"
				<< std::endl;
		outfilerm.close();

		// reset LocalNetworkInformation.dat
		// LocalNetworkInformation.dat saves records, for example: Host Names, Host Ports, Network Address, Masks, Interfaces
		// all these things are about local network
		std::ofstream outfilerl("LocalNetworkInformation.dat", std::ios::out);
		outfilerl.setf(std::ios::left);
		outfilerl << "#LocalNetworkInformation" << std::endl << std::setw(30)
				<< "#HostName" << std::setw(30) << "Host IP Address"
				<< std::setw(15) << "HostPort" << std::setw(25)
				<< "NetworkAddress" << std::setw(25) << "Mask" << std::setw(15)
				<< "Interface" << std::endl;
		outfilerl.close();

		// reset Sendlog.dat
		// Sendlog.dat saves logs of client side
		std::ofstream outfilerc("Sendlog.dat", std::ios::out);
		outfilerc.close();

		// reset adp.ini
		// adp.ini saves all settings of adp
		std::ofstream outfileri("adp.ini", std::ios::out);
		// set default DelayTime = 15
		outfileri << "DelayTime = 15" << std::endl;
		// set default WITHIPV6 = False, that means show nothing in screen
		outfileri << "WITHIPV6 = False" << std::endl;
		// set default Interface = Default, that means use default interface
		outfileri << "Interface = Default" << std::endl;
		// set LostTime = 3, that means the records which more than 3 hours can not be connected, will be deleted
		outfileri << "LostTime = 3" << std::endl;
		// set ServerSidePortIPv4 = 5000
		outfileri << "ServerSidePortIPv4 = 5000" << std::endl;
		// set ServerSidePortIPv6 = 8000
		outfileri << "ServerSidePortIPv6 = 8000" << std::endl;
		// set Extern IP = "" means
		outfileri << "ExternIP = " << std::endl;
		outfileri.close();

		std::cout << "All files reseted" << std::endl;
	}
	// when something else has been inputed
	else
	{
		// show the message "Parameter Fault" and show the help
		std::cerr << "Parameter Fault" << std::endl;
		showHelp();
	}
}

/**
 *@brief show help informations
 */
void ControlCenter::showHelp()
{
	std::cout << "Axxel Discovery Protocol Parameter Help:" << std::endl;
	std::cout << "no Parameter   : running in Daemon mode" << std::endl;
	std::cout << "-m             : running with Menu" << std::endl;
	std::cout << "-h/-?          : Help " << std::endl;
	std::cout << "-ri            : reset adp.ini" << std::endl;
	std::cout << "-rl            : reset LocalNetworkInformation.dat"
			<< std::endl;
	std::cout << "-rm            : reset List.dat" << std::endl;
	std::cout << "-rs            : reset Recvlog.dat" << std::endl;
	std::cout << "-rc            : reset Sendlog.dat" << std::endl;
	std::cout << "-sl            : setup LocalNetworkInformation.dat"
			<< std::endl;
	std::cout << "-sm            : setup List.dat" << std::endl;
	std::cout << "-ss            : setup Recvlog.dat" << std::endl;
	std::cout << "-sc            : setup Sendlog.dat" << std::endl;
	std::cout << "-ra            : reset All Files" << std::endl << std::endl;

}

/**
 *@brief change /etc/syscrl.conf, in order to avoid the TIME_WAIT state by ports
 *@details these four scripts should be changed
 *@details net.ipv4.tcp_syncookies = 1
 *@details net.ipv4.tcp_tw_reuse = 1
 *@details net.ipv4.tcp_tw_recycle = 1
 *@details net.ipv4.tcp_fin_timeout = 30
 *@details other things can not be changed
 *//*
 void ControlCenter::setupBetriebSystem()
 {
 // copy /etc/sysctl.conf to work directory
 system("cp /etc/sysctl.conf ./");
 // open this file and ready to change it
 std::ifstream infile("sysctl.conf");
 // declare a vector based on string type
 // vector can adjust their size dynamically
 // vSysctl saves all records of sysctl.conf
 // the four scripts, which we needed, will first be stored in vSysctl, and then save vSysctl in file
 std::vector<std::string> vSysctl;
 std::string sSysctl;
 // bSyncookies = false means that, the record of net.ipv4.tcp_syncookies is not in file sysctl.conf
 // it is just the init value
 bool bSyncookies = false;
 // bTwReuse = false means that, the record of net.ipv4.tcp_tw_reuse is not in file sysctl.conf
 // it is just the init value
 bool bTwReuse = false;
 // bTwRecycle = false means that, the record of net.ipv4.tcp_tw_recycle is not in file sysctl.conf
 // it is just the init value
 bool bTwRecycle = false;
 // bFinTimeout = false means that, the record of net.ipv4.tcp_fin_timeout is not in file sysctl.conf
 // it is just the init value
 bool bFinTimeout = false;

 // get the records of sysctl.conf line by line and store it in sSysctl
 while (getline(infile, sSysctl))
 {
 // check whether net.ipv4.tcp_syncookies already in syscontl.conf or not
 if (sSysctl[0] != 35 && sSysctl.find("net.ipv4.tcp_syncookies") < 20
 && sSysctl.find("net.ipv4.tcp_syncookies") >= 0)
 {
 // means net.ipv4.tcp_syncookies already in syscontl.conf
 bSyncookies = true;
 // store "net.ipv4.tcp_syncookies = 1" in vSysctl
 vSysctl.push_back("net.ipv4.tcp_syncookies = 1");
 }
 // check whether net.ipv4.tcp_tw_reuse" already in syscontl.conf or not
 else if (sSysctl[0] != 35 && sSysctl.find("net.ipv4.tcp_tw_reuse") < 20
 && sSysctl.find("net.ipv4.tcp_tw_reuse") >= 0)
 {
 // means net.ipv4.tcp_tw_reuse already in syscontl.conf
 bTwReuse = true;
 // store "net.ipv4.tcp_tw_reuse = 1" in vSysctl
 vSysctl.push_back("net.ipv4.tcp_tw_reuse = 1");
 }
 // check whether net.ipv4.tcp_tw_recycle already in syscontl.conf or not
 else if (sSysctl[0] != 35
 && sSysctl.find("net.ipv4.tcp_tw_recycle") < 20
 && sSysctl.find("net.ipv4.tcp_tw_recycle") >= 0)
 {
 // means net.ipv4.tcp_tw_recycle already in syscontl.conf
 bTwRecycle = true;
 // store "net.ipv4.tcp_tw_recycle = 1" in vSysctl
 vSysctl.push_back("net.ipv4.tcp_tw_recycle = 1");
 }
 // check whether net.ipv4.tcp_fin_timeout already in syscontl.conf or not
 else if (sSysctl[0] != 35
 && sSysctl.find("net.ipv4.tcp_fin_timeout") < 20
 && sSysctl.find("net.ipv4.tcp_fin_timeout") >= 0)
 {
 // means net.ipv4.tcp_fin_timeout already in syscontl.conf
 bFinTimeout = true;
 // store "net.ipv4.tcp_fin_timeout = 1" in vSysctl
 vSysctl.push_back("net.ipv4.tcp_fin_timeout = 30");
 }
 else
 {
 // none of those four scripts has been found
 // just store this line in vSysctl and change nothing
 vSysctl.push_back(sSysctl);
 }
 }
 // when net.ipv4.tcp_syncookies has not been found in sysctl.conf
 // store "net.ipv4.tcp_syncookies = 1" in vSysctl
 if (!bSyncookies)
 {
 vSysctl.push_back("net.ipv4.tcp_syncookies = 1");
 }
 else
 {
 }
 // when net.ipv4.tcp_tw_reuse has not been found in sysctl.conf
 // store "net.ipv4.tcp_tw_reuse = 1" in vSysctl
 if (!bTwReuse)
 {
 vSysctl.push_back("net.ipv4.tcp_tw_reuse = 1");
 }
 else
 {
 }
 // when net.ipv4.tcp_tw_recycle has not been found in sysctl.conf
 // store "net.ipv4.tcp_tw_recycle = 1" in vSysctl
 if (!bTwRecycle)
 {
 vSysctl.push_back("net.ipv4.tcp_tw_recycle = 1");
 }
 else
 {
 }
 // when net.ipv4.tcp_fin_timeout has not been found in sysctl.conf
 // store "net.ipv4.tcp_fin_timeout = 1" in vSysctl
 if (!bFinTimeout)
 {
 vSysctl.push_back("net.ipv4.tcp_fin_timeout = 30");
 }
 else
 {
 }
 infile.close();

 // save vSysctl in sysctl and overwrite all the informations
 std::ofstream outfile("sysctl.conf", std::ios::out);
 for (unsigned int i = 0; i < vSysctl.size(); i++)
 {
 outfile << vSysctl.at(i) << std::endl;
 }
 outfile.close();
 // clean and release the memory, which are used by vSysctl
 vSysctl.clear();
 // overwrite /etc/sysctl.conf with sysctl.conf in work directory
 system("mv sysctl.conf /etc/sysctl.conf");
 // let operating system apply the changes
 system("/sbin/sysctl -p");
 }
 */

/**
 *@brief the menu of ControlCenter
 */
void ControlCenter::menu()
{
	beginControl();

	// show the menu
	while (1)
	{
		system("clear");
		std::cout << "Axxel Discovery Protocol Control Menu" << std::endl;
		std::cout << "option:" << std::endl;
		std::cout << "[d]: set delay time of Client" << std::endl;
		std::cout << "[i]: show running information" << std::endl;
		std::cout << "[l]: show LocalNetworkInformationList" << std::endl;
		std::cout << "[m]: show MainList" << std::endl;
		std::cout << "[s]: show Server side Log" << std::endl;
		std::cout << "[c]: show Client side Log" << std::endl;
		std::cout << "[r]: show running Variable" << std::endl;
		std::cout << "[e]: Exit" << std::endl;
		std::cout << "choose a option :  ";

		// chFlag store the key, which the user just inputed
		char chFlag;
		std::cin >> chFlag;

		// setup the interval for sending packets
		if (chFlag == 'd')
		{
			std::cout << std::endl << "The interval for sending Pakets"
					<< std::endl;
			std::cout << "Now sending interval: " << iDelayTime << "s"
					<< std::endl;
			std::cout
					<< "Client's Delay Time [second] ( '0' means no change ) = ";
			int iValue;
			// when the user inputs 0, means no change
			std::cin >> iValue;
			if (iValue != 0)
			{
				iDelayTime = iValue;
			}
			else
			{
				continue;
			}
		}
		// show the running Informations
		else if (chFlag == 'i')
		{
			system("clear");
			std::cout << "running Informations: (return to Menu press 'q')"
					<< std::endl;
			bShowInformation = true;

			// endless loop 
			// when user input 'q', exit it and show the informations not any more
			while (1)
			{
				if (getchar() == 'q')
				{
					bShowInformation = false;
					break;
				}
				else
				{
				}
			}
		}

		// show LocalNetworkInformation.dat
		else if (chFlag == 'l')
		{
			system("reset");
			system("cat LocalNetworkInformation.dat");
			std::cout << std::endl << std::endl
					<< "Press any key return to Menu..." << std::endl;
			// press any key return to menu
			getchar();
			getchar();
		}
		// show List.dat
		else if (chFlag == 'm')
		{
			system("reset");
			system("cat List.dat");
			std::cout << std::endl << std::endl
					<< "Press any key return to Menu..." << std::endl;
			// press any key return to menu
			getchar();
			getchar();
		}
		// show Recvlog.dat
		else if (chFlag == 's')
		{
			system("reset");
			system("cat Recvlog.dat");
			std::cout << std::endl << std::endl
					<< "Press any key return to Menu..." << std::endl;
			// press any key return to menu
			getchar();
			getchar();
		}
		// show Sendlog.dat
		else if (chFlag == 'c')
		{
			system("reset");
			system("cat Sendlog.dat");
			std::cout << std::endl << std::endl
					<< "Press any key return to Menu..." << std::endl;
			// press any key return to menu
			getchar();
			getchar();
		}
		// exit this Program
		else if (chFlag == 'e')
		{
			exit(1);
		}
		else if (chFlag == 'r')
		{
			system("reset");
			std::cout << "The running Variables:" << std::endl;
			std::cout << "Delay Time : " << iDelayTime << std::endl;
			std::cout << "WITHIPV6 : " << bWITHIPV6 << std::endl;
			std::cout << "Interface: " << sINTERFACE << std::endl;
			std::cout << "Lost Time : " << iLostTime << std::endl;
			std::cout << "Server Side Port IPv4 :" << iServerSidePortIPv4
					<< std::endl;
			std::cout << "Server Side Port IPv6 :" << iServerSidePortIPv6
					<< std::endl;
			std::cout << "Extern IP Address : " << sExternIPAddress << ":"
					<< sExternPort << std::endl;
			std::cout << std::endl << std::endl
					<< "Press any key return to Menu..." << std::endl;
			// press any key return to menu
			getchar();
			getchar();

		}
		else
		{
			continue;
		}
	}
}

/**
 *@brief configure the Program from the setting file "adp.ini"
 */
void ControlCenter::configureProgram()
{
	std::ifstream infile("adp.ini");

	if (!infile)
	{
		try
		{

			std::ofstream outfile("adp.ini", std::ios::out);
			if (!outfile)
			{
				throw Exception(2, "Can not create 'adp.ini'" + getTime());
			}
			// set default DelayTime = 15
			outfile << "DelayTime = 15" << std::endl;
			// set default ShowInformation = False, that means show nothing in screen
			outfile << "WITHIPV6 = False" << std::endl;
			// set default Interface = Default, that means use default interface
			outfile << "Interface = Default" << std::endl;
			// set LostTime = 3, that means the records which more than 3 hours can not be connected, will be deleted
			outfile << "LostTime = 3" << std::endl;
			// set ServerSidePortIPv4 = 5000
			outfile << "ServerSidePortIPv4 = 5000" << std::endl;
			// set ServerSidePortIPv6 = 8000
			outfile << "ServerSidePortIPv6 = 8000" << std::endl;
			outfile << "ExternIP = " << std::endl;
			outfile.close();
		} catch (Exception &Exception)
		{
			Exception.dispose();
		}
	}

	std::string sLine;
	// store the delay time from adp.ini temporary
	int iNewDelayTime = 0;
	// store the 'WITHIPV6' switch from adp.ini temporary
	bool bNewWITHIPV6 = false;
	// only use IPV6
	bool bNewWITHIPV6ONLY=false;
	// store the Interface from adp.ini temporary
	std::string sNewInterface = "";
	// store the LostTime from adp.ini temporary
	int iNewLostTime = 0;
	// store ServerSidePortIPv4 from adp.ini temporary
	int iNewServerSidePortIPv4 = 0;
	// store ServerSidePortIPv6 from adp.ini temporary
	int iNewServerSidePortIPv6 = 0;

	std::string sNewExternIPAddress;
	std::string sNewExternPort;

	bool bDelayTimeChangeFlag = false;
	bool bInterfaceChangeFlag = false;
	bool bWITHIPV6ChangeFlag = false;
	bool bLostTimeChangeFlag = false;
	bool bServerSidePortIPv4ChangeFlag = false;
	bool bServerSidePortIPv6ChangeFlag = false;
	bool bExternIPChangeFlag = false;
	// read adp.ini in sList line by line
	while (getline(infile, sLine))
	{
		// when the first char is '#', this line is a commentary
		if (sLine[0] != 35)
		{
			bool bDelayTimeFlag = false;
			bool bInterfaceFlag = false;
			bool bWITHIPV6Flag = false;
			bool bLostTimeFlag = false;
			bool bServerSidePortIPv4Flag = false;
			bool bServerSidePortIPv6Flag = false;
			bool bExternIPFlag = false;

			// find the line of delay time and then read the value
			if (sLine.find("DelayTime", 0) == 0)
			{
				bDelayTimeFlag = true;
				bDelayTimeChangeFlag = true;
			}
			// find the line of WITHIPV6 and then read the value
			else if (sLine.find("WITHIPV6", 0) == 0)
			{
				bWITHIPV6Flag = true;
				bWITHIPV6ChangeFlag = true;
			}
			// find the line of Interface and then read the value
			else if (sLine.find("Interface", 0) == 0)
			{
				bInterfaceFlag = true;
				bInterfaceChangeFlag = true;
			}
			// find the line of LostTime and then read the value
			else if (sLine.find("LostTime", 0) == 0)
			{
				bLostTimeFlag = true;
				bLostTimeChangeFlag = true;
			}
			// find the line of ServerSidePortIPv4 and then read the value
			else if (sLine.find("ServerSidePortIPv4", 0) == 0)
			{
				bServerSidePortIPv4Flag = true;
				bServerSidePortIPv4ChangeFlag = true;
			}
			// find the line of ServerSidePortIPv6 and then read the value
			else if (sLine.find("ServerSidePortIPv6", 0) == 0)
			{
				bServerSidePortIPv6Flag = true;
				bServerSidePortIPv6ChangeFlag = true;
			}
			else if (sLine.find("ExternIP", 0) == 0)
			{
				bExternIPFlag = true;
				bExternIPChangeFlag = true;
			}
			else
			{
			}
			// read the value for delay time
			if (bDelayTimeFlag)
			{
				for (unsigned int i = sLine.find("=", 0) + 1;
						i < sLine.length(); i++)
				{
					if (sLine[i] >= 48 && sLine[i] <= 57)
					{
						iNewDelayTime = iNewDelayTime * 10 + sLine[i] - 48;
					}
					else
					{

					}
				}

			}
			// read the value for WITHIPV6
			else if (bWITHIPV6Flag)
			{
				if ((sLine.find("True", 0) < 30 && sLine.find("True", 0) > 0)
						|| (sLine.find("true", 0) < 30
								&& sLine.find("true", 0) > 0)
						|| (sLine.find("TRUE", 0) < 30
								&& sLine.find("TRUE", 0) > 0))
				{
					bNewWITHIPV6 = true;
					bNewWITHIPV6ONLY=false;
				}
				else if ((sLine.find("Only", 0) < 30 && sLine.find("Only", 0) > 0)
						|| (sLine.find("only", 0) < 30
								&& sLine.find("only", 0) > 0)
						|| (sLine.find("ONLY", 0) < 30
								&& sLine.find("ONLY", 0) > 0))
				{
					bNewWITHIPV6=true;
					bNewWITHIPV6ONLY=true;
				}
				else
				{

				}
			}
			// read the value for Interface
			else if (bInterfaceFlag)
			{
				char cInterface[10];
				memset(cInterface, 0, sizeof(cInterface));
				int iPosition = 0;
				for (unsigned int i = sLine.find("=", 0) + 1;
						i < sLine.length(); i++)
				{
					if (sLine[i] != ' ')
					{
						cInterface[iPosition++] = sLine[i];
					}
					else
					{

					}
				}
				sNewInterface = cInterface;
			}
			// read the value for LostTime
			else if (bLostTimeFlag)
			{
				for (unsigned int i = sLine.find("=", 0) + 1;
						i < sLine.length(); i++)
				{
					if (sLine[i] >= 48 && sLine[i] <= 57)
					{
						iNewLostTime = iNewLostTime * 10 + sLine[i] - 48;
					}
					else
					{

					}
				}
			}
			// read the value for ServerSidePortIPv4
			else if (bServerSidePortIPv4Flag)
			{
				for (unsigned int i = sLine.find("=", 0) + 1;
						i < sLine.length(); i++)
				{
					if (sLine[i] >= 48 && sLine[i] <= 57)
					{
						iNewServerSidePortIPv4 = iNewServerSidePortIPv4 * 10
								+ sLine[i] - 48;
					}
					else
					{

					}
				}
			}
			// read the value for ServerSidePortIPv6
			else if (bServerSidePortIPv6Flag)
			{
				for (unsigned int i = sLine.find("=", 0) + 1;
						i < sLine.length(); i++)
				{
					if (sLine[i] >= 48 && sLine[i] <= 57)
					{
						iNewServerSidePortIPv6 = iNewServerSidePortIPv6 * 10
								+ sLine[i] - 48;
					}
					else
					{

					}
				}
			}

			else if (bExternIPFlag)
			{
				char cIPAddress[50];
				memset(cIPAddress, 0, sizeof(cIPAddress));
				char cPort[10];
				memset(cPort, 0, sizeof(cPort));

				int iPosition = 0;
				int iSeparator = 0;
				for (unsigned int i = sLine.find("=", 0) + 1;
						i < sLine.length(); i++)
				{
					if (sLine[i] != ' ' && sLine[i] != ':' && iSeparator == 0)
					{
						cIPAddress[iPosition++] = sLine[i];
					}
					else if (sLine[i] != ' ' && sLine[i] != ':'
							&& iSeparator == 1)
					{
						cPort[iPosition++] = sLine[i];
					}
					else if (sLine[i] == ':')
					{
						iSeparator++;
						iPosition = 0;

					}
				}
				sNewExternIPAddress = cIPAddress;
				sNewExternPort = cPort;
			}
			else
			{

			}
		}
		else
		{

		}
	}
	infile.close();
	// refresh the value of iDelayTime
	if (bDelayTimeChangeFlag)
	{
		iDelayTime = iNewDelayTime;

	}
	// refresh the value of bWITHIPV6
	if (bWITHIPV6ChangeFlag)
	{
		bWITHIPV6 = bNewWITHIPV6;
		bWITHIPV6ONLY=bNewWITHIPV6ONLY;

	}
	// refresh the value of sINTERFACE
	if (bInterfaceChangeFlag)
	{
		sINTERFACE = sNewInterface;
		if (sINTERFACE == "Default" || sINTERFACE == "default"
				|| sINTERFACE == "DEFAULT")
		{
			sINTERFACE = getDefaultInterface();
		}
		else
		{

		}
	}
	// refresh the value of iLostTime
	if (bLostTimeChangeFlag)
	{
		iLostTime = iNewLostTime;

	}
	// refresh the value of iServerSidePortIPv4
	if (bServerSidePortIPv4ChangeFlag)
	{
		iServerSidePortIPv4 = iNewServerSidePortIPv4;

	}
	// refresh the value of iServerSidePortIPv6
	if (bServerSidePortIPv6ChangeFlag)
	{
		iServerSidePortIPv6 = iNewServerSidePortIPv6;

	}
	//
	if (bExternIPChangeFlag)
	{
		if (sNewExternIPAddress != "" && sNewExternPort != "")
		{
			sExternIPAddress = sNewExternIPAddress;
			sExternPort = sNewExternPort;
		}
		else
		{

		}

	}

}

/**
 *@brief create the setting file "adp.ini"
 */
void ControlCenter::createSettingFile()
{
	std::ifstream infile("adp.ini");

	// the first time running the program
	// there is no adp.ini
	if (!infile)
	{
		std::ofstream outfile("adp.ini", std::ios::out);
		// set default DelayTime = 15
		outfile << "DelayTime = 15" << std::endl;
		// set default WITHIPV6 = False, that means show nothing in screen
		outfile << "WITHIPV6 = False" << std::endl;
		// set default Interface = Default, that means use default interface
		outfile << "Interface = Default" << std::endl;
		// set LostTime = 3, that means the records which more than 3 hours can not be connected, will be deleted
		outfile << "LostTime = 3" << std::endl;
		// set ServerSidePortIPv4 = 5000
		outfile << "ServerSidePortIPv4 = 5000" << std::endl;
		// set ServerSidePortIPv6 = 8000
		outfile << "ServerSidePortIPv6 = 8000" << std::endl;
		// set Extern IP = "" means
		outfile << "ExternIP = " << std::endl;
		outfile.close();
	}
	infile.close();
	configureProgram();
}
