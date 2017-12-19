#ifdef WIN32
#undef UNICODE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <winscard.h>

#define CHECK(f, rv) \
if (SCARD_S_SUCCESS != rv) \
{ \
	printf(f ": %s\n", pcsc_stringify_error(rv)); \
	return -1; \
}

int main(void)
{
 	LONG rv;

 	SCARDCONTEXT hContext;
 	LPTSTR mszReaders;
 	SCARDHANDLE hCard;
 	DWORD dwReaders, dwActiveProtocol, dwRecvLength;

 	SCARD_IO_REQUEST pioSendPci;
 	BYTE pbRecvBuffer[258];
 	
 	//Generate Random Number 
 	BYTE cmd1[] = {0x80,0x84,0x00,0x00,0x08}; 
 	//BYTE cmd2[] = { 0x00, 0x00, 0x00, 0x00 };

 	unsigned int i;

 	rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
 	CHECK("SCardEstablishContext", rv)

	#ifdef SCARD_AUTOALLOCATE
 	dwReaders = SCARD_AUTOALLOCATE;

 	rv = SCardListReaders(hContext, NULL, (LPTSTR)&mszReaders, &dwReaders);
 	CHECK("SCardListReaders", rv)
	#else
 	rv = SCardListReaders(hContext, NULL, NULL, &dwReaders);
 	CHECK("SCardListReaders", rv)

 	mszReaders = calloc(dwReaders, sizeof(char));
 	rv = SCardListReaders(hContext, NULL, mszReaders, &dwReaders);
 	CHECK("SCardListReaders", rv)
	#endif
 	printf("Reader name: %s\n", mszReaders);

 	rv = SCardConnect(hContext, mszReaders, SCARD_SHARE_SHARED,
  	SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);
 	CHECK("SCardConnect", rv)

 	switch(dwActiveProtocol)
 	{
  		case SCARD_PROTOCOL_T0:
   			pioSendPci = *SCARD_PCI_T0;
   			break;

  		case SCARD_PROTOCOL_T1:
   			pioSendPci = *SCARD_PCI_T1;
   			break;
 	}
 	
 	printf("Send command: "); 	
 	for(i=0; i<sizeof(cmd1); i++)
  		printf("%02X ", cmd1[i]); 	
  	printf("\n");
  		
 	dwRecvLength = sizeof(pbRecvBuffer);
 	rv = SCardTransmit(hCard, &pioSendPci, cmd1, sizeof(cmd1), NULL, pbRecvBuffer, &dwRecvLength);
 	CHECK("SCardTransmit", rv)

 	printf("Response data: "); 	
 	for(i=0; i<dwRecvLength-2; i++)
  		printf("%02X ", pbRecvBuffer[i]);
  	printf("\nStatus code: ");
  	printf("%02X %02X", pbRecvBuffer[dwRecvLength-2],pbRecvBuffer[dwRecvLength-1]);	
 	printf("\n");

 	rv = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
 	CHECK("SCardDisconnect", rv)

	#ifdef SCARD_AUTOALLOCATE
 	rv = SCardFreeMemory(hContext, mszReaders);
 	CHECK("SCardFreeMemory", rv)
	#else
 	free(mszReaders);
	#endif

 	rv = SCardReleaseContext(hContext);

 	CHECK("SCardReleaseContext", rv)

 	return 0;
}