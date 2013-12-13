/*
 * adp_gsoap.h
 * Project: Axxel Discovery Protocol
 * Created on: 2012-8-4
 * Author: Li, Yuan
 */

// This Header File is only used for create the SOAP Files
#ifndef ADP_GSOAP_H_
#define ADP_GSOAP_H_

// This Function includes 3 Parameters
// 2 strings are used for sending Packets
// int* receives the responses
int adp_List__tauschen(std::string, std::string, std::string, int *);

#endif /* ADP_GSOAP_H_ */
