// SimpleSample.cpp : Defines the entry point for the console application.
//
// PCAN-Basic-C-Console SimpleSample.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <time.h>
#include "pcanbasic.h"

// Function declaration
int LoadDLL();
int UnloadDLL();
bool GetFunctionAdress(HINSTANCE h_module);
//  switch listen Onyl Mode on/off
TPCANStatus SetListenOnlyMode(TPCANHandle g_hChannel, bool mode);


//typdef of Functions
typedef TPCANStatus (__stdcall *PCAN_Initialize)(TPCANHandle Channel, TPCANBaudrate Btr0Btr1, TPCANType HwType , DWORD IOPort , WORD Interrupt);
typedef TPCANStatus (__stdcall *PCAN_Uninitialize)( TPCANHandle Channel);
typedef TPCANStatus (__stdcall *PCAN_Reset)(TPCANHandle Channel);
typedef TPCANStatus (__stdcall *PCAN_GetStatus)(TPCANHandle Channel);
typedef TPCANStatus (__stdcall *PCAN_Read)(TPCANHandle Channel, TPCANMsg* MessageBuffer, TPCANTimestamp* TimestampBuffer);
typedef TPCANStatus (__stdcall *PCAN_Write)(TPCANHandle Channel, TPCANMsg* MessageBuffer);
typedef TPCANStatus (__stdcall *PCAN_FilterMessages)(TPCANHandle Channel, DWORD FromID, DWORD ToID, TPCANMode Mode);
typedef TPCANStatus (__stdcall *PCAN_GetValue)(TPCANHandle Channel, TPCANParameter Parameter, void* Buffer, DWORD BufferLength);
typedef TPCANStatus (__stdcall *PCAN_SetValue)(TPCANHandle Channel, TPCANParameter Parameter, void* Buffer, DWORD BufferLength);
typedef TPCANStatus (__stdcall *PCAN_GetErrorText)(TPCANStatus Error, WORD Language, LPSTR Buffer);

//declaration
PCAN_Initialize g_CAN_Initialize;
PCAN_Uninitialize g_CAN_Uninitialize;
PCAN_Reset g_CAN_Reset;
PCAN_GetStatus  g_CAN_GetStatus;
PCAN_Read g_CAN_Read;
PCAN_Write  g_CAN_Write;
PCAN_FilterMessages  g_CAN_FilterMessages;
PCAN_GetValue  g_CAN_GetValue;
PCAN_SetValue  g_CAN_SetValue;
PCAN_GetErrorText  g_CAN_GetErrorText;

// name of DLL
char g_LibFileName[] = "PCANBasic";
LPCWSTR g_LibFileP = (LPCWSTR)g_LibFileName;
//DLL Instance Handle
HINSTANCE g_i_DLL;
// TPCANHandle
TPCANHandle g_hChannel;
TPCANBaudrate g_Baudrate;
// nur für non PNP
TPCANType g_CANType;
DWORD g_IOPort;
WORD g_Int;

int main(int argc, char *argv[])
{
	int ret,i;
	TPCANStatus CANStatus;
	TPCANMsg SendMessageBuffer; 
	TPCANMsg ReadMessageBuffer; 
	TPCANTimestamp MessageTimeStamp;
	char Buffer[1024];
	char *pBuffer;

	pBuffer = Buffer;
    
	printf("PCAN-Basic DLC Tester\n");
	printf("using 125k\n");
	printf("Send evry 250ms ID 0x732h with a DLC of 15\n");
	
	//Default USB Channel1
	g_hChannel = PCAN_USBBUS1;

	SendMessageBuffer.ID = 0x732;
	SendMessageBuffer.MSGTYPE = PCAN_MESSAGE_STANDARD;
	SendMessageBuffer.DATA[0] = 0x7f;
	SendMessageBuffer.DATA[1] = 0x00;
	SendMessageBuffer.DATA[2] = 0x00;
	SendMessageBuffer.DATA[3] = 0x00;
	SendMessageBuffer.DATA[4] = 0x00;
	SendMessageBuffer.DATA[6] = 0x00;
	SendMessageBuffer.DATA[5] = 0x00;
	SendMessageBuffer.DATA[7] = 0x00;
	SendMessageBuffer.LEN = 15;

	if(argc>1) //we have a parameter...
	{
		if(strcmp(argv[1], "help")==0)
		{
			printf("usage: DLC Sample ###\n where ### is usb | pci | pcc\n");
			printf("press any key to close");
			_getch();
			exit(0);
		}
		if(strcmp(argv[1], "usb")==0)
		{
			g_hChannel = PCAN_USBBUS1;
			printf("use PCAN-USB Channel 1\n");
		}
		if(strcmp(argv[1], "pci")==0)
		{
			g_hChannel = PCAN_PCIBUS1;
			printf("use PCAN-PCI Channel 1\n");
		}
		if(strcmp(argv[1], "pcc")==0)
		{
			g_hChannel = PCAN_PCCBUS1;
			printf("use PCAN-PC Cards Channel 1\n");
		}
	}
	ret = LoadDLL();
	if(ret!=0)
	{
	 printf("Load DLL: %i", ret);
	 exit(-1);
	}


	// Init der PCANBasic Applikation
	CANStatus = g_CAN_Initialize(g_hChannel, PCAN_BAUD_500K , 0, 0, 0); 
	if(CANStatus!=PCAN_ERROR_OK)
	{
		printf("Error while Init CAN Interface: 0x%x \n",CANStatus);
		//DLL entladen..
		UnloadDLL();
		printf("press any key to close");
		_getch();
		//und raus
		exit(-1);

	}
	printf("press any key to start\n");
	printf("and press any key while run to stop\n");

	_getch();
	printf("Start sending...\n");
    do
      {
        CANStatus = g_CAN_Write(g_hChannel, &SendMessageBuffer);
		if(CANStatus != PCAN_ERROR_OK)
		{
			printf("Erro while sending CAN Frame with DLC 15\n");
			g_CAN_GetErrorText(CANStatus, 0,pBuffer);
			printf("%s\n", Buffer);
		}	
        Sleep (250);   // alle 250mSec

        CANStatus = g_CAN_Read(PCAN_USBBUS1, &ReadMessageBuffer, &MessageTimeStamp);
		while(CANStatus != PCAN_ERROR_QRCVEMPTY)
		{
		 if(ReadMessageBuffer.MSGTYPE == PCAN_MESSAGE_STATUS)
		 {
			printf("We received a Status Message - please check the Databytes for more information.\n");
		 }
		 else
		 {
			if(CANStatus == PCAN_ERROR_OK) //now we have e real CAN Frame..
			{	
				if (ReadMessageBuffer.LEN > 0)  // A DLC could also be 0..but we only show ID´s with DATA
				{
					// report other messages
					printf("Receive - ");
					printf("ID: 0x%x\tDLC: %d\tMSGTYPE: %d\t",ReadMessageBuffer.ID, ReadMessageBuffer.LEN, ReadMessageBuffer.MSGTYPE);
					for (i = 0; i < ReadMessageBuffer.LEN; i++)
						printf (" %02X", ReadMessageBuffer.DATA[i]);
					printf ("\r\n");

				}
			} //endif STATUS OK
		}//else Status Message
        Sleep (2); 
		CANStatus = g_CAN_Read(PCAN_USBBUS1, &ReadMessageBuffer, &MessageTimeStamp);
	   }
      }while(!_kbhit()); //Solange bis Tatse gedrückt wird...
	printf("end of loop ...\n");
	//DLL entladen..
	UnloadDLL();
	return 0;
}



/***************************************************************************************

 Dynamic Load of the DLL and all function pointer

****************************************************************************************/

//
// Function: Load DLL
// Parameter: none
// ret value: 0 if OK, -1 if DLL not found or can not open, -2 if function pointer not found
//
// load the DLL and get function pointers
//


int LoadDLL()
{
	if(g_i_DLL==NULL)
	{
		g_i_DLL = LoadLibrary(g_LibFileName);
		if(g_i_DLL == NULL)
		{
			printf("ERROR: can not load pcanbasic.dll\n");
			return -1;
		}	
		else
		{
			printf("DLL Handle: 0x%x\n",g_i_DLL);
			if(GetFunctionAdress( g_i_DLL )==true)
			{
				printf("Load function adress for pcan_basic.dll\n");
			}
			else
			{
				printf("ERROR: can not load Function Adress\n");
				return -2;
			}
		}
	}
	return 0;
}


//
// Function: GetFunctionAdress
// Parameter: instance of DLL
// ret value: true if OK false if pointer not vallid
//
// load the function pointer from the DLL spec. by handle
//



bool GetFunctionAdress(HINSTANCE h_module)
{
  //Lade alle Funktionen
  if(h_module == NULL)
   return false;

  g_CAN_Initialize = (PCAN_Initialize) GetProcAddress(h_module, "CAN_Initialize");
  if(g_CAN_Initialize == NULL)
   return false;

  g_CAN_Uninitialize = (PCAN_Uninitialize) GetProcAddress(h_module, "CAN_Uninitialize");
  if(g_CAN_Uninitialize == NULL)
   return false;

  g_CAN_Reset = (PCAN_Reset) GetProcAddress(h_module, "CAN_Reset");
  if(g_CAN_Reset == NULL)
   return false;

  g_CAN_GetStatus = (PCAN_GetStatus) GetProcAddress(h_module, "CAN_GetStatus");
  if(g_CAN_GetStatus == NULL)
   return false;

  g_CAN_Read = (PCAN_Read) GetProcAddress(h_module, "CAN_Read");
  if(g_CAN_Read == NULL)
   return false;

  g_CAN_Write = (PCAN_Write) GetProcAddress(h_module, "CAN_Write");
  if(g_CAN_Write == NULL)
   return false;

  g_CAN_FilterMessages = (PCAN_FilterMessages) GetProcAddress(h_module, "CAN_FilterMessages");
  if(g_CAN_FilterMessages == NULL)
   return false;

  g_CAN_GetValue = (PCAN_GetValue) GetProcAddress(h_module, "CAN_GetValue");
  if(g_CAN_GetValue == NULL)
   return false;

  g_CAN_SetValue = (PCAN_SetValue) GetProcAddress(h_module, "CAN_SetValue");
  if(g_CAN_SetValue == NULL)
   return false;

  g_CAN_GetErrorText = (PCAN_GetErrorText) GetProcAddress(h_module, "CAN_GetErrorText");
  if(g_CAN_GetErrorText == NULL)
   return false;

  return true;
}
//
// Function: Unload DLL
// Parameter: none
// ret value: 0 if OK 
//
// unload the DLL and free all pointers
//

int UnloadDLL()
{
 if(g_i_DLL)
 {
  FreeLibrary(g_i_DLL);
  g_CAN_Initialize = NULL;
  g_CAN_Uninitialize = NULL;
  g_CAN_Reset = NULL;
  g_CAN_GetStatus = NULL;
  g_CAN_Read = NULL;
  g_CAN_Write = NULL;
  g_CAN_FilterMessages = NULL;
  g_CAN_GetValue = NULL;
  g_CAN_SetValue = NULL;
  g_CAN_GetErrorText = NULL;
  return 0;
 }
 return -1;
}