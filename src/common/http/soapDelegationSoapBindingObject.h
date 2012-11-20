/* soapDelegationSoapBindingObject.h
   Generated by gSOAP 2.7.13 from /root/build/gfal2/src/common/http/delegation.h
   Copyright(C) 2000-2009, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/

#ifndef soapDelegationSoapBindingObject_H
#define soapDelegationSoapBindingObject_H
#include "soapH.h"

/******************************************************************************\
 *                                                                            *
 * Service Object                                                             *
 *                                                                            *
\******************************************************************************/

class DelegationSoapBindingService : public soap
{    public:
	DelegationSoapBindingService()
	{ static const struct Namespace namespaces[] =
{
	{"SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", "http://www.w3.org/*/soap-envelope", NULL},
	{"SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/", "http://www.w3.org/*/soap-encoding", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{"tns", "http://www.gridsite.org/namespaces/delegation-1", NULL, NULL},
	{NULL, NULL, NULL, NULL}
};
	if (!this->namespaces) this->namespaces = namespaces; };
	virtual ~DelegationSoapBindingService() { };
	/// Bind service to port (returns master socket or SOAP_INVALID_SOCKET)
	virtual	SOAP_SOCKET bind(const char *host, int port, int backlog) { return soap_bind(this, host, port, backlog); };
	/// Accept next request (returns socket or SOAP_INVALID_SOCKET)
	virtual	SOAP_SOCKET accept() { return soap_accept(this); };
	/// Serve this request (returns error code or SOAP_OK)
	virtual	int serve() { return soap_serve(this); };
};

/******************************************************************************\
 *                                                                            *
 * Service Operations (you should define these globally)                      *
 *                                                                            *
\******************************************************************************/


SOAP_FMAC5 int SOAP_FMAC6 tns__getProxyReq(struct soap*, std::string _delegationID, struct tns__getProxyReqResponse &_param_1);

SOAP_FMAC5 int SOAP_FMAC6 tns__getNewProxyReq(struct soap*, struct tns__getNewProxyReqResponse &_param_2);

SOAP_FMAC5 int SOAP_FMAC6 tns__renewProxyReq(struct soap*, std::string _delegationID, struct tns__renewProxyReqResponse &_param_3);

SOAP_FMAC5 int SOAP_FMAC6 tns__putProxy(struct soap*, std::string _delegationID, std::string _proxy, struct tns__putProxyResponse &_param_4);

SOAP_FMAC5 int SOAP_FMAC6 tns__getTerminationTime(struct soap*, std::string _delegationID, struct tns__getTerminationTimeResponse &_param_5);

SOAP_FMAC5 int SOAP_FMAC6 tns__destroy(struct soap*, std::string _delegationID, struct tns__destroyResponse &_param_6);

#endif