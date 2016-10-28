// SimpleSample.cpp : Defines the entry point for the console application.
//
// PCAN-Basic-C-Console SimpleSample.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <conio.h>
#include <thread>
#include "CAN.h"
#include "Message.h"

using namespace std;

void Process_Msg(void)
{
	/*
	Do processing of Received Message
	and clear the Buffer
	*/
	if(GlobalReadBuffer.LEN > 0)
	{
		std::cout<<std::hex <<"ID:"<<((void *)GlobalReadBuffer.ID)<<" "<<"DLC:"<<((void *)GlobalReadBuffer.LEN)<<" "<<"MSGTYPE:"<<((void *)GlobalReadBuffer.MSGTYPE)<<std::endl;
	}
	memset(&GlobalReadBuffer,0,sizeof(ClearBuffer));
}

void Readloop(void)
{
	CAN_Read();
}

bool getInput(char *c)
{
	if (_kbhit())
	{
		*c = _getch();
		return true;
		// A key was pressed
	}else{
		return false;
		// A key wasn't pressed
	}
}

int main(int argc, char *argv[])
{
	int ret,i;
	char Exitkey;
	
	ret = LoadDLL();
	if(ret!=0)
	{
		cout<<"Error Loading PCAN DLL"<<endl;
		exit(-1);
	}
	InitCAN();
	
	for(i=0; i<10; i++)
	{
		CAN_Write(&DiagnosticMessage);
	}
	thread t2(Readloop);
	t2.detach();
	while(!getInput(&Exitkey))
	{
		if(GlobalReadBuffer.LEN > 0)
		{
			Process_Msg();
		}
	}
	DeInitCAN();

	return 0;
}

