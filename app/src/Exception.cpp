/**
 * Exception.cpp
 * Project: Axxel Discovery Protocol
 * Created on: 2012-8-4
 * Author: Li, Yuan
 *
 * Exception system for Axxel Discovery Protocol
 * Show the Fault Messages and save them in Syslog
 *
 */

#include "axxel_discovery_protokoll.h"

/**
 * Constructor
 */
Exception::Exception(int iFaultCode, std::string sFaultMessage)
{
	this->iFaultCode = iFaultCode;
	this->sFaultMessage = sFaultMessage;
}

/**
 * save faultmessages in Log file
 */
void Exception::saveInLog()
{
	std::ofstream outfile("Log", std::ios::app);
	outfile << sFaultMessage << std::endl;
	outfile.close();
}

/**
 *@brief dispose errors
 *@details iFaultCode = -1# : gsoap errors
 *@details iFaultCode = 1 : daemon errors
 *@details iFaultCode = 2 : open files errors
 *@details iFaultCode = 3 : get time errors
 *@details iFaultCode = 4 : get ip address errors
 *@details iFaultCode = 5 : get default interface errors
 *@details iFaultCode = 6 : get host name or domain name errors
 *@details iFaultCode = 7 : iterator errors
 *@details iFaultCode = 8 : records warnings
 *@details iFaultCode = 9 : unexpected errors
 */
void Exception::dispose()
{
	saveInLog();
	if (iFaultCode == -11 || iFaultCode == -12 || iFaultCode == -13
			|| iFaultCode == -14 || iFaultCode == 1 || iFaultCode == 2
			|| iFaultCode == 5 || iFaultCode == 7 || iFaultCode == 4)
	{
		exit(iFaultCode);
	}
	else
	{

	}
}
