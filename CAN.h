#include <iostream>
#include <windows.h>
#include "pcanbasic.h"


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

// Function declaration
int LoadDLL();
int UnloadDLL();
bool GetFunctionAdress(HINSTANCE h_module);

//CAN Related Functions
void InitCAN(void);
void DeInitCAN(void);
void CAN_Write(TPCANMsg* MessageBuffer);
void CAN_Read(void);

//Structure Variable for doing memset
extern TPCANMsg ClearBuffer;

//  switch listen Onyl Mode on/off
TPCANStatus SetListenOnlyMode(TPCANHandle g_hChannel, bool mode);


extern TPCANMsg GlobalReadBuffer; 










