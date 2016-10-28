#include <iostream>
#include <windows.h>
#include "CAN.h"

//Declaration
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

//DLL Instance Handle
HINSTANCE g_i_DLL;
// name of DLL
LPCWSTR g_LibFileP = L"PCANBasic";

// TPCANHandle
TPCANHandle g_hChannel = PCAN_USBBUS1;
TPCANBaudrate g_Baudrate;

TPCANStatus CANStatus;
TPCANMsg ReadMessageBuffer;  
TPCANTimestamp MessageTimeStamp;

TPCANMsg GlobalReadBuffer;

//Buffer for storing Data
char Buffer[1024];
char *pBuffer;

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
		//g_i_DLL = LoadLibrary(g_LibFileName);
		g_i_DLL = LoadLibrary(g_LibFileP);
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

bool GetFunctionAdress(HINSTANCE h_module)
{
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

/***************************************************************************************

Intialize CAN Module

****************************************************************************************/
void InitCAN(void)
{
	CANStatus = g_CAN_Initialize(g_hChannel, PCAN_BAUD_500K , 0, 0, 0); 
	if(CANStatus!=PCAN_ERROR_OK)
	{
		std::cout<<"Error while Init CAN Interface: "<<CANStatus<<std::endl;
		UnloadDLL();
		exit(-1);
	}
}

/***************************************************************************************

DeIntialize CAN Module

****************************************************************************************/
void DeInitCAN(void)
{
	UnloadDLL();
}

/***************************************************************************************

Write a CAN Message

****************************************************************************************/
void CAN_Write(TPCANMsg* MessageBuffer)
{
	CANStatus = g_CAN_Write(g_hChannel,MessageBuffer);
	if(CANStatus != PCAN_ERROR_OK)
	{
		std::cout<<"Erro while sending CAN Frame"<<std::endl;
		g_CAN_GetErrorText(CANStatus, 0,pBuffer);
		std::cout<< Buffer<<std::endl;
	}	
}

/***************************************************************************************

Raed CAN Message or Status

****************************************************************************************/
void CAN_Read(void)
{
	int i=0;
	while(true)
	{
		CANStatus = g_CAN_Read(PCAN_USBBUS1, &ReadMessageBuffer, &MessageTimeStamp);
		while(CANStatus != PCAN_ERROR_QRCVEMPTY)
		{
			if(ReadMessageBuffer.MSGTYPE == PCAN_MESSAGE_STATUS)
			{
			std::cout<<"We received a Status Message - please check the Databytes for more information."<<std::endl;
			}
			else
			{
			if(CANStatus == PCAN_ERROR_OK) //now we have e real CAN Frame..
			{	
				if (ReadMessageBuffer.LEN > 0)  // A DLC could also be 0..but we only show ID´s with DATA
				{
					// report other messages
					GlobalReadBuffer = ReadMessageBuffer;
					//for (i = 0; i < ReadMessageBuffer.LEN; i++)
						//std::cout<<((void *)ReadMessageBuffer.DATA[i])<<std::ends;
				}
				memset(&ReadMessageBuffer,0,sizeof(ClearBuffer));
			} //endif STATUS OK
		}//else Status Message
		Sleep (2); 
		CANStatus = g_CAN_Read(PCAN_USBBUS1, &ReadMessageBuffer, &MessageTimeStamp);
		}
	}
}









