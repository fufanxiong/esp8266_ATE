/*	TestItem.cpp
 */

#include "stdafx.h"
#include "CommHead.h"
#include "Newdevice.h"
#include "Config.h"
#include "PD218.h"
#include "PD218Dlg.h"
#include "err.h"
#include "GPIB.h"
#include "tset.h"
#include "TestItem.h"
#include "TestHeader.h"
#include "soundIOlib.h"
#include "TestEngine.h"
#include "TestFlash.h"
#include "Dongle.h"
#include "mtk_lib.h"
#include "uEnergyTest.h"
//#include "AgN4010BT4API.h"
//#include "AgN4010BT4_PublicDefine.h"

#include <fstream.h>
#include <string>
#include <stdarg.h>
#include <direct.h>
#include <stdio.h> 
#include <stdlib.h>
#include <math.h>

#include "agilent_n4010_wlantest.h"

#pragma comment (lib,"TestEngine.lib")
#pragma comment (lib,"TestFlash.lib")
#pragma comment (lib,"uEnergyTest.lib")
//#pragma comment (lib,"AgN4010BT4API.lib")

//MBIAS_IO
#define MIC_BIAS_A_gpio		    'B'
#define MIC_BIAS_A_igpio		12

#define MIC_BIAS_B_gpio		    'D'
#define MIC_BIAS_B_igpio		 8

//AIO
#define AIO_0_gpio		'B'
#define AIO_0_igpio		9

#define AIO_1_gpio		'B'
#define AIO_1_igpio		8

#define LED_0_gpio		'B'
#define LED_0_igpio		 7
#define LED_1_gpio		'B'
#define LED_1_igpio	     6
#define LED_2_gpio		'B'
#define LED_2_igpio		 5


//using namespace std;

uint32 ispi_key[4] = {0x72656566,0x6F6E6574,0x776F7468,0x6F757266};
#ifdef set_8711
char gpio[] =  {  'D','D','D','D','D','D',    //PIO0-5
                  'C','C','C',                //PIO6-8
                  'A','A','A','A','A','C','C', //PIO9-15
				  'A','A','A',              // no use
                  'D','D',                 //PIO19-20
                  'C','C','D','D','D','D','D','D', //PIO21-28
				  'B','B','B',               //PIO29-31     
};
#else
char gpio[] =  {  'D','D','D','D','D','D',    //PIO0-5
                  'C','C','C',                //PIO6-8
                  'A','A','A','A','A','C','C', //PIO9-15
				  'C','C','D',              // no use
                  'D','D',                 //PIO19-20
                  'D','D','D','D','D','D','D','D', //PIO21-28
				  'B','B','B',               //PIO29-31     
};	
#endif

char rtl87xx_gpio1[] =  
{ 		   	'R','G','B','C','W','A',    //PIO0-5
                  	'C','A','C',                //PIO6-8
                  	'0','0','0','0','0','0','0', //PIO9-15
			'0','0','0',              // no use
                  	'0','0',                 //PIO19-20
                  	'0','0','0','0','0','0','0','0', //PIO21-28
			'0','0','0',               //PIO29-31     
};


char rtl87xx_gpio2[] =  
{  			'W','W','W','W','W','3',    //PIO0-5
                  	'3','5','0',                //PIO6-8
                  	'0','0','0','0','0','0','0', //PIO9-15
			'0','0','0',              // no use
                  	'0','0',                 //PIO19-20
                  	'0','0','0','0','0','0','0','0', //PIO21-28
			'0','0','0',               //PIO29-31     
};
#ifdef set_8711
int  igpio[] = {  5,4,3,2,1,0,     //PIO0-5
                  12,11,10,        //PIO6-8
                  12,11,10,9,8,9,8,     //PIO9-15
				  0,0,0,                 // no use
				  7,6,                //PIO19-20
				  7,6,15,14,13,12,11,10,   //PIO21-28
				  7,6,5,        //PIO29-31
};
#else
int  igpio[] = {  5,4,3,2,1,0,     //PIO0-5
                  12,11,10,        //PIO6-8
                  12,11,10,9,8,9,8,     //PIO9-15
				  7,6,15,                 // no use
				  14,13,                //PIO19-20
				  12,11,10,0,0,0,0,0,   //PIO21-28
				  0,0,0,        //PIO29-31
};	
#endif

char mcu_mbias[][3] =
{
    {'D', 8, 'A'},       //Mbias-a
    {'B', 12, 'B'}       //Mbias-b
};


//--------------------------------------------------------------------------------------------------------------
//yhl 2018/10/25
int  parse_mac_txt(void);
int  esp_send_mac(void);
void delchar(char*p,char x);
//2018/10/28 yhl
char venorid_send[64]={0};
 char secret_send[64]={0};
char pk_send[64]={0};
 char mac_send[256]={0};
char esp_mac[64]={0};
//--------------------------------------------------------------------------------------------------------------


bool spi_unlock(unsigned long delay_time,CString &file_name, CString &trans)
{  
    SECURITY_ATTRIBUTES sa;  
    HANDLE hRead,hWrite;  
      
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);  
    sa.lpSecurityDescriptor = NULL;  
    sa.bInheritHandle = TRUE;  
      
    if (!CreatePipe(&hRead,&hWrite,&sa,0))  
    {  
        return FALSE;
    }

    char command[256];
    strcpy(command,"cmd.exe /C ");
    strcat(command," spiunlock.exe unlock ");
    strcat(command,file_name.GetBuffer(0));
	strcat(command, trans.GetBuffer(0));

    STARTUPINFO si; 
    PROCESS_INFORMATION pi;  
      
    si.cb = sizeof(STARTUPINFO);  
    GetStartupInfo(&si);  
      
    si.hStdError = hWrite; //把创建进程的标准错误输出重定向到管道输入  
    si.hStdOutput = hWrite; //把创建进程的标准输出重定向到管道输入  
    si.wShowWindow = SW_HIDE;  
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;  
      
    //关键步骤，CreateProcess函数参数意义请查阅MSDN  
    if (!CreateProcess(NULL, command,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))      
    {
        CloseHandle(hWrite);      
        CloseHandle(hRead);   
        return FALSE;     
    }
    CloseHandle(hWrite); 

	char *buffer = new char[1024];
    DWORD bytesRead;
    bool success = 0;
	bool ck_result = true;
	DWORD op_idx = 0;
    while(true)
    {
		bytesRead = 0;
        if (ReadFile(hRead,&buffer[op_idx],1024,&bytesRead,NULL) == NULL) break;
		bytesRead += op_idx;

		op_idx = 0;
		if(ck_result) 
		{
			while(bytesRead >= 7) 
			{
				if ((buffer[op_idx] == 'S') && (buffer[op_idx+1] == 'u') &&
					(buffer[op_idx+2] == 'c') && (buffer[op_idx+3] == 'c') &&
					(buffer[op_idx+4] == 'e') && (buffer[op_idx+5] == 's') &&
					(buffer[op_idx+6] == 's'))
				{
					success = 1;
					ck_result = 0;
					break;
				}
				else if ((buffer[op_idx] == 'F') && (buffer[op_idx+1] == 'a') &&
					(buffer[op_idx+2] == 'i') && (buffer[op_idx+3] == 'l') &&
					(buffer[op_idx+4] == 'e') && (buffer[op_idx+5] == 'd'))
				{
					success = 0;
					ck_result = 0;
					break;
				}

				op_idx += 1;
				bytesRead -= 1;
			}
		}

		if(ck_result) 
		{
			int i = 0;
			while(bytesRead) 
			{
				buffer[i++] = buffer[op_idx++];
				bytesRead -= 1;
			}
			op_idx = i;
		}
		else
		{
			op_idx = 0;
		}
	}

	delete []buffer;
    CloseHandle(hRead);
	Sleep(delay_time);
	if(success == 1) return(true);
	return(false);
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
bool execute_bat(char *command, STARTUPINFO *si, PROCESS_INFORMATION *pi)
{
   //if (!CreateProcess(NULL,"cmd.exe /C nvscmd.exe -usb 0 erase",NULL,NULL,TRUE,NULL,NULL,NULL,si,pi))
	if (!CreateProcess(NULL,command,NULL,NULL,TRUE,NULL,NULL,NULL,si,pi))      
    { 
        return FALSE;     
    }

    return (true);
}


//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
bool read_bat(HANDLE hRead)
{
    char *buffer = new char[1024];
    DWORD bytesRead;
    bool success = 0;
	bool ck_result = true;
	DWORD op_idx = 0;
    while(true)
    {
		bytesRead = 0;
        if (ReadFile(hRead,&buffer[op_idx],1024,&bytesRead,NULL) == NULL) break;
		bytesRead += op_idx;

		op_idx = 0;
		if(ck_result) 
		{
			while(bytesRead >= 7) 
			{
				if ((buffer[op_idx] == 'S') && (buffer[op_idx+1] == 'u') &&
					(buffer[op_idx+2] == 'c') && (buffer[op_idx+3] == 'c') &&
					(buffer[op_idx+4] == 'e') && (buffer[op_idx+5] == 's') &&
					(buffer[op_idx+6] == 's'))
				{
					success = 1;
					ck_result = 0;
					break;
				}
				else if ((buffer[op_idx] == 'F') && (buffer[op_idx+1] == 'a') &&
					(buffer[op_idx+2] == 'i') && (buffer[op_idx+3] == 'l') &&
					(buffer[op_idx+4] == 'e') && (buffer[op_idx+5] == 'd'))
				{
					success = 0;
					ck_result = 0;
					break;
				}

				op_idx += 1;
				bytesRead -= 1;
			}
		}

		if(ck_result) 
		{
			int i = 0;
			while(bytesRead) 
			{
				buffer[i++] = buffer[op_idx++];
				bytesRead -= 1;
			}
			op_idx = i;
		}
		else
		{
			op_idx = 0;
		}
	}

	delete []buffer;
 
	if (success == 1) return (true);
    return (false);
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
bool flash_extern(unsigned long delay_time,CString &file_name, CString port_num)
{
	SECURITY_ATTRIBUTES sa;  
    HANDLE hRead, hWrite; 
	char command[256];
	CString vp_cmd;
	bool success = 0;
      
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;  
    sa.bInheritHandle = TRUE;  
      
    if (!CreatePipe(&hRead,&hWrite,&sa,0))  
    {   
		return FALSE;
    }	
	 
	//format erase cmd
	//vp_cmd.Format("cmd.exe /C nvscmd.exe -usb %d erase", port_num); 
    //strcpy(command, vp_cmd);

    strcpy(command,"cmd.exe /C ");
    strcat(command," nvscmd.exe -usb ");
    strcat(command,port_num.GetBuffer(0));
	strcat(command," erase");
	
	STARTUPINFO si;  
    PROCESS_INFORMATION pi;       
    si.cb = sizeof(STARTUPINFO);  
    GetStartupInfo(&si);  
      
    si.hStdError = hWrite; //把创建进程的标准错误输出重定向到管道输入  
    si.hStdOutput = hWrite; //把创建进程的标准输出重定向到管道输入  
    si.wShowWindow = SW_HIDE;  
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;  

	if(!execute_bat(&command[0], &si, &pi))
	{
	    CloseHandle(hWrite);      
        CloseHandle(hRead); 
		return (FALSE);
	}
    CloseHandle(hWrite);
	if(read_bat(hRead)) success = 1;
	CloseHandle(hRead);
    if(success != 1) return(false);	
	//return(true);

//#if 0	
	//format burn cmd
	if (!CreatePipe(&hRead,&hWrite,&sa,0))  
    {   
		return FALSE;
    }

	memset(command, 0, 256*sizeof(char));    
	strcpy(command, "cmd.exe /C ");
    strcat(command, " nvscmd.exe -usb ");
    strcat(command, port_num.GetBuffer(0));
	strcat(command, " burn ");
	strcat(command, file_name.GetBuffer(0));
	strcat(command," all");

	if(!execute_bat(&command[0], &si, &pi))
	{
		CloseHandle(hWrite);      
        CloseHandle(hRead); 
	    return (FALSE);
	}	
    
	CloseHandle(hWrite); 
	if(read_bat(hRead)) success = 1;
	CloseHandle(hRead);
	Sleep(delay_time);
	if(success == 1) return(true);
	return(false);
}



//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
const CString NULL_RANGE = "--  --  --";
int CTestItem::ItemSum = 0;
bool CTestItem::pwr_on_fail = false;
bool CTestItem::have_fail_item = false;
bool CTestItem::led0_port_update = true;
bool CTestItem::led1_port_update = true;
/* fu add for rf para save*/
int CTestItem::fre_index[2] = {0,0};
int CTestItem::pwr_low_index[2] = {0,0};
int CTestItem::pwr_middle_index[2] = {0,0};
int CTestItem::pwr_high_index[2] = {0,0};
double CTestItem::freoffset_para[2] = {0.0,0.0};
double CTestItem::low_avpower[2] = {0.0,0.0};
double CTestItem::middle_avpower[2] = {0.0,0.0};
double CTestItem::high_avpower[2] = {0.0,0.0};
double CTestItem::low_average_evm[2] = {0.0,0.0};
double CTestItem::middle_average_evm[2] = {0.0,0.0};
double CTestItem::high_average_evm[2] = {0.0,0.0};


CTestItem::CTestItem(const char* pname, testSet *Tset, int ID)
{
    dNode.cur = this;
    identifier = ID;
	tset = Tset;
	index = tset->tItemSum++;
    name = pname;
    result = false;
    ItemTFMask = 0;

	terminate_test_if_err_accur = true;

	err_min = ERR_MIN - 1;
    err_max = ERR_MIN - 1;

}

void CTestItem::ClearResult(void)
{
	param = "";
	result = false;
	is_tested = false;
}


void CTestItem::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);
	tset->tlist->AddItem(itemstr,name,NULL_RANGE,"", "");
}

int CTestItem::ItemTestReady(ConfigFile &cfg)
{	
	return(NO_ERR);
}


int CTestItem::ItemTestClean(ConfigFile &cfg)
{
	return(NO_ERR);
}


int CTestItem::TestOp(ConfigFile &cfg)
{
	param = "";
	result = false;
	is_tested = true;

	Sleep(2000);
	return(true);
};


void CTestItem::update_range(void)
{
}


/*
CString CTestItem::format_result(void)
{
	format_record_par(param);
	return (param);
}*/


void CTestItem::show_message(void)
{
	TSET_MSG(MSG_SHOW_PAR,tset);
}


void CTestItem::record_header(ofstream &fs)
{
        fs<<name<<", ";
}


void CTestItem::record_result(ofstream &fs)
{
        format_record_par(param);
        fs<<param<<",";
        if(result) fs<<"/P,";
        else if(!is_tested) fs << "NC,";
        else fs << "/F,";
}


void CTestItem::wait_continue(void)
{
        pMainWnd->waitContinueOp(true);
        while(pMainWnd->wait_continue)
        {
            if(pMainWnd->terminal_test) break;
            Sleep(100);
        }
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_delay::CTObj_delay(const char* pname,testSet *Tset,int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
	delay_time = 0;//cfg.Vulong(slaveXtalClrOffsetDelay);
}

void CTObj_delay::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	range.Format("delay:%d",delay_time);
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}

int CTObj_delay::TestOp(ConfigFile &cfg)
{
	result = true;
	if(!delay_time) return(true);

	is_tested = true;
	unsigned long loss_time = 0;
	unsigned long delay_time_temp = delay_time;
	if(delay_time_temp > 1000) {
	    Sleep(1000);

	    delay_time_temp -= 1000;
	    loss_time += 1000;
	    param = "";
	    param.Format("delay:%d",loss_time);
	    show_message();
	}
	if(delay_time_temp) Sleep(delay_time_temp);
	loss_time += delay_time_temp;

	param = "";
	param.Format("delay:%d ok",loss_time);
	return(NO_ERR);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_conn_spi::CTObj_conn_spi(const char* pname, testSet *Tset,int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
	connect_times = cfg.Vulong(slaveConnectTimes);

}

void CTObj_conn_spi::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	if(tset->dev_id == TEST_DEV_ID_A)
	{
       range.Format("USB SPI(0) C:%d", connect_times);
	}
	else 
	{
       range.Format("USB SPI(1) C:%d", connect_times);
	}

	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0), "","");
}

int CTObj_conn_spi::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;
	CsrHandle_t iHandle(0);  
	unsigned long i = 0;
	tset->xtal_adj_is_tested = false;  ///xtal adj test set false

    if(!tset->pSpiObj->spi_allocated)
	{
		param = "dut2 no spi allocated";
		return (ERRSTOP);
	}
	


	int32 retVal = UET_ERR_GENERAL;
	for(i=0;i<connect_times;i++) {
		param = "";
		param.Format("try at:%s %d times",tset->pSpiObj->spi_port->port_name(),i+1);
		show_message();
		
		retVal = uetOpen(tset->pSpiObj->spi_port->port_num(), "./cskey_db.xml", &iHandle);
		if(retVal == UET_OK)
		{
			tset->pSpiObj->iHandle = iHandle;
			param =  "Connected at ";
			if(tset->dev_id == TEST_DEV_ID_A) {
				param += "USB SPI(0)";
			}else {
				param += "USB SPI(1)";
			}
			result = true;
			break;
		}
		else if(iHandle != 0)
		{
			printf("%s",uetGetLastError(iHandle));
			uetClose(iHandle);
		}
		Sleep(100);
	}
	if(retVal == UET_OK) result = true;
    else {
		/*
		param =  "try connect at ";
		if(tset->dev_id == TEST_DEV_ID_A) {
		  param += "USB SPI(0)";
		}else{
		  param += "USB SPI(1)";
		}
		param += " err";*/
		param.Format("result codes:%d",retVal);
	}

	//add CSR
	if(!result) return(ERRSTOP);

	// Read the CS settings for the design from the configuration settings file
	//retVal = uetCsCacheReadFromFile(handle, "boardSettings.keyr");
	
	// Read into the cache from chip (CS only)
	retVal = uetCsCacheReadFromChip(tset->pSpiObj->iHandle, UET_NVM_TYPE_SPIFLASH, 1);
	
	// Read the firmware image into the CS cache from a file
	//retVal = uetCsCacheReadFromFile(tset->pSpiObj->iHandle, "./flash.xuv");

	if(retVal != UET_OK)
	{
		printf("Error reading from file: %s\n",uetGetLastError(tset->pSpiObj->iHandle));
	}
	retVal = uetLoadPtFirmware(tset->pSpiObj->iHandle, "./ptest_CSR101x.xbv", 1);
	if(retVal != UET_OK)
	{
		printf("Error loading ptest firmware: %s\n",uetGetLastError(tset->pSpiObj->iHandle));
	}


	return(result);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#if 1
volatile bool dut_pwr_is_on = false;
//volatile bool dut_pwr_atset_is_on = true;
//volatile bool dut_pwr_btset_is_on = true;

CTObj_pwrDut::CTObj_pwrDut(const char* pname, testSet *Tset,int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{

}


void CTObj_pwrDut::gen_prompt()
{
   	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	if(tset->dev_id == TEST_DEV_ID_A)
	   range.Format("U5 PE14 pull high");
	else
       range.Format("U5 PE14 pull high");
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");

}

int CTObj_pwrDut::ItemTestReady(ConfigFile &cfg)
{
    mutex_holde = false;
	
	if (!pMainWnd->glbResource_get(GRESOURCE_MCU2))
	{
		param = "u5 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_holde = true;
	return(NO_ERR);
}


int CTObj_pwrDut::ItemTestClean(ConfigFile &cfg)
{

	if (mutex_holde)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU2);
		mutex_holde = false;
	}
	
    return(NO_ERR);
}

CTObj_pwrDut::TestOp(ConfigFile &cfg)
{
    is_tested = true;
	result = false;

    if(!pMCU2_Com)
	{
	    param = "null u5 Com";
		return (false);
	}

	/////////////////////
	
	if(dut_pwr_is_on)
	{
	   param = "powered";
	   result = true;
	   return(NO_ERR);
	}
	/*if(!pMCU2_Com->set_dut_pwr_off(1, MCU_PIO_DUT_PWR, MCU_PIO_NUM_DUT_PWR))
	{ 
	   param = "set dut pwr err";
	   return(ERR_TERMINAL);
	}	
	Sleep(100);
	*/
	if(!pMCU2_Com->set_dut_pwr_on(1, MCU_PIO_DUT_PWR, MCU_PIO_NUM_DUT_PWR))
	{ 
	   param = "set dut pwr err";
	   return(ERR_TERMINAL);
	}
	   
	if(!pMCU2_Com->set_dut_spi_on(1, MCU_PIO_SPI,MCU_PIO_NUM_SPI))
	{
		param= "set dut spi on err";
		return(ERR_TERMINAL);
	}
	   dut_pwr_is_on = true;
	   result = true;
	   
	   param = "dut connect pwr ok";
	   //param.Format("dut%d connect pwr ok", ((tset->dev_id ==TEST_DEV_ID_A)?1:2));
	   return(NO_ERR);
}

#endif
#if 1
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_Battery_on::CTObj_Battery_on(const char *pname, testSet *Tset, int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
}


void CTObj_Battery_on::gen_prompt()
{
  char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	if(tset->dev_id == TEST_DEV_ID_A)
	  range.Format("U5 PE13 pull high");
	else
	  range.Format("U5 PE8 pull high");
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");

}

int CTObj_Battery_on::ItemTestReady(ConfigFile &cfg)
{
    mutex_hodle1 = false;
	mutex_hodle2 = false;
 
	if(!pMainWnd->glbResource_get(GRESOURCE_MCU1))
	{
		param = "u18 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_hodle1 = true;

	if(!pMainWnd->glbResource_get(GRESOURCE_MCU2))
	{
		param = "u5 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_hodle2 = true;
	return(NO_ERR);
}


int CTObj_Battery_on::ItemTestClean(ConfigFile &cfg)
{	   
	if (mutex_hodle1)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU1);
        mutex_hodle1 = false;
	}
	
	if (mutex_hodle2)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU2);
        mutex_hodle2 = false;
	}
    return(NO_ERR);
}

#define DEV_OP_PROMP(str) do{ if(!pMainWnd->terminal_test) UPDATE_STATIC(IDC_CurTestObj,str); } while(0)

int CTObj_Battery_on::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;
	CString ShowMessage;
	CString temp;
	int i=0;

		//path route to N4010A
	if(!pMCU1_Com->set_rout_4010_on(0, MCU_PIO_DUT_4010_METER, MCU_PIO_NUM_DUT_4010_METER))
	{
	    param ="switch route to 4010 err";
        return (false);
	}

	if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pMCU1_Com->set_4010_route_dut1(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut1 err";
			return (false);
		}	
	}
	else
	{
		if(!pMCU1_Com->set_4010_route_dut2(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut2 err";
			return (false);
		}
	}

	if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY))
		{
			param = "set dut battery err";
			return (false);
		}

		if(!pMCU2_Com->set_dut_current_on(1, MCU_PIO_DUT1_CURRENT,MCU_PIO_NUM_DUT1_CURRENT))
		{
			param = "set dut current err";
			return (false);
		}
		
		if(pRTL8711_1_Com)
		{					
			delete(pRTL8711_1_Com);
			pRTL8711_1_Com = NULL;
		}
		//上电的时候并不能保证在normal模式，mp模式下connect一定会失败(因为需要回复一条ATST的指令)
		//接下来的读版本，io口都是确定切换到normal下,这里能保证com口获取到。
		// 5次重连机会
	
		for(i=0;i<5;i++)
		{
			Sleep(100);
			//串口线连接到板上的COM口。
			temp = "ESP8266_A 正在分配COM口...";
			DEV_OP_PROMP(temp);
			pRTL8711_1_Com = new RTL87XX_COM("ESP8266_A"); 
			if(pRTL8711_1_Com->connect("", 0)) 
			{	
				UPDATE_TITLE();
				//正常连接后要一定延时,让8711运行稳定再操作
				//Sleep(1000);
				Sleep(100);
				break;
			}
			else
			{
				delete(pRTL8711_1_Com);
				pRTL8711_1_Com = NULL;
				if(i==4)
				{
					ShowMessage += "ESP8266_A 分配COM口失败!!\n";
					return (false);
				}
			}			
		}
	
	}	
	else
	{
		if(!pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY))
		{
			param = "set dut battery err";
			return (false);
		}

		if(!pMCU1_Com->set_dut_current_on(0, MCU_PIO_DUT2_CURRENT,MCU_PIO_NUM_DUT2_CURRENT))
		{
			param = "set dut current err";
			return (false);
		}
		
		if(pRTL8711_2_Com)
		{					
			delete(pRTL8711_2_Com);
			pRTL8711_2_Com = NULL;
		}
		
		// 5次重连机会
		
		for(i=0;i<5;i++)
		{
			Sleep(100);
			//串口线连接到板上的COM口。
			temp = "ESP8266_B 正在分配COM口...";
			DEV_OP_PROMP(temp);
			pRTL8711_2_Com = new RTL87XX_COM("ESP8266_B"); 
			if(pRTL8711_2_Com->connect("", 0)) 
			{	
				UPDATE_TITLE();
				//正常连接后要一定延时,让8711运行稳定再操作
				//Sleep(1000);
				Sleep(100);
				break;
			}
			else
			{
				delete(pRTL8711_2_Com);
				pRTL8711_2_Com = NULL;
				if(i==4)
				{
					ShowMessage += "ESP8266_B 分配COM口失败!!\n";
					return (false);
				}
			}
		}
	
	}	

	 //param = "dut battery on ok";
	 param.Format("dut%d battery on ok", ((tset->dev_id ==TEST_DEV_ID_A)?1:2));
	 result = true;
	 return (result);

}
#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_Battery_off::CTObj_Battery_off(const char *pname, testSet *Tset, int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
}


void CTObj_Battery_off::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);
	
	CString range;
	if(tset->dev_id == TEST_DEV_ID_A)
		range.Format("U5 PE13 pull low");
	else
		range.Format("U5 PE8 pull low");
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
	
}

int CTObj_Battery_off::ItemTestReady(ConfigFile &cfg)
{
    mutex_hodle1 = false;
	mutex_hodle2 = false;
	
	if(!pMainWnd->glbResource_get(GRESOURCE_MCU1))
	{
		param = "u18 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_hodle1 = true;
	
	if(!pMainWnd->glbResource_get(GRESOURCE_MCU2))
	{
		param = "u5 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_hodle2 = true;
	return(NO_ERR);
}


int CTObj_Battery_off::ItemTestClean(ConfigFile &cfg)
{	   
	if (mutex_hodle1)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU1);
        mutex_hodle1 = false;
	}
	
	if (mutex_hodle2)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU2);
        mutex_hodle2 = false;
	}
    return(NO_ERR);
}


int CTObj_Battery_off::TestOp(ConfigFile &cfg)
{
    is_tested = true;
	result = false;
	
	if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY))
		{
			param = "set dut battery err";
			return (false);
		}
		#if 1
		if(pRTL8711_1_Com)
		{
			delete(pRTL8711_1_Com);
			pRTL8711_1_Com = NULL;
		}
		#endif
	}
	else
	{
		if(!pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY))
		{
			param = "set dut battery err";
			return (false);
		}
		#if 1
		if(pRTL8711_2_Com)
		{
			delete(pRTL8711_2_Com);
			pRTL8711_2_Com = NULL;
		}
		#endif
	}
	
	param.Format("dut%d battery off ok", ((tset->dev_id ==TEST_DEV_ID_A)?1:2));
	result = true;
	return (result);
	
}




//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_dut_into_mp_mode::CTObj_dut_into_mp_mode(const char *pname, testSet *Tset, int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
}


void CTObj_dut_into_mp_mode::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);
	
	CString range;
	if(tset->dev_id == TEST_DEV_ID_A)
		range.Format("dut A into MP mode");
	else
		range.Format("dut B into MP mode");
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
	
}

int CTObj_dut_into_mp_mode::ItemTestReady(ConfigFile &cfg)
{
    mutex_hodle1 = false;
	mutex_hodle2 = false;
	
	if(!pMainWnd->glbResource_get(GRESOURCE_MCU1))
	{
		param = "u18 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_hodle1 = true;
	
	if(!pMainWnd->glbResource_get(GRESOURCE_MCU2))
	{
		param = "u5 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_hodle2 = true;
	return(NO_ERR);
}


int CTObj_dut_into_mp_mode::ItemTestClean(ConfigFile &cfg)
{	   
	if (mutex_hodle1)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU1);
        mutex_hodle1 = false;
	}
	
	if (mutex_hodle2)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU2);
        mutex_hodle2 = false;
	}
    return(NO_ERR);
}


//yhl 2018/10/28
int CTObj_dut_into_mp_mode::TestOp(ConfigFile &cfg)
{
    is_tested = true;
	result = false;
	CString cmd_str;
	CString cmd_set;
	int i=0,j=0;
	if(tset->dev_id == TEST_DEV_ID_A)
	{
		for(i=0;i<3;i++)
		{
			//set pio21 推挽输出
			cmd_str.Format("MCU+SET(1,%c,%04x,5)\r\n", gpio[0], (1 << igpio[21]));//arg2=A~G,arg3=0001~FFFF
			if(!pMCU2_Com->send_and_verify(cmd_str, 0, "MCU+OK\r\n", 0))
			{
			   param.Format("MP mcu1 set PIO err");
				//return (false);
				j=1;
			}
			Sleep(50);

			cmd_set.Format("MCU+WR(1,%c,%04x,%04x)\r\n", gpio[0], (1 << igpio[21]),(0000));

			if(!pMCU2_Com->send_and_verify(cmd_set, 0, "MCU+OK\r\n", 0))
			{
			   param.Format("MP mcu1 write PIO err");
			   j=1;
			//return (false);
			}
			Sleep(50);
			if(j==0)break;
		}
		if(i>=3)return (false);
	}
	else
	{
		for(i=0;i<3;i++)
		{
			//set pio21 推挽输出
			cmd_str.Format("MCU+SET(0,%c,%04x,5)\r\n", gpio[0], (1 << igpio[21]));//arg2=A~G,arg3=0001~FFFF
			if(!pMCU1_Com->send_and_verify(cmd_str, 0, "MCU+OK\r\n", 0))
			{
			   	param.Format("MP mcu2 set PIO21 err");
				//return (false);
				 j=1;
			}
	
			Sleep(50);

			cmd_set.Format("MCU+WR(0,%c,%04x,%04x)\r\n", gpio[0], (1 << igpio[21]),(0000));

			if(!pMCU1_Com->send_and_verify(cmd_set, 0, "MCU+OK\r\n", 0))
			{
			   param.Format("MP mcu2 write PIO21 err");
			    j=1;
				//return (false);
			}
			Sleep(50);
			if(j==0)break;
		}
		if(i>=3)return (false);	
	}
	param.Format("set MP mode ok");
	result = true;
	return (result);

}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_dut_into_normal_mode::CTObj_dut_into_normal_mode(const char *pname, testSet *Tset, int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
}


void CTObj_dut_into_normal_mode::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);
	
	CString range;
	if(tset->dev_id == TEST_DEV_ID_A)
		range.Format("dut A into normal mode");
	else
		range.Format("dut B into normal mode");
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
	
}



int CTObj_dut_into_normal_mode::TestOp(ConfigFile &cfg)
{
    is_tested = true;
	result = false;
	CString cmd_str;	
CString cmd_set;
char	ate_start[]="ate start\r\n";
char	rx_buf[50]={0};
int i=0,j=0;
	if(tset->dev_id == TEST_DEV_ID_A)
	{
		for(i=0;i<3;i++)
		{
			//set pio21 推挽输出
			cmd_str.Format("MCU+SET(1,%c,%04x,5)\r\n", gpio[0], (1 << igpio[21]));//arg2=A~G,arg3=0001~FFFF
			if(!pMCU2_Com->send_and_verify(cmd_str, 0, "MCU+OK\r\n", 0))
			{
			   param.Format("NOR mcu1 set PIO err");
			   j=1;
			//return (false);
			}
			Sleep(50);

			cmd_set.Format("MCU+WR(1,%c,%04x,%04x)\r\n", gpio[0], (1 << igpio[21]),(1 << igpio[21]));

			if(!pMCU2_Com->send_and_verify(cmd_set, 0, "MCU+OK\r\n", 0))
			{
			   param.Format("NOR mcu1 write PIO err");
			   j=1;
			//return (false);
			}
			Sleep(50);
		       if(j==0)
			   break;
		}
		if(i>=3) return (false);

	}
	else
	{
		for(i=0;i<3;i++)
		{
			//set pio21 推挽输出
			cmd_str.Format("MCU+SET(0,%c,%04x,5)\r\n", gpio[0], (1 << igpio[21]));//arg2=A~G,arg3=0001~FFFF
			if(!pMCU1_Com->send_and_verify(cmd_str, 0, "MCU+OK\r\n", 0))
			{
			   	param.Format("NOR mcu2 set PIO21 err");
				//return (false);
			}
			Sleep(50);

			cmd_set.Format("MCU+WR(0,%c,%04x,%04x)\r\n", gpio[0], (1 << igpio[21]),(1 << igpio[21]));

			if(!pMCU1_Com->send_and_verify(cmd_set, 0, "MCU+OK\r\n", 0))
			{
			   param.Format("NOR mcu2 set PIO21 write err");
				//return (false);
			}
			Sleep(50);
			  if(j==0)
			   break;
		}	
		if(i>=3) return (false);
	}
	param.Format("set normal mode ok");
	result = true;
	return (result);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#if 1
volatile bool dut_spi_is_on = false;
volatile bool dut_spi_atset_is_on = false;
volatile bool dut_spi_btset_is_on = false;

CTObj_spi_switch_on::CTObj_spi_switch_on(const char* pname, testSet *Tset,int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{

}


void CTObj_spi_switch_on::gen_prompt()
{
   	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	if(tset->dev_id == TEST_DEV_ID_A)
	   range.Format("U5 PE11 pull low");
	else
       range.Format("U5 PE11 pull low");
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");

}

int CTObj_spi_switch_on::ItemTestReady(ConfigFile &cfg)
{
	mutex_holde1 = false;
	
	if (!pMainWnd->glbResource_get(GRESOURCE_MCU1))
	{
		param = "u18 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_holde1 = true;

    mutex_holde2 = false;
	
	if (!pMainWnd->glbResource_get(GRESOURCE_MCU2))
	{
		param = "u5 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_holde2 = true;
	return(NO_ERR);
}


int CTObj_spi_switch_on::ItemTestClean(ConfigFile &cfg)
{

	if (mutex_holde1)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU1);
		mutex_holde1 = false;
	}

	if (mutex_holde2)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU2);
		mutex_holde2 = false;
	}
	
    return(NO_ERR);
}

int CTObj_spi_switch_on::TestOp(ConfigFile &cfg)
{
       is_tested = true;
	   result = false;
	
       if(!pMCU2_Com)
	   {
		  param = "null u5 Com";
		  return (false);
	   }
		
	   if(tset->dev_id == TEST_DEV_ID_A)
	   {
		   //pMCU2_Com->set_dut_vreg_on(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG);
		   //pMCU2_Com->set_dut_vreg_off(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG);
		   
		   if(!pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY))
		   {
			   param = "set dut battery err";
			   return (false);
		   }
	   }
	   else
	   {
		   //pMCU1_Com->set_dut_vreg_on(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG);
		   //pMCU1_Com->set_dut_vreg_off(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG);
		   
		   //也是用U5
		   if(!pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY))
		   {
			   param = "set dut battery err";
			   return (false);
		   }
	   }
		
	   //烧录脚短路
	   if(tset->dev_id == TEST_DEV_ID_A)
	   {		   
		   if(!pMCU2_Com->set_dut_vreg_on(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG))
		   {
			   param = "burning feet short circuit err";
			   return (ERR_STOP);
		   }
		   
		   
	   }
	   else
	   {	

		   if(!pMCU1_Com->set_dut_vreg_on(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG))
		   {
			   param = "burning feet short circuit err";
			   return (ERR_STOP);
		   }
		   
	   }   

	   if(dut_spi_is_on)
	   {
		   if(tset->dev_id == TEST_DEV_ID_A)
		   {
			   dut_spi_atset_is_on = true;
		   }
		   else
		   {
				dut_spi_btset_is_on = true;
		   }
		   param = "spi switch on";
		   result = true;
		   return(NO_ERR);
	   }
	   
	   if(!pMCU2_Com->set_dut_spi_on(1, MCU_PIO_SPI, MCU_PIO_NUM_SPI))
	   { 
		   param = "set dut spi on err";
		   return(ERR_TERMINAL);
	   }
	   if(tset->dev_id == TEST_DEV_ID_A)
	   {
		   dut_spi_atset_is_on = true;
	   }
	   else
	   {
		   dut_spi_btset_is_on = true;
	   }
	   dut_spi_is_on = true;
	   result = true;
	   
	  // Sleep(2000);
	   param = "spi switch on ok";
	   return(NO_ERR);
}
#endif
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#if 1
//volatile bool dut_spi_is_on = false;
//volatile bool dut_spi_atset_is_on = true;
//volatile bool dut_spi_btset_is_on = true;

CTObj_spi_switch_off::CTObj_spi_switch_off(const char* pname, testSet *Tset,int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{

}


void CTObj_spi_switch_off::gen_prompt()
{
   	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	if(tset->dev_id == TEST_DEV_ID_A)
	   range.Format("U5 PE11 pull high");
	else
       range.Format("U5 PE11 pull high");
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");

}

int CTObj_spi_switch_off::ItemTestReady(ConfigFile &cfg)
{
#if 0
    mutex_holde = false;
	
	if (!pMainWnd->glbResource_get(GRESOURCE_MCU2))
	{
		param = "u5 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_holde = true;
#endif
	return(NO_ERR);
}


int CTObj_spi_switch_off::ItemTestClean(ConfigFile &cfg)
{
	
	if (mutex_holde1)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU1);
		mutex_holde1 = false;
	}

	if (mutex_holde2)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU2);
		mutex_holde2 = false;
	}
	
    return(NO_ERR);
}

int CTObj_spi_switch_off::TestOp(ConfigFile &cfg)
{
       is_tested = true;
	   result = false;
	
	   if(tset->dev_id == TEST_DEV_ID_A)
	   {
		   dut_spi_atset_is_on = true;
		   if(dut_spi_btset_is_on)
		   {
				dut_spi_atset_is_on = false;
				while(dut_spi_btset_is_on)Sleep(100);
		   }

	   }
	   else
	   {
		   if(dut_spi_atset_is_on)
		   {
				dut_spi_btset_is_on = false;
				while(dut_spi_atset_is_on)Sleep(100);
		   }
	   }
		
	   mutex_holde1 = false;
	   
	   if (!pMainWnd->glbResource_get(GRESOURCE_MCU1))
	   {
		   param = "u18 gsource err";
		   return(ERR_TERMINAL);
	   }
		mutex_holde1 = true;

	   mutex_holde2 = false;
	   
	   if (!pMainWnd->glbResource_get(GRESOURCE_MCU2))
	   {
		   param = "u5 gsource err";
		   return(ERR_TERMINAL);
	   }
		mutex_holde2 = true;
		
		if(!pMCU1_Com)
		{
			param = "null u18 Com";
			return (false);
	   }

       if(!pMCU2_Com)
	   {
		  param = "null u5 Com";
		  return (false);
	   }

#if 0
	   if(tset->dev_id == TEST_DEV_ID_A)
	   {
		   //pMCU2_Com->set_dut_vreg_on(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG);
		   //pMCU2_Com->set_dut_vreg_off(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG);
		   
		   if(!pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY))
		   {
			   param = "set dut battery err";
			   return (false);
		   }
	   }
	   else
	   {
		   //pMCU1_Com->set_dut_vreg_on(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG);
		   //pMCU1_Com->set_dut_vreg_off(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG);
		   
		   //也是用U5
		   if(!pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY))
		   {
			   param = "set dut battery err";
			   return (false);
		   }
	   }
#endif		
	   //烧录脚断路
	   if(tset->dev_id == TEST_DEV_ID_A)
	   {		   
		   if(!pMCU2_Com->set_dut_vreg_off(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG))
		   {
			   param = "set dut vreg on err";
			   return (ERR_STOP);
		   }
		   
		   
	   }
	   else
	   {	

		   if(!pMCU1_Com->set_dut_vreg_off(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG))
		   {
			   param = "set dut vreg on err";
			   return (ERR_STOP);
		   }
		   
	   }   

	   if(dut_spi_is_on == false)
	   {
		   if(tset->dev_id == TEST_DEV_ID_A)
		   {
			   dut_spi_atset_is_on = false;
		   }
		   else
		   {
			   dut_spi_btset_is_on = false;
			}
		   param = "spi switch off";
		   result = true;
		   return(NO_ERR);
	   }
	   
	   if(!pMCU2_Com->set_dut_spi_off(1, MCU_PIO_SPI, MCU_PIO_NUM_SPI))
	   { 
		   param = "set dut spi off err";
		   return(ERR_TERMINAL);
	   }
	   dut_spi_is_on = false;
	   if(tset->dev_id == TEST_DEV_ID_A)
	   {
		   dut_spi_atset_is_on = false;
	   }
	   else
	   {
		   dut_spi_btset_is_on = false;
	   }
	   result = true;
	   
	  // Sleep(2000);
	   param = "spi switch off ok";
	   return(NO_ERR);
}
#endif
#if 1
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_Vreg_on::CTObj_Vreg_on(const char *pname, testSet *Tset, int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{

}

void CTObj_Vreg_on::gen_prompt()
{
   char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	if(tset->dev_id == TEST_DEV_ID_A)
	  range.Format("U5 PE10 pull high");
	else
	  range.Format("U18 PE10 pull high");
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");


}

int CTObj_Vreg_on::ItemTestReady(ConfigFile &cfg)
{
    mutex_hodle1 = false;
	mutex_hodle2 = false;
	if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pMainWnd->glbResource_get(GRESOURCE_MCU2))
		{
			param = "u5 gsource err";
			return(ERR_TERMINAL);
		}
		mutex_hodle2 = true;
		return(NO_ERR);
	}
	else
	{
		if(!pMainWnd->glbResource_get(GRESOURCE_MCU1))
		{
			param = "u18 gsource err";
			return(ERR_TERMINAL);
		}
		mutex_hodle1 = true;
		return(NO_ERR);
	}

}


int CTObj_Vreg_on::ItemTestClean(ConfigFile &cfg)
{
	   
	if (mutex_hodle2)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU2);
        mutex_hodle2 = false;
	}
	   
	if (mutex_hodle1)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU1);
	    mutex_hodle1 = false;
	}
	   
    return(NO_ERR);
}


int CTObj_Vreg_on::TestOp(ConfigFile &cfg)
{
    is_tested = true;
	result = false;

	if(!pMCU1_Com || !pMCU2_Com)
	{
	     param = "null com";
		 return(false);
	}
	
	if(tset->dev_id == TEST_DEV_ID_A)
	{		
		if(!pMCU2_Com->set_dut_current_on(1, MCU_PIO_DUT1_CURRENT, MCU_PIO_NUM_DUT1_CURRENT))
		{
			param = "set dut current err";
			return (ERR_STOP);
		}
		

		if(!pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY))
		{
			param = "set dut battery err";
			return (ERR_STOP);
		}

		param.Format("dut1 pwr on ok");
		result = true;
		return (result);
		
	}
	else
	{	
		if(!pMCU1_Com->set_dut_current_on(0, MCU_PIO_DUT2_CURRENT, MCU_PIO_NUM_DUT2_CURRENT))
		{
			param = "set dut current err";
			return (ERR_STOP);
		}
		
		
		if(!pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY))
		{
			param = "set dut battery err";
			return (ERR_STOP);
		}

		param.Format("dut2 pwr on ok");
		result = true;
		return (result);	
	}   
}
#endif
#if 0

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

CTObj_reconnect::CTObj_reconnect(const char *pname, testSet *Tset, int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{

}

bool CTObj_reconnect::reconnect(void)
{
        uint32 iHandle(0);     
	    struct test_sel_t &tsel = tset->tsel;
	    CBsDevice *pobj = tset->pSpiObj;
        tset->reset_times += 1;
		if(flResetAndStart() != TFL_OK) return(false);
        if(tsel.reset_delay) Sleep(tsel.reset_delay);

        if(pobj->spi_port->port_type() == PORT_TYPE_USB)
		{
			iHandle = openTestEngine(USB,pobj->spi_port->port_num(), 0, 1000, 500);
		}
		else
		{
			iHandle = openTestEngineSpiTrans(pobj->spi_port->port_num(),0);
		}

	    if(iHandle != 0) {
			tset->pSpiObj->iHandle = iHandle;	
		}
		else
		{
			param = "dut reconnect err";
			return(false);
		}
        
        return(true);
}

#endif
#if 1
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_discn_spi::CTObj_discn_spi(const char* pname, testSet *Tset, 
								 int ID,ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
//	terminate_test_if_err_accur = false;
}


void CTObj_discn_spi::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);
	CString range;
	
	if(tset->dev_id == TEST_DEV_ID_A)
	{
		range = "disconnect USB SPI(0)";
	} 
	else 
	{
		range = "disconnect USB SPI(1)";
	}

	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}

int CTObj_discn_spi::TestOp(ConfigFile &cfg)
{

	is_tested = true;
    result = false;
	tset->rf_is_tested = false;         ///8852 test set false

	if(!tset->pSpiObj->spi_allocated)
	{
		param = "dut2 no spi allocated";
		return (ERRSTOP);
	}

	if(tset->pSpiObj->iHandle) uetClose(tset->pSpiObj->iHandle);
	tset->pSpiObj->iHandle = 0;
	//Sleep(50);

	param.Format("disconnect %s ok",tset->pSpiObj->spi_port->port_name());
	
	result = true;
	return(result);
}

#endif
#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_psKeyCheck::CTObj_psKeyCheck(const char* pname,testSet *Tset,int ID,unsigned long key_index,
				   unsigned long key_value,unsigned long type_string,ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
	key = cfg.VCString(key_value);
	ps_key = cfg.Vulong(key_index);
	is_string = cfg.ItemSelect(type_string);
}


void CTObj_psKeyCheck::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	range.Format("ps key(%d):%s",ps_key,key);

	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}

int CTObj_psKeyCheck::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;
	const int len = 256;
	uint16 ver_len = 0;
	uint16 ver_read[len/2] = {0};

	int32 status = psRead(tset->pSpiObj->iHandle,(uint16)ps_key,PS_STORES_IFR,len/2,ver_read,&ver_len);
	if(status != TE_OK)
	{
		param = "";
		param.Format("read ps key:%d err",ps_key);
	}
	else 
	{
		if(!ver_len) param = "record empty";
		else if(is_string)
		{
			char buf[len+1] = {0};
			for(int i=0;i<ver_len*2;i++) 
			{
				buf[i++] = ver_read[i/2]&0xff;
				buf[i] = (ver_read[i/2]>>8)&0xff;
			}
			param = buf;
			if(param == key) result = true;
		}
		else
		{
			CString temp;
			param = "";
			for(int i=0;i<ver_len;i++) {
				temp = "";
				temp.Format("%.4x",ver_read[i]);
				if(i) param += " ";
				param += temp;
			}
			if(param == key) result = true;
		}
	}
	return(result);
}
#endif
#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_read_bkpskey::CTObj_read_bkpskey(const char* pname,testSet *Tset,int ID,ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
}


void CTObj_read_bkpskey::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range = "bk:";
	struct bk_pskey_item *item = tset->backup_pskey->head;
	CString temp;
	while(item) {
		temp = "";
		temp.Format("%d ",item->pskey);
		range += temp;
		item = item->next;
	}

	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}

int CTObj_read_bkpskey::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;
	const int len = 512;
	uint16 ver_len = 0;
	uint16 ver_read[len+1] = {0};
	int32 status;
	struct bk_pskey_item *item;

	param = "clear all backup pskey";
	show_message();
	item = tset->backup_pskey->head;
	while(item) {
		tset->backup_pskey->set_data(item,0,NULL);
		item = item->next;
	}

	item = tset->backup_pskey->head;
	while(item) {
		param = "";
		param.Format("read ps key:%d",item->pskey);
		show_message();
		status = psRead(tset->pSpiObj->iHandle,(uint16)item->pskey,PS_STORES_IFR,len,ver_read,&ver_len);
		if(status != TE_OK) {
			param = "";
			param.Format("read ps key:%d err",item->pskey);
			return(result);
		}
		tset->backup_pskey->set_data(item,ver_len,ver_read);
		item = item->next;
	}

	param = "read backup pskey ok";
	result = true;
	return(result);
}
#endif
#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_write_bkpskey::CTObj_write_bkpskey(const char* pname,testSet *Tset, int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
}

void CTObj_write_bkpskey::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range = "bk:";
	struct bk_pskey_item *item = tset->backup_pskey->head;
	CString temp;
	while(item) {
		temp = "";
		temp.Format("%d ",item->pskey);
		range += temp;
		item = item->next;
	}
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}

int CTObj_write_bkpskey::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;
	const int len = 512;
	uint16 ver_len = 0;
	uint16 ver_read[len+1] = {0};
	int32 status;
	struct bk_pskey_item *item;

	param = "ready writing...";
	show_message();

	item = tset->backup_pskey->head;
	while(item) {
		param = "";
		param.Format("write ps key:%d",item->pskey);
		show_message();
		status = psWriteVerify(tset->pSpiObj->iHandle,(uint16)item->pskey,PS_STORES_IF,item->data_len,item->data);
		if(status != TE_OK) {
			param = "";
			param.Format("write ps key:%d err",item->pskey);
			return(result);
		}
		item = item->next;
	}

	CString temp;
	param = "backup pskey ok:";

	item = tset->backup_pskey->head;
	while(item) {
		temp = "";
		temp.Format("(%d)",item->pskey);
		param += temp;
		for(int i=0;i<item->data_len;i++) {
			temp = "";
			temp.Format("%.4x ",item->data[i]);
			param += temp;
		}
		item = item->next;
	}

	result = true;
	return(result);
}
#endif
#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int char2int(char data)
{
	if((data >= '0') && (data <= '9')) data = data-'0';
	else {
		data |= 0x20;
		if((data>= 'a') && (data <= 'f')) data = data-'a'+10;
		else return(0);
	}
	return(data);
}

CTObj_write_pskey::CTObj_write_pskey(const char* pname,testSet *Tset, int ID,
									 int ps_key_base,ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
	ps_key = cfg.Vulong(ps_key_base);
	key = cfg.VCString(ps_key_base+1);
	data_len = 0;
	data = NULL;

	unsigned short temp;
	unsigned short temp_data[256] = {0};
	int length = key.GetLength();
	for(int i=0;i<length;i++) {
		temp = 0;
		temp = (temp<<4)+char2int(key[i++]);
		if(i>=length) break;
		temp = (temp<<4)+char2int(key[i++]);
		if(i>=length) break;
		temp = (temp<<4)+char2int(key[i++]);
		if(i>=length) break;
		temp = (temp<<4)+char2int(key[i++]);

		temp_data[data_len++] = temp;
		if(i>=length) break;
	}

	if(data_len) {
		data = new unsigned short[data_len];
		for(int i=0;i<data_len;i++) data[i] = temp_data[i];
	}
}

CTObj_write_pskey::~CTObj_write_pskey()
{
	if(data) delete []data;
	data = NULL;
}


void CTObj_write_pskey::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	range.Format("%d:",ps_key);
	range += key;
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}



int CTObj_write_pskey::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;
	const int len = 512;
	uint16 ver_len = 0;
	uint16 ver_read[len+1] = {0};
	int32 status;

	param = "";
	param.Format("wps key %d:",ps_key);
	show_message();
	status = psWriteVerify(tset->pSpiObj->iHandle,ps_key,PS_STORES_IF,data_len,data);
	if(status != TE_OK) {
		param = "";
		param.Format("write ps key:%d err",ps_key);
		return(result);
	}

	param += key;
	param += " ok";
	result = true;
	return(result);
}
#endif
#if 1

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_current::CTObj_current(const char* pname,testSet *Tset,int ID,int cfj_item, int on_off, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{

	on_off_cur = on_off;
	current_min = cfg.VDouble(cfj_item+1);
	current_max = cfg.VDouble(cfj_item+2);
	stable_time = cfg.VDouble(cfj_item+3);
	resistor = cfg.VDouble(cfj_item+4);
	if(current_max < current_min) current_max = (current_min + 10);
	
	if(resistor == 0) resistor = 0.1;
}

void CTObj_current::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	if(on_off_cur == 0)
	{
	  range.Format("current[%.1lf,%.1lf]uA", current_min, current_max);
	}
	else
	{
	  range.Format("current[%.1lf,%.1lf]mA", current_min, current_max);
	}
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}


int CTObj_current::ItemTestReady(ConfigFile &cfg)
{
    mutex_hodle1 = false;
	mutex_hodle2 = false;
	
	if (!pMainWnd->glbResource_get(GRESOURCE_MCU2))
	{
		param = "u5 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_hodle1 = true;
	
	if (!pMainWnd->glbResource_get(GRESOURCE_CURRENT))
	{
		param = "mv-meter gsource err";
		return(ERR_TERMINAL);
	}
	mutex_hodle2 = true;
	
	   
	   return(NO_ERR);
}

int CTObj_current::ItemTestClean(ConfigFile &cfg)
{
    
	if(mutex_hodle1)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU2);
		mutex_hodle1 = false;
	}
	
	if(mutex_hodle2)
	{
		pMainWnd->glbResource_put(GRESOURCE_CURRENT);
		mutex_hodle2 = false;
	}
   	
    return(NO_ERR);
}


int CTObj_current::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;
	int i=0;
	int j = 0;
	if(!pMultimeter)
	{ 
		param = "null device";
		return(ERR_STOP);
	}
	
	double current = 0;
	double voltage = 0;
	double voltage_min = current_min*resistor;
	double voltage_max = current_max*resistor;
 
	if(tset->dev_id == TEST_DEV_ID_A)
	{
	    if(!pMCU2_Com->set_dut1_rout_meter_on(1, MCU_PIO_DUT2_METER, MCU_PIO_NUM_DUT2_METER))
		{
		     param = "set dut1 route meter err";
			 return(ERR_STOP);
		}
	}
	else
	{
	    if(!pMCU2_Com->set_dut2_rout_meter_on(1, MCU_PIO_DUT2_METER, MCU_PIO_NUM_DUT2_METER))
		{
		    param = "set dut2 route meter err";
			return(ERR_STOP);
		}
	}

	do
	{
		if(i++ > 4)
		{
			//param = "try to get five timer cur,but it not a correct";
			param.Format("try five,not get[result:%d]",j);
			return(ERR_STOP);
			//break;
		}	
		if(stable_time) Sleep(stable_time);
		if(!pMultimeter->get_voltage(&voltage))
		{
			j++;
			//param = "get value err";
			//return(ERR_STOP);
		}
	}while((voltage < voltage_min)||(voltage > voltage_max));
	
	if(on_off_cur == 0)  //关机电流
	{
	    //current = (voltage/resistor*1000);   //off current
		current = (voltage);
	}
	else                //测试模式下,开机电流
	{	
		current = (voltage/resistor);
	/*
		if(tset->dev_id == TEST_DEV_ID_A)
		{
			current -= 11;    //机架电路内部消耗电流，需要减去11mA
		}
		else
		{
			current -= 9.5;    //机架电路内部消耗电流，需要减去9.5mA
		}
	*/
	}
	//if(current == 0)
		//AfxMessageBox("get value=0");	
	if(current < 0)  {
		//AfxMessageBox("get minus current");
		//current = -current;
		current = 0;
	}

	param = "";
    if(on_off_cur == 0)
	{
	   param.Format("%.3lfuA",current);
	}
	else
	{
	   param.Format("%.3lfmA",current);
	}
	
	if((current >= current_min) && (current <= current_max)) result = true;
	
	if(!result) return(ERR_STOP);
	return(NO_ERR);
}
#endif
#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_clr_XtalOffset::CTObj_clr_XtalOffset(const char* pname, testSet *Tset, int ID,
				 ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
}

int CTObj_clr_XtalOffset::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	int max_try_times = 2;
	result = false;

	uint32 lap;
	uint8  uap;
	uint16 nap;
	if(psReadBdAddr(tset->pSpiObj->iHandle, &lap, &uap, &nap) != TE_OK)
	{
		param = "read bt address err";
		return (ERR_STOP);
	}

	tset->r_addr.lap = lap;
	tset->r_addr.nap = nap;
	tset->r_addr.uap = uap;
	tset->r_addr.is_valid = true;

	if(tset->gloden_idx())
	{ 
		param = "";
		param.Format("%.4X%.2X %.6X is golden\n",tset->r_addr.nap,tset->r_addr.uap,tset->r_addr.lap);
		result = true;
		return (result);
	}
		
	int32 status = ~TE_OK;
	for(int i=0;i<max_try_times;i++) 
	{
		status = psWriteXtalOffset(tset->pSpiObj->iHandle,0);
		if(status == TE_OK) break;
	}
	if(status != TE_OK) param =  "CTObj_clr_XtalOffset error";
	else 
	{
		param =  "clear XtalOffset ok";
		result = true;
	}
	
	return(result);
}
#endif
#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_vm_enable::CTObj_vm_enable(const char* pname, testSet *Tset, int ID, bool disable,
				 ConfigFile &cfg):CTestItem(pname, Tset, ID)
{
	vm_disable = disable;
	if(vm_disable) time = cfg.Vulong(slaveSetVmDisTime);
	else time = cfg.Vulong(slaveSetVmEnTime);
}

int CTObj_vm_enable::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;
	int32 status = psWriteVmDisable(tset->pSpiObj->iHandle,vm_disable);
	if(status != TE_OK) 
	{
		if(vm_disable) param =  "vm disable err";
		else param =  "vm enable err";
	}
	else 
	{
		if(time) Sleep(time);
		uint8 cur_st;
		status = psReadVmDisable(tset->pSpiObj->iHandle,&cur_st);
		if((status == TE_OK) && (!!cur_st == vm_disable)) 
		{
			//进入测试模式 VREG状态变化(VREG ON->VM DISABLE->VREG OFF->RESET->VREG ON)
			if(tset->dev_id == TEST_DEV_ID_A)
			{
#if 0				
				if(!pMCU2_Com->set_dut_vreg_off(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG))
				{
					if(vm_disable) param =  "vm disable ok, vreg off err";
                    else param =  "vm enable ok, vreg off err";
					return (ERR_STOP);
				}
#endif
				
				if(vm_disable) param =  "vm disable ok";
				else param =  "vm enable ok";
				result = true;
			}
			else
			{
#if 0
				if(!pMCU1_Com->set_dut_vreg_off(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG))
				{
					if(vm_disable) param =  "vm disable ok, vreg off err";
                    else param =  "vm enable ok, vreg off err";
					return (ERR_STOP);
				}
#endif   

				if(vm_disable) param =  "vm disable ok";
				else param =  "vm enable ok";
				result = true;
			}
		}
		else
		{
			param =  "verify err";
			return (ERR_STOP);
		}
	}
	return(result);
}
#endif
#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
unsigned short CTObj_codec_io_read::pio = 0xffff;
CTObj_codec_io_read::CTObj_codec_io_read(const char* pname, testSet *Tset, int ID, bool clr,
					   ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
	clr_or_store = clr;
	ps_key = 0x01b9;
}

int CTObj_codec_io_read::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;
	
	uint32 status;
	if(clr_or_store) {
		uint16 ver_len = 0;
		uint16 io = 0;
		status = psRead(tset->pSpiObj->iHandle,ps_key,PS_STORES_IFR,1,&io,&ver_len);
		if(status != TE_OK) {
			pio = 0xffff;
			param = "no codec pio set ok";
			result = true;
			return(result);
		}

		pio = io;
		if(pio <= 15)
		{
			status = psClear(tset->pSpiObj->iHandle,ps_key,PS_STORES_IF);
			if(status != TE_OK) param.Format("clr codec pio err");
			else {
				result = true;
				param.Format("clr codec pio:%x ok",pio);
			}
		}
		else
		{
			result = true;
			param = "codec pio is not set ok";
		}
		return(result);
	}else {
		if((pio == 0xffff) || (pio > 15)) {
			param.Format("codec pio is set ok");
			result = true;
			return(result);
		}

		status = psWriteVerify(tset->pSpiObj->iHandle,ps_key,PS_STORES_IF,1,&pio);
		if(status != TE_OK) {
			param = "";
			param.Format("set back codec io:%x err",pio);
			result = false;
			return(result);
		}
		param = "";
		param.Format("set back codec io:%x ok",pio);
		result = true;
		return(result);
	}
}
#endif
#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_set_io_dir::CTObj_set_io_dir(const char* pname, testSet *Tset, int ID,
							               bool is_bs,ConfigFile &cfg):CTestItem(pname, Tset, ID)
{
    //PIO0-15 GPIO, PIO16-31,alternative pio
	out_io_mask = 
        IS_IO_TEST(0,TPIO_0)|IS_IO_TEST(1,TPIO_1)|IS_IO_TEST(2,TPIO_2)|
		IS_IO_TEST(3,TPIO_3)|IS_IO_TEST(4,TPIO_4)|IS_IO_TEST(5,TPIO_5)|
		IS_IO_TEST(6,TPIO_6)|IS_IO_TEST(7,TPIO_7)|IS_IO_TEST(8,TPIO_8)|
		IS_IO_TEST(9,TPIO_9)|IS_IO_TEST(10,TPIO_10)|IS_IO_TEST(11,TPIO_11)|
		IS_IO_TEST(12,TPIO_12)|IS_IO_TEST(13,TPIO_13)|IS_IO_TEST(14,TPIO_14)|IS_IO_TEST(15,TPIO_15)|
		IS_IO_TEST(19,TPIO_19)|IS_IO_TEST(20,TPIO_20)|IS_IO_TEST(21,TPIO_21)|
		IS_IO_TEST(22,TPIO_22)|IS_IO_TEST(23,TPIO_23)|IS_IO_TEST(24,TPIO_24)|
		IS_IO_TEST(25,TPIO_25)|IS_IO_TEST(26,TPIO_26)|IS_IO_TEST(27,TPIO_27)|
		IS_IO_TEST(28,TPIO_28)|IS_IO_TEST(29,TPIO_29)|IS_IO_TEST(30,TPIO_30)|
		IS_IO_TEST(31,TPIO_31);
}


void CTObj_set_io_dir::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	range.Format("check io mask:0x%.8x",out_io_mask);
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}

int CTObj_set_io_dir::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;
	uint32 err_line = 0;

	uint32 io_mask_pio = out_io_mask;
	uint32 status = TE_OK;
	if((status == TE_OK) && (out_io_mask&(1ul<<TPIO_29))) {
		io_mask_pio &=  ((1ul<<TPIO_29)^0xffffffff);
		status = bccmdLedEnable(tset->pSpiObj->iHandle,0,1);
		if(status != TE_OK) 
		{
			param = "set led0 pio err";
			return(result);
		}
	}
	if((status == TE_OK) && (out_io_mask&(1ul<<TPIO_30))) {
		io_mask_pio &=  ((1ul<<TPIO_30)^0xffffffff);
		status = bccmdLedEnable(tset->pSpiObj->iHandle,1,1);
		if(status != TE_OK) 
		{
			param = "set led1 pio err";
			return(result);
		}
	}
	if((status == TE_OK) && (out_io_mask&(1ul<<TPIO_31))) {
		io_mask_pio &=  ((1ul<<TPIO_31)^0xffffffff);
		status = bccmdLedEnable(tset->pSpiObj->iHandle,2,1);
		if(status != TE_OK) 
		{
			param = "set led2 pio err";
			return(result);
		}
	}

	//except led0-2,pio31-29 can map as pio
	if(status == TE_OK) {
		status = bccmdMapPio32(tset->pSpiObj->iHandle,0x1fff0000,0x1fff0000,&err_line);
		if(status != TE_OK) 
		{	
			param.Format("map line:%.8x err", err_line);
			return(result);
		}
	}
	
	if((status == TE_OK) && io_mask_pio) {
		status = bccmdSetPio32(tset->pSpiObj->iHandle,io_mask_pio,io_mask_pio,0,&err_line);
		if(status != TE_OK) 
		{
			param = "";
			param.Format("io set err:0x%.8x",err_line);
			return(result);
		}
	}
	
    param = "map and set pio ok";
	result = true;
	return(result);
}
#endif

#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//LED 只作输出
CTObj_led_io::CTObj_led_io(const char* pname, testSet *Tset,int ID, int pin_num,
				     ConfigFile &cfg):CTObj_io_port(pname, Tset, ID, pin_num)
{
	if(ID == TID_PIO29)
	{
	    led = 0;
		led_io_num = LED_0_igpio;
		mcu_gpio_set_input_cmd.Format("MCU+SET(%d,%c,%04x,1)\r\n", 
		   ((tset->dev_id == TEST_DEV_ID_A)?1:0), LED_0_gpio, (1<<LED_0_igpio));    //设置输入口，没有上拉，下拉
	    mcu_gpio_rd_cmd.Format("MCU+RD(%d,%c,%04x)\r\n",
			((tset->dev_id == TEST_DEV_ID_A)?1:0), LED_0_gpio, (1<<LED_0_igpio));
	}
	else if(ID == TID_PIO30)
	{
		led = 1;
		led_io_num = LED_1_igpio; 
		mcu_gpio_set_input_cmd.Format("MCU+SET(%d,%c,%04x,1)\r\n",
			 ((tset->dev_id == TEST_DEV_ID_A)?1:0), LED_1_gpio, (1<<LED_1_igpio));
		 mcu_gpio_rd_cmd.Format("MCU+RD(%d,%c,%04x)\r\n",
			((tset->dev_id == TEST_DEV_ID_A)?1:0), LED_1_gpio,  (1<<LED_1_igpio));
	}
	else if(ID == TID_PIO31)
	{
	    led = 2;
		led_io_num = LED_2_igpio;
		mcu_gpio_set_input_cmd.Format("MCU+SET(%d,%c,%04x,1)\r\n",
			 ((tset->dev_id == TEST_DEV_ID_A)?1:0), LED_2_gpio, (1<<LED_2_igpio));
		mcu_gpio_rd_cmd.Format("MCU+RD(%d,%c,%04x)\r\n",
			((tset->dev_id == TEST_DEV_ID_A)?1:0), LED_2_gpio,  (1<<LED_2_igpio));
	}
}


void CTObj_led_io::gen_prompt(void)
{
    char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	if(tset->dev_id == TEST_DEV_ID_A)
	    range.Format("u5 P%c%d read led_io", gpio[pio_name], (igpio[pio_name]));
	else
	    range.Format("u18 P%c%d read led_io", gpio[pio_name], (igpio[pio_name]));
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");

}


bool CTObj_led_io::set_led_out(int lv)
{
	int32 status;
	status = bccmdLedEnable(tset->pSpiObj->iHandle, led, lv);
	if(status != TE_OK) {
		param = "";
		param.Format("set led:%.d lv:%.d err", led, lv);
		return(false);
	}
	
	Sleep(100);
	param = "";
	param.Format("set led:%.d lv:%.d ok",led, lv);
	return(true);


}
bool CTObj_led_io::is_led_lv(int lv)
{

	if(!pMCU1_Com || !pMCU2_Com)
	{
	   param = "null COMs";
	   return (false);
	}
	
	//mcu read from pio
	unsigned long bit_val = 0;
	char c;
	char *pbuf = NULL;
	char rx_buf[32] = {0};
	if(tset->dev_id == TEST_DEV_ID_A)
	{
	   //read led
	   if(!pMCU2_Com->send_and_verify(mcu_gpio_set_input_cmd, 0, "MCU+OK\r\n", 0))
	   {
	        param = "u5 set led err";
			return (false);
	   }

	   if(!pMCU2_Com->send_and_get(mcu_gpio_rd_cmd, 0, &rx_buf[0], 0))
	   {
	       param = "u5 read led err";
		   return (false);
	   }

#if 0       
	   pbuf = &rx_buf[0];
	   while(c = *pbuf)
	   {
	      if(c >= '0' && c <= '9')
		  {
             bit_val = bit_val*16 + (c-'0');
		  }
		  pbuf++;
	   }     
	   if(lv)
	   {
		   if(bit_val == (0)) {
			   param.Format("%s tHlv:0x%.8x ok",out_pio_name[pio_name], bit_val);
			   return(true);
		   }
		   else {
			   param.Format("%s tHlv:0x%.8x err",out_pio_name[pio_name], bit_val);
			   return(false);
		   }
	   }
	   else
	   {
		   if(bit_val == (1 << led_io_num)) {
			   param.Format("%s tLlv:0x%.8x ok",in_pio_name[pio_name], bit_val);
			   return(true);
		   }
		   else {
			    param.Format("%s tLlv:0x%.8x err",in_pio_name[pio_name], bit_val);
			    return(false);
		   }
	   }
#endif

	}
	else
	{
		if(!pMCU1_Com->send_and_verify(mcu_gpio_set_input_cmd, 0, "MCU+OK\r\n", 0))
		{
	        param = "u18 set led err";
			return (false);
		}
	    if(!pMCU1_Com->send_and_get(mcu_gpio_rd_cmd, 0, &rx_buf[0], 0))
		{
	       param = "u18 read led err";
		   return (false);
		}
#if 0
	   pbuf = &rx_buf[0];
	   while(c = *pbuf)
	   {
	      if(c >= '0' && c <= '9')
		  {
             bit_val = bit_val*16 + (c-'0');
		  }
		  pbuf++;
	   }

	   if(lv)
	   {
		   if(bit_val == 0) {
			   param.Format("%s tHlv:0x%.8x ok",out_pio_name[pio_name], bit_val);
			   return(true);
		   }
		   else {
			   param.Format("%s tHlv:0x%.8x err",out_pio_name[pio_name], bit_val);
			   return(false);
		   }
	   }
	   else
	   {
		   if(bit_val == (1 << led_io_num)) {
			   param.Format("%s tLlv:0x%.8x ok",out_pio_name[pio_name], bit_val);
			   return(true);
		   }
		   else {
			   param.Format("%s tLlv:0x%.8x err",out_pio_name[pio_name], bit_val);
			   return(false);
		   }
	   }
#endif
	}
	
	   pbuf = &rx_buf[0];
	   while(c = *pbuf)
	   {
		   if(c >= '0' && c <= '9')
		   {
			   bit_val = bit_val*16 + (c-'0');
		   }
		   pbuf++;
	   }
	   
	   if(lv)
	   {
		   if(bit_val == 0) {
			   param.Format("%s tHlv:0x%.8x ok",out_pio_name[pio_name], bit_val);
			   return(true);
		   }
		   else {
			   param.Format("%s tHlv:0x%.8x err",out_pio_name[pio_name], bit_val);
			   return(false);
		   }
	   }
	   else
	   {
		   if(bit_val == (1 << led_io_num)) {
			   param.Format("%s tLlv:0x%.8x ok",out_pio_name[pio_name], bit_val);
			   return(true);
		   }
		   else {
			   param.Format("%s tLlv:0x%.8x err",out_pio_name[pio_name], bit_val);
			   return(false);
		   }
	   }
}


int CTObj_led_io::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;
	int32 status;

    status = set_led_out(1);
	if(status != TE_OK) return(false);
	if(!is_led_lv(1)) return(result);

	status = set_led_out(0);
	if(status != TE_OK) return(false);
	if(!is_led_lv(0)) return(result);
	
	param = "";
	param.Format("%s [H:%.4x L:%.4x] ok",out_pio_name[pio_name], 0, (1 << led_io_num));
	result = true;
	return(result);
	

  return result;
}
#endif
#if 0


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//mbias 只作输出
CTObj_mbias::CTObj_mbias(const char* pname,testSet *Tset,int ID,ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
   delay_time = cfg.Vulong(s_pio_wait_time);

   //format cmd string
	if(ID == TID_MIBAS_IO0)
	{ 
		pin_num = MIC_BIAS_A_igpio;
		pin_name = MBIAS_A;
		pio_name = 0;
		instance = 0;
		mcu_gpio_set_input_cmd.Format("MCU+SET(%d,%c,%04x,1)\r\n", 
			((tset->dev_id == TEST_DEV_ID_A)?1:0), MIC_BIAS_A_gpio, (1 << pin_num));
	    mcu_gpio_rd_cmd.Format("MCU+RD(%d,%c,%04x)\r\n", 
			((tset->dev_id == TEST_DEV_ID_A)?1:0), MIC_BIAS_A_gpio, (1 << pin_num));
	}
	if(ID == TID_MIBAS_IO1)
	{
		pin_num = MIC_BIAS_B_igpio;
        pin_name = MBIAS_B;
		pio_name = 1;
		instance = 1;
		mcu_gpio_set_input_cmd.Format("MCU+SET(%d,%c,%04x,1)\r\n", 
			((tset->dev_id == TEST_DEV_ID_A)?1:0), MIC_BIAS_B_gpio, (1 << pin_num));
	    mcu_gpio_rd_cmd.Format("MCU+RD(%d,%c,%04x)\r\n", 
			((tset->dev_id == TEST_DEV_ID_A)?1:0), MIC_BIAS_B_gpio, (1 << pin_num));
	}
}


void CTObj_mbias::gen_prompt()
{

	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	if(tset->dev_id == TEST_DEV_ID_A)
	    range.Format("u5 P%c%d read mbias_%c", mcu_mbias[pio_name][0], (mcu_mbias[pio_name][1]), mcu_mbias[pio_name][2]);
	else
	    range.Format("u18 P%c%d read mbias_%c", mcu_mbias[pio_name][0], (mcu_mbias[pio_name][1]), mcu_mbias[pio_name][2]);
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}

int CTObj_mbias::ItemTestReady(ConfigFile &cfg)
{
    mutex_hodle1 = false;
	mutex_hodle2 = false;

	if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pMainWnd->glbResource_get(GRESOURCE_MCU2))
		{
			param = "u5 gsource err";
			return(ERR_TERMINAL);
		}
		mutex_hodle1 = true;
	}
    else
	{
		if(!pMainWnd->glbResource_get(GRESOURCE_MCU1))
		{
			param = "u18 gsource err";
			return(ERR_TERMINAL);
		}
		mutex_hodle2 = true;
	}
	return(NO_ERR);
}


int CTObj_mbias::ItemTestClean(ConfigFile &cfg)
{
    if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(mutex_hodle1)
		{			
			pMainWnd->glbResource_put(GRESOURCE_MCU2);
			mutex_hodle1 = false;	
		}
	}
	else
	{
		if(mutex_hodle2)
		{	
			pMainWnd->glbResource_put(GRESOURCE_MCU1);
			mutex_hodle2 = false;	
		}
	}

    return(NO_ERR);
}


signed long int CTObj_mbias::set_mbias_out(int instance, bool lv)
{
	CBsDevice *obj;
	obj = tset->pSpiObj;
	int32 status = bccmdSetMicBiasIf(obj->iHandle,instance,lv,0);
	if(status != TE_OK) {
		param = "";
		param.Format("MicBias set lv:%.d err",lv);
		return(false);
	}
	Sleep(100);
	param = "";
	param.Format("MicBias set lv:%.d ok",lv);
	return(true);	
}


bool CTObj_mbias::is_mbias_lv(bool lv)
{
	if(!pMCU1_Com || !pMCU2_Com)
	{
	   param = "null COMs";
	   return (false);
	}
	
	//mcu read from pio
	unsigned long bit_val = 0;
	char c;
	char *pbuf = NULL;
	char rx_buf[32] = {0};
	if(tset->dev_id == TEST_DEV_ID_A)
	{
	   //read mbias A
	   if(!pMCU2_Com->send_and_verify(mcu_gpio_set_input_cmd, 0, "MCU+OK\r\n", 0))
	   {
	        param = "u5 set mbias err";
			return (false);
	   }

	   if(!pMCU2_Com->send_and_get(mcu_gpio_rd_cmd, 0, &rx_buf[0], 0))
	   {
	       param = "u5 read mbias err";
		   return (false);
	   }
	}
	else
	{
		if(!pMCU1_Com->send_and_verify(mcu_gpio_set_input_cmd, 0, "MCU+OK\r\n", 0))
		{
	        param = "u18 set mbias err";
			return (false);
		}
	    if(!pMCU1_Com->send_and_get(mcu_gpio_rd_cmd, 0, &rx_buf[0], 0))
		{
	       param = "u18 read mbias err";
		   return (false);
		}
	}
	
	pbuf = &rx_buf[0];
	   while(c = *pbuf)
	   {
		   if(c >= '0' && c <= '9')
		   {
			   bit_val = bit_val*16 + (c-'0');
		   }
		   pbuf++;
	   }
	   
	   if(lv)
	   {
		   if(bit_val == 0) {
			   param.Format("%s tHlv:0x%.6x ok",out_pio_name[pin_name], bit_val);
			   return(true);
		   }
		   else {
			   param.Format("%s tHlv:0x%.6x err",out_pio_name[pin_name], bit_val);
			   return(false);
		   }
	   }
	   else
	   {
		   if(bit_val == (1ul<<pin_num)) {
			   param.Format("%s tLlv:0x%.6x ok",out_pio_name[pin_name], bit_val);
			   return(true);
		   }
		   else {
			   param.Format("%s tLlv:0x%.6x err",out_pio_name[pin_name], bit_val);
			   return(false);
		   }
	   }
}


CTObj_mbias::TestOp(ConfigFile &cfg)
{
    is_tested = true;
	result = false;

	int32 status = set_mbias_out(instance, 1);
	if(status != TE_OK) return(false);
	if(!is_mbias_lv(1)) return(result);

	status = set_mbias_out(instance, 0);
	if(status != TE_OK) return(false);
	if(!is_mbias_lv(0)) return(result);
	
	param = "";
	param.Format("%s %.6x %.6x ok",out_pio_name[pin_name],(0 << pin_num),(1 << pin_num));
	result = true;
	return(result);
}
#endif
#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//Aio 只作输入
CTObj_Aio_port::CTObj_Aio_port(const char* pname, testSet *Tset, int ID, ConfigFile &cfg):
		CTestItem(pname, Tset,ID)
{
	time = cfg.Vulong(s_pio_wait_time);
	low_lv_ad_data = cfg.Vulong(s_Aio_lAD_data);
	high_lv_ad_data = cfg.Vulong(s_Aio_hAD_data);
    

	//format cmd string
	if(ID == TID_AIO0)
	{ 
	    pio_name = TAIO_0; //PB9
		aio = 0;
		mcu_gpio_set_output_cmd.Format("MCU+SET(%d,%c,%04x,5)\r\n", 
			((tset->dev_id == TEST_DEV_ID_A)?1:0), AIO_0_gpio, (1 << AIO_0_igpio));
	    mcu_gpio_wr_high_cmd.Format("MCU+WR(%d,%c,%04x,%04x)\r\n", 
			((tset->dev_id == TEST_DEV_ID_A)?1:0), AIO_0_gpio, (1 << AIO_0_igpio), (1 << AIO_0_igpio));
        mcu_gpio_wr_low_cmd.Format("MCU+WR(%d,%c,%04x,%04x)\r\n", 
			((tset->dev_id == TEST_DEV_ID_A)?1:0), AIO_0_gpio, (1 << AIO_0_igpio), (0 << AIO_0_igpio));
	}
	if(ID == TID_AIO1)
	{
	    pio_name = TAIO_1;  //PB8
		aio = 1;
		mcu_gpio_set_output_cmd.Format("MCU+SET(%d,%c,%04x,5)\r\n", 
			((tset->dev_id == TEST_DEV_ID_A)?1:0), AIO_1_gpio, (1 << AIO_1_igpio));
	    mcu_gpio_wr_high_cmd.Format("MCU+WR(%d,%c,%04x,%04x)\r\n", 
			((tset->dev_id == TEST_DEV_ID_A)?1:0), AIO_1_gpio, (1 << AIO_1_igpio), (1 << AIO_1_igpio));
		mcu_gpio_wr_low_cmd.Format("MCU+WR(%d,%c,%04x,%04x)\r\n", 
			((tset->dev_id == TEST_DEV_ID_A)?1:0), AIO_1_gpio, (1 << AIO_1_igpio), (0 << AIO_1_igpio));
	}

}


void CTObj_Aio_port::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	range.Format("Low AD:<%.3x High AD:>%.3x", low_lv_ad_data, high_lv_ad_data);
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}


int CTObj_Aio_port::ItemTestReady(ConfigFile &cfg)
{
    mutex_hodle1 = false;
	mutex_hodle2 = false;

	if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pMainWnd->glbResource_get(GRESOURCE_MCU2))
		{
			param = "u5 gsource err";
			return(ERR_TERMINAL);
		}
		mutex_hodle1 = true;
	}
    else
	{
		if(!pMainWnd->glbResource_get(GRESOURCE_MCU1))
		{
			param = "u18 gsource err";
			return(ERR_TERMINAL);
		}
		mutex_hodle2 = true;
	}
	return(NO_ERR);
}


int CTObj_Aio_port::ItemTestClean(ConfigFile &cfg)
{
    if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(mutex_hodle1)
		{			
			pMainWnd->glbResource_put(GRESOURCE_MCU2);
			mutex_hodle1 = false;	
		}
	}
	else
	{
		if(mutex_hodle2)
		{	
			pMainWnd->glbResource_put(GRESOURCE_MCU1);
			mutex_hodle2 = false;	
		}
	}

    return(NO_ERR);
}


bool CTObj_Aio_port::is_pio_lv(int in_pio,bool lv)
{
	const char *lv_show[2] = {"low ad","high ad"};
    int32 status;

	if(time) Sleep(time);
	uint16 ad_data = 0;
	uint16 vref_data = 0;
	uint16 vref_constant = 0;
	uint8 num_bits = 0;

	int count = 15;
	while(count--)
	{
		status = bccmdGetAio(tset->pSpiObj->iHandle,aio,&ad_data,&num_bits);

		if(status == TE_OK) break;
		param.Format("Aio read try: %d", (16-count));
		show_message();
		Sleep(50);
	}
	
	if(status != TE_OK)
	{
		param = "";
		param.Format("%s get AD data err",out_pio_name[in_pio]);
		return(false);
	}


	if(8 == num_bits) ad_data &= 0xff;
	else if(10 == num_bits) ad_data &= 0x3ff;
	
	param = "";
	param.Format("%s %s:%.3x ",out_pio_name[in_pio],lv_show[lv],ad_data);
	if(lv)
	{
		high_data = ad_data;
		if(ad_data < high_lv_ad_data)
		{
			param += "err";
			return(false);
		}
	}
	else
	{
		low_data = ad_data;
		if(ad_data > low_lv_ad_data) 
		{
			param += "err";
			return(false);
		}
	}
	return(true);
}


int CTObj_Aio_port::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;

    if(!pMCU1_Com || !pMCU2_Com)
	{
	   param = "null COM";
	   return (false);
	}

	if(tset->dev_id == TEST_DEV_ID_A)
	{
	   if(!pMCU2_Com->send_and_verify(mcu_gpio_set_output_cmd, 0, "MCU+OK\r\n", 0))
	   {
	      param = "u5 set err";
	      return (false); 
	   }

       if(!pMCU2_Com->send_and_verify(mcu_gpio_wr_high_cmd, 0, "MCU+OK\r\n", 0))
	   {
	      param = "u5 wr high err";
	      return (false); 
	   }
	
	}
	else
	{
	   if(!pMCU1_Com->send_and_verify(mcu_gpio_set_output_cmd, 0, "MCU+OK\r\n", 0))
	   {
	      param = "u18 set err";
	      return (false); 
	   }

       if(!pMCU1_Com->send_and_verify(mcu_gpio_wr_high_cmd, 0, "MCU+OK\r\n", 0))
	   {
	      param = "u18 wr high err";
	      return (false); 
	   }
	}
	Sleep(50);
	if(!is_pio_lv(pio_name,1)) return(result);

    
	//输入低电平
	if(tset->dev_id == TEST_DEV_ID_A)
	{
       if(!pMCU2_Com->send_and_verify(mcu_gpio_set_output_cmd, 0, "MCU+OK\r\n", 0))
	   {
	      param = "u5 set err";
	      return (false); 
	   }
		if(!pMCU2_Com->send_and_verify(mcu_gpio_wr_low_cmd, 0, "MCU+OK\r\n", 0))
	   {
	      param = "u5 wr low err";
	      return (false); 
	   }
	}
	else
	{
       if(!pMCU1_Com->send_and_verify(mcu_gpio_set_output_cmd, 0, "MCU+OK\r\n", 0))
	   {
	      param = "u18 set err";
	      return (false);
	   }
	   
		if(!pMCU1_Com->send_and_verify(mcu_gpio_wr_low_cmd, 0, "MCU+OK\r\n", 0))
	   {
	      param = "u18 wr low err";
	      return (false); 
	   }
	}

	Sleep(50);
	if(!is_pio_lv(pio_name,0)) return(result);

	param = "";
	param.Format("%s %.6x %.6x ok",out_pio_name[pio_name],low_data,high_data);
	result = true;
	return(result);
}
#endif
#if 0

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_uart_cts_rts::CTObj_uart_cts_rts(const char* pname,testSet *Tset,int ID, ConfigFile &cfg):
			CTestItem(pname, Tset, ID)
{
}

void CTObj_uart_cts_rts::gen_prompt()
{
	char item_str[8];
	itoa(Index(),item_str,10);

	CString range;
	range.Format("alter to pio16 test");
	tset->tlist->AddItem(item_str,name,range.GetBuffer(0),"","");

}

int CTObj_uart_cts_rts::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;

	int32 status = radiotestCtsRtsLb(tset->pSpiObj->iHandle);
	if(status == TE_OK) {
		param =  "uart_cts_rts loop back ok";
		result = true;
	}
	else if(status == TE_ERROR) {
		param = "uart_cts_rts loop back err";
	}else param = "uart_cts_rts dev op err";

	return(result);
}
#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_uart_test::CTObj_uart_test(const char* pname,testSet *Tset,int ID, ConfigFile &cfg):
CTestItem(pname, Tset, ID)
{
	if(tset->dev_id == TEST_DEV_ID_A)
	{
		port = cfg.VCString(s_uart_dut1_portname);
		baudrate = cfg.Vint(s_uart_dut1_baudrate);
	}
	else
	{
		port = cfg.VCString(s_uart_dut2_portname);
		baudrate = cfg.Vint(s_uart_dut2_baudrate);
	}
}

void CTObj_uart_test::gen_prompt()
{
	char item_str[8];
	itoa(Index(),item_str,10);
	
	CString range;
	range.Format("port:%s baudrate:%d",port,baudrate);
	tset->tlist->AddItem(item_str,name,range.GetBuffer(0),"","");
	
}

int CTObj_uart_test::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;
	
//static const CString PORT = "COM13";

//	static const uint32 BAUDRATE = 9600;
	
	int32 status = uetUartLbTest(tset->pSpiObj->iHandle,port,baudrate);
	if(status == UET_OK) 
	{
		param =  "UART Loopback test PASSED";
		result = true;
	}
	else if(status == UET_ERR_TEST_FAILED) 
	{
		param = "UART Loopback test FAILED";
	}
	else 
	{
		param.Format("Error during UART Loopback test: %s",uetGetLastError(tset->pSpiObj->iHandle)) ;
	}
	
	return(result);
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#if 0

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_pcm_in_out::CTObj_pcm_in_out(const char* pname,testSet *Tset,int ID, ConfigFile &cfg):
			CTestItem(pname, Tset, ID)
{
}


void CTObj_pcm_in_out::gen_prompt()
{
	char item_str[8];
	itoa(Index(),item_str,10);

	CString range;
	range.Format("pcm_in pcm_out self test");
	tset->tlist->AddItem(item_str,name,range.GetBuffer(0),"","");

}


int CTObj_pcm_in_out::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;

	uint32 mapMask = ((1ul << 17) | (1ul << 18));      // The lines we can map as PIOs, pio17,18
    uint32 direction = (1ul << 18);    // Direction - (0) as input (1) as output
    uint32 value = (1ul << 18);        // State of the lines to set (applies to output lines only)
    uint32 errLines;
	int32 success;


	// Map lines that have other alternate functions as PIOs
    success = bccmdMapPio32(tset->pSpiObj->iHandle, mapMask, mapMask, &errLines);
	if(success != TE_OK)
	{
		// Checks if any lines could not be mapped as PIOs
		if(errLines != 0)
		{
			param.Format("%08x lines mapped as PIOs err", errLines);	
		}
		else
		{
		    param = "map pio err";
		}
		return(result);
	}
	else
	{
		// Set the direction and value of the PIO lines.
		success = bccmdSetPio32(tset->pSpiObj->iHandle, mapMask, direction, value, &errLines);	
		// Checks if any lines could not be mapped as PIOs
		if(success != TE_OK)
		{
			// Checks which PIO lines could not be set as input or output
			if(errLines != 0)
			{
				param.Format("%08x lines set err", errLines);		
			}
			else
			{
				param = "set pio err";
			}
		}
	}

	Sleep(2000);
	success = bccmdGetPio32(tset->pSpiObj->iHandle, &direction, &value);
	if(success != TE_OK)
	{
		param = "get pio value err";
		return (result);
	}
    
	//uint32 direction_result = direction ^ ((1ul << 18));
	uint32 value_result = value ^ ((1ul << 17) | (1ul << 18));

	if(value_result != 0)
	{ 
		param = "pio value err";
        return (result);
	}
	else
	{
	  param = "PCM_IN read PCM_OUT value ok";
	  result = true;
	}

	return (result);
}
#endif
#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_usb_np_io::CTObj_usb_np_io(const char* pname,testSet *Tset,int ID, ConfigFile &cfg):
		CTestItem(pname, Tset,ID)
{
	USB_VID = (unsigned short)cfg.Vulong(s_usb_vid,true);
	USB_PID = (unsigned short)cfg.Vulong(s_usb_pid,true);
}

int CTObj_usb_np_io::TestOp(ConfigFile &cfg)
{	

	is_tested = true;
	result = false;
	result = is_usb_dev_connected(USB_VID,USB_PID);

	if(result) {
		param = "";
		param.Format("usb:vid_%.4x pid_%.4x ok",USB_VID,USB_PID);
	}else {
		param = "";
		param.Format("usb:vid_%.4x pid_%.4x err",USB_VID,USB_PID);
	}
	return(result);


}
#endif
#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_vbus_usb33_io::CTObj_vbus_usb33_io(const char* pname,testSet *Tset, int ID,
					 ConfigFile &cfg):CTestItem(pname,Tset,ID)
{

}


bool CTObj_vbus_usb33_io::set_vbus(bool lv)
{
   if(tset->dev_id == TEST_DEV_ID_A)
	{
        mcu_gpio_set_output_cmd.Format("MCU+SET(1,E,%04x,5)", (1 << 7));   //PE7 output high, vbus 5V power on
        mcu_gpio_wr_cmd.Format("MCU+WR(1,E,%04x,%04x)", (1 << 7),(lv << 7));
		if(!pMCU2_Com->send_and_verify(mcu_gpio_set_output_cmd, 0, "MCU+OK\r\n", 0))
		{
		    param = "mcu set err";
			return false;
		}
		if(!pMCU2_Com->send_and_verify(mcu_gpio_wr_cmd, 0, "MCU+OK\r\n", 0))
		{
			param = "mcu wr err";
			return false;
		}
	}
	else
	{
		mcu_gpio_set_output_cmd.Format("MCU+SET(0,E,%04x,5)", (1 << 8));    //PE8 output High, vbus 5V power on
		mcu_gpio_wr_cmd.Format("MCU+WR(0,E,%04x,%04x)", (1 << 8),(lv << 8));
		if(!pMCU1_Com->send_and_verify(mcu_gpio_set_output_cmd, 0, "MCU+OK\r\n", 0))
		{
		    param = "mcu set err";
			return false;
		}
		if(!pMCU1_Com->send_and_verify(mcu_gpio_wr_cmd, 0, "MCU+OK\r\n", 0))
		{
			param = "mcu wr err";
			return false;
		}
	}

    return (true);
}


bool CTObj_vbus_usb33_io::is_vbus_lv(bool lv)
{
#if 0
	enum 
	{
		CHAGE_TRICK = 0,
		CHAGE_HIGH_CUR,
		CHAGE_IDLE_ERR,
		CHAGE_STANDBY,
		CHAGE_NO_POWER,
	};
#endif
	uint16 charg_statu = 0xffff;

	int32 status = bccmdChargerStatus(tset->pSpiObj->iHandle,&charg_statu);
	if(status != TE_OK) {
		param = "get vbus statu err";
		return(false);
	}
	if(lv) {
		if(charg_statu > CHAGE_STANDBY) {
			param = "";
			param.Format("vbus high statu:%x err",charg_statu);
			return(false);
		}
		
	}else {
		if(charg_statu != CHAGE_NO_POWER) {
			param = "";
			param.Format("vbus low statu:%x err",charg_statu);
			return(false);
		}
		else
		{
		    param.Format("vbus low statu:%x err",charg_statu);
		}
	}

    return(true);

}

int CTObj_vbus_usb33_io::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;
    	
    int32 status = set_vbus(1);
	if(status != TE_OK) return(false);
	if(!is_vbus_lv(1)) return(result);

	param = "vbus io ok";
	result = true;
	return(result);

}
#endif
#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_audio_io::CTObj_audio_io(const char* pname,testSet *Tset,int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
        if(tset->dev_id == TEST_DEV_ID_A) pcfg = &pMainWnd->AAudio_cfg;
        else pcfg = &pMainWnd->BAudio_cfg;

        CDutGlodenPar *pgloden = pMainWnd->dutGloden[DUT_PAR_IDX];
        max_thd = pgloden->thd;
        min_amp = pgloden->min_lv;
        max_amp = pgloden->max_lv;

        /*1:left, 2: right, 3: both*/
        spk_ch_mode = pCfgf.get_spk_ch_mode();
        mic_ch_mode = pCfgf.get_mic_ch_mode();
        
        rec_time = cfg.Vulong(s_audio_rec_max_time);
        wait_time = cfg.Vulong(s_audio_max_time);
        if(wait_time < rec_time) wait_time = rec_time;

        watch_times = cfg.Vulong(s_audio_watch_times);
        if(watch_times < 8) watch_times = 8;
        av_times = 6;
        audio_valid_coef = 0.8;

        data_valid = false;
        par_is_gloden = false;
}


void CTObj_audio_io::gen_prompt(void)
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
        range.Format("Thd<=%.1lf%% Amp[%.1lf,%.1lf]",max_thd,min_amp,max_amp);
	tset->tlist->AddItem(itemstr,name, range.GetBuffer(0), "", "");
}


void CTObj_audio_io::update_range(void)
{
        ConfigFile &cfg = pCfgf;
        CDutGlodenPar *pgloden = pMainWnd->dutGloden[DUT_PAR_IDX];
        bool update = false;

        int gloden = tset->gloden_idx();
        if(gloden)
        { 
                par_is_gloden = true;
                update = true;
                pgloden = pMainWnd->dutGloden[gloden];    
        }
        else if(par_is_gloden)
        { 
                par_is_gloden = false;
                update = true;
        }

        if(update)
        { 
                max_thd = pgloden->thd;
                min_amp = pgloden->min_lv;
                max_amp = pgloden->max_lv;

                item_range = "";
                item_range.Format("Thd<=%.1lf%% Amp[%.1lf,%.1lf]",max_thd,min_amp,max_amp);
                TSET_MSG(MSG_SHOW_RANGE,tset);
        }
}

int CTObj_audio_io::ItemTestReady(ConfigFile &cfg)
{
        mutex_holde1 = false;
		mutex_holde2 = false;
        if(!pMainWnd->glbResource_get(GRESOURCE_SOUNDKARD)) 
        {
           param = "get SNDCARD gsource err";
           return(ERR_STOP);
        }
        mutex_holde1 = true;
        
		if(!pMainWnd->glbResource_get(GRESOURCE_MCU1))
		{
		   param = "get u18 gsource err";
           return(ERR_STOP);
		}
		mutex_holde2 = true;

	return(NO_ERR);
}

int CTObj_audio_io::ItemTestClean(ConfigFile &cfg)
{
        if(mutex_holde1)
        { 
                pMainWnd->glbResource_put(GRESOURCE_SOUNDKARD);
                mutex_holde1 = false;
        }
        
		if(mutex_holde2)
        { 
                pMainWnd->glbResource_put(GRESOURCE_MCU1);
                mutex_holde2 = false;
        }

	return(NO_ERR);
}

void CTObj_audio_io::ClearResult(void)
{
        CTestItem::ClearResult();
	    data_valid = false;
        param = "";
}

int CTObj_audio_io::TestOp(ConfigFile &cfg)
{  
	    CAudio_par &audio_obj = pMainWnd->audio_obj;
        CBsDevice *pobj = tset->pSpiObj;
        struct static_obj_t back[2];
        result = false;
        is_tested = true;
        data_valid = false;
	   
	   if(!pMCU1_Com)
	   {
	      param = "null com";
		  return(ERR_NOMAL);
	   }

       //audio short route pc to pc off
	   if(!pMCU1_Com->set_test_board_short_pc_off(0, MCU_PIO_AUDIO_CHK, MCU_PIO_NUM_AUDIO_CHK))
	   {
	       param = "set audio short to pc off err";
		   return(ERR_NOMAL);
	   }

       //audio test dut sw on
       if(tset->dev_id == TEST_DEV_ID_A)
	   {
		   pMCU2_Com->set_dut_5v_off(1, MCU_PIO_DUT1_5V, MCU_PIO_NUM_DUT1_5V);

		   if(!pMCU1_Com->set_dut1_rout_audio_on(0,MCU_PIO_DUT2_AUDIO,MCU_PIO_NUM_DUT2_AUDIO))
		   {
               param = "set dut1 audio rout err";
		       return(ERR_NOMAL);
		   }
	   }
	   else
	   {
		   pMCU1_Com->set_dut_5v_off(0, MCU_PIO_DUT2_5V, MCU_PIO_NUM_DUT2_5V);

		   if(!pMCU1_Com->set_dut2_rout_audio_on(0,MCU_PIO_DUT2_AUDIO,MCU_PIO_NUM_DUT2_AUDIO))
		   {
		       param = "set dut2 audio rout err";
		       return(ERR_NOMAL);
		   }
	   }
	   
       //Ampifier sw on, for 158 module
	   if(pCfgf.ItemSelect(s_audio_amp_sel))
	   {
		   if(!pMCU1_Com->set_dut_spk_amp_on(0, MCU_PIO_DUT_SPK_AMP, MCU_PIO_NUM_DUT_SPK_AMP))
		   {
			   param = "set amplifier on err";
		       return(ERR_NOMAL);
		   }
	   }


	   int mode = pCfgf.Vint(s_audio_mic_spk_mode);
	   int32 status = radiotestStereoCodecLB(pobj->iHandle, 44100, mode);
	   if(status != TE_OK)
	   {
		   Sleep(50);
		   status = radiotestStereoCodecLB(pobj->iHandle, 44100, mode);  //try 2nd time
		   if(status!= TE_OK)
		   {
			   param =  "audio loop back err";
			   result = false;
			   return(ERR_STOP);
		   }
	   }

        param = "setup audio deal";
        show_message();

        audio_par = *pcfg;
        audio_par.valid = false;
        audio_par.record_num = watch_times;
        if(audio_par.RShowID && audio_par.LShowID)
        { 
                audio_par.valid_lv = audio_par.Llv * audio_valid_coef;
                if(audio_par.valid_lv > (audio_par.Rlv * audio_valid_coef))
                        audio_par.valid_lv = audio_par.Rlv * audio_valid_coef;
        }
        else if(audio_par.RShowID)
        { 
                audio_par.valid_lv =audio_par.Rlv * audio_valid_coef;
        }
        else
        { 
                audio_par.valid_lv = audio_par.Llv * audio_valid_coef;
        }
        audio_par.RShowID = pcfg->RShowID;
        audio_par.LShowID = pcfg->LShowID;

        back[0].id_idx = audio_par.RShowID;
        back[1].id_idx = audio_par.LShowID;
        pMainWnd->SendMessage(WM_AUDIO_TEST,(WPARAM)0,(LPARAM)back);
            
        audio_obj = audio_par;
        param = "wait audio deal";
        show_message();
        if(!pMainWnd->start_audio_deal(&audio_obj,rec_time))  param = "start audio deal fail";
        else
        {
                unsigned long time = 0;
                while(true)
                {
                        if(!pMainWnd || !pMainWnd->soundio) return(ERR_TERMINAL);
                        if(audio_obj.valid)
                        {
                                if(pcfg->RShowID && pcfg->LShowID) 
                                        param.Format("L(%.2lf%% %.0lf) R(%.2lf%% %.0lf)",
                                                     audio_obj.Lthd,audio_obj.Llv,audio_obj.Rthd,audio_obj.Rlv);
                                else if(pcfg->LShowID) param.Format("L(%.2lf%% %.0lf)",audio_obj.Lthd,audio_obj.Llv);
                                else if(pcfg->RShowID) param.Format("R(%.2lf%% %.0lf)",audio_obj.Rthd,audio_obj.Rlv);
                                show_message();
                        }
                        if(time >= wait_time) break;
                        Sleep(100);
                        time += 100;
                }
                
                int deal_len = 0;
                int rx_len = pMainWnd->audio_tdata_len;
                if(rx_len > MAX_AUDIO_TDATA) rx_len = MAX_AUDIO_TDATA;
                int idx = pMainWnd->audio_tdata_idx;
                CAudio_par *tdata = pMainWnd->audio_tdata;
                CAudio_par temp_par;
                if((rx_len < (av_times+2))) param = "audio data small";
                else if(!pMainWnd->get_fine_audio_value(&temp_par))
                {
                        param = "get data err";
                        result = false;
                }
                else
                {
                        CAudio_par &pc_audio = pMainWnd->pc_cfg;
                        data_valid = true;
                        Lresult = true;
                        Rresult = true;
                        Llv = temp_par.Llv;
                        Rlv = temp_par.Rlv;
                        Lthd = temp_par.Lthd;
                        Rthd = temp_par.Rthd;
                        if(Lthd > pc_audio.Lthd) Lthd -= pc_audio.Lthd;
                        else Lthd = 0.0;
                        if(Rthd > pc_audio.Rthd) Rthd -= pc_audio.Rthd;
                        else Rthd = 0.0;

                        Llv = tset->gloden_vamplitude * Llv;
                        Llv = Llv/pcfg->Llv;
                        Rlv = tset->gloden_vamplitude * Rlv;
                        Rlv = Rlv/pcfg->Rlv;
                        if((Llv < min_amp) || (max_amp < Llv)) Lresult = false;
                        if((Rlv < min_amp) || (max_amp < Rlv)) Rresult = false;

                        if(pcfg->RShowID && pcfg->LShowID)
                        { 
                                result = (Lresult && Rresult && (Lthd <= max_thd) && (Rthd <= max_thd)); 

                                param = "";
                                param.Format("L(%.2lf%% %.1lf) R(%.2lf%% %.1lf)",Lthd,Llv,Rthd,Rlv);
                        }
                        else if(pcfg->RShowID)
                        {
                                result = (Rresult && (Rthd <= max_thd)); 

                                param = "";
                                param.Format("R(%.2lf%% %.1lf)",Rthd,Rlv);
                        }
                        else if(pcfg->LShowID)
                        {
                                result = (Lresult && (Lthd <= max_thd)); 

                                param = "";
                                param.Format("L(%.2lf%% %.1lf)",Lthd,Llv);
                        }
                }

                pMainWnd->stop_audio_deal(&audio_obj);
                pMainWnd->SendMessage(WM_AUDIO_TEST,(WPARAM)1,(LPARAM)back);
        }

        if(result) return(NO_ERR);
        return(ERR_STOP);
}

void CTObj_audio_io::record_header(ofstream &fs)
{
        if(pcfg->RShowID && pcfg->LShowID)
        {
                fs<<"Audio L-THD(%),Audio L-level(mv),Audio R-THD(%),Audio R-level(mv)"<<", ";
        }
        else if(pcfg->LShowID) fs<<"Audio L-THD(%),Audio L-level(mv)"<<", ";
        else fs<<"Audio R-THD(%),Audio R-level(mv)"<<", ";
}

void CTObj_audio_io::record_result(ofstream &fs)
{
        CString temp;

        if(pcfg->RShowID && pcfg->LShowID)
        {
                if(!data_valid) 
                {
                        temp = param;
                        format_record_par(temp);
                        if(is_tested) temp += ",/F,/F,/F,";
                        else temp += ",NC,NC,NC,";
                }
                else
                {
                        temp = "";
                        temp.Format("%.2lf,%.1lf,%.2lf,%.1lf,",Lthd,Llv,Rthd,Rlv);
                }
        }
        else if(pcfg->LShowID)
        {
                if(!data_valid) 
                {
                        temp = param;
                        format_record_par(temp);
                        if(is_tested) temp += ",/F,";
                        else temp += ",NC,";
                }
                else
                {
                        temp = "";
                        temp.Format("%.2lf,%.1lf,",Lthd,Llv);
                }
        }
        else 
        {
                if(!data_valid) 
                {
                        temp = param;
                        format_record_par(temp);
                        if(is_tested) temp += ",/F,";
                        else temp += ",NC,"; 
                }
                else
                {
                        temp = "";
                        temp.Format("%.2lf,%.1lf,",Rthd,Rlv);
                }
        }

        fs << temp;
        if(result) fs<<"/P,";
        else if(!is_tested) fs << "NC,";
        else fs << "/F,";
}
#endif
#if 1

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_read_addr::CTObj_read_addr(const char* pname, testSet *Tset,int ID, 
								 ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
}

int CTObj_read_addr::TestOp(ConfigFile &cfg)
{
	static const CString BD_ADDR_CS_NAME = "bdaddr";
	

	is_tested = true;
	result = false;
	tset->r_addr.is_valid = false;


	uint16 bdAddrValue[3];
	uint16 length = 3;

	int32 retVal = uetCsCacheReadItem(tset->pSpiObj->iHandle, BD_ADDR_CS_NAME, bdAddrValue, &length);

    if(retVal != UET_OK) 
	{
		param =  "read bt addr err";
    }
	else 
	{
        //read adrr to check if is gloden
		tset->r_addr.nap = bdAddrValue[2];
		tset->r_addr.uap = bdAddrValue[1]>>8;
		tset->r_addr.lap = ((bdAddrValue[1]&0xff)<<16) | bdAddrValue[0];
		tset->r_addr.is_valid = true;

		tset->is_golden = false;
		if(tset->gloden_idx()) 
			tset->is_golden = true;
        tset->pdev_addr = &tset->r_addr;

		param = "";
		if(tset->is_golden)
		{
		   param.Format("bt addr:%.4X%.2X %.6X is golden",tset->r_addr.nap,tset->r_addr.uap,tset->r_addr.lap);
		}
		else
		{
		   param.Format("bt addr:%.4X%.2X %.6X",tset->r_addr.nap,tset->r_addr.uap,tset->r_addr.lap);
		}
		result = true;
	}

	return(result);
}
#endif
#if 1


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_input_addr::CTObj_input_addr(const char* pname,testSet *Tset,int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
	p_address = &tset->addr;   //输入框中的蓝牙地址或自动生成的蓝牙地址
	man_input_type = cfg.ItemSelect(s_InputAddress_man);
    default_address_write_only = cfg.ItemSelect(slaveDefAddrWOnly);
	
	unsigned long temp = cfg.Vulong(slaveAddrNap_Uap,true);
	nap = (int)((temp>>8)&0xffff);
	uap = (int)(temp&0xff);
	
	//== slaveAddrCur_Lap
	temp = cfg.Vulong(slaveAddrLap,true);
	min_lap = (int)temp;
	temp = cfg.Vulong(slaveTerminalAddrLap,true);
	max_lap = (int)temp;

}

void CTObj_input_addr::gen_prompt(void)
{
	char itemstr[8];
	itoa(Index(),itemstr,10);
	
	CString range;
	if(man_input_type) range.Format("%.4X%.2X[%.6X,%.6X]",nap,uap,min_lap,max_lap);
	else range.Format("<= %.4X%.2X %.6X",nap,uap,max_lap);
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}

bool CTObj_input_addr::is_jx_addr(void)
{
        if(!p_address->is_valid) 
			return(false);

        p_address->is_valid = false;
        //if((nap != p_address->nap) || (uap != p_address->uap)) 
		//	return(false);
        if(p_address->lap > max_lap) 
			return(false);
        if(p_address->lap < min_lap) 
			return(false);
        //if((p_address->lap >= 0x9e8b00) && (p_address->lap <= 0x9e8b3f)) 
		//	return(false);

        p_address->is_valid = true;
        return(true);
}

bool CTObj_input_addr::auto_gen_addr(ConfigFile &cfg)
{
        if(!man_input_type) return(false);

        if(p_address->is_valid)
        {
            param = "";
		    param.Format("%.4X%.2X %.6X",p_address->nap,p_address->uap,p_address->lap);
            return(true);
        }

        if(!pMainWnd->glbResource_get(GRESOURCE_ADDR))
        {
            param = "get gsource err";
            return(false);
        }
		pMainWnd->address_input_running = 1;
		pMainWnd->SendMessage(WM_INPUT_BT_ADDRESS,(WPARAM)NULL,(LPARAM)p_address);  //auto gen: bt address current (lap+1)
		while(pMainWnd->address_input_running);
		pMainWnd->glbResource_put(GRESOURCE_ADDR);
		
		if(p_address->is_valid) 
		{
			param = "";
			param.Format("%.4X%.2X %.6X",p_address->nap,p_address->uap,p_address->lap);
			return(true);
		}
	
        param = "NULL";
        return(false);
}

int CTObj_input_addr::TestOp(ConfigFile &cfg)
{
	    is_tested = true;
	    result = false;
	char *show_mac;
        if(man_input_type)
        { 
            result = auto_gen_addr(cfg);

            if(!result) return(ERR_NOMAL);
            else return(NO_ERR);
        }

        if(tset->dev_id == TEST_DEV_ID_A)
        { 
                CString temp = pMainWnd->adev_input_addr;
                if(!pMainWnd->adev_address_inputed) param = "not input";
                else
                {
					//从输入框输入，设置为蓝牙写入地址
					if(!get_format_bt_addr(temp,tset->addr) || !is_jx_addr()) 
						param = temp;
					else
					{
						//param.Format("%.4X%.2X %.6X",p_address->nap,p_address->uap,p_address->lap);
						param = temp;
						pMainWnd->cur_mac_data = temp;
						pMainWnd->m_adev_address = temp;
						result = true;
					}
                }

                temp = "";
                pMainWnd->adev_address_inputed = false;
                UPDATE_ADDRINPUT(IDC_ADEV_ADDRESS,temp);
        }
        else
        {
                CString temp = pMainWnd->bdev_input_addr;
                if(!pMainWnd->bdev_address_inputed) param = "not input";
                else
                {
                        //if(!get_format_bt_addr(temp,tset->addr) || !is_jx_addr()) 
                        //	param = temp;
                        //else
                        {
                             //param.Format("%.4X%.2X %.6X",p_address->nap,p_address->uap,p_address->lap);
                             //show_mac = (char*)((LPCTSTR)temp);
                           //strupr(show_mac);//小写变大写//yhl 2018/10/31
							 param = temp;
							 pMainWnd->cur_mac_data = temp;
							 pMainWnd->m_bdev_address = temp;
                             result = true;
                        }
                }

                temp = "";
                pMainWnd->bdev_address_inputed = false;
                UPDATE_ADDRINPUT(IDC_BDEV_ADDRESS,temp);
        }

        if(!result) 
			return(ERR_NOMAL);
        else 
			return(NO_ERR);
}
#endif
#if 1
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_waddr::CTObj_waddr(const char* pname,testSet *Tset, int ID, 
								   ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
	man_input_type = cfg.ItemSelect(s_InputAddress_man);
	default_address_write_only = cfg.ItemSelect(slaveDefAddrWOnly);
}

bool CTObj_waddr::is_dev_addr_equ_input(void)
{
    return (tset->addr.is_valid &&
		   tset->r_addr.is_valid &&
		   (tset->addr.nap == tset->r_addr.nap) &&
		   (tset->addr.uap == tset->r_addr.uap) &&
		   (tset->addr.lap == tset->r_addr.lap)
		);
}

int CTObj_waddr::TestOp(ConfigFile &cfg)
{
	    CBsDevice *pobj = tset->pSpiObj;
        struct bt_address_s* paddr = &tset->addr;
        result = false;
        is_tested = true;
		static const CString BD_ADDR_CS_NAME = "bdaddr";
		CString cmd_str;

        int gloden = tset->gloden_idx();
        if(gloden)
        { 
                result = true;
                param = tset->gloden_idx_name();
                return(NO_ERR);
        }
        if(is_dev_addr_equ_input())
        { 
                result = true;
                param = "same with input";
                return(NO_ERR);
        }

        //在不是手动输入蓝牙地址情况下，
		//当只写默认地址时，读到的地址既不是dut默认地址，也不是默认uap/nap grp，就不写默认地址，保持蓝牙现有地址
		if(!man_input_type && (default_address_write_only && !tset->is_default_addr()))
        { 
                result = true;
                param = "not default";
                return(NO_ERR);
        }
		/*
        if(!paddr->is_valid)
        { 
                param = "invalid address";
                return(ERR_NOMAL);
        }
		*/

		

		
        tset->pdev_addr = &tset->addr;
        //param = "";
        //param.Format("%.4X%.2X %.6X",paddr->nap,paddr->uap,paddr->lap);
		//param = pMainWnd->cur_mac_data;

		//iwpriv config_set wmap,11a,00e04c871234	//11a:(hex)00e04c871234:(hex)		
		//Sleep(1000);
		if(tset->dev_id == TEST_DEV_ID_B)
		{
			if(!pRTL8711_2_Com)
			{
			   	param = "null RTL87xx2 COM";
			   	return (false);
			}
			
			//param = pMainWnd->bdev_input_addr;
			param = pMainWnd->m_bdev_address;
			cmd_str.Format("iwpriv config_set wmap,11a,%s\r\n",param);
			//pRTL8711_2_Com->send(cmd_str, 0);
			if(!pRTL8711_2_Com->send_and_search(cmd_str, 0, "[MEM]", 0)) 
			{
			    param = "rtl87xx set err";
				return (false);
			}
		}
		else
		{
			if(!pRTL8711_1_Com)
			{
			   	param = "null RTL87xx1 COM";
			   	return (false);
			}
			
			//param = pMainWnd->adev_input_addr;
			param = pMainWnd->m_adev_address;
			cmd_str.Format("iwpriv config_set wmap,11a,%s\r\n",param);
			//pRTL8711_1_Com->send(cmd_str, 0);
			if(!pRTL8711_1_Com->send_and_search(cmd_str, 0, "[MEM]", 0)) 
			{
			    param = "rtl87xx set err";
				return (false);
			}
		}

		
        result = true;

		return(NO_ERR);
}
#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#if 0
CTObj_modify_address::CTObj_modify_address(const char* pname,testSet *Tset,int ID, ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
	old_addr.lap = 0;
	old_addr.nap = 0;
	old_addr.uap = 0;
	old_addr.is_valid = 0;

	new_addr.is_valid = cfg.get_tx_rand_uid(false,new_addr.nap,new_addr.uap,new_addr.lap);
}


void CTObj_modify_address::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	if(!new_addr.is_valid) {
		range.Format("BT cfj Addr:%.4x%.2x-%.6x",0,0,0);
	}else range.Format("BT cfj Addr:%.4x%.2x-%.6x",
			   new_addr.nap,new_addr.uap,new_addr.lap);
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}

int CTObj_modify_address::TestOp(ConfigFile &cfg)
{
	uint16 nap;
	uint8 uap;
	uint32 lap;
	result = false;
	is_tested = true;

	if(!new_addr.is_valid) {
		param =  "address config err";
		return(false);
	}

	    int32 status = psReadBdAddr(tset->pSpiObj->iHandle,&lap,&uap,&nap);
        if(status != TE_OK) {
		param =  "read address err";
		return(false);
        }

	if((new_addr.nap == nap) && (new_addr.uap == uap)){
		param = "";
		param.Format("mAddr:%.4x%.2x-%.6x is ok",nap,uap,lap);
	}else {
		new_addr.lap = lap;
		status = psWriteBdAddr(tset->pSpiObj->iHandle,new_addr.lap,new_addr.uap,new_addr.nap);
		if(status != TE_OK) {
			param = "";
			param.Format("mAddr(nap uap lap):%.4x %.2x %.6x err",
				     new_addr.nap,new_addr.uap,new_addr.lap);
			return(false);
		}

		param = "";
		param.Format("mAddr:%.4x%.2x-%.6x to %.4x%.2x-%.6x ok",nap,uap,lap,
			     new_addr.nap,new_addr.uap,new_addr.lap);
	}

	result = true;
	return(result);
}

#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

#if 0
CTObj_rf_pwr::CTObj_rf_pwr(const char* pname, testSet *Tset,int ID, struct rf_pwr_s &par,
			   bool is_bs,ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
	rf_par = par;
}

void CTObj_rf_pwr::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	range.Format("CH:%.2d Lv[%.1f %.1f] PA:%d %d",
		     rf_par.ch,rf_par.min,rf_par.max,rf_par.internal_pa,rf_par.external_pa);
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}

int CTObj_rf_pwr::TestOp(ConfigFile &cfg)
{
	result = false;
	is_tested = true;

	VSpecAnaly *Spec= pSpecAnaly;
	if(!Spec) {
		param = "no SpecAnaly";
		return(false);
	}

	param = "update Spec span";
	show_message();
	if(!Spec->UpdateSpan()) {
		param = "SpecAnaly op err";
		return(false);
	}

	param = "update Spec rband";
	show_message();
	if(!Spec->UpdateRB()) {
		param = "SpecAnaly op err";
		return(false);
	}

	param = "";
	param.Format("set Spec freq:%.1fMHz",rf_par.ch);
	show_message();
	if(!Spec->SetCenterF(2400+rf_par.ch,'M')) {
		param = "SpecAnaly op err";
		return(false);
	}

	int32 status = radiotestTxstart(tset->pSpiObj->iHandle,
					2400+rf_par.ch,
					rf_par.internal_pa,
					rf_par.external_pa,0);
        if(status != TE_OK) {
		param =  "enter radio test mode err";
		return(false);
        }
	if(rf_par.stable_time) Sleep(rf_par.stable_time);

	double f = 0.0,lv = 0.0;
	if(!pSpecAnaly->get_measure_value(f,lv)) {
		param = "freq dev read err";
		result = false;
		return(false);
	}

	if((lv >= rf_par.min) && (lv <= rf_par.max)) result = true;
	param = "";
	param.Format("CH:%.2d rf_pwr:%.1fdBm",rf_par.ch,lv);
	return(result);
}

#endif


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#if 0

CTObj_crystal_adj::CTObj_crystal_adj(const char* pname, testSet *Tset,int ID, 
				     ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
	use_ch = cfg.Vulong(slaveXtalCHValue);
	fcounter_offset = cfg.VDouble(ParFCountfreqOffset);

	if(use_ch < 4) use_ch = 4;
	else if(use_ch > 80) use_ch = 80;

	crystal_par_max = cfg.Vint(slaveXtalParMax);
	crystal_par_min = cfg.Vint(slaveXtalParMin);
	tx_open_delay = cfg.Vulong(slaveXtalTxOpenDelay); 

	if(!stable_time) stable_time = 100;
	center_freq = (2400+use_ch)*1000;
}

void CTObj_crystal_adj::gen_prompt()
{
	char itemstr[8];
	CString range;

	range.Format("CH:%d,loss:%.2fK,range:%d~%d",use_ch,fcounter_offset,crystal_par_min,crystal_par_max);
	itoa(Index(),itemstr,10);
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}


int CTObj_crystal_adj::ItemTestReady(ConfigFile &cfg)
{   
	mutex_hodle2 = false;

	if(!pMainWnd->glbResource_get(GRESOURCE_CRYSTAL_ADJ))
	{
		param = "get gsource err";
		return(ERR_STOP);
	}
	mutex_hodle2 = true;

	return(NO_ERR);
}

int CTObj_crystal_adj::ItemTestClean(ConfigFile &cfg)
{

	if(mutex_hodle2)
	{ 
		pMainWnd->glbResource_put(GRESOURCE_CRYSTAL_ADJ);
		mutex_hodle2 = false;
	}

	return(NO_ERR);
}


bool CTObj_crystal_adj::getFreq_Fcounter(double &f)
{
    FCOUNTER_DEV *pcounter= pFCounter;
	if(!pcounter) {
		param = "no FCounter";
		return(false);
	}
	
	param = "";
	param.Format("%.1fMHz(dly:%d)",2400+use_ch,tx_open_delay);
	show_message();
	int32 status = radiotestTxstart(tset->pSpiObj->iHandle,2400+use_ch,50,255,0);
        if(status != TE_OK)
		{
		   param =  "enter radio test mode err";
		   return(false);
        }
	if(tx_open_delay) Sleep(tx_open_delay);
	
	param = "read freq";
	show_message();
        if(!pcounter->ReadFrequency(f,this))
        { 
		    param = "read freq err";
		    return(false);
        }
        return(true);
}


int CTObj_crystal_adj::TestOp(ConfigFile &cfg)
{
	result = false;
	is_tested = true;
	int32 status;
	int i;


	double freq = 0.0;
	double freq_save = 0.0;

	FCOUNTER_DEV *pcounter= pFCounter;
	if(!pcounter)
	{
		param = "no FCounter";
		return(false);
	}

    if(!pMCU1_Com)
	{
	   param = "null com";
	   return (ERR_STOP);
	}
	
	uint32 lap;
	uint8  uap;
	uint16 nap;
	if(psReadBdAddr(tset->pSpiObj->iHandle, &lap, &uap, &nap) != TE_OK)
	{
		param = "read bt address err";
		return (ERR_STOP);
	}

	tset->r_addr.lap = lap;
	tset->r_addr.nap = nap;
	tset->r_addr.uap = uap;
	tset->r_addr.is_valid = true;

	if(tset->gloden_idx())
	{ 
		param = "";
		param.Format("%.4X%.2X %.6X is golden\n",tset->r_addr.nap,tset->r_addr.uap,tset->r_addr.lap);
		tset->xtal_adj_is_tested = true;
		result = true;
		return (result);
	}	

	if(!pMCU1_Com->set_rout_freq_on(0, MCU_PIO_DUT_8852_METER,MCU_PIO_NUM_DUT_8852_METER))
	{
		param = "set route freq err";
		return (ERR_STOP);
	}
	//Sleep(2000);

	//dut1 route to freq and 8852
	if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pMCU1_Com->set_freq_route_dut1(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch freq to dut1 err";
			return (ERR_STOP);
		}
	}
	else
	{
		if(!pMCU1_Com->set_freq_route_dut2(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch freq to dut2 err";
			return (ERR_STOP);
		}
	}
    //Sleep(1000);

    param = "";
	param.Format("%.1fMHz(dly:%dms)",2400+use_ch,tx_open_delay);
	show_message();

	i = 3;
	do
	{
	    status = radiotestTxstart(tset->pSpiObj->iHandle,2400+use_ch,50,255,0);
		if(status == TE_OK) break;
		Sleep(50);
	}while(i--);

	if(status != TE_OK)
	{
		param =  "enter radio test mode err";
		return(ERR_STOP);
	}
	
	if(tx_open_delay) Sleep(tx_open_delay);
	param = "read freq";
	show_message();
	if(!pcounter->ReadFrequency(freq,this))
	{ 
		param = "read freq err";
		return(ERR_STOP);
	}

	//转成接收
	radiotestRxstart1(tset->pSpiObj->iHandle,2400+use_ch, 0, 0);

	freq_save = freq;
	freq += (fcounter_offset)*1000;    //频率计补偿,KHz转成Hz
	freq = freq/1000000/(2400+use_ch);
	freq = (freq-1)*1024*1024;
	int16 offset = (int16)freq;
	if((offset < crystal_par_min) || (offset > crystal_par_max)) 
	{
		param = "";
		param.Format("offset:%d out range,freq:%fMHz",offset,freq_save);
		return(ERR_STOP);
	}

	status = psWriteXtalOffset(tset->pSpiObj->iHandle,offset);
    if(status != TE_OK) 
	{
		param =  "psWriteXtalOffset err";
		return(ERR_STOP);
    }

	param = "";
	param.Format("offset:%d,freq:%fMHz",offset, freq_save);
	result = true;
	tset->xtal_adj_is_tested = true;
	return(result);
}
#endif
#if 0

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_reset::CTObj_reset(const char* pname, testSet *Tset, int ID, unsigned long rtime,bool need_vreg_on, 
						 ConfigFile &cfg):CTestItem(pname, Tset, ID)
{
	times = cfg.Vulong(s_RstTryTimes);
	time = rtime;
	delay_time = cfg.Vulong(s_RstDelayTime);
	need_vreg_on_s = need_vreg_on;
}

void CTObj_reset::gen_prompt()
{
	char itemstr[8];
	CString range;

	range.Format("delay:%d",delay_time);
	itoa(Index(),itemstr,10);
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}

int CTObj_reset::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;
    int32 status;

	for(int i=0;i<times;i++)
	{
		param = "";
		param.Format("reset bt try:%d",i+1);
		show_message();

		status = bccmdSetWarmReset(tset->pSpiObj->iHandle,1000);
		if(status == TE_OK) 
		{
			param = "";
			param.Format("delay:%d",delay_time);
			show_message();

			unsigned long ttime = 0;
			while(delay_time && (ttime < delay_time)) 
			{
				Sleep(500);
				ttime += 500;

				param = "";
				param.Format("delay:%d/%d",ttime,delay_time);
				show_message();
			}

			param = "";
			if(need_vreg_on_s)
			{
				if(tset->dev_id == TEST_DEV_ID_A)
				{
					if(!pMCU2_Com->set_dut_vreg_on(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG))
					{
						param = "reset bt ok, vreg on err";
						return (ERR_STOP);
					}
				}
				else
				{
					if(!pMCU1_Com->set_dut_vreg_on(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG))
					{
						param = "reset bt ok, vreg on err";
						return (ERR_STOP);
					}	
				}
				param += "reset ok, vreg on ok";
				result = true;
				return(result);
			}
			else
			{
				param += "reset bt ok";
				result = true;
				return(result);
			}	
		}
		if(status == TE_INVALID_HANDLE) break;
		if(time) Sleep(time);
	}

    param =  "reset bt err";
	return(result);
}
#endif
#if 1
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_flash_write::CTObj_flash_write(const char* pname,testSet *Tset, int ID, bool *cd,
									 ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
	xtal = 26; /* can only be 10, 16, 26, 32 */
	delay = 1;
	file = cfg.VCString(s_write_flash_filename);
	condition = cd;

}


void CTObj_flash_write::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	range.Format("(xtal:%dMHz delay:%d) %s",xtal,delay,file);
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}


int CTObj_flash_write::ItemTestReady(ConfigFile &cfg)
{
        mutex_holde = false;
        if(!pMainWnd->glbResource_get(GRESOURCE_FLASH_WR))
        {
                param = "get gsource err";
                return(ERR_TERMINAL);
        }
        mutex_holde = true;
	return(NO_ERR);
}

int CTObj_flash_write::ItemTestClean(ConfigFile &cfg)
{
        if(mutex_holde)
        { 
                pMainWnd->glbResource_put(GRESOURCE_FLASH_WR);
                mutex_holde = false;
        }
	return(NO_ERR);
}

#if 0
int CTObj_flash_write::TestOp(ConfigFile &cfg)
{
	result = false;
	is_tested = true;
	
	param = "Initialise flash...";
	show_message();
	if(flInitTrans(tset->pSpiObj->spi_port->port_num(), xtal, delay) != TFL_OK)
    {
		param.Format("Failed to initialise flash");
        return(ERR_STOP);
    }

	param = "StopProcessor";
	show_message();
	if(flStopProcessor() != TFL_OK)
	{
		param = "StopProcessor err";
		flClose();
		return(ERR_STOP);
		
	}

	flSetPios(11,10,0);

	param = "Erase flash...";
	show_message();
	if(flErase() != TFL_OK)
	{
		param.Format("Failed to Erase flash");
		flClose();
		return(ERR_STOP);
    }

	param = "Read flash program files...";
	show_message();
    if(flReadProgramFiles("flash.xuv") != TFL_OK)
    {
		param.Format("Failed to read flash program files");
        flClose();
        return(ERR_STOP);
    }

    if(flProgramBlock() != TFL_OK)
    {
		param.Format("Failed to spawn flash program thread");
        flClose();
        return(ERR_STOP);
    }
#if 1
	
	param.Format("ProgramSpawn");
	show_message();
    int32 progress;
	int time = 0;
    do
    {
        Sleep(1000);
        progress = flGetProgress();
		param = "";
		param.Format("writing(%ds):%d",++time,progress);
		param += "%";
		show_message();
    }while(progress < 100);
	
    int32 error = flGetLastError();
    if(error != TFL_OK)
    {
		param.Format("Programming failed with error: ");
        flClose();
        return(ERR_STOP);
    }
#endif
    flClose();
	
	param.Format("write flash ok");
	result = true;
	return(result);
}
#endif
#if 1
int CTObj_flash_write::TestOp(ConfigFile &cfg)
{
	result = false;
	is_tested = true;

	//param = "wait cal crc32...";
	//show_message();
#if 0
	if(!tset->file_crc32_cal)
	{
		tset->file_crc32_cal = 1;
		tset->file_crc32_err = true;
		if(cfg.wflash_crc32_is_ok(tset->xpv_crc32,tset->xdv_crc32))
			tset->file_crc32_err = false;
	}

	if(tset->file_crc32_err)
	{
		param = "";
		param.Format("err crc32:xpv:%.8x xdv:%.8x err",tset->xpv_crc32,tset->xdv_crc32);
		return(result);
	}
#endif
	if(condition && *condition)
	{
		param = "flash prog ok and no write";
		result = true;
		return(result);
	}


	param = "";
	param.Format("InitTrans at %s",tset->pSpiObj->spi_port->port_num());
	show_message();
    if(flInitTrans(tset->pSpiObj->spi_port->port_num(),xtal,delay) != TFL_OK)
	{
		param = "";
		param.Format("InitTrans at %s err",tset->pSpiObj->spi_port->port_num());
		return(result);
	}
	
	param = "StopProcessor";
	show_message();
	int16 aa=0;
	if((aa=flStopProcessor()) != TFL_OK)
	{
		param = "StopProcessor err";
		flClose();
		return(ERR_STOP);
		
	}
	
	flSetPios(11,10,0);
	
	param = "Erase flash...";
	show_message();
	if(flErase() != TFL_OK)
	{
		param.Format("Failed to Erase flash");
		flClose();
		return(ERR_STOP);
    }

	param = "";
	param.Format("LoadFiles %s",file.GetBuffer(0));
	show_message();
	if(flReadProgramFiles(file) != TFL_OK)
	{
		param = "";
		param.Format("LoadFiles \"%s\" err",file.GetBuffer(0));
		flClose();
		return(ERR_STOP);
	}

	param = "ProgramSpawn";
	show_message();
	if(flProgramSpawn() != TFL_OK)
	{
		param = "";
		param = "write flash err";
		flClose();
		return(ERR_STOP);
	}

	int progress, use_time = 0;
	do{
		Sleep(1000);
		progress = flGetProgress();
		use_time += 1;
		param = "";
		param.Format("use time:%ds,writing:%d",use_time,progress);
		param += "%";
		show_message();
	}while (progress < 100);

	int32 error = flGetLastError();
	if(error != TFL_OK)
	{
		param = "";
		param.Format("write flash with error:%.d",error);
		flClose();
		return(ERR_STOP);
	}
	flResetAndStart();
	flClose();
	Sleep(2000);
	param.Format("write flash ok");
	result = true;
	return(result);
}
#endif
#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_rback_cs::CTObj_rback_cs(const char* pname,testSet *Tset,
									 int ID,ConfigFile &cfg):CTestItem(pname,Tset,ID)
{

}

void CTObj_rback_cs::gen_prompt()
{
	char itemstr[8];
	CString range;

	range.Format("----------");
	itoa(Index(),itemstr,10);
	tset->tlist->AddItem(itemstr,name,range,"","","");
}

int CTObj_rback_cs::TestOp(ConfigFile &cfg)
{
	result = false;
	
	static const CString BD_ADDR_CS_NAME = "bdaddr";
	static const CString CRYSTAL_TRIM_CS_NAME = "crystal_ftrim";
	static const CString TX_POWER_LEVEL_CS_NAME = "tx_power_level";

	uint16 bdAddrValue[3];
	uint16 length = 3;
	
	int32 retVal = uetCsCacheReadItem(tset->pSpiObj->iHandle, BD_ADDR_CS_NAME, bdAddrValue, &length);
	
    if(retVal != UET_OK) 
	{
		param =  "read bt addr err";
		return(result);
    }
	else
	{
		tset->tset_cfgset.bk_addr.nap = bdAddrValue[2];
		tset->tset_cfgset.bk_addr.uap = bdAddrValue[1]>>8;
		tset->tset_cfgset.bk_addr.lap = ((bdAddrValue[1]&0xff)<<16) | bdAddrValue[0];
		tset->tset_cfgset.bk_addr.is_valid = true;
	}

	uint16 trimVal;
	length = 1;
	
	retVal = uetCsCacheReadItem(tset->pSpiObj->iHandle, CRYSTAL_TRIM_CS_NAME, &trimVal, &length);
	
    if(retVal != UET_OK) 
	{
		param =  "read crystal ftrim err";
		return(result);
    }
	else
	{
		tset->tset_cfgset.crystal_ftrim = trimVal;
	}

	uint16 PowerLelVal;
	length = 1;
	
	retVal = uetCsCacheReadItem(tset->pSpiObj->iHandle, TX_POWER_LEVEL_CS_NAME, &PowerLelVal, &length);
	
    if(retVal != UET_OK) 
	{
		param =  "read crystal ftrim err";
		return(result);
    }
	else
	{
		tset->tset_cfgset.tx_power_level = PowerLelVal;
	}






	result = true;
	param = "";
	param.Format("addr:%.4X%.2X%.6X,f:%d,p:%d",\
				tset->tset_cfgset.bk_addr.nap,\
				tset->tset_cfgset.bk_addr.uap,\
				tset->tset_cfgset.bk_addr.lap,\
				tset->tset_cfgset.crystal_ftrim,\
				tset->tset_cfgset.tx_power_level);

	return(result);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_wback_cs::CTObj_wback_cs(const char* pname,testSet *Tset,
									 int ID,ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
	
}

void CTObj_wback_cs::gen_prompt()
{
	char itemstr[8];
	CString range;

	range.Format("----------");
	itoa(Index(),itemstr,10);
	tset->tlist->AddItem(itemstr,name,range,"","","");
}

int CTObj_wback_cs::TestOp(ConfigFile &cfg)
{
	result = false;

	static const CString BD_ADDR_CS_NAME = "bdaddr";
	static const CString CRYSTAL_TRIM_CS_NAME = "crystal_ftrim";
	static const CString TX_POWER_LEVEL_CS_NAME = "tx_power_level";
	
	uint16 bdAddrValue[3];
	uint16 length = 3;
	bdAddrValue[0] = tset->tset_cfgset.bk_addr.lap & 0xffff;
	bdAddrValue[1] = (tset->tset_cfgset.bk_addr.uap<<8) | (tset->tset_cfgset.bk_addr.lap>>16) ;
	bdAddrValue[2] = tset->tset_cfgset.bk_addr.nap;
	int32 retVal = uetCsCacheWriteItem(tset->pSpiObj->iHandle, BD_ADDR_CS_NAME, bdAddrValue, &length);
	
    if(retVal != UET_OK) 
	{
		param =  "write bt addr err";
		return(result);
    }
	
	uint16 trimVal = tset->tset_cfgset.crystal_ftrim;
	length = 1;
	
	retVal = uetCsCacheWriteItem(tset->pSpiObj->iHandle, CRYSTAL_TRIM_CS_NAME, &trimVal, &length);
	
    if(retVal != UET_OK) 
	{
		param =  "write crystal ftrim err";
		return(result);
    }
	
	uint16 PowerLelVal = tset->tset_cfgset.tx_power_level;
	length = 1;
	
	retVal = uetCsCacheWriteItem(tset->pSpiObj->iHandle, TX_POWER_LEVEL_CS_NAME, &PowerLelVal, &length);
	
    if(retVal != UET_OK) 
	{
		param =  "write tx power level err";
		return(result);
    }
	
	retVal = uetCsCacheWriteToChip(tset->pSpiObj->iHandle, UET_NVM_TYPE_SPIFLASH, 1);
	if(retVal != UET_OK)
	{
		param = uetGetLastError(tset->pSpiObj->iHandle);
		return(result);
	}

	result = true;
	param = "";
	param.Format("write back cs ok");
	return(result);
}



#if 0

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_flash_verify::CTObj_flash_verify(const char* pname, testSet *Tset,int ID, bool write_cd,
									   ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
	xtal = 26; /* can only be 10, 16, 26, 32 */
	delay = 1;
	file = cfg.VCString(s_wfile_xpv_name);
	for_write = write_cd;
	verify_result = false;
}


void CTObj_flash_verify::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	range.Format("(x:%d d:%d) %s",xtal,delay,file);
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}

int CTObj_flash_verify::TestOp(ConfigFile &cfg)
{
	result = false;
	verify_result = false;
	is_tested = true;

	param = "wait cal crc32...";
	show_message();

    if(!tset->file_crc32_cal)
	{
		tset->file_crc32_cal = 1;
		tset->file_crc32_err = true;
		if(cfg.wflash_crc32_is_ok(tset->xpv_crc32,tset->xdv_crc32)) 
			tset->file_crc32_err = false;
	}

	if(tset->file_crc32_err)
	{
		param = "";
		param.Format("err crc32:xpv:%.8x xdv:%.8x err",tset->xpv_crc32,tset->xdv_crc32);
		return(result);
	}
	
	param = "";
    param.Format("InitTrans at %s",tset->pSpiObj->spi_port->port_num());
	
	show_message();
	
	if(flInitTrans(tset->pSpiObj->spi_port->port_num(),xtal,delay) != TFL_OK)
	{
		param = "";
		param.Format("InitTrans at %s err", tset->pSpiObj->spi_port->port_num());
		return(result);
	}

	param = "StopProcessor";
	show_message();
	if(flStopProcessor() != TFL_OK)
	{
		param = "StopProcessor err";
		flClose();
		return(ERR_STOP);
	}

	param = "";
	param.Format("LoadFiles %s",file.GetBuffer(0));
	show_message();
	if(flReadProgramFiles(file) != TFL_OK) {
		param = "";
		param.Format("LoadFiles \"%s\" err",file.GetBuffer(0));
		flClose();
		return(result);
	}

	param = "verify...";
	show_message();
	if(flVerify() != TFL_OK)
	{
		if(for_write)
		{
			result = true;
			param = "flash is not same with file";
		}
		else
			param = "verify err";
		flClose();
		
		return(ERR_STOP);
	}

	flClose();

	param = "verify ok";
	verify_result = true;
	result = true;
	return(result);
}
#endif
#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_rback_pskey::CTObj_rback_pskey(const char* pname,struct bk_pskey_item* item,testSet *Tset,
									 int ID,ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
	ps_item = item;
}

void CTObj_rback_pskey::gen_prompt()
{
	char itemstr[8];
	CString range;

	range.Format("pskey:0x%x",ps_item->pskey);
	itoa(Index(),itemstr,10);
	tset->tlist->AddItem(itemstr,name,range,"","","");
}

int CTObj_rback_pskey::TestOp(ConfigFile &cfg)
{
	result = false;

	const int len = 512;
	uint16 ver_len = 0;
	uint16 ver_read[len+1] = {0};
	int32 status;
	
	param = "";
	param.Format("read ps key:%d",ps_item->pskey);
	show_message();
	status = psRead(tset->pSpiObj->iHandle,(uint16)ps_item->pskey,PS_STORES_IFR,len,ver_read,&ver_len);
	if(status != TE_OK) 
	{
		param = "";
		param.Format("read ps key:%d err",ps_item->pskey);
		return(ERR_STOP);
	}
	tset->backup_pskey->set_data(ps_item,ver_len,ver_read);

	result = true;
	param = "";
	param.Format("rpskey(%x):",ps_item->pskey);
	CString temp = "";
	for(int i=0;i<ps_item->data_len;i++) 
	{
		temp = "";
		temp.Format("%.4x ",ps_item->data[i]);
		param += temp;
	}
	return(result);
}
#endif
#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_wback_pskey::CTObj_wback_pskey(const char* pname,struct bk_pskey_item* item,testSet *Tset,
									 int ID,ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
	ps_item = item;
}

void CTObj_wback_pskey::gen_prompt()
{
	char itemstr[8];
	CString range;

	range.Format("pskey:0x%x",ps_item->pskey);
	itoa(Index(),itemstr,10);
	tset->tlist->AddItem(itemstr,name,range,"","","");
}

int CTObj_wback_pskey::TestOp(ConfigFile &cfg)
{
	result = false;

	const int len = 512;
	uint16 ver_len = 0;
	uint16 ver_read[len+1] = {0};
	int32 status;
	
	param = "";
	param.Format("write ps key:%d",ps_item->pskey);
	show_message();
	status = psWriteVerify(tset->pSpiObj->iHandle,(uint16)ps_item->pskey,PS_STORES_IF,ps_item->data_len,ps_item->data);
	if(status != TE_OK) 
	{
		param = "";
		param.Format("write ps key:%d err",ps_item->pskey);
		return(ERR_STOP);
	}

	result = true;
	param = "";
	param.Format("wpskey(%x):",ps_item->pskey);
	CString temp = "";
	for(int i=0;i<ps_item->data_len;i++) 
	{
		temp = "";
		temp.Format("%.4x ",ps_item->data[i]);
		param += temp;
	}
	return(result);
}
#endif
#if 0


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_SpiLock::CTObj_SpiLock(const char* pname,testSet *Tset,int ID,ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
	reset_need = cfg.ItemSelect(s_SpiLock_reset);
	file = cfg.VCString(s_wload_xpv_name);

	file_crc32_err = 0;
	if(!cfg.wloader_crc32_is_ok(xpv_crc32,xdv_crc32))
	{
		file_crc32_err = true;
		xpv_crc32 = 0;
		xdv_crc32 = 0;
	}

	xtal = 26; /* can only be 10, 16, 26, 32 */
	delay = 1;
}

void CTObj_SpiLock::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	range.Format("%s:%.8x xdv:%.8x",file,xpv_crc32,xdv_crc32);
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0), "","");
}


bool CTObj_SpiLock::update_loader(void)
{
	param = "wait cal crc32...";
	show_message();
	if(file_crc32_err) 
	{
		param = "";
		param.Format("crc32:xpv:%.8x xdv:%.8x err",xpv_crc32,xdv_crc32);
		return(false);
	}
	
	if(flInitTrans(tset->pSpiObj->spi_port->port_num(),xtal,delay) != TFL_OK)
	{
		param = "";
		param.Format("InitTrans at %s err",tset->pSpiObj->spi_port->port_num());
		return(false);
	}

	param = "StopProcessor";
	show_message();
	if(flStopProcessor() != TFL_OK)
	{
		param = "StopProcessor err";
		flClose();
		return(false);
	}

	param = "";
	param.Format("LoadFiles %s",file.GetBuffer(0));
	show_message();
	if(flReadProgramFiles(file) != TFL_OK) 
	{
		param = "";
		param.Format("LoadFiles \"%s\" err",file.GetBuffer(0));
		flClose();
		return(false);
	}

	param = "ProgramSpawn";
	show_message();
	if(flProgramSpawn() != TFL_OK)
	{
		param = "";
		param = "write loader err";
		flClose();
		return(false);
	}

	int progress,use_time = 0;
	do{
		Sleep(1000);
		progress = flGetProgress();
		use_time += 1;
		param = "";
		param.Format("use time:%ds,writing:%d",use_time,progress);
		param += "%";
		show_message();
	}while (progress < 100);

	int32 error = flGetLastError();
	if(error != TFL_OK)
	{
		param = "";
		param.Format("write loader with error:%.d",error);
		flClose();
		return(false);
	}
	flClose();

	return(true);
}

int CTObj_SpiLock::TestOp(ConfigFile &cfg)
{
	result = false;
	is_tested = true;
    uint16 status;

	if (bccmdGetSpiLockStatus(tset->pSpiObj->iHandle,&status) != TE_OK)
	{
	    param = "read Status failed";
	}
	else
	{
	    switch(status)
	    {
	    case 0:
		{
		    int status = bccmdSetSpiLockCustomerKey(tset->pSpiObj->iHandle,ispi_key);
			if(status!= TE_OK)
		    {
				param = "Set Key failed";
		    }
		    else goto ____do_lock_spi;
		}
		break;
	    case 1:
			param = "SPI locked, customer key not set";
		break;
	    case 2:
	    ____do_lock_spi:
		{
		    uint16 temp = bccmdSpiLockInitiateLock(tset->pSpiObj->iHandle);
			uint16 err_code = teGetLastError(tset->pSpiObj->iHandle);

			if((temp != TE_OK) && (TE_ERROR_BCCMD_PERMISSION_DENIED == err_code)) 
			{
				if(tset->pSpiObj->iHandle) closeTestEngine(tset->pSpiObj->iHandle);
				tset->pSpiObj->iHandle = 0;

				result = update_loader();
				if(!result) return (ERR_STOP);
                tset->pSpiObj->iHandle = openTestEngineSpiTrans(tset->pSpiObj->spi_port->port_num(),0);
				
				if(!tset->pSpiObj->iHandle) return(ERR_STOP);

				if((temp = bccmdSetSpiLockCustomerKey(tset->pSpiObj->iHandle,ispi_key)) != TE_OK)
				{
					param = "Set Key failed";
					return(ERR_STOP);
				}
				temp = bccmdSpiLockInitiateLock(tset->pSpiObj->iHandle);
			}

			if (temp != TE_OK)
		    {
				param = "Set spi lock failed";
				param.Format("Set spi lock failed:%d",teGetLastError(tset->pSpiObj->iHandle));
		    }
		    else
		    {
				if(reset_need) bccmdSetWarmReset(tset->pSpiObj->iHandle,1000);
				param = "Set spi lock ok";
				result = true;
		    }
		}
		break;
	    case 3:
			param = "SPI locked";
			result = true;
		break;
	    default:
			param = "unknown SPI lock status";
		break;
	    }
	}
	return(result);
}
#endif
#if 0

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_SpiUnlock::CTObj_SpiUnlock(const char* pname,testSet *Tset,int ID,ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
	key_file = cfg.VCString(slaveSpiKeyfile);
	if(key_file == "") key_file = "";
}

int CTObj_SpiUnlock::TestOp(ConfigFile &cfg)
{
	is_tested = true;
	result = false;
	uint32 iHandle(0);

	iHandle = openTestEngineSpiTrans(tset->pSpiObj->spi_port->port_num(),0);

	if(!iHandle)
	{
		CString spi_trans = tset->pSpiObj->spi_port->get_trans();
		if(spi_unlock(1000, key_file, spi_trans))
		{
			iHandle = openTestEngineSpiTrans(tset->pSpiObj->spi_port->port_num(),0);
		}
		
		if(iHandle)
		{
			closeTestEngine(iHandle);
			param = "unlock success";
			result = true;
			return(result);
		}
		else
		{
			param = "unlock failed";
			return(ERR_STOP);
		}
	}
	else
	{
		closeTestEngine(iHandle);
		param = "device is unlocked";
		result = true;
		return(result);
	}	
}
#endif
#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_extern_flash::CTObj_extern_flash(const char* pname,testSet *Tset,int ID,ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
   	flash_file = "GXmesh.flash.xuv";	//cfg.VCString(slaveFlashPartionfile);
	if(flash_file == "") flash_file = "";
}

void CTObj_extern_flash::gen_prompt(void)
{
    char itemstr[8];
	itoa(Index(),itemstr,10);
	
	CString range;
	if(flash_file == "") 
		range ="null file";
	else
	    range.Format("burn %s ",flash_file);
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}

int CTObj_extern_flash::TestOp(ConfigFile &cfg)
{
    is_tested = true;
	result = false;
	CString port_num;

    CString port_str = tset->pSpiObj->spi_port->port_name();  //"USB SPI (0)";	
	
	int nBegin = port_str.FindOneOf("(");
	int nEnd = port_str.FindOneOf(")");
	port_num = port_str.Mid((nBegin + 1), (nEnd-nBegin-1));

	if(!flash_extern(1000, flash_file, port_num))
	{
	   param = "extern burn failed";
	   return(ERR_STOP);
	}
    else
	{
		result = true;
		param = "extern burn ok";
		return(result);
	}	
}
#endif

//-------------------------------------------------------------------------------------------------
const CString NULL_UNIT = "--";
BOOL DecideReadMac(char *buf)
{
	char i;
	char tempbuf[5];
	memcpy(tempbuf,buf,5);

	for(i=0;i<4;i++)
	{
		if(!((tempbuf[i]>='0')&&(tempbuf[i]<='9'))&&!((tempbuf[i]>='a') && (tempbuf[i]<='f'))&&!((tempbuf[i]>='A') && (tempbuf[i]<='F'))) 
			return(false);
	}
	return(true);
}
CTObj_WriteMACAddr::CTObj_WriteMACAddr(const char* pname,testSet *Tset,int ID, 
							 ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
}

void CTObj_WriteMACAddr::gen_prompt(CSortListCtrl &promp)
{
	char itemstr[8];
	itoa(index+1,itemstr,10);
	
	promp.AddItem(itemstr,name,"char number = 12",NULL_UNIT,"","");
}
int CTObj_WriteMACAddr::TestOp(ConfigFile &cfg)
{
	CString strMACAddrTemp, strMACAddr1, strMACAddr2, strMACAddr3, strMACAddr4, strMACAddr5, strMACAddr6;
	CString strE2pAddr, Cmd, cmd;
	CString wanMac4,wanMac5,wanMac6;
	int iE2pAddr1, iE2pAddr2, iE2pAddr3;
	CString cmd_str;
	result = false;
	
	pMainWnd->cur_mac_data = "";
	pMainWnd->input_mac_lock = 1;
	iE2pAddr1 = cfg.Vint(BsParWriteMAC1StartE2pAddr);
	iE2pAddr2 = cfg.Vint(BsParWriteMAC2StartE2pAddr);
	iE2pAddr3 = cfg.Vint(BsParWriteMAC3StartE2pAddr);
	
	pMainWnd->SendMessage(WM_INPUT_MACAddr,(WPARAM)NULL,(LPARAM)NULL);
	pMainWnd->input_mac_lock;
	while(pMainWnd->input_mac_lock);

	if((pMainWnd->cur_mac_data.Find(_T(':')) == -1)&&(pMainWnd->cur_mac_data.Find(_T('-')) == -1))
	{
		if(pMainWnd->cur_mac_data.GetLength() == 12)
		{
			param = pMainWnd->cur_mac_data;
			strMACAddrTemp = pMainWnd->cur_mac_data;
			strMACAddr1 = strMACAddrTemp.Left(2);
			strMACAddrTemp = strMACAddrTemp.Right(10);
			strMACAddr2 = strMACAddrTemp.Left(2);
			strMACAddrTemp = strMACAddrTemp.Right(8);
			strMACAddr3 = strMACAddrTemp.Left(2);
			strMACAddrTemp = strMACAddrTemp.Right(6);
			strMACAddr4 = strMACAddrTemp.Left(2);
			strMACAddrTemp = strMACAddrTemp.Right(4);
			strMACAddr5 = strMACAddrTemp.Left(2);
			strMACAddr6 = strMACAddrTemp.Right(2);

		}
		else
		{
			param = "MAC格式错误";
			return(false);
		}
	}
	else
	{
		if(pMainWnd->cur_mac_data.GetLength() == 17)
		{
			param = pMainWnd->cur_mac_data;
			strMACAddrTemp = pMainWnd->cur_mac_data;
			strMACAddr1 = strMACAddrTemp.Left(2);
			strMACAddrTemp = strMACAddrTemp.Right(14);
			strMACAddr2 = strMACAddrTemp.Left(2);
			strMACAddrTemp = strMACAddrTemp.Right(11);
			strMACAddr3 = strMACAddrTemp.Left(2);
			strMACAddrTemp = strMACAddrTemp.Right(8);
			strMACAddr4 = strMACAddrTemp.Left(2);
			strMACAddrTemp = strMACAddrTemp.Right(5);
			strMACAddr5 = strMACAddrTemp.Left(2);
			strMACAddr6 = strMACAddrTemp.Right(2);
		}
		else
		{
			param = "MAC格式错误";
			return(false);
		}
	}

	//iwpriv config_set wmap,11a,00e04c871234	//11a:(hex)00e04c871234:(hex)
	cmd_str.Format("iwpriv config_set wmap,11a,\r\n",param);
	//Sleep(1000);	
	if(tset->dev_id == TEST_DEV_ID_B)
	{
		if(!pRTL8711_2_Com)
		{
		   	param = "null RTL87xx2 COM";
		   	return (false);
		}
		pRTL8711_2_Com->send(cmd_str, 0);
	}
	else
	{
		if(!pRTL8711_1_Com)
		{
		   	param = "null RTL87xx1 COM";
		   	return (false);
		}
		pRTL8711_1_Com->send(cmd_str, 0);
	}
	
	result = true;
	return(result);
}
int CTObj_WriteMACAddr::WriteMacResult(CBsDevice *Obj,int addr,CString mac_a,CString mac_b,CString mac_c,CString mac_d,CString mac_e,CString mac_f)
{
	int i,j;	
	char value1[5] = {0};
	char value2[5] = {0};
	char value3[5] = {0};
	CString readmac;
	CString oldmac;
	
	oldmac=mac_b+mac_a+mac_d+mac_c+mac_f+mac_e;
	
	for(j=0;j<5;j++)
	{
		Sleep(500);
		//if(!Obj->ReadE2p(addr, value1))
		//	continue;
		if(!DecideReadMac(value1))
			continue;
		//if(!Obj->ReadE2p(addr+2, value2))
		//	continue;
		if(!DecideReadMac(value1))
			continue;
		//if(!Obj->ReadE2p(addr+4, value3))
		//	continue;
		if(!DecideReadMac(value1))
			continue;
		break;
	}

	for(i=0;i<4;i++)
	{
		readmac+=value1[i];
	}
	for(i=0;i<4;i++)
	{
		readmac+=value2[i];
	}
	for(i=0;i<4;i++)
	{
		readmac+=value3[i];
	}
	
	if(stricmp(readmac.GetBuffer(readmac.GetLength()),oldmac.GetBuffer(oldmac.GetLength()))==0)
		return 1;
	else
		return 0;
}


//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
CTObj_AutoWriteMACAddr::CTObj_AutoWriteMACAddr(const char* pname,testSet *Tset,int ID, 
							 ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
	testnumber = cfg.Vint(BsWriteMacaddrNumber);
	macaddr = cfg.VCString(BsMacaddr);
	iE2pAddr = cfg.Vint(BsParWriteMAC1StartE2pAddr);
	macaddnum = cfg.Vint(BsMacaddnum);
}

void CTObj_AutoWriteMACAddr::gen_prompt(CSortListCtrl &promp)
{
	char itemstr[8];
	itoa(index+1,itemstr,10);
	
	promp.AddItem(itemstr,name,"char number = 12",NULL_UNIT,"","");
}
//yhl_mac 2018/10/26
int CTObj_AutoWriteMACAddr::TestOp(ConfigFile &cfg)
{
	int flag;	
	int i = 0;
	CString readMac;
	CString temp2;
	char value1[5] = {0};
	char value2[5] = {0};
	char value3[5] = {0};
	unsigned long macnum = 0;		
	unsigned long readnum = 0;			//E2P值
	static int num=-1;
	static CString statmacaddr;
	CString	cmd_str;
	static unsigned long cfgIntMacMin=0,tempnum = 0;//最小值 //当前值
//-------------------------------------------------------------------------------------
       #if 0//yhl_mac 2018/10/26
	parse_mac_txt();
	esp_send_mac();
	cmd_str.Format(&mac_send[0]);
	if(!com_A_B->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
	{
	    	param = "rtl87xx get err";
		return (false);
	}
	//rx_buf="ss";
       #endif
//-------------------------------------------------------------------------------------
	if (num==-1)              //如果是第一次则将总测试数量赋给num
	{
		num = testnumber;
		if(macaddr.GetLength()!=12 && macaddr.GetLength()!=17)
		{
			param = "错误的MAC地址";	
			result = false;
			return(false);
		}
		statmacaddr=shortMAC(macaddr);
		//获取MAC后6位整形最小值
		cfgIntMacMin = a16ToUint((macaddr.Right(6)).GetBufferSetLength((macaddr.Right(6)).GetLength()));
		//获取当前将写MAC的后6位整形值
		tempnum = cfgIntMacMin;//a16ToUint((statmacaddr.Right(6)).GetBufferSetLength((statmacaddr.Right(6)).GetLength()));
	}
	
	//读取设备E2P的MAC地址
/*	for(i=0;i<5;i++)
	{
		Sleep(500);
		if(!Obj->ReadE2p(iE2pAddr, value1))
			continue;
		if(!DecideReadMac(value1))
			continue;
		if(!Obj->ReadE2p(iE2pAddr+2, value2))
			continue;
		if(!DecideReadMac(value1))
			continue;
		if(!Obj->ReadE2p(iE2pAddr+4, value3))
			continue;
		if(!DecideReadMac(value1))
			continue;
		break;
	}

	for (i=2;i<4;i++)
	{
		readMac += value1[i];
	}
	for (i=0;i<2;i++)
	{
		readMac += value1[i];
	}
	for (i=2;i<4;i++)
	{
		readMac += value2[i];
	}
	for (i=0;i<2;i++)
	{
		readMac += value2[i];
	}
	for (i=2;i<4;i++)
	{
		readMac += value3[i];
	}
	for (i=0;i<2;i++)
	{
		readMac += value3[i];
	}
	Sleep(1000);
	//查看E2P的MAC是否在最小值和当前值之间
	if(0==statmacaddr.Left(6).CompareNoCase(readMac.Left(6)))
	{
		readnum = a16ToUint((readMac.Right(6)).GetBufferSetLength((readMac.Right(6)).GetLength()));
		if (readnum >= cfgIntMacMin && readnum <= tempnum)
		{
			if(tempnum == readnum)
			{
				macnum = tempnum+macaddnum;
				temp2.Empty();
				temp2.Format("%06x",macnum);
				cfg.ItemSet(BsMacaddr,(macaddr.Left(6) + temp2));
			}	
			param = readMac;
			result = true;
			return(true);
		}
	}
	*/
	//开始写mac地址
	if(num)
	{
		flag = MacTestOp(statmacaddr);
		if(flag==false)
		{
			param="写入失败";	
			result=false;
			return(false);
		}
		else
		{
			num--;
			tempnum +=macaddnum;
			temp2.Empty();
			temp2.Format("%06x",tempnum);
			statmacaddr = macaddr.Left(6) + temp2;
			cfg.ItemSet(BsMacaddr,statmacaddr);
			return(true);
		}
	}
	else
	{
		param = "beyond limit";
		AfxMessageBox("已超过mac地设定范围，请重新设定!");
		return 0;
	}
			
}

int CTObj_AutoWriteMACAddr::MacTestOp(CString mac,ConfigFile &cfg)
{
	CString strMACAddrTemp, strMACAddr1, strMACAddr2, strMACAddr3, strMACAddr4, strMACAddr5, strMACAddr6;
	CString strE2pAddr, Cmd, cmd = "iwpriv ra0 e2p ";
	CString wanMac4 , wanMac5 , wanMac6,cmd_str;
	int iE2pAddr1, iE2pAddr2, iE2pAddr3 ,temp;
	result = false;
	
	pMainWnd->cur_mac_data = mac;

	iE2pAddr1 = cfg.Vint(BsParWriteMAC1StartE2pAddr);
	iE2pAddr2 = cfg.Vint(BsParWriteMAC2StartE2pAddr);
	iE2pAddr3 = cfg.Vint(BsParWriteMAC3StartE2pAddr);

	if((pMainWnd->cur_mac_data.Find(_T(':')) == -1)&&(pMainWnd->cur_mac_data.Find(_T('-')) == -1))
	{
		if(pMainWnd->cur_mac_data.GetLength() == 12)
		{
			param = pMainWnd->cur_mac_data;
			strMACAddrTemp = pMainWnd->cur_mac_data;
			strMACAddr1 = strMACAddrTemp.Left(2);
			strMACAddrTemp = strMACAddrTemp.Right(10);
			strMACAddr2 = strMACAddrTemp.Left(2);
			strMACAddrTemp = strMACAddrTemp.Right(8);
			strMACAddr3 = strMACAddrTemp.Left(2);
			strMACAddrTemp = strMACAddrTemp.Right(6);
			strMACAddr4 = strMACAddrTemp.Left(2);
			strMACAddrTemp = strMACAddrTemp.Right(4);
			strMACAddr5 = strMACAddrTemp.Left(2);
			strMACAddr6 = strMACAddrTemp.Right(2);
			temp = strtol(strMACAddr6,NULL,16);
		}
		else
		{
			param = "MAC格式不正确";
			return(false);
		}
	}
	else
	{
		if(pMainWnd->cur_mac_data.GetLength() == 17)
		{
			param = pMainWnd->cur_mac_data;
			strMACAddrTemp = pMainWnd->cur_mac_data;
			strMACAddr1 = strMACAddrTemp.Left(2);
			strMACAddrTemp = strMACAddrTemp.Right(14);
			strMACAddr2 = strMACAddrTemp.Left(2);
			strMACAddrTemp = strMACAddrTemp.Right(11);
			strMACAddr3 = strMACAddrTemp.Left(2);
			strMACAddrTemp = strMACAddrTemp.Right(8);
			strMACAddr4 = strMACAddrTemp.Left(2);
			strMACAddrTemp = strMACAddrTemp.Right(5);
			strMACAddr5 = strMACAddrTemp.Left(2);
			strMACAddr6 = strMACAddrTemp.Right(2);
		}
		else
		{
			param = "MAC格式不正确";
			return(false);
		}
	}
	//iwpriv config_set wmap,11a,00e04c871234	//11a:(hex)00e04c871234:(hex)
	cmd_str.Format("iwpriv config_set wmap,11a,\r\n",param);
	//Sleep(1000);
	if(tset->dev_id == TEST_DEV_ID_B)
	{
		if(!pRTL8711_2_Com)
		{
		   	param = "null RTL87xx2 COM";
		   	return (false);
		}
		
		pRTL8711_2_Com->send(cmd_str, 0);
	}
	else
	{
		if(!pRTL8711_1_Com)
		{
		   	param = "null RTL87xx1 COM";
		   	return (false);
		}
		pRTL8711_1_Com->send(cmd_str, 0);
	}
	
	result = true;
	return(result);
}

int CTObj_AutoWriteMACAddr::WriteMacResult(CBsDevice *Obj,int addr,CString mac_a,CString mac_b,CString mac_c,CString mac_d,CString mac_e,CString mac_f)
{
	int i,j;	
	char value1[5] = {0};
	char value2[5] = {0};
	char value3[5] = {0};
	CString readmac;
	CString oldmac;

	oldmac=mac_b+mac_a+mac_d+mac_c+mac_f+mac_e;
	
	for(j=0;j<5;j++)
	{
		Sleep(500);
		//if(!Obj->ReadE2p(addr, value1))
		//	continue;
		if(!DecideReadMac(value1))
			continue;
		//if(!Obj->ReadE2p(addr+2, value2))
		//	continue;
		if(!DecideReadMac(value1))
			continue;
		//if(!Obj->ReadE2p(addr+4, value3))
		//	continue;
		if(!DecideReadMac(value1))
			continue;
		break;
	}
	
	for(i=0;i<4;i++)
	{
		readmac+=value1[i];
	}
	for(i=0;i<4;i++)
	{
		readmac+=value2[i];
	}
	for(i=0;i<4;i++)
	{
		readmac+=value3[i];
	}

	if(stricmp(readmac.GetBuffer(readmac.GetLength()),oldmac.GetBuffer(oldmac.GetLength()))==0)
		return 1;
	else
		return 0;
}
/********************************
去掉mac地址分号
********************************/
CString CTObj_AutoWriteMACAddr::shortMAC(CString &str)
{
	CString temp1;
	CString temp2;
	CString temp3;

	temp1 =str;
	if(temp1.GetLength()==12)
		return temp1;
	else
		temp2 = temp1.Left(2);
		temp3 = temp1.Right(14);
		temp2 += temp3.Left(2);
		temp3 = temp1.Right(11);
		temp2 += temp3.Left(2);
		temp3 = temp1.Right(8);
		temp2 += temp3.Left(2);
		temp3 = temp1.Right(5);
		temp2 += temp3.Left(2);
		temp2 += temp3.Right(2);
		return temp2;
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

int hexCharToValue(char ch)
{
	int result = -1;
	if(ch >= '0' && ch <= '9')
	{
		result = (int)(ch - '0');
	}
	else if(ch >= 'a' && ch <= 'f')
	{
		result = (int)(ch - 'a') + 10;
	}
	else if(ch >= 'A' && ch <= 'F')
	{
		result = (int)(ch - 'A') + 10;
	}

	return result;
}

/********************************************** ***/
/*字符串转换为十六进制，每两个字符转换为一个字节*/
/*eg:"2B"===>0x2b*/
/*************************************************/
int hexStr2Hex( char chH, char chL)
{
	int tempL,tempH;
	int result = -1;
	tempL = hexCharToValue(chL);
	tempH = hexCharToValue(chH);
	if((tempL != -1)&&(tempH != -1))
	{
		result = tempH*16+tempL;		
	}
	return result;
}
//yhl_mac 2018/10/26
bool read_rtl8711_golden_flag( RTL87XX_COM* com_A_B, uint8 &golden_current_Index, CString &param)
{
	CString cmd_str;
	CString answer_str;
	char rx_buf[512] = {0};
	int ret;
	bool result = false;
	int temp = 0;
	//0xC7
	//iwpriv config_get rmap,c7,1		//c7:(hex)1:(dec)
	//iwpriv config_set wmap,c7,08		//c7:(hex)08:(hex)
	//Sleep(500);	
	#if 1//yhl_mac 2018/10/26
	cmd_str.Format("+R\r\n");
	if(!com_A_B->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))
	{

	}
	else
	{
		cmd_str.Format("wr mac successfuly\r\n");
		com_A_B->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256);
	}
	return result;
	#else

	

	cmd_str.Format("iwpriv config_get rmap,c7,1\r\n");	
	memset(rx_buf, 0, 256*sizeof(char));
	if(!com_A_B->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
	{
	    	param = "rtl87xx get err";
		return (false);
	}	
	//if(rx_buf)//"Private Message: 0xFF"	
	{
		answer_str = rx_buf;//2018/10/22
		ret = answer_str.Find("Private Message: 0x");
		temp = hexStr2Hex(rx_buf[ret+19],rx_buf[ret+20]);
		if(temp == 0x55) result = true;
	}

	//iwpriv config_set wmap,c9,20		//c9:(hex)20:(hex)
	//iwpriv config_get rmap,c9,1		//c9:(hex)1:(dec)
	if(result)
	{	
		//Sleep(100);
		#if 0//yhl_read 2018/10/26
		cmd_str.Format("+R\r\n");//mac
		#endif
		cmd_str.Format("iwpriv config_get rmap,c9,1\r\n");	
		memset(rx_buf, 0, 256*sizeof(char));
		if(!com_A_B->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
		{
		    	param = "rtl87xx get err";
			return (false);
		}
		//if(rx_buf)//"Private Message: 0xFF"		
		{
			answer_str.Empty();
			answer_str = rx_buf;
			ret = answer_str.Find("Private Message: 0x");// 19
			temp = hexStr2Hex(rx_buf[ret+19],rx_buf[ret+20]);
			if((temp!=-1)&&(temp<0x40))
			{	
				golden_current_Index = (uint8)temp;
				result = true;
			}
			else result = false;
		}
	}
	return result;
	#endif
}

//-------------------------------------------------------------------------------------------------
CTObj_DisplayMAC::CTObj_DisplayMAC(const char* pname,testSet *Tset, int ID, 
								   ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
	BrAddr = cfg.Vint(BsParWriteMAC1StartE2pAddr);
}

void CTObj_DisplayMAC::gen_prompt()
//void CTObj_DisplayMAC::gen_prompt()
{
       #if 1
	char itemstr[8];
	CString range;
	itoa(index+1,itemstr,10);
	//promp.AddItem(itemstr,name,"","MAC is","","");	  
	range.Format("MAC is......");
       tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
       #else
	   	char itemstr[8];
	itoa(Index(),itemstr,10);
	
	CString range;
	if(tset->dev_id == TEST_DEV_ID_A)
		range.Format("dut A into normal mode");
	else
		range.Format("dut B into normal mode");
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
     #endif
}
int CTObj_DisplayMAC::TestOp(ConfigFile &cfg)
{
	CString cmd_str;
	CString answer_str;
	CString temp_mac,search_mac;
	char rx_buf[512] = {0};
	int ret,i;	
	int temp = 0;
	uint8 current_Index = 32;
	char read_mac[]="read mac\r\n";
	result = false;
	#if 0
	if(tset->dev_id == TEST_DEV_ID_B)
	{	
		temp_mac = pMainWnd->m_bdev_address;			
	}
	else
	{	
		temp_mac = pMainWnd->m_adev_address;					
	}
	for(i=0; i<3; i++)
	{
		memset(rx_buf,0,512);
		if(tset->dev_id == TEST_DEV_ID_B)
		{
			if(!pRTL8711_2_Com)
			{
			   	param = "null RTL87xx2 COM";
			   	return (false);
			}		
			if(!pRTL8711_2_Com->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
			{
			    	param = "rtl87xx2 get err";
				return (false);
			}	
		}
		else
		{
			if(!pRTL8711_1_Com)
			{
			   	param = "null RTL87xx1 COM";
			   	return (false);
			}
			if(!pRTL8711_1_Com->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
			{
			    	param = "rtl87xx1 get err";
				return (false);
			}
		}

			answer_str.Empty();
			answer_str = rx_buf;//2018/10/22
			ret = answer_str.Find("Private Message: 0x");
			if(ret)
			{
				param.Empty();
				param = answer_str.Mid(ret+19,27);
				param.Replace(" 0x","");
				if(tset->is_golden)
					param+=" is golden";
			
			

				if(!cfg.ItemSelect(slaveWAddressSel))	//has mac input
				{ 
					AfxMessageBox("请打开配置里面输入mac\n");
					param.Empty();
					param = "not get mac";
					return (false);
				}

				if(param == temp_mac)
				{
					param.Empty();
					param = "get mac " + temp_mac;
					break;
				}
			}

			if(i>=2)
			{
				search_mac = "get mac is " + param;
				param.Empty();
				param = search_mac;
				return (false);
			}
		
	}
#else
	if(tset->dev_id == TEST_DEV_ID_B)
	{            
			if(pRTL8711_2_Com->send_and_get(read_mac, 0, rx_buf, 0, 2000, 256))
			{ 
			       if(strlen(rx_buf)!=12)
			       	{
					param = "read mac command fail";
						result=false;		
					return(false);
			       	}
				else
				{
					strlwr(rx_buf);//大写字符变小写				
			       		param =&rx_buf[0];
				}
			}
			else
			{
				param = "read mac command fail";
					result=false;		
					return(false);
			}
			
		}
	else
		{
			if(pRTL8711_1_Com->send_and_get(read_mac, 0, rx_buf, 0, 2000, 256))
			{
			       if(strlen(rx_buf)!=12)
			       	{
					param = "read mac command fail";
						result=false;		
					return(false);
			       	}
				else
				{
					strlwr(rx_buf);//大写字符变小写				
			       		param =&rx_buf[0];
				}
			}
			else
			{
				param = "read mac command fail";
					result=false;		
					return(false);
			}	
			
		}

#endif
	result=true;		
	return(true);
}
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
CTObj_ShowSoftVer::CTObj_ShowSoftVer(const char* pname,testSet *Tset, int ID, 
								   ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
	//time = 1000;
	soft_ver = cfg.VCString(Par_set_soft_ver);
}

void CTObj_ShowSoftVer::gen_prompt()
{
	#if 0
	char itemstr[8];	
	itoa(index+1,itemstr,10);	
	promp.AddItem(itemstr,name,"","ESP8266_soft_ver is","","");
	#else
  	char itemstr[8];
	CString range;
	itoa(index+1,itemstr,10);
	range.Format("ESP8266_soft_ver is");
       tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
	#endif
}

int CTObj_ShowSoftVer::TestOp(ConfigFile &cfg)
{
	CString cmd_str;	
	CString answer_str;
	char rx_buf[512] = {0};
	int ret,i,j;
	result = false;
	CString get_soft_ver;
	CString answer_find;
	
	if(tset->dev_id == TEST_DEV_ID_B)
	{
		if(!pRTL8711_2_Com)
		{
		   	param = "null esp82xx COM";
		   	return (false);
		}

		//Sleep(2000);
		cmd_str.Format("print_ver\r");
		for(i=0;i<5;i++)
		{
			Sleep(100);
			memset(rx_buf,0,512);
			answer_str = "";
			answer_find = "";
			if(!pRTL8711_2_Com->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
			{
				if(i>=4)
				{
			    		param = "esp82xx get ver err";
					return (false);
				}
				else
					continue;
			}
			param=&rx_buf[0];
			#if 0
			answer_str = rx_buf;
			ret = answer_str.Find("[ATVN]=");
			if(ret < 0)
			{
				if(i>=4)
				{
			    		param = "esp82xx get ver err";
					return (false);
				}
				else
					continue;
				
			}
			answer_find = answer_str.Mid(ret+7,10);
			ret = answer_find.Find('[');
		#if 0
			soft_ver = param.Left(ret); 
			param.Empty();
			param.Format("rtl87xx1_ver=");
			param += soft_ver;
		#else
			get_soft_ver = answer_find.Left(ret);
			//param.Empty();
			//param.Format("rtl87xx1_ver=");
			if(soft_ver == "")
			{
				param.Empty();
				param.Format("please set correct ver");
				AfxMessageBox("set sotf ver is null");
				return (false);
			}
			if(get_soft_ver == soft_ver)
			{
				param.Empty();
				param.Format("get correct ver=");
				param += get_soft_ver;
				break;		//get correct so break
			}
			else
			{
			#if 0
				param.Empty();
				param.Format("get  ver=");
				param += get_soft_ver;
				param += " set ver=";
				param += soft_ver;
				return (false);
			#else
				if(i >= 4)
				{
					param.Empty();
					param.Format("get  ver=");
					param += get_soft_ver;
					param += " set ver=";
					param += soft_ver;
					return (false);
				}
				else
					continue;
			#endif
			}
		#endif
		#endif
		}
	}
	else
	{
		if(!pRTL8711_1_Com)
		{
		   	param = "null esp82xx COM";
		   	return (false);
		}
		//Sleep(2000);
		cmd_str.Format("print_ver\r");
		for(i=0;i<5;i++)
		{
			Sleep(100);
			memset(rx_buf,0,512);
			answer_str = "";
			answer_find = "";
			if(!pRTL8711_1_Com->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
			{
				if(i>=4)
				{
			    		param = "esp82xx get ver err";
					return (false);
				}
				else
					continue;
			}
			param=&rx_buf[0];
			#if 0
			answer_str = rx_buf;//2018/10/22
			ret = answer_str.Find("[ATVN]=");
			if(ret < 0)
			{
				if(i>=4)
				{
			    		param = "esp82xx get ver err";
					return (false);
				}
				else
					continue;
				
			}
			answer_find = answer_str.Mid(ret+7,10);
			ret = answer_find.Find('[');
		#if 0
			soft_ver = param.Left(ret); 
			param.Empty();
			param.Format("rtl87xx1_ver=");
			param += soft_ver;
		#else
			get_soft_ver = answer_find.Left(ret);
			//param.Empty();
			//param.Format("rtl87xx1_ver=");
			if(soft_ver == "")
			{
				param.Empty();
				param.Format("please set correct ver");
				AfxMessageBox("set sotf ver is null");
				return (false);
			}
			if(get_soft_ver == soft_ver)
			{
				param.Empty();
				param.Format("get correct ver=");
				param += get_soft_ver;
			}
			else
			{
			#if 0
				param.Empty();
				param.Format("get  ver=");
				param += get_soft_ver;
				param += " set ver=";
				param += soft_ver;
				return (false);
			#else
				if(i >= 4)
				{
					param.Empty();
					param.Format("get  ver=");
					param += get_soft_ver;
					param += " set ver=";
					param += soft_ver;
					return (false);
				}
				else
					continue;
			#endif
			}
		#endif
		#endif
			break;
		}
		
	}

	result = true;
	return(true);
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_io_port::CTObj_io_port(const char* pname, testSet *Tset,int ID, int pin_num,
			     ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
	time = 100;//cfg.Vulong(s_pio_wait_time);
	pio_name = pin_num;    //pio序号，如PIO1,PIO2
}


int CTObj_io_port::ItemTestReady(ConfigFile &cfg)
{
    	mutex_hodle1 = false;
	mutex_hodle2 = false;
	if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pMainWnd->glbResource_get(GRESOURCE_MCU2))
		{
			param = "mcu2 gsource err";
			return(ERR_TERMINAL);
		}
		mutex_hodle2 = true;
		return(NO_ERR);
	}
	else
	{
		if(!pMainWnd->glbResource_get(GRESOURCE_MCU1))
		{
			param = "mcu1 gsource err";
			return(ERR_TERMINAL);
		}
		mutex_hodle1 = true;
		return(NO_ERR);
	}

}


int CTObj_io_port::ItemTestClean(ConfigFile &cfg)
{
	   if(tset->dev_id == TEST_DEV_ID_A)
	   {
		   if (mutex_hodle2)
		   {
		      	pMainWnd->glbResource_put(GRESOURCE_MCU2);
              	mutex_hodle2 = false;
		   }
	   }
	   else
	   {
	       if (mutex_hodle1)
		   {
		      	pMainWnd->glbResource_put(GRESOURCE_MCU1);
	          	mutex_hodle1 = false;
		   }
	   }
    return(NO_ERR);
}


void CTObj_io_port::gen_prompt()
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	CString range;
	if(tset->dev_id == TEST_DEV_ID_A)
	{ 
		if(pio_name==0)//R,G,B,CW,WW		//pio_name=9
			range.Format("A MCU IO test");		 
		else//C0,C2,C3		 		 
			range.Format("A 8266 IO test");
//	range.Format("A P%c%d input", gpio[pio_name], (igpio[pio_name]));
	}
	else
	{  
		if(pio_name==0)//R,G,B,CW,WW		//pio_name=9
			range.Format("B MCU IO test");			 
		else//C0,C2,C3 			 
			range.Format("B 8266 IO test");
//	range.Format("B P%c%d input", gpio[pio_name], (igpio[pio_name]));
	}
	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}

#if 0
int CTObj_io_port::TestOp(ConfigFile &cfg)
{
	result =false;
		
	if(!set_pio(pio_name,1)) return(false);
	if(!is_pio_lv(pio_name,1)) return(false);
	
	if(!set_pio(pio_name,0)) return(false);
	if(!is_pio_lv(pio_name,0)) return(false);
	
	param = "";
	param.Format("[H:%d L:%d] ok",high_data, low_data);
	//param.Format("%s input [H:%d L:%d] ok",out_pio_name[pio_name],high_data, low_data);
	result = true;
	return(true);	
}


bool CTObj_io_port::set_pio(int pio_name, bool lv)
{
	int i=0,count=0;
	
	if(!pMCU1_Com || !pMCU2_Com)
	{
	   param = "null MCU COM";
	   return (false);
	}
	
	if(pio_name==0)//R,G,B,CW,WW
		 count=5;
	else
		 count=3;//C0,C2,C3

	//这里控制所有IO口在MCU端为输入端，RTL87XX端为输出端

	//mcu write to pio
	CString cmd_str;	
	if(tset->dev_id == TEST_DEV_ID_B)
	{
		if(!pRTL8711_2_Com)
		{
		   param = "null RTL87xx COM";
		   return (false);
		}	
	   
	   //Sleep(100);


		
		for(i=0;i<count;i++)
		{
			//if(count==5)
				cmd_str.Format("MCU+SET(0,%c,%04x,3)\r\n", gpio[pio_name+i], (1 << igpio[pio_name+i]));//arg2=A~G,arg3=0001~FFFF
			//else
			//	cmd_str.Format("MCU+SET(0,%c,%04x,3)\r\n", gpio[pio_name-i], (1 << igpio[pio_name-i]));//arg2=A~G,arg3=0001~FFFF
			if(!pMCU1_Com->send_and_verify(cmd_str, 0, "MCU+OK\r\n", 0))
			{
			    param.Format("mcu1 set PIO%d err",(pio_name+i));
				return (false);
			}
		}
/*	   
	   //Sleep(100);
	   //cmd_str.Format("ATIO=set[C0,1]\r\n");
	   cmd_str.Format("ATIO=set[%c%c,1]\r\n",rtl87xx_gpio1[pio_name],rtl87xx_gpio2[pio_name]);
	   //if(!pRTL8711_2_Com->send_and_verify(cmd_str, 0, "[ATIO]=OK", 0))//不可用
	   if(!pRTL8711_2_Com->send_and_search(cmd_str, 0, "[ATIO]=OK", 0))
	   {
	        param = "rtl87xx2 set err";
			return (false);
	   }
	   //Sleep(100);
*/	   
	   if(lv)
			cmd_str.Format("ATIO=out[%c%c,1]\r\n",rtl87xx_gpio1[pio_name],rtl87xx_gpio2[pio_name]);
	   else 
			cmd_str.Format("ATIO=out[%c%c,0]\r\n",rtl87xx_gpio1[pio_name],rtl87xx_gpio2[pio_name]);
	   if(!pRTL8711_2_Com->send_and_search(cmd_str, 0, "[ATIO]=OK", 0))
	   {
	        param = "rtl87xx2 out err";
			return (false);
	   }
	   //Sleep(100);
	}
	else
	{
		if(!pRTL8711_1_Com)
		{
		   param = "null RTL87xx1 COM";
		   return (false);
		}
		
		//Sleep(100);
		for(i=0;i<count;i++)
		{
			cmd_str.Format("MCU+SET(1,%c,%04x,3)\r\n", gpio[pio_name+i], (1 << igpio[pio_name+i]));//arg2=A~G,arg3=0001~FFFF
			if(!pMCU2_Com->send_and_verify(cmd_str, 0, "MCU+OK\r\n", 0))
			{
			    param.Format("mcu2 set PIO%d err",(pio_name+i));
				return (false);
			}
		}
/*
		//Sleep(100);
		//cmd_str.Format("ATIO=set[RW,1]\r\n");
	   	cmd_str.Format("ATIO=set[%c%c,1]\r\n",rtl87xx_gpio1[pio_name],rtl87xx_gpio2[pio_name]);
		//if(!pRTL8711_1_Com->send_and_verify(cmd_str, 0, "[ATIO]=OK", 0))//不可用
		if(!pRTL8711_1_Com->send_and_search(cmd_str, 0, "[ATIO]=OK", 0))
		{
		    param = "rtl87xx1 set err";
			return (false);
		}
		//Sleep(100);
*/		
	   	if(lv)
			cmd_str.Format("ATIO=out[%c%c,1]\r\n",rtl87xx_gpio1[pio_name],rtl87xx_gpio2[pio_name]);
	   	else 
			cmd_str.Format("ATIO=out[%c%c,0]\r\n",rtl87xx_gpio1[pio_name],rtl87xx_gpio2[pio_name]);
		if(!pRTL8711_1_Com->send_and_search(cmd_str, 0, "[ATIO]=OK", 0))
		{
		    param = "rtl87xx1 out err";
			return (false);
		}
		//Sleep(100);
	}
	return (true);
}


bool CTObj_io_port::is_pio_lv(int pio_name,bool lv)
{
	CString cmd_str;
	CString answer_str;
	char rx_buf[128] = {0};
	int i=0,count=0;
	
    	//if(time) Sleep(time);
	Sleep(200);
	//Sleep(200);	
	
	if(pio_name==0)//R,G,B,CW,WW
		 count=5;
	else
		 count=3;//C0,C2,C3

	for(i=0;i<count;i++)
	{
		if(lv)
			answer_str.Format("MCU+RD(%04x)\r\n", (1 << igpio[pio_name+i]));
		else
			answer_str.Format("MCU+RD(0000)\r\n");

		if(tset->dev_id == TEST_DEV_ID_B)
		{
			cmd_str.Format("MCU+RD(0,%c,%04x)\r\n", gpio[pio_name+i], (1 << igpio[pio_name+i]));//arg2=A~G,arg3=0001~FFFF
			//Sleep(100);
			if(!pMCU1_Com->send_and_verify(cmd_str, 0, answer_str, 0))
			//if(!pMCU2_Com->send_and_get(cmd_str, 0, rx_buf, 0))//可用
			{
					param.Format("mcu1 get PIO%d err",(pio_name+i));
				return (false);
			}
			//param.Empty();
			//param = rx_buf;
			//return (false);
		}
		else
		{
			cmd_str.Format("MCU+RD(1,%c,%04x)\r\n", gpio[pio_name+i], (1 << igpio[pio_name+i]));//arg2=A~G,arg3=0001~FFFF
			//Sleep(100);
			if(!pMCU2_Com->send_and_verify(cmd_str, 0, answer_str, 0))
			//if(!pMCU1_Com->send_and_get(cmd_str, 0, rx_buf, 0))//可用	
			{
					param.Format("mcu2 get PIO%d err",(pio_name+i));
				return (false);
			}
			//param.Empty();
			//param = rx_buf;
			//return (false);
		}
	}	
	if(lv)
		high_data = 1;
	else
		low_data = 0;
	return(true);
}
#else
int CTObj_io_port::TestOp(ConfigFile &cfg)
{
	result =false;
	#if 0
		if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pMCU1_Com->set_4010_route_dut1(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut1 err";
			return (false);
		}	
	}
	else
	{
		if(!pMCU1_Com->set_4010_route_dut2(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut2 err";
			return (false);
		}
	}
	#endif
#if 0		
	if(!set_pio(pio_name,1)) return(false);
	//if(!is_pio_lv(pio_name,1)) return(false);
	
	//if(!set_pio(pio_name,0)) return(false);		//only test set high 
	//if(!is_pio_lv(pio_name,0)) return(false);
#else
	//SET IDEA:if set 1 is get true ,test is pass,if set 1 is get false ,we will set 0.\
	//if set 0 is true,test is pass,else test io is fairing. 
	if(!set_pio(pio_name,1))
	{
		if(!set_pio(pio_name,0)) 
			return(false);
	}
#endif
	param = "";
#if 1
	if(high_data == 1)
		param.Format("[H:%d] ok",high_data);
	else if(low_data == 0)
		param.Format("[L:%d] ok",low_data);
#else		
	param.Format("[H:%d L:%d] ok",high_data, low_data);
#endif
	//param.Format("%s input [H:%d L:%d] ok",out_pio_name[pio_name],high_data, low_data);
	result = true;
	return(true);	
}

bool CTObj_io_port::set_pio(int pio_name, bool lv)
{
	int i=0,count=0,temp_val_mcu=0,temp_val_8711=0;
	char rx_buf[512] = {0};
	char ate_start[]="ate start\r\n";
	char esp8266_gpio1[] =  
	{
		'R','W','G','C','B','A',    //PIO0-5    	
	};
	if(!pMCU1_Com || !pMCU2_Com)
	{
	   param = "null MCU COM";
	   return (false);
	}

	if(pio_name == 0)
		count = 5;//yhl 2018/10/31
	//这里控制所有IO口在MCU端为输入端，RTL87XX端为输出端
	//mcu write to pio
	CString cmd_str;	
	if(tset->dev_id == TEST_DEV_ID_B)
	{
		if(!pRTL8711_2_Com)
		{
		   param = "null ESP8266 COM";
		   return (false);
		}	
	   

		for(i=0;i<count;i++){

				cmd_str.Format("MCU+SET(0,%c,%04x,1)\r\n", gpio[i], (1 << igpio[i+19]));//arg2=A~G,arg3=0001~FFFF
			if(!pMCU1_Com->send_and_verify(cmd_str, 0, "MCU+OK\r\n", 0))
			{
			    param.Format("mcu1 set PIO%d err ,temp=%04x",(pio_name+i),temp_val_mcu);
				return (false);
			}
			Sleep(20);
		}

		for(i=0;i<count;i++)
		{

			if(lv)
				cmd_str.Format("gpio check %c%d\r\n",esp8266_gpio1[i],1);
			else
				cmd_str.Format("gpio check %c%d\r\n",esp8266_gpio1[i],0);

			if(!pRTL8711_2_Com->send_and_search(cmd_str, 0, "ss", 0))
	   		{
	   			param.Format("esp8266 out err i =%d",(pio_name+i));
				return (false);
	   		}


			Sleep(100);
			if(!is_pio_lv(pio_name,lv,i))
				return (false);
		}

	}
	else
	{
		if(!pRTL8711_1_Com)
		{
		   param = "null ESP8266 COM";
		   return (false);
		}

		for(i=0;i<count;i++){
				cmd_str.Format("MCU+SET(1,%c,%04x,1)\r\n", gpio[i], (1 << igpio[i+19]));//arg2=A~G,arg3=0001~FFFF
			if(!pMCU2_Com->send_and_verify(cmd_str, 0, "MCU+OK\r\n", 0))
			{
			    param.Format("mcu1 set PIO%d err ,temp=%04x",(pio_name+i),temp_val_mcu);
				return (false);
			}
			Sleep(20);
		}

//start read io//yhl 2018/10/31
		for(i=0;i<count;i++)
		{

			if(lv)//yhl 2018/10/31
				cmd_str.Format("gpio check %c%d\r\n",esp8266_gpio1[i],1);
			else
				cmd_str.Format("gpio check %c%d\r\n",esp8266_gpio1[i],0);

						//yhl 2018/10/31	
			if(!pRTL8711_1_Com->send_and_search(cmd_str, 0, "ss", 0))
		   	{
		   			param.Format("esp8266 out err i =%d",(pio_name+i));
		        	
					return (false);
		   	}
			else
				{
					param = "esp8266 io ok";
				}
			Sleep(100);
			if(!is_pio_lv(pio_name,lv,i))
				return (false);
		}
	}
	return (true);
}

bool CTObj_io_port::is_pio_lv(int pio_name,bool lv,int i)
{
	CString cmd_str;
	CString cmd_str1;
	CString answer_str;
	CString answer_str1;
	uint8 gpio_number[]={4,15,12,13,14};
	int j,temp=0,count=0;
    Sleep(100);		//fu change to 100ms
#if 0	
	if(pio_name==0)//R,G,B,CW,WW
		 count=6;
	else
		 count=3;//C0,C2,C3
#else

#ifdef set_8711
	if(pio_name==0)
		count = 9;
#else
	if(pio_name==0)
		count = 5;
#endif
#endif
	//for(i=0;i<count;i++)
	//{
	//把这个地方移到for循环，就可以做成某一个为低其他为高的检测
#if 0
		if(lv){
			answer_str.Format("MCU+RD(%04x)\r\n", (1 << igpio[pio_name+i]));
			answer_str1.Format("MCU+RD(0000)\r\n");
		}
		else{
			//answer_str1.Format("MCU+RD(%04x)\r\n", (1 << igpio[pio_name+i]));
			answer_str.Format("MCU+RD(0000)\r\n");
			answer_str1.Format("MCU+RD(0000)\r\n");
		}
#endif		

		//for(j=0;j<4;j++)
			//temp |= (1 << igpio[i]);
		if(tset->dev_id == TEST_DEV_ID_B)
		{
			//cmd_str.Format("MCU+RD(0,%c,%04x)\r\n", gpio[0], (1 << igpio[pio_name+i])|(1 << igpio[pio_name+i+1])|(1 << igpio[pio_name+i+2]));//arg2=A~G,arg3=0001~FFFF
			//cmd_str.Format("MCU+RD(0,%c,%04x)\r\n", gpio[0], temp);
			//cmd_str1.Format("MCU+RD(0,%c,%04x)\r\n", gpio[6], gpio_val_87);//arg2=A~G,arg3=0001~FFFF
			//Sleep(100);
#if 0
			if(pio_name == 0){
	#if 0
				if((!pMCU1_Com->send_and_verify(cmd_str, 0, answer_str, 0))||(!pMCU1_Com->send_and_verify(cmd_str1, 0, answer_str1, 0)))
				//if(!pMCU2_Com->send_and_get(cmd_str, 0, rx_buf, 0))//可用
				{
						param.Format("mcu1 get PIO%d err",(pio_name+i));
					return (false);
				}
	#else
			for(j=0;j<count;j++){
				cmd_str.Format("MCU+RD(0,%c,%04x)\r\n", gpio[0], (1 << igpio[pio_name+j]));//arg2=A~G,arg3=0001~FFFF
				if(i == j){
					if(!pMCU1_Com->send_and_verify(cmd_str, 0, answer_str, 0)) {
						param.Format("mcu1 get PIO%d ^^io=%04x",(pio_name+i),(1 << igpio[pio_name+i])|(1 << igpio[pio_name+i+1])|(1 << igpio[pio_name+i+2])|(1 << igpio[pio_name+i+3]));
							return (false);
					}
				}else {
				//cmd_str.Format("MCU+RD(0,%c,%04x)\r\n", gpio[0], (1 << igpio[pio_name+i+4]));
					if(!pMCU1_Com->send_and_verify(cmd_str, 0, answer_str1, 0)) {
						param.Format("mcu1 get PIO%d @",(pio_name+i));
							return (false);
					}
				}
			}
#if 0			//主要是8711的引脚没有对应上，先屏蔽
			for(j=0;j<3;j++){
				cmd_str.Format("MCU+RD(0,%c,%04x)\r\n", gpio[6], (1 << igpio[6+j]));//arg2=A~G,arg3=0001~FFFF
				if(!pMCU1_Com->send_and_verify(cmd_str, 0, answer_str1, 0)) {
						param.Format("mcu1 get PIO%d j=%d^^",(pio_name+i),j);
							return (false);
				}
			}
#endif		
	#endif
			}else {
				if((!pMCU1_Com->send_and_verify(cmd_str, 0, answer_str1, 0))||(!pMCU1_Com->send_and_verify(cmd_str1, 0, answer_str, 0)))
				//if(!pMCU2_Com->send_and_get(cmd_str, 0, rx_buf, 0))//可用
				{
						param.Format("mcu1 get PIO%d err",(pio_name+i));
					return (false);
				}	

			}
#else
			for(j=0;j<count;j++){
				cmd_str.Format("MCU+RD(0,%c,%04x)\r\n", gpio[pio_name+4-j], (1 << igpio[pio_name+4-j+19]));//arg2=A~G,arg3=0001~FFFF
				if(lv){
					answer_str.Format("MCU+RD(%04x)\r\n", (1 << igpio[pio_name+4-j+19]));
					answer_str1.Format("MCU+RD(0000)\r\n");
				}
				else{
					answer_str1.Format("MCU+RD(%04x)\r\n", (1 << igpio[pio_name+4-j+19]));
					answer_str.Format("MCU+RD(0000)\r\n");
					//answer_str1.Format("MCU+RD(0000)\r\n");
				}
				if(i == j)
				{
					if(!pMCU1_Com->send_and_verify(cmd_str, 0, answer_str, 0)) {
						//param.Format("mcu1 get PIO%d j=%d ^^",(pio_name+i),(pio_name+j));
						param.Format("mcu2 get PIO=%d ^^",gpio_number[(pio_name+j)]);
							return (false);
					}
				}
				else 
				{
					if(!pMCU1_Com->send_and_verify(cmd_str, 0, answer_str1, 0)) {
						//param.Format("mcu1 get PIO%d j=%d @",(pio_name+i),(pio_name+j));
						param.Format("mcu2 get PIO=%d ^^",gpio_number[(pio_name+j)]);
							return (false);
					}
				}
				Sleep(20);			// fu add 
			}
#if 0
			if(pio_name == 0){
				for(j=0;j<3;j++){
					cmd_str.Format("MCU+RD(0,%c,%04x)\r\n", gpio[6], (1 << igpio[6+j]));//arg2=A~G,arg3=0001~FFFF
					if(lv)
						answer_str1.Format("MCU+RD(0000)\r\n");
					else
						answer_str1.Format("MCU+RD(%04x)\r\n", (1 << igpio[6+j]));
					if(!pMCU1_Com->send_and_verify(cmd_str, 0, answer_str1, 0)) {
							param.Format("mcu1 get PIO%d j=%d ^^",(pio_name+i),(j+6));
								return (false);
					}
				}
			}else {
				for(j=0;j<6;j++){
					cmd_str.Format("MCU+RD(0,%c,%04x)\r\n", gpio[0], (1 << igpio[j]));//arg2=A~G,arg3=0001~FFFF
					if(lv)
						answer_str1.Format("MCU+RD(0000)\r\n");
					else
						answer_str1.Format("MCU+RD(%04x)\r\n", (1 << igpio[j]));
					if(!pMCU1_Com->send_and_verify(cmd_str, 0, answer_str1, 0)) {
							param.Format("mcu1 get PIO%d j=%d ^^",(pio_name+i),j);
								return (false);
					}
				}
			}
#endif

#endif
			//param.Empty();
			//param = rx_buf;
			//return (false);
		}
		else
		{
			//cmd_str.Format("MCU+RD(1,%c,%04x)\r\n", gpio[0], gpio_val_mcu);//arg2=A~G,arg3=0001~FFFF
			//cmd_str1.Format("MCU+RD(1,%c,%04x)\r\n", gpio[6], (1 << gpio_val_87));//arg2=A~G,arg3=0001~FFFF
			//Sleep(100);
#if 0
						if(pio_name == 0){
	#if 0
							if((!pMCU1_Com->send_and_verify(cmd_str, 0, answer_str, 0))||(!pMCU1_Com->send_and_verify(cmd_str1, 0, answer_str1, 0)))
							//if(!pMCU2_Com->send_and_get(cmd_str, 0, rx_buf, 0))//可用
							{
									param.Format("mcu1 get PIO%d err",(pio_name+i));
								return (false);
							}
	#else
						for(j=0;j<count;j++){
							cmd_str.Format("MCU+RD(1,%c,%04x)\r\n", gpio[0], (1 << igpio[pio_name+j]));//arg2=A~G,arg3=0001~FFFF
							if(i == j){
								if(!pMCU2_Com->send_and_verify(cmd_str, 0, answer_str, 0)) {
									param.Format("mcu2 get PIO%d ^^io=%04x",(pio_name+i),(1 << igpio[pio_name+i])|(1 << igpio[pio_name+i+1])|(1 << igpio[pio_name+i+2])|(1 << igpio[pio_name+i+3]));
										return (false);
								}
							}else {
							//cmd_str.Format("MCU+RD(0,%c,%04x)\r\n", gpio[0], (1 << igpio[pio_name+i+4]));
								if(!pMCU2_Com->send_and_verify(cmd_str, 0, answer_str1, 0)) {
									param.Format("mcu2 get PIO%d @",(pio_name+i));
										return (false);
								}
							}
						}
#if 0			//主要是8711的引脚没有对应上，先屏蔽
						for(j=0;j<3;j++){
							cmd_str.Format("MCU+RD(0,%c,%04x)\r\n", gpio[6], (1 << igpio[6+j]));//arg2=A~G,arg3=0001~FFFF
							if(!pMCU1_Com->send_and_verify(cmd_str, 0, answer_str1, 0)) {
									param.Format("mcu1 get PIO%d j=%d^^",(pio_name+i),j);
										return (false);
							}
						}
#endif		
	#endif
						}else {
							if((!pMCU2_Com->send_and_verify(cmd_str, 0, answer_str1, 0))||(!pMCU2_Com->send_and_verify(cmd_str1, 0, answer_str, 0)))
							//if(!pMCU2_Com->send_and_get(cmd_str, 0, rx_buf, 0))//可用
							{
									param.Format("mcu1 get PIO%d err",(pio_name+i));
								return (false);
							}	
			
						}
#else
						for(j=0;j<count;j++){
							cmd_str.Format("MCU+RD(1,%c,%04x)\r\n", gpio[pio_name+4-j], ( 1<<igpio[pio_name+4-j+19]));//arg2=A~G,arg3=0001~FFFF
							if(lv){
								answer_str.Format("MCU+RD(%04x)\r\n", (1<< igpio[pio_name+4-j+19]));
								answer_str1.Format("MCU+RD(0000)\r\n");
							}
							else{
								answer_str1.Format("MCU+RD(%04x)\r\n", (1<<igpio[pio_name+4-j+19]));
								answer_str.Format("MCU+RD(0000)\r\n");
								//answer_str1.Format("MCU+RD(0000)\r\n");
							}
							if(i == j){
								if(!pMCU2_Com->send_and_verify(cmd_str, 0, answer_str, 0)) {
									
									//param.Format("mcu2 get PIO%d j=%d ^^",(pio_name+i),(pio_name+j));
									param.Format("mcu2 get PIO=%d ^^",gpio_number[(pio_name+j)]);
										return (false);
								}
								else
								{
									Sleep(30);
								}
							}
							else {
								if(!pMCU2_Com->send_and_verify(cmd_str, 0, answer_str1, 0)) {
									//param.Format("mcu2 get PIO%d j=%d @",(pio_name+i),(pio_name+j));
									param.Format("mcu2 get PIO=%d ^^",gpio_number[(pio_name+j)]);
										return (false);
								}
								
							}
							Sleep(20);			// fu add 
						}

#if 0			
						if(pio_name == 0){
							for(j=0;j<3;j++){
								cmd_str.Format("MCU+RD(1,%c,%04x)\r\n", gpio[6], (1 << igpio[6+j]));//arg2=A~G,arg3=0001~FFFF
								if(lv)
									answer_str1.Format("MCU+RD(0000)\r\n");
								else
									answer_str1.Format("MCU+RD(%04x)\r\n", (1 << igpio[6+j]));
								if(!pMCU2_Com->send_and_verify(cmd_str, 0, answer_str1, 0)) {
										param.Format("mcu2 get PIO%d j=%d ^^",(pio_name+i),(j+6));
											return (false);
								}
							}
						}else {
							for(j=0;j<6;j++){
								cmd_str.Format("MCU+RD(1,%c,%04x)\r\n", gpio[0], (1 << igpio[j]));//arg2=A~G,arg3=0001~FFFF
								if(lv)
									answer_str1.Format("MCU+RD(0000)\r\n");
								else
									answer_str1.Format("MCU+RD(%04x)\r\n", (1 << igpio[j]));
								if(!pMCU2_Com->send_and_verify(cmd_str, 0, answer_str1, 0)) {
										param.Format("mcu2 get PIO%d j=%d ^^",(pio_name+i),j);
											return (false);
								}
							}
						}
#endif
			
#endif

			//param.Empty();
			//param = rx_buf;
			//return (false);
		}
	//}

	if(lv)
		high_data = 1;
	else
		low_data = 0;
	return(true);
}


#endif
int read_rtl8711_xtal_cap(RTL87XX_COM* com_A_B, uint8 &current_xcap, CString &param)
{
	CString cmd_str;
	CString answer_str;
	char rx_buf[512] = {0};
	int ret;
	int result = false;
	int temp = 0;
		

	//0xC9
	//iwpriv config_set wmap,c9,20		//c9:(hex)20:(hex)
	//iwpriv config_get rmap,c9,1		//c9:(hex)1:(dec)	
       #if 0//yhl
	cmd_str.Format("iwpriv config_get rmap,c9,1\r\n");					   
	if(!com_A_B->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
	{
	    param = "rtl87xx get err";
		return (false);
	}
	//if(rx_buf)//"Private Message: 0xFF"
	{
		answer_str.Empty();
		answer_str = rx_buf;
		ret = answer_str.Find("Private Message: 0x");
		temp = hexStr2Hex(rx_buf[ret+19],rx_buf[ret+20]);
		if(temp < 0x40) 
		{	
			current_xcap = (uint8)temp;
			result = true;
		}
		else
			return (false);
	}
	return result;
       #else
	result = true;
	return result;
	#endif
}

int write_rtl8711_xtal_cap(RTL87XX_COM* com_A_B, uint8 current_xcap, CString &param)
{
	CString cmd_str;
	CString answer_str;
	char rx_buf[512] = {0};
	int ret;
	int result = false;
	int temp = 0;
	#if 0//yhl	
	if(current_xcap > 63)
		return (false);
	//0xC9
	//iwpriv config_set wmap,c9,20		//c9:(hex)20:(hex)
	//iwpriv config_get rmap,c9,1		//c9:(hex)1:(dec)	
	//Sleep(500);	
	cmd_str.Format("iwpriv config_set wmap,c9,%02x\r\n",current_xcap);
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err write";
		return (false);
	}
	//Sleep(100);
	cmd_str.Format("iwpriv config_get rmap,c9,1\r\n");					   
	if(!com_A_B->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
	{
	    	param = "rtl87xx get err write";
		return (false);
	}
	//if(rx_buf)//"Private Message: 0xFF"
	{
		answer_str.Empty();
		answer_str = rx_buf;
		ret = answer_str.Find("Private Message: 0x");
		temp = hexStr2Hex(rx_buf[ret+19],rx_buf[ret+20]);
		if(temp == current_xcap) result = true;
	}
	return result;
	#else
	result = true;
	return result;
	#endif
}

//yhl problem
int read_rtl8711_power_Efuse(RTL87XX_COM* com_A_B, uint8 current_channel, CString &param)
{
	CString cmd_str;
	CString answer_str;
	char rx_buf[512] = {0};
	int ret;
	//int result = false;
	int temp = 0;
	//yhl 2018/10/23
	return true;	
	//CCK
	//0x20->ch1,ch2
	//0x21->ch3,ch4,ch5
	//0x22->ch6,ch7,ch8
	//0x23->ch9,ch10,ch11
	//0x24->ch12,ch13
	//0x25->ch14
	//BW40M
	//0x26->ch1,ch2
	//0x27->ch3,ch4,ch5
	//0x28->ch6,ch7,ch8
	//0x29->ch9,ch10,ch11
	//0x2a->ch12,ch13
	
	//iwpriv config_get rmap,c9,1		//c9:(hex)1:(dec)	
	if(current_channel < 6)
	{
		cmd_str.Format("iwpriv config_get rmap,20,1\r\n");					   
		if(!com_A_B->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
		{
		    param = "rtl87xx get err";
			return (false);
		}
		//if(rx_buf)//"Private Message: 0xFF"
		{
			answer_str.Empty();
			answer_str = rx_buf;//2018/10/22
			ret = answer_str.Find("Private Message: 0x");
			temp = hexStr2Hex(rx_buf[ret+19],rx_buf[ret+20]);
			if(temp < 0x40) 
			{	
				com_A_B->tx_pwr_l = (uint8)temp;
			}
			else
				return (false);
		}
	}
	
	else if(current_channel > 11)
	{
		cmd_str.Format("iwpriv config_get rmap,24,1\r\n");					   
		if(!com_A_B->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
		{
		    param = "rtl87xx get err";
			return (false);
		}
		//if(rx_buf)//"Private Message: 0xFF"
		{
			answer_str.Empty();
			answer_str = rx_buf;//2018/10/22
			ret = answer_str.Find("Private Message: 0x");
			temp = hexStr2Hex(rx_buf[ret+19],rx_buf[ret+20]);
			if(temp < 0x40) 
			{	
				com_A_B->tx_pwr_h = (uint8)temp;
			}
			else
				return (false);
		}
	}
	else
	{
		cmd_str.Format("iwpriv config_get rmap,22,1\r\n");					   
		if(!com_A_B->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
		{
			param = "rtl87xx get err";
			return (false);
		}
		//if(rx_buf)//"Private Message: 0xFF"
		{
			answer_str.Empty();
			answer_str = rx_buf;
			ret = answer_str.Find("Private Message: 0x");
			temp = hexStr2Hex(rx_buf[ret+19],rx_buf[ret+20]);
			if(temp < 0x40) 
			{	
				com_A_B->tx_pwr_m = (uint8)temp;
			}
			else
				return (false);
		}
	}

	
	return true;
}








//yhl problem
//这里要考虑有些项不测试，但要求根据测试项填写值(或者填默认值)进模块
int write_rtl8711_power_Efuse(RTL87XX_COM* com_A_B, uint8 current_channel, uint8 index_difference, CString &param)
{
	CString cmd_str;
	return true;//yhl 2018/10/23
	//CString answer_str;
	//char rx_buf[128] = {0};
	//int ret;
	//int result = false;
	//int temp = 0;
	
	//CCK
	//0x20->ch1,ch2
	//0x21->ch3,ch4,ch5
	//0x22->ch6,ch7,ch8
	//0x23->ch9,ch10,ch11
	//0x24->ch12,ch13
	//0x25->ch14
	//BW40M
	//0x26->ch1,ch2
	//0x27->ch3,ch4,ch5
	//0x28->ch6,ch7,ch8
	//0x29->ch9,ch10,ch11
	//0x2a->ch12,ch13
	//0x2B->tx_pwr_index_difference 
	


	//if(com_A_B->tx_pwr_index_difference > 63)//-8~7;-8~7
	//{	
	//	return (false);
	//}

	
	//0xC9
	//iwpriv config_set wmap,c9,20		//c9:(hex)20:(hex)
	if(current_channel < 6)
	{	
		if(com_A_B->tx_pwr_l > 63)
			return (false);
		cmd_str.Format("iwpriv config_set wmap,20,%02x\r\n",com_A_B->tx_pwr_l);
		if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
		{
		    param = "rtl87xx set err power1";
			return (false);
		}
		cmd_str.Format("iwpriv config_set wmap,21,%02x\r\n",com_A_B->tx_pwr_l);
		if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
		{
		    param = "rtl87xx set err power2";
			return (false);
		}
		cmd_str.Format("iwpriv config_set wmap,26,%02x\r\n",com_A_B->tx_pwr_l);
		if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
		{
		    param = "rtl87xx set err power3";
			return (false);
		}
		cmd_str.Format("iwpriv config_set wmap,27,%02x\r\n",com_A_B->tx_pwr_l);
		if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
		{
		    param = "rtl87xx set err power4";
			return (false);
		}
	}
	else if(current_channel > 11)
	{	
		if(com_A_B->tx_pwr_h > 63)
			return (false);

		cmd_str.Format("iwpriv config_set wmap,24,%02x\r\n",com_A_B->tx_pwr_h);
		if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
		{
		    param = "rtl87xx set err power5";
			return (false);
		}
		cmd_str.Format("iwpriv config_set wmap,25,%02x\r\n",com_A_B->tx_pwr_h);
		if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
		{
		    param = "rtl87xx set err power6";
			return (false);
		}
		cmd_str.Format("iwpriv config_set wmap,2a,%02x\r\n",com_A_B->tx_pwr_h);
		if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
		{
		    param = "rtl87xx set err power7";
			return (false);
		}

		//----------------when write high tx power end

		//band20  and ofdm
		cmd_str.Format("iwpriv config_set wmap,2b,%02x\r\n",index_difference);
		if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
		{
		    param = "rtl87xx set err power8";
			return (false);
		}

		// thermal meter
		cmd_str.Format("iwpriv config_set wmap,ca,%02x\r\n",26);			
		if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
		{
		    param = "rtl87xx set err power9";
			return (false);
		}

		//---------------end write another para
	}
	else	
	{	
		if(com_A_B->tx_pwr_m > 63)
			return (false);
		cmd_str.Format("iwpriv config_set wmap,22,%02x\r\n",com_A_B->tx_pwr_m);
		if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
		{
		    param = "rtl87xx set err power10";
			return (false);
		}
		cmd_str.Format("iwpriv config_set wmap,23,%02x\r\n",com_A_B->tx_pwr_m);
		if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
		{
		    param = "rtl87xx set err power11";
			return (false);
		}
		cmd_str.Format("iwpriv config_set wmap,28,%02x\r\n",com_A_B->tx_pwr_m);
		if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
		{
		    param = "rtl87xx set err power12";
			return (false);
		}
		cmd_str.Format("iwpriv config_set wmap,29,%02x\r\n",com_A_B->tx_pwr_m);
		if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
		{
		    param = "rtl87xx set err power13";
			return (false);
		}
	}
///*
#if 0
	//cmd_str.Format("iwpriv config_set wmap,2b,%x\r\n",com_A_B->tx_pwr_index_difference);
	cmd_str.Format("iwpriv config_set wmap,2b,%02x\r\n",index_difference);
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
#endif
//*/	
	//result = true;
	return true;
}




int set_rtl8711_xtal_cap(RTL87XX_COM* com_A_B, uint8 current_xcap, CString &param)
{
	CString cmd_str;
	#if 0//yhl	
	if(current_xcap > 63)
		return (false);

	cmd_str.Format("iwpriv mp_phypara xcap=%d\r\n",current_xcap);
	if(!com_A_B->send_and_search(cmd_str, 0, "Private Message:", 0))
	{
    	param = "rtl87xx set err cap";
		return (false);
	}
	return true;
      #else
	return true;
       #endif
	
}

int set_rtl8711_power(RTL87XX_COM* com_A_B, uint8 current_pow, CString &param)
{
	CString cmd_str;
	#if 0
	if(current_pow > 63)
		return (false);
	
	cmd_str.Format("iwpriv mp_txpower patha=%d,pathb=0\r\n",current_pow);//a通道42dB功率
	if(!com_A_B->send_and_search(cmd_str, 0, "Private Message:", 0))
	{
    	param = "rtl87xx set err";
		return (false);
	}
       #endif
	return true;
}
int set_rtl8711_channel(RTL87XX_COM* com_A_B, uint8 current_channel, CString &param)
{
	CString cmd_str;
	#if 0//yhl
	if((current_channel > 14)||(current_channel < 1))
		return (false);
	
	cmd_str.Format("iwpriv mp_channel %d\r\n",current_channel);
	if(!com_A_B->send_and_search(cmd_str, 0, "Private Message:", 0))
	{
    	param = "rtl87xx set err channel";
		return (false);
	}
	return true;
	#else
	if((current_channel > 14)||(current_channel < 1))
	return (false);
	 
	cmd_str.Format("esp_tx %d 0xc\r",current_channel);
	if(!com_A_B->send_and_search(cmd_str, 0, "channel", 0))
	{
    	param = "esp82xx set err channel";
		return (false);
	} 
	return true;
	#endif
}

int init_rtl8711_tx_test( RTL87XX_COM* com_A_B, int tset_dev_id, CString &param)
{
	CString cmd_str;
	int i;
	/***************************************************
	从实际的调用角度来看，下面的代码是可以屏蔽的，因为不管是
	RF测试还是金机校验其实在之前就切换到mp，这里只是保险一点。
	但是因为获取不到返回的信息而超时也是会占用一定时间。
	***************************************************/
#if 0//yhl
#if 0			
	cmd_str.Format("ATSR\r\n");//to MP	
	if(com_A_B->send_and_search(cmd_str, 0, "Recover OTA signature success.", 0))
	{//如果切换成功表示当前在MP模式，需要重启模块
		if(tset_dev_id == TEST_DEV_ID_B)
		{
			delete(pRTL8711_2_Com);
			pRTL8711_2_Com = NULL;
			if(!pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY))
			{
				param = "set dut battery err";
				return (false);
			}
			Sleep(200);
			if(!pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY))
			{
				param = "set dut battery err";
				return (false);
			}
						
			// 5次重连机会
			for(i=0;i<5;i++)			
			{	    		
				Sleep(500);
				pRTL8711_2_Com = new RTL87XX_COM("R8711_B"); 
				if(pRTL8711_2_Com->connect("", 0)) 
				{	
					UPDATE_TITLE();
					Sleep(1000);
					break;
				}
				else
				{
					delete(pRTL8711_2_Com);
					pRTL8711_2_Com = NULL;
					if(i==4)
						return (false);
				}
			}			
		}
		else
		{			
			delete(pRTL8711_1_Com);
			pRTL8711_1_Com = NULL;	
			if(!pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY))
			{
				param = "set dut battery err";
				return (false);
			}
			Sleep(200);
			if(!pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY))
			{
				param = "set dut battery err";
				return (false);
			}
			
			// 5次重连机会
			for(i=0;i<5;i++)			
			{	    		
				Sleep(500);
				pRTL8711_1_Com = new RTL87XX_COM("R8711_A"); 
				if(pRTL8711_1_Com->connect("", 0)) 
				{	
					UPDATE_TITLE();
					Sleep(1000);
					break;
				}
				else
				{
					delete(pRTL8711_1_Com);
					pRTL8711_1_Com = NULL;
					if(i==4)
						return (false);
				}
			}
		}
	}
#endif	
/*
	Sleep(200);
	Sleep(200);

	cmd_str.Format("\r\n");
	com_A_B->send(cmd_str, 0);
	cmd_str.Format("\r\n");
	com_A_B->send(cmd_str, 0);
	cmd_str.Format("\r\n");
	com_A_B->send(cmd_str, 0);
*/
//init
	//Sleep(500);//ms		
	cmd_str.Format("iwpriv mp_start\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err init";
		return (false);
	}
/*
	cmd_str.Format("iwpriv config_get rmap,0,256\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}

	cmd_str.Format("iwpriv config_get rmap,100,256\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
		param = "rtl87xx set err";
		return (false);
	}
*/
	cmd_str.Format("iwpriv mp_phypara xcap=32\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err init";
		return (false);
	}
/*
	cmd_str.Format("iwpriv mp_bandwidth 40M=0,shortGI=0\r\n");//20M带宽，长间隔
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
*/	
	cmd_str.Format("iwpriv mp_channel 1\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err init";
		return (false);
	}
	//cmd_str.Format("iwpriv mp_rate 135\r\n");//802.11n//MCS7
	cmd_str.Format("iwpriv mp_rate 108\r\n");//802.11g//OFDM_54M
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err init";
		return (false);
	}
	cmd_str.Format("iwpriv mp_bandwidth 40M=0,shortGI=0\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err init";
		return (false);
	}
	cmd_str.Format("iwpriv mp_txpower patha=42,pathb=0\r\n");//a通道42dB功率
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err init";
		return (false);
	}	
	cmd_str.Format("iwpriv mp_ant_tx a\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err init";
		return (false);
	}
/*	
	cmd_str.Format("iwpriv mp_ant_rx a\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
*/	
/*	
	cmd_str.Format("iwpriv mp_reset_stats\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
*/
	return (true);
       #else
	return (true);
	#endif
}


int start_rtl8711_tx_power( RTL87XX_COM* com_A_B, uint8 current_pow, CString &param)
{
	CString cmd_str;
	#if 0//yhl 2018/10/23
	cmd_str.Format("iwpriv mp_ctx background,pkt\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err start";
		return (false);
	}

	if(current_pow > 63)
		return (false);
	
	cmd_str.Format("iwpriv mp_txpower patha=%d,pathb=0\r\n",current_pow);//a通道42dB功率
	if(!com_A_B->send_and_search(cmd_str, 0, "Private Message:", 0))
	{
    	param = "rtl87xx set err start";
		return (false);
	}
	return (true);
	#else
	if(current_pow > 63)
		return (false);
	return (true);
	#endif	
}

int stop_rtl8711_tx( RTL87XX_COM* com_A_B, CString &param)
{
	CString cmd_str;
	
       #if 0//yhl
	cmd_str.Format("iwpriv mp_ctx stop\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err stop";
		return (false);
	}
	#endif
/*	
	cmd_str.Format("iwpriv mp_query\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
	cmd_str.Format("iwpriv mp_arx phy\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
	cmd_str.Format("iwpriv mp_arx mac\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
*/
       #if 0//yhl
	cmd_str.Format("iwpriv mp_stop\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err stop";
		return (false);
	}
      #else
	cmd_str.Format("cmdstop\r");
	if(!com_A_B->send_and_search(cmd_str, 0, "Tx Over", 0)) //可用
	{
	   //param = "esp8266xx set err tx stop";
		//return (false);
	}
       #endif
	return (true);
}
int stop_rtl8711_rx( RTL87XX_COM* com_A_B, CString &param)
{
	CString cmd_str;
	
       #if 0//yhl
	cmd_str.Format("iwpriv mp_arx stop\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
	#endif
/*	
	cmd_str.Format("iwpriv mp_query\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
	cmd_str.Format("iwpriv mp_arx phy\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
	cmd_str.Format("iwpriv mp_arx mac\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
*/
	#if 0//yhl
	cmd_str.Format("iwpriv mp_stop\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
       #else
	cmd_str.Format("cmdstop\r");
	if(!com_A_B->send_and_search(cmd_str, 0, "Desired", 0)) //可用
	{
	    //param = "esp8266 set err rx stop";
		//return (false);
	}
       #endif
	return (true);
}
//------------------------------------------------------------------------
int stop_esp8266_tx( RTL87XX_COM* com_A_B, CString &param)
{
	CString cmd_str;
	cmd_str.Format("cmdstop\r");
	if(!com_A_B->send_and_search(cmd_str, 0, "Tx Over", 0)) //可用
	{
	   param = "esp8266xx set err tx stop";
		return (false);
	}
	return (true);
}
//------------------------------------------------------------------------
int stop_esp8266_rx( RTL87XX_COM* com_A_B, CString &param)
{
	CString cmd_str;
	cmd_str.Format("cmdstop\r");
	if(!com_A_B->send_and_search(cmd_str, 0, "Desired", 0)) //可用
	{
	    param = "esp8266 set err rx stop";
		return (false);
	}
	return (true);
}
//-----------------------------------------------------------------------------
int start_esp8266_rx(RTL87XX_COM* com_A_B, uint8 current_channel, CString &param)
{
	CString cmd_str;

	if((current_channel > 14)||(current_channel < 1))
	return (false);
	
	cmd_str.Format("esp_rx %d 0xc\r",current_channel);
	if(!com_A_B->send_and_search(cmd_str, 0, "channel", 0))
	{
    	param = "esp8266 set err rx channel";
		return (false);
	}
	return true;
}

int start_esp8266_tx(RTL87XX_COM* com_A_B, uint8 current_channel, CString &param)
{
	CString cmd_str;

	if((current_channel > 14)||(current_channel < 1))
	return (false);
	
	cmd_str.Format("esp_tx %d 0xc\r",current_channel);
	if(!com_A_B->send_and_search(cmd_str, 0, "channel", 0))
	{
    		param = "esp8266 set err tx channel";
		return (false);
	}
	return true;
}
//------------------------------------------------------------------
int start_rtl8711_rx( RTL87XX_COM* com_A_B, CString &param)
{
	CString cmd_str;
	
	//start rx	
	#if 0//yhl
	cmd_str.Format("iwpriv mp_ant_rx a\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
	
	cmd_str.Format("iwpriv mp_reset_stats\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
	
	cmd_str.Format("iwpriv mp_query\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
	cmd_str.Format("iwpriv mp_arx phy\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
	cmd_str.Format("iwpriv mp_arx mac\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
	#else
	cmd_str.Format("iwpriv mp_arx mac\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
	#endif
	return (true);
}


int get_rtl8711_rx_data( RTL87XX_COM* com_A_B, double &receive_ok, double &receive_err, CString &param)
{
	CString cmd_str;
	CString answer_str;
	char rx_buf[512] = {0};
	int ret_head,ret_end;
	//int result = false;
	int temp = 0,i = 0;	
	double Receive_OK=0;
	double Receive_error=0;
	int Receive_Counter=0;
	int Receive_OFDM=0;

       #if 0//yhl
	cmd_str.Format("iwpriv mp_query\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
	
/*	
# iwpriv mp_arx phy	
Private Message: Phy Received packet OK:5 CRC error:61 FA Counter: 539 OFDM_FA:496 CCK_FA:43
[MEM] After do cmd, available heap 312128
*/
	
	cmd_str.Format("iwpriv mp_arx phy\r\n");
	if(!com_A_B->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
	//if(rx_buf)//"Private Message: 0xFF"
	{
		answer_str.Empty();
		answer_str = rx_buf;
		
		ret_head = answer_str.Find("OK:") + 3;// 3
		ret_end = answer_str.Find(" CRC error:");// 11
		//十进制位数
		temp = ret_end - ret_head;
		for(i=0;i<temp;i++)
		{	
			Receive_OK = Receive_OK * 10;
			Receive_OK += hexCharToValue(rx_buf[ret_head + i]);
		}

		ret_head = ret_end + 11;
		ret_end = answer_str.Find(" FA Counter: ");// 13
		//十进制位数
		temp = ret_end - ret_head;
		for(i=0;i<temp;i++)
		{	
			Receive_error = Receive_error * 10;
			Receive_error += hexCharToValue(rx_buf[ret_head + i]);
		}

		ret_head = ret_end + 13;
		ret_end = answer_str.Find(" OFDM_FA:");// 9
		//十进制位数
		temp = ret_end - ret_head;
		for(i=0;i<temp;i++)
		{	
			Receive_Counter = Receive_Counter * 10;
			Receive_Counter += hexCharToValue(rx_buf[ret_head + i]);
		}

		ret_head = ret_end + 9;
#if 0		
//有可能没有" CCK_FA:"		
		ret_end = answer_str.Find(" CCK_FA:");// 8
		//十进制位数
		temp = ret_end - ret_head;
#else

		for(temp=0;temp<10;temp++)
		{	
			if((hexCharToValue(rx_buf[ret_head + temp])) == -1)
				break;
		}
#endif
		for(i=0;i<temp;i++)
		{	
			Receive_OFDM = Receive_OFDM * 10;
			Receive_OFDM += hexCharToValue(rx_buf[ret_head + i]);
		}
		receive_ok = Receive_OK;
		receive_err = Receive_error;
/*		
		if((Receive_OK)&& ((Receive_OK + Receive_error) > 900))
		{
			error_per = (Receive_error/(Receive_OK + Receive_error))*100;
			param.Format("err per:%.2lf%%",center_err_per);
			show_message();
		}
		else
		{
		    param = "rtl87xx receive err";
			return (false);
		}
*/		
	}
	
	cmd_str.Format("iwpriv mp_arx mac\r\n");
	if(!com_A_B->send_and_search(cmd_str, 0, "[MEM]", 0)) //可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
       #else
	cmd_str.Format("cmdstop\r");
	#if 0
	  if(!com_A_B->send_and_search(cmd_str, 0, "Correct", 0)) //可用
	   	{
			  param = "rtl87xx set err";
		return (false);

	   	}
	  #else 
	if(!com_A_B->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
	{
	    param = "rtl87xx set err";
		return (false);
	}
	#endif
		answer_str.Empty();//2018/10/22
		answer_str = rx_buf;
		
		ret_head = answer_str.Find("Correct: ")+9;// 3
		ret_end = answer_str.Find(" Desired:");// 11
		//十进制位数
		temp = ret_end - ret_head;
		for(i=0;i<temp;i++)
		{	
			Receive_OK = Receive_OK * 10;
			Receive_OK += hexCharToValue(rx_buf[ret_head + i]);
		}

		ret_head = answer_str.Find("err: ")+5;// 3
		ret_end = answer_str.Find(" err_fcs:");// 11
		//十进制位数
		temp = ret_end - ret_head;
		for(i=0;i<temp;i++)
		{	
			Receive_error = Receive_error * 10;
			Receive_error += hexCharToValue(rx_buf[ret_head + i]);
		}
		receive_ok = Receive_OK;
		receive_err = Receive_error;
	#endif
	return (true);
}

void  OutputLog(LPCTSTR logName, CString msg)
{
	try
	{
		//设置文件的打开参数
		CStdioFile outFile(logName, CFile::modeNoTruncate | CFile::modeCreate | CFile::modeWrite | CFile::typeText);
		CString msLine;
		CTime tt = CTime::GetCurrentTime();

		//作为Log文件，经常要给每条Log打时间戳，时间格式可自由定义，
		//这里的格式如：2010-June-10 Thursday, 15:58:12
		msLine = tt.Format("[%Y-%B-%d %A, %H:%M:%S] ") + msg;
		//msLine += "/r/n";

		//在文件末尾插入新纪录
		outFile.SeekToEnd();
		outFile.WriteString(msLine);
		outFile.Close();
	}
	catch (CFileException *fx)
	{
		fx->Delete();
	}
}


int Create_N4010A_Instrument(CString N4010Address,CString &param)
{
	if(AfxOleInit()==0)
	{
		param = "set Instrument init err";
		return(false);
	}
	if(Inst.CreateDispatch("Agilent_N4010_WLanTest.Instrument")==0)
	{
		param = "set Instrument Create err";
		return(false);
	}	
	Inst.SetInstrumentVISAResourceString(N4010Address);
	Inst.SetShowSplashScreen(true);
	Inst.ActivateInstrument();
	Inst.SetWlanMode();
	Inst.state = true;
	return (true);
}
int Release_N4010A_Instrument()
{
	Inst.Dispose();
	Inst.ReleaseDispatch();
	AfxOleTerm();
	Inst.state = false;
	return (true);
}

//-------------------------------------------------------------------------------------------------

#define ONLY_READ		//difine only read ,don,t calibration

RF_test_integration::RF_test_integration(const char * pname,testSet * Tset,int ID,ConfigFile & cfg):CTestItem(pname,Tset,ID)
{

	/*
		2b high 4bit --> HT20
		2b LOW  44bit -->ofdm
		high 4bit 0x0-0x7  in efuse 0~7  , 0x8-0xf  in efuse -8 ~ -1,简单点描述是我们设置还是正数，不过读efuse的时候解读不一样
		low 4bit  类似
		软件里面设置是-8~7,所以人为的+8转换。
	*/
	int dw20_difference_dw40;
	int ofdm_difference_dw40;
	
	dw20_difference_dw40 = cfg.Vint(Par_bw20_bw40_df);
	ofdm_difference_dw40 = cfg.Vint(Par_ofdm_bw40_df);

	if(dw20_difference_dw40 < 0)
		dw20_difference_dw40 += 16 ;

	if(ofdm_difference_dw40 < 0)
		ofdm_difference_dw40 += 16;
	//index_difference = (dw20_difference_dw40&0x0f)<<4;
	//index_difference |= (ofdm_difference_dw40&0x0f);

	//pwr_min = cfg.VDouble(ParN4010A_MinPW);
    //pwr_max = cfg.VDouble(ParN4010A_MaxPW);
    index_difference = (dw20_difference_dw40&0x0f)<<4;
	index_difference |= (ofdm_difference_dw40&0x0f);
	pwr_min = cfg.VDouble(ParN4010A_MinPW);
	pwr_max = cfg.VDouble(ParN4010A_MaxPW);
	fre_instrument_loss = cfg.VDouble(Par_instru_foff_loss);
		
	par_is_gloden = false;
}

void RF_test_integration::gen_prompt(void)
{
	char itemstr[8];
       itoa(Index(),itemstr,10);

      // CDutGlodenPar *par = pMainWnd->dutGloden[DUT_PAR_IDX];
        
       //neg_max = par->rf_max_neg_foffset;
	//pos_max = par->rf_max_pos_foffset;

       CString range;
       //range.Format("offset:%.2lfK~%.2lfK", neg_max, pos_max);	
	//range.Format("%.lfMHz +/- %.lfHz",center_fre,fre_offset);
	range.Format("rf integration test");
       tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}

void RF_test_integration::update_range(void)
{
        ConfigFile &cfg = pCfgf;
        CDutGlodenPar *pgloden = pMainWnd->dutGloden[DUT_PAR_IDX];
        bool update = false;

}

int RF_test_integration::ItemTestReady(ConfigFile &cfg)
{   
	mutex_hodle1 = false;
	if (!pMainWnd->glbResource_get(GRESOURCE_MCU1))
	{
		param = "u18 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_hodle1 = true;

	mutex_hodle2 = false;
	
	if(!pMainWnd->glbResource_get(GRESOURCE_BT_TSET))
	{
		param = "get gsource err";
		return(ERR_STOP);
	}
	mutex_hodle2 = true;
	
	return(NO_ERR);
}

int RF_test_integration::ItemTestClean(ConfigFile &cfg)
{
	
	if(mutex_hodle2)
	{ 
		pMainWnd->glbResource_put(GRESOURCE_BT_TSET);
		mutex_hodle2 = false;
	}
	
	if (mutex_hodle1)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU1);
        mutex_hodle1 = false;
	}
	   
	return(NO_ERR);
}

#ifdef ONLY_READ		//后面可以用一个config来做
//yhl 2018/10/18//problem
int RF_test_integration::TestOp(ConfigFile &cfg)
{
	result =false ;
	bool result_err = false;
	is_tested = true;
	int upperlimit_Index = cfg.Vint(ParN4010A_MaxPWLEL);
	int lowerlimit_Index = cfg.Vint(ParN4010A_MinPWLEL);
	
	uint8 temp,fre_offset_Index;
	CString m_szN4010Address = cfg.VCString(ParN4010A_Port);
	double center_fre = 2.412e9;
	int i=0,j=0,get_range=0,n=0;
	double	PowerRange1 = 0;
	double	PowerRange2 = 0;
	double	TriggerLevel = 0;
	long	MaxSymbolsUsed = 0;
	double	MaxPacketLength = 0;
	double instrument_compensate=0;
	double	PowerRange = 0;
	uint8    channel_one=1;
	CString err_str="esp8266 test";
	/*freq set para*/
	double freqoffset = 0;
	double fre_offset = cfg.VDouble(ParN4010A_freqOffset);// 2000Hz
	uint8 fre_current_Index = 32;//cfg.Vint(ParFreqIndex);
	double	freqoffsetlimit = fre_offset;

	
	/*tx power / evm  set para*/
	uint8 current_Index[3] = {(uint8)cfg.Vint(ParN4010A_PWLEL),(uint8)cfg.Vint(ParN4010A_PWLEL),(uint8)cfg.Vint(ParN4010A_PWLEL)};
	//uint8 current_Index1 = (uint8)cfg.Vint(ParN4010A_PWLEL);
	//uint8 current_Index2 = (uint8)cfg.Vint(ParN4010A_PWLEL);
	double avpower[3]={0};
	double avpower_test;
	double average_evm[3];
	//double avpower1;
	//double avpower2;
	int channel[3]={1,7,13};
	//int channel=0;
	//int channel1=7;
	//int channel2=13;
	double path_power_loss[3]={0,0,0};
	//double path_power_loss1;
	//double path_power_loss2;
	double path_evm_loss[3]={0,0,0};
	//double path_evm_loss1;
	//double path_evm_loss2;
	double evm_sum;
	double power_sum;
	double current_power;
	double current_evm;
	double current_evm_dBm;
	CString cmd_set_mode;
	 CString cmd_set_value;
	uint8 current_Index_tx;		//read tx power index and set

	if(tset->dev_id == TEST_DEV_ID_A)
	{
			path_power_loss[0] = cfg.VDouble(s_DevA_LCH_pathLoss);
			path_power_loss[1] = cfg.VDouble(s_DevA_MCH_pathLoss);
			path_power_loss[2]= cfg.VDouble(s_DevA_HCH_pathLoss);

			path_evm_loss[0] = cfg.VDouble(Par_a_evm_loss);
			path_evm_loss[1] = cfg.VDouble(Par_a_evm_lossm);
			path_evm_loss[2] = cfg.VDouble(Par_a_evm_lossh);
	}
	else
	{
			path_power_loss[0] = cfg.VDouble(s_DevB_LCH_pathLoss);
			path_power_loss[1] = cfg.VDouble(s_DevB_MCH_pathLoss);
			path_power_loss[2]= cfg.VDouble(s_DevB_HCH_pathLoss);
			
			path_evm_loss[0] = cfg.VDouble(Par_b_evm_loss);
			path_evm_loss[1] = cfg.VDouble(Par_b_evm_lossm);
			path_evm_loss[2] = cfg.VDouble(Par_b_evm_lossh);
	}
	
	if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pMCU1_Com->set_4010_route_dut1(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut1 err";
			return (false);
		}	
	}
	else
	{
		if(!pMCU1_Com->set_4010_route_dut2(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut2 err";
			return (false);
		}
	}

	if(!pBtTestSet)
	{
		param = "null device";
		return(ERR_NOMAL);
	}
	if(!pMCU1_Com)
	{
		param = "null com";
		return(ERR_NOMAL);
	}
	bt_TSET *tSet= pBtTestSet;
	if(!tSet || !tSet->dev_is_ready())
	{
		param = "N4010A err";
		return(ERR_NOMAL);
	}

	#if 1
	//when test golden mechina ,wo need set instrument compensate
	//instrument compensate
	instrument_compensate = cfg.VDouble(Par_instrument_compensate);
	if(((N4010A_DEV*)pBtTestSet)->setPathOffset(instrument_compensate)==false)	
	{
		param = "write rf path loss err";
		//show_prompt(err_str);
		return(false);
	}


	//double  fre_instrument_set_loss = fre_instrument_loss;

	center_fre = 2.412e9 + (channel[0]-1)*0.005e9;
	Inst.SetFrequency(center_fre);

	if(tset->dev_id == TEST_DEV_ID_B)
	{
		start_esp8266_tx(pRTL8711_2_Com,channel[0],param);
	}
	else
	{	
		start_esp8266_tx(pRTL8711_1_Com,channel[0],param);
	}
	
//yhl 2018/10/24
	Inst.IEEE80211a_Autorange_Run();
			#if 0
			PowerRange = Inst.GetIEEE80211a_Autorange_PowerRange();
			TriggerLevel = Inst.GetIEEE80211a_Autorange_TriggerLevel();
			MaxSymbolsUsed = Inst.GetIEEE80211a_Autorange_MaxSymbolsUsed();
			#else
			PowerRange =20;
			TriggerLevel =0.1;
			MaxSymbolsUsed =60;
			#endif
	//MaxPacketLength = Inst.GetIEEE80211a_Autorange_MaxPacketLength();
	
	Inst.SetPowerRange(PowerRange);
	//Inst.SetMaxPacketLength(MaxPacketLength);
	Inst.SetMaxSymbolsUsed(MaxSymbolsUsed);//802.11a/g
	Inst.SetTriggerStyle(2);
	Inst.SetTriggerLevel(TriggerLevel);
	Inst.SetTriggerDelay(0);
	Inst.SetTriggerHoldoff(1e-6);	
	Inst.SetIEEE80211a_Demod_MeasureGatedPower(true);
	
/*  可以直接读出第一次42位置上的tx power*/
//---------------------------------------------------------------------------------
	Sleep(100);//这里一定要有延时
	Inst.IEEE80211a_Demod_Initiate();
	for(i=0;i<5;i++)
	{								
		freqoffset = Inst.GetIEEE80211a_Demod_FrequencyError();
		freqoffset -= fre_instrument_loss;		//-4k mean less than centre 4k ,+4k mean more than centre 4k
		if(fabs(freqoffset)<50000)
		break;
	}		
			param.Format("%d:%.lfHz",fre_current_Index,freqoffset);
			show_message();

		if(tset->dev_id == TEST_DEV_ID_B)
		{
			stop_esp8266_tx(pRTL8711_2_Com,param);
		}
		else
		{   
			stop_esp8266_tx(pRTL8711_1_Com,param);
		}
			

		for(i=0;i<3;i++)
		{
						
			if(tset->dev_id == TEST_DEV_ID_B)
			{	
				start_esp8266_tx(pRTL8711_2_Com,channel[i],param);
			}
			else
			{ 
				start_esp8266_tx(pRTL8711_1_Com,channel[i],param);
			}
			
			center_fre = 2.412e9 + (channel[i]-1)*0.005e9;
			Inst.SetFrequency(center_fre);
			Inst.IEEE80211a_Autorange_Run();
			#if 0
			PowerRange = Inst.GetIEEE80211a_Autorange_PowerRange();
			TriggerLevel = Inst.GetIEEE80211a_Autorange_TriggerLevel();
			MaxSymbolsUsed = Inst.GetIEEE80211a_Autorange_MaxSymbolsUsed();
			#else
			PowerRange =20;
			TriggerLevel =0.1;
			MaxSymbolsUsed =60;
			#endif
			
			Inst.SetPowerRange(PowerRange);
			Inst.SetMaxSymbolsUsed(MaxSymbolsUsed);//802.11a/g
			Inst.SetTriggerStyle(2);
			Inst.SetTriggerLevel(TriggerLevel);
			Inst.SetTriggerDelay(0);
			Inst.SetTriggerHoldoff(1e-6);			
			Inst.SetIEEE80211a_Demod_MeasureGatedPower(true);
		
			evm_sum = 0;
			power_sum = 0;
			Inst.IEEE80211a_Demod_Initiate();	
			Sleep(100);
			for(j=0;j<5;j++)
			{						
				//这里获取仪器数据有时候会不成功			
				
				//当IEEE80211a_Demod_Initiate获取数据不成功时候去调用GetIEEE80211a_Demod_RmsEVM不会抛出异常
				current_evm = Inst.GetIEEE80211a_Demod_RmsEVM();
				//EVM(dB)=20*log10（evm(%)）
				current_evm_dBm = 20*log10(current_evm/100);				
						evm_sum  += current_evm_dBm;
				
				//Inst.IEEE80211a_Demod_Initiate();
					//Sleep(100);//这里一定要有延时
 				avpower_test= Inst.GetIEEE80211a_Demod_GatedPower();
				power_sum  += avpower_test;
				param.Format("%d:%.2lfdBm",current_Index_tx,avpower_test);
			}
			//current_evm_dBm = evm_sum/10;
			//avpower = power_sum/10;
					avpower[i] = power_sum/5;
						average_evm[i] = evm_sum/5;
			avpower[i]+= path_power_loss[i];
			average_evm[i] += path_evm_loss[i];
			param.Format("%d:%.2lfdBm",current_Index_tx,avpower[i]);
			
			if(tset->dev_id == TEST_DEV_ID_B)
		{
			stop_esp8266_tx(pRTL8711_2_Com,param);
		}
		else
		{   
			stop_esp8266_tx(pRTL8711_1_Com,param);
		}

		}
				/*save para*/
		save_adjust_freqoffset(fre_current_Index,freqoffset);
		for(j=0;j<3;j++)
		{
			save_adjust_power(current_Index[j],avpower[j],j);
			save_read_evm(average_evm[j],j);
		}
		result = true;
		return (true);
	#else
	/************************/
	/************************/
	//path route to N4010A
	if(!pMCU1_Com->set_rout_4010_on(0, MCU_PIO_DUT_4010_METER, MCU_PIO_NUM_DUT_4010_METER))
	{
		param ="switch route to 4010 err";
		return (ERR_NOMAL);
	}

	
	//dut1 route to N4010A
	if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pMCU1_Com->set_4010_route_dut1(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut1 err";
			return (ERR_NOMAL);
		}
	}
	else
	{
		if(!pMCU1_Com->set_4010_route_dut2(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut2 err";
			return (ERR_NOMAL);
		}
	}
	//Sleep(2000);
	//yhl 2018/10/24
	//double  fre_instrument_set_loss = fre_instrument_loss;

	center_fre = 2.412e9 + (channel[0]-1)*0.005e9;
	//center_fre = 2.412e9 + (channel[0]-1)*0.005e9;
	Inst.SetFrequency(center_fre);
//yhl 2018/10/18	//problem_test

	if(tset->dev_id == TEST_DEV_ID_B)
	{
		start_esp8266_tx(pRTL8711_2_Com,channel[0],param);
	}
	else
	{	
		start_esp8266_tx(pRTL8711_1_Com,channel[0],param);
	}
	
	Sleep(100);
	//instrument compensate
	instrument_compensate = cfg.VDouble(Par_instrument_compensate);
	if(((N4010A_DEV*)pBtTestSet)->setPathOffset(instrument_compensate)==false)	
	{
		param = "write rf path loss err";
		return(false);
	}

	
//yhl 2018/10/24
	Inst.IEEE80211a_Autorange_Run();
	PowerRange = Inst.GetIEEE80211a_Autorange_PowerRange();
	TriggerLevel = Inst.GetIEEE80211a_Autorange_TriggerLevel();
	MaxSymbolsUsed = Inst.GetIEEE80211a_Autorange_MaxSymbolsUsed();
	//MaxPacketLength = Inst.GetIEEE80211a_Autorange_MaxPacketLength();
	
	Inst.SetPowerRange(PowerRange);
	//Inst.SetMaxPacketLength(MaxPacketLength);
	Inst.SetMaxSymbolsUsed(MaxSymbolsUsed);//802.11a/g
	Inst.SetTriggerStyle(2);
	Inst.SetTriggerLevel(TriggerLevel);
	Inst.SetTriggerDelay(0);
	Inst.SetTriggerHoldoff(1e-6);	
	Inst.SetIEEE80211a_Demod_MeasureGatedPower(true);

	
	
	//fu close test golden
	//判断是否是金机
	if(tset->is_golden)
	{
		Sleep(100);
		Inst.IEEE80211a_Demod_Initiate();
		freqoffset = Inst.GetIEEE80211a_Demod_FrequencyError();
		if(fabs(freqoffset)>100000)
		{
			param = "read freq err";
			result_err = true;
			goto __err;
		}

		param.Format("golden:%d:%.lfHz",fre_current_Index,freqoffset);

		if(tset->dev_id == TEST_DEV_ID_B)
		{
			stop_esp8266_tx(pRTL8711_2_Com,param);
		}
		else
		{   
			stop_esp8266_tx(pRTL8711_1_Com,param);
		}		
	}
	else		
	{	
/*  可以直接读出第一次42位置上的tx power*/
//---------------------------------------------------------------------------------
			Sleep(100);//这里一定要有延时
			for(i=0;i<5;i++)
			{				
				Inst.IEEE80211a_Demod_Initiate();				
				freqoffset = Inst.GetIEEE80211a_Demod_FrequencyError();
				freqoffset -= fre_instrument_loss;		//-4k mean less than centre 4k ,+4k mean more than centre 4k
				if(fabs(freqoffset)<50000)
					break;
			}		
			param.Format("%d:%.lfHz",fre_current_Index,freqoffset);
			show_message();
	
//---------------------------------------------------------------------------------
		if(tset->dev_id == TEST_DEV_ID_B)
		{
			stop_esp8266_tx(pRTL8711_2_Com,param);
		}
		else
		{   
			stop_esp8266_tx(pRTL8711_1_Com,param);
		}

for(i=0;i<3;i++)
{
			
			if(tset->dev_id == TEST_DEV_ID_B)
			{	
				start_esp8266_tx(pRTL8711_2_Com,channel[i],param);
				//Sleep(1000);
			}
			else
			{ 
				start_esp8266_tx(pRTL8711_1_Com,channel[i],param);
			}

			center_fre = 2.412e9 + (channel[i]-1)*0.005e9;
			Inst.SetFrequency(center_fre);
			Inst.IEEE80211a_Autorange_Run();
		#if 1
			PowerRange = Inst.GetIEEE80211a_Autorange_PowerRange();
			TriggerLevel = Inst.GetIEEE80211a_Autorange_TriggerLevel();
			MaxSymbolsUsed = Inst.GetIEEE80211a_Autorange_MaxSymbolsUsed();
			//MaxPacketLength = Inst.GetIEEE80211a_Autorange_MaxPacketLength();
			
			Inst.SetPowerRange(PowerRange);
			//Inst.SetMaxPacketLength(MaxPacketLength);
			Inst.SetMaxSymbolsUsed(MaxSymbolsUsed);//802.11a/g
			Inst.SetTriggerStyle(2);
			Inst.SetTriggerLevel(TriggerLevel);
			Inst.SetTriggerDelay(0);
			Inst.SetTriggerHoldoff(1e-6);			
		#else
			Inst.SetPowerRange(20.0);
			//Inst.SetMaxPacketLength(970e-6);
			Inst.SetMaxSymbolsUsed(60);//802.11a/g
			Inst.SetTriggerStyle(2);
			Inst.SetTriggerLevel(0.1);
			Inst.SetTriggerDelay(0);
			Inst.SetTriggerHoldoff(1e-6);				
		#endif
			Inst.SetIEEE80211a_Demod_MeasureGatedPower(true);
		#if 1
			evm_sum = 0;
			power_sum = 0;
			Inst.IEEE80211a_Demod_Initiate();
			for(j=0;j<5;j++)
			{						
				//这里获取仪器数据有时候会不成功			
				//Inst.IEEE80211a_Demod_Initiate();	
					//Sleep(100);//这里一定要有延时
				//当IEEE80211a_Demod_Initiate获取数据不成功时候去调用GetIEEE80211a_Demod_RmsEVM不会抛出异常
				current_evm = Inst.GetIEEE80211a_Demod_RmsEVM();
				current_evm_dBm = 20*log10(current_evm/100);				
				evm_sum  += current_evm_dBm;
				
				//Inst.IEEE80211a_Demod_Initiate();
					Sleep(100);//这里一定要有延时
 				avpower_test= Inst.GetIEEE80211a_Demod_GatedPower();
				power_sum  += avpower_test;
				param.Format("%d:%.2lfdBm",current_Index_tx,avpower_test);
			}

			current_evm_dBm = evm_sum/5;
				avpower[i] = power_sum/5;
			avpower[i]+= path_power_loss[i];
						average_evm[i] = evm_sum/5;
			average_evm[i] += path_evm_loss[i];
			param.Format("%d:%.2lfdBm",current_Index_tx,avpower[i]);
			
		#else			
			//这里获取仪器数据有时候会不成功
			Inst.IEEE80211a_EVM_Initiate();
			//当IEEE80211a_EVM_Initiate获取数据不成功时候去调用GetIEEE80211a_EVM_Result不会抛出异常
			current_evm_dBm = Inst.GetIEEE80211a_EVM_Result();//(dB)			
			Inst.AveragePower_Initiate();
			avpower = Inst.GetAveragePower_Result();
		#endif			

			if(tset->dev_id == TEST_DEV_ID_B)
			{
				stop_esp8266_tx(pRTL8711_2_Com,param);
				
			}
			else
			{	
				stop_esp8266_tx(pRTL8711_1_Com,param);
				
			}

		}
	if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pMCU1_Com->set_4010_route_dut2(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut2 err";
			return (ERR_NOMAL);
		}
	}
		/*save para*/
		save_adjust_freqoffset(fre_current_Index,freqoffset);
		for(j=0;j<3;j++)
		{
			save_adjust_power(current_Index[j],avpower[j],j);
			save_read_evm(average_evm[j],j);
		}
	}		
__err:
	if(result_err) 
	{
	         result = false;
			return (false);

	}
	else
	{
			result = true;
			return (true);
	}
#endif
    return(NO_ERR);

}	

#else

int RF_test_integration::TestOp(ConfigFile &cfg)
{
	result = false;
	bool result_err = false;
	is_tested = true;
	int upperlimit_Index = cfg.Vint(ParN4010A_MaxPWLEL);
	int lowerlimit_Index = cfg.Vint(ParN4010A_MinPWLEL);
	
	uint8 temp,fre_offset_Index;
	CString m_szN4010Address = cfg.VCString(ParN4010A_Port);
	double center_fre = 2.412e9;
	int i=0,j=0,get_range=0;
	double	PowerRange1 = 0;
	double	PowerRange2 = 0;
	double	TriggerLevel = 0;
	long	MaxSymbolsUsed = 0;
	double	MaxPacketLength = 0;
	double instrument_compensate=0;
	double	PowerRange = 0;
	CString err_str="esp8266 test";
#if 0
	double evm_sum = 0;
	double average_evm = 0;
	double current_evm = 0;
	double current_evm_dBm = 0;

#endif
	/*freq set para*/
	double freqoffset = 0;
	double fre_offset = cfg.VDouble(ParN4010A_freqOffset);// 2000Hz
	uint8 fre_current_Index = 32;//cfg.Vint(ParFreqIndex);
	double	freqoffsetlimit = fre_offset;
	
	/*tx power / evm  set para*/
	uint8 current_Index[3] = {(uint8)cfg.Vint(ParN4010A_PWLEL),(uint8)cfg.Vint(ParN4010A_PWLEL),(uint8)cfg.Vint(ParN4010A_PWLEL)};
	//uint8 current_Index1 = (uint8)cfg.Vint(ParN4010A_PWLEL);
	//uint8 current_Index2 = (uint8)cfg.Vint(ParN4010A_PWLEL);
	double avpower[3];
	double average_evm[3];
	//double avpower1;
	//double avpower2;
	int channel[3]={1,7,13};
	//int channel1=7;
	//int channel2=13;
	double path_power_loss[3];
	//double path_power_loss1;
	//double path_power_loss2;
	double path_evm_loss[3];
	//double path_evm_loss1;
	//double path_evm_loss2;
	double evm_sum;
	double current_evm;
	double current_evm_dBm;
	
	if(tset->dev_id == TEST_DEV_ID_A)
	{
			path_power_loss[0] = cfg.VDouble(s_DevA_LCH_pathLoss);
			path_power_loss[1] = cfg.VDouble(s_DevA_MCH_pathLoss);
			path_power_loss[2]= cfg.VDouble(s_DevA_HCH_pathLoss);

			path_evm_loss[0] = cfg.VDouble(Par_a_evm_loss);
			path_evm_loss[1] = cfg.VDouble(Par_a_evm_lossm);
			path_evm_loss[2] = cfg.VDouble(Par_a_evm_lossh);
	}
	else
	{
			path_power_loss[0] = cfg.VDouble(s_DevB_LCH_pathLoss);
			path_power_loss[1] = cfg.VDouble(s_DevB_MCH_pathLoss);
			path_power_loss[2]= cfg.VDouble(s_DevB_HCH_pathLoss);
			
			path_evm_loss[0] = cfg.VDouble(Par_b_evm_loss);
			path_evm_loss[1] = cfg.VDouble(Par_b_evm_lossm);
			path_evm_loss[2] = cfg.VDouble(Par_b_evm_lossh);
	}



	if(!pBtTestSet)
	{
		param = "null device";
		return(ERR_NOMAL);
	}
	if(!pMCU1_Com)
	{
		param = "null com";
		return(ERR_NOMAL);
	}
	bt_TSET *tSet= pBtTestSet;
	if(!tSet || !tSet->dev_is_ready())
	{
		param = "N4010A err";
		return(ERR_NOMAL);
	}
	
	//path route to N4010A
	if(!pMCU1_Com->set_rout_4010_on(0, MCU_PIO_DUT_4010_METER, MCU_PIO_NUM_DUT_4010_METER))
	{
		param ="switch route to 4010 err";
		return (ERR_NOMAL);
	}
	//dut1 route to N4010A
	if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pMCU1_Com->set_4010_route_dut1(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut1 err";
			return (ERR_NOMAL);
		}	
	}
	else
	{
		if(!pMCU1_Com->set_4010_route_dut2(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut2 err";
			return (ERR_NOMAL);
		}
	}
	Sleep(200);

	center_fre = 2.412e9 + (channel[0]-1)*0.005e9;
	Inst.SetFrequency(center_fre);
	
	if(tset->dev_id == TEST_DEV_ID_B)
	{
		init_rtl8711_tx_test(pRTL8711_2_Com,TEST_DEV_ID_B,param);
		start_rtl8711_tx_power(pRTL8711_2_Com,current_Index[0],param);
	}
	else
	{		
		init_rtl8711_tx_test(pRTL8711_1_Com,TEST_DEV_ID_A,param);		
		start_rtl8711_tx_power(pRTL8711_1_Com,current_Index[0],param);
	}
	
	Sleep(100);
	//instrument compensate
	instrument_compensate = cfg.VDouble(Par_instrument_compensate);
	if(((N4010A_DEV*)pBtTestSet)->setPathOffset(instrument_compensate)==false)	
	{
		param = "write rf path loss err";
		return(false);
	}

	
#if 1
	Inst.IEEE80211a_Autorange_Run();
	PowerRange = Inst.GetIEEE80211a_Autorange_PowerRange();
	TriggerLevel = Inst.GetIEEE80211a_Autorange_TriggerLevel();
	MaxSymbolsUsed = Inst.GetIEEE80211a_Autorange_MaxSymbolsUsed();
	//MaxPacketLength = Inst.GetIEEE80211a_Autorange_MaxPacketLength();
	
	Inst.SetPowerRange(PowerRange);
	//Inst.SetMaxPacketLength(MaxPacketLength);
	Inst.SetMaxSymbolsUsed(MaxSymbolsUsed);//802.11a/g
	Inst.SetTriggerStyle(2);
	Inst.SetTriggerLevel(TriggerLevel);
	Inst.SetTriggerDelay(0);
	Inst.SetTriggerHoldoff(1e-6);	
#else	
	Inst.SetPowerRange(20.0);
	Inst.SetMaxSymbolsUsed(60);//802.11a/g
	Inst.SetTriggerStyle(2);
	Inst.SetTriggerLevel(0.1);
	Inst.SetTriggerDelay(0);
	Inst.SetTriggerHoldoff(1e-6);
#endif
	Inst.SetIEEE80211a_Demod_MeasureGatedPower(true);
	
	
	
	//fu close test golden
	//判断是否是金机
	//yhl
	if((tset->is_golden == false) && (tset->r_addr.is_valid == false))
	{		
		if(tset->dev_id == TEST_DEV_ID_B)
		{				   
			//tset->is_golden = read_rtl8711_golden_flag(pRTL8711_2_Com,current_Index,param);

		}	
		else
		{
			//tset->is_golden = read_rtl8711_golden_flag(pRTL8711_1_Com,current_Index,param);			   				   
		}
									
		if(tset->is_golden)
		{
			tset->r_addr.is_valid = true;
			param = "";
			if(tset->dev_id == TEST_DEV_ID_B) param.Format("rtl87xx2 is golden");
			else param.Format("rtl87xx1 is golden");
		}	
	}

	if(tset->is_golden)
	{
		if(tset->dev_id == TEST_DEV_ID_B)
		{
			read_rtl8711_xtal_cap(pRTL8711_2_Com,pRTL8711_2_Com->x_tal,param);
			set_rtl8711_xtal_cap(pRTL8711_2_Com,pRTL8711_2_Com->x_tal,param);
			//current_Index = pRTL8711_2_Com->x_tal;
		}
		else
		{   			
			read_rtl8711_xtal_cap(pRTL8711_1_Com,pRTL8711_1_Com->x_tal,param);
			set_rtl8711_xtal_cap(pRTL8711_1_Com,pRTL8711_1_Com->x_tal,param);
			//current_Index = pRTL8711_2_Com->x_tal;
		}
		Sleep(100);
		Inst.IEEE80211a_Demod_Initiate();
		freqoffset = Inst.GetIEEE80211a_Demod_FrequencyError();
		if(fabs(freqoffset)>100000)
		{
			param = "read freq err";
			result_err = true;
			goto __err;
		}

		param.Format("golden:%d:%.lfHz",fre_current_Index,freqoffset);

		if(tset->dev_id == TEST_DEV_ID_B)
		{
			stop_rtl8711_tx(pRTL8711_2_Com,param);
		}
		else
		{   
			stop_rtl8711_tx(pRTL8711_1_Com,param);
		}		
	}
	else		
	{	
//-----------------------------------------------------------------		
		do			
		{	
			if(tset->dev_id == TEST_DEV_ID_B)
			{
				//set_rtl8711_xtal_cap(pRTL8711_2_Com,fre_current_Index,param);
				//start_rtl8711_tx_power(pRTL8711_2_Com,42,param);
				set_rtl8711_channel(pRTL8711_2_Com,channel,err_str);
			}
			else
			{   
				//set_rtl8711_xtal_cap(pRTL8711_1_Com,fre_current_Index,param);
				//start_rtl8711_tx_power(pRTL8711_1_Com,42,param);
				set_rtl8711_channel(pRTL8711_1_Com,channel,err_str);
			}
			//Sleep(1000);		
			Sleep(100);//这里一定要有延时
			//Sleep(100);
			for(i=0;i<10;i++)
			{
				//Hz//GetIEEE80211a_Demod_FrequencyError();//Transmitter "Demod" Test (DSSS/OFDM)
				// Set up the instrument parameters
/*				
				Inst.SetFrequency(2.412e9);
				Inst.SetPowerRange(20.0);
				Inst.SetMaxSymbolsUsed(60);//802.11a/g
				Inst.SetTriggerDelay(0);
				Inst.SetTriggerHoldoff(1e-6);
				Inst.SetTriggerLevel(0.1);
				Inst.SetTriggerStyle(2);
				//Inst.SetIEEE80211a_Demod_MeasureGatedPower(true);
				//Sleep(200);
*/				
				Inst.IEEE80211a_Demod_Initiate();
//*/				
				//Sleep(100);
				freqoffset = Inst.GetIEEE80211a_Demod_FrequencyError();
				//avpower = Inst.GetIEEE80211a_Demod_GatedPower();
				//param.Format("%d:%.lfHz",current_Index,freqoffset);
				//show_message();		
				freqoffset -= fre_instrument_loss;		//-4k mean less than centre 4k ,+4k mean more than centre 4k
				if(fabs(freqoffset)<100000)
					break;
				//Sleep(500);
			}		
			param.Format("%d:%.lfHz",fre_current_Index,freqoffset);
			show_message();
///*			
			if(fabs(freqoffset)>100000)
			{
				param = "read freq err";
				result_err = true;
				goto __err;
			}
//*/		
/*
			freqoffset += freq_compensate;
*/
			if((freqoffset>0) && (freqoffset>freqoffsetlimit))
			{
				//大概 4KHz对应一个等级
				if(freqoffset > 10000)
					temp = (uint8)(freqoffset/3000);
				else
					temp = (uint8)(freqoffset/4000);
				if(temp > 0)
					fre_current_Index -= temp;
				else
					fre_current_Index --;
			}
			else if((freqoffset<0) && (fabs(freqoffset)>freqoffsetlimit))
			{
				if(fabs(freqoffset) > 10000)
					temp = (uint8)(fabs(freqoffset)/3000);
				else
					temp = (uint8)(fabs(freqoffset)/4000);
				if(temp > 0)
					fre_current_Index += temp;
				else
					fre_current_Index ++;
			}
			else
			{
				break;
			}
			if((fre_current_Index <= 0)||(fre_current_Index >= 63))
			{
				param = "this is a bad module in fre";
				result_err = true;
				goto __err;
			}
		}while(fabs(freqoffset)>freqoffsetlimit);//while((upperlimi_Index - lowerlimit_Index) > 1);//

		if(fabs(freqoffset)>freqoffsetlimit)
		{
			param.Format("%d:%.lfHz",fre_current_Index,freqoffset);
			//pMainWnd->SendMessage(WM_FLASH_CUR_ITEM_NO_RESUL,(WPARAM)NULL,(LPARAM)this);
			result_err = true;
			goto __err;
		}
/*  可以直接读出第一次42位置上的tx power*///power
		for(j=0;j<3;j++)
		{
			evm_sum = 0;
			current_evm = 0;
			current_evm_dBm = 0;
			if(j != 0)
			{
				if(tset->dev_id == TEST_DEV_ID_B)
				{
					set_rtl8711_channel(pRTL8711_2_Com,(uint8)channel[j],param);
				}
				else
				{
					set_rtl8711_channel(pRTL8711_1_Com,(uint8)channel[j],param);
				}

				center_fre = 2.412e9 + (channel[j]-1)*0.005e9;
					Inst.SetFrequency(center_fre);				
					#if 0				
					Inst.SetPowerRange(PowerRange2);
					//Inst.SetMaxPacketLength(MaxPacketLength);
					Inst.SetMaxSymbolsUsed(MaxSymbolsUsed);//802.11a/g
					Inst.SetTriggerStyle(2);
					Inst.SetTriggerLevel(TriggerLevel);
					Inst.SetTriggerDelay(0);
					Inst.SetTriggerHoldoff(1e-6);			
					#else
					Inst.SetPowerRange(20.0);
					//Inst.SetMaxPacketLength(970e-6);
					Inst.SetMaxSymbolsUsed(60);//802.11a/g
					Inst.SetTriggerStyle(2);
					Inst.SetTriggerLevel(0.1);
					Inst.SetTriggerDelay(0);
					Inst.SetTriggerHoldoff(1e-6);	
					#endif																	
					Inst.SetIEEE80211a_Demod_MeasureGatedPower(true);
			}
			do
			{
			       #if 0
				if(tset->dev_id == TEST_DEV_ID_B)
				{
					start_rtl8711_tx_power(pRTL8711_2_Com,current_Index[j],param);
				}
				else
				{ 
					start_rtl8711_tx_power(pRTL8711_1_Com,current_Index[j],param);
				}
				#endif
				//Sleep(200);//这里一定要有延时		
				Sleep(100);				
				for(i=0;i<10;i++)
				{
/*
					//Inst.SetChannel(2,channel);//IEEE802_11g=2//enum RadioFormatEnum Format
					center_fre = 2.412e9 + (channel-1)*0.005e9;
					Inst.SetFrequency(center_fre);				
					#if 0				
					Inst.SetPowerRange(PowerRange2);
					//Inst.SetMaxPacketLength(MaxPacketLength);
					Inst.SetMaxSymbolsUsed(MaxSymbolsUsed);//802.11a/g
					Inst.SetTriggerStyle(2);
					Inst.SetTriggerLevel(TriggerLevel);
					Inst.SetTriggerDelay(0);
					Inst.SetTriggerHoldoff(1e-6);			
					#else
					Inst.SetPowerRange(20.0);
					//Inst.SetMaxPacketLength(970e-6);
					Inst.SetMaxSymbolsUsed(60);//802.11a/g
					Inst.SetTriggerStyle(2);
					Inst.SetTriggerLevel(0.1);
					Inst.SetTriggerDelay(0);
					Inst.SetTriggerHoldoff(1e-6);	
					#endif																	
					Inst.SetIEEE80211a_Demod_MeasureGatedPower(true);
*/
					#if 1
					Inst.IEEE80211a_Demod_Initiate();								
					avpower[j]= Inst.GetIEEE80211a_Demod_GatedPower();
					#else
					Inst.AveragePower_Initiate();
					avpower = Inst.GetAveragePower_Result();
					#endif
					avpower[j] += path_power_loss[j];

					param.Format("%d:%.2lfdBm",current_Index[j],avpower[j]);
					show_message();

					if(avpower[j] > 0)
						break;
				}
				if(avpower[j] > pwr_max)
				{
					//大概每变化一个等级对应avpower读数变化0.5dBm
					temp = (uint8)((avpower[j]-pwr_max)/0.5);
					if(temp > 0)
						current_Index[j] -= temp;
					else
						current_Index[j]--;
				}
				else if(avpower[j] < pwr_min)
				{
					temp = (uint8)((pwr_min-avpower[j])/0.5);
					if(temp > 0)
						current_Index[j] += temp;
					else
						current_Index[j]++;
				}
				else
				{
					break;
				}

				if((current_Index[j] <= 0)||(current_Index[j] >= 63))
				{
					param = "this is a bad module in tx power";
					result_err = true;
					goto __err;
				}
			
			}while((avpower[j] < pwr_min)||(avpower[j] > pwr_max));//while((upperlimit_Index - lowerlimit_Index) > 0);

/*pwr get so get evm */
#if 1
			for(i=0;i<10;i++)
			{
				Inst.IEEE80211a_Demod_Initiate();
				//Sleep(100);			
				current_evm = Inst.GetIEEE80211a_Demod_RmsEVM();
				//EVM(dB)=20*log10（evm(%)）
				current_evm_dBm = 20*log10(current_evm/100);		
				evm_sum  += current_evm_dBm;
		
				//param.Format("%.2lf%%:%.2lfdbm",current_evm,current_evm_dBm);
				//show_message();
			}
			average_evm[j] = evm_sum/10;
#else	
			Inst.IEEE80211a_EVM_Initiate();
			average_evm = Inst.GetIEEE80211a_EVM_Result();//(dB)
#endif	
			average_evm[j] += path_evm_loss[j];
			param.Format("aversge EVM:%.2lfdbm",average_evm[j]);
			show_message();
		}

		/* test all item*/
		
		if(tset->dev_id == TEST_DEV_ID_B)
		{
			stop_rtl8711_tx(pRTL8711_2_Com,param);
		}
		else
		{	
			stop_rtl8711_tx(pRTL8711_1_Com,param);
		}

		/*save para*/
		save_adjust_freqoffset(fre_current_Index,freqoffset);
		for(j=0;j<3;j++)
		{
			save_adjust_power(current_Index[j],avpower[j],j);
			save_read_evm(average_evm[j],j);
		}

		/*write to flash*/
		if(fabs(freqoffset)>freqoffsetlimit)
		{
			param.Format("%d:%.lfHz",current_Index,freqoffset);
			//pMainWnd->SendMessage(WM_FLASH_CUR_ITEM_NO_RESUL,(WPARAM)NULL,(LPARAM)this);
			result_err = true;
			goto __err;
		}
		else
		{		
			if(tset->dev_id == TEST_DEV_ID_B)
			{
				if(!write_rtl8711_xtal_cap(pRTL8711_2_Com,fre_current_Index,param))
				{
				    param = "rtl87xx2 set err";
					result_err = true;
					goto __err;
				}
				pRTL8711_2_Com->x_tal = fre_current_Index;
			}	
			else
			{
				if(!write_rtl8711_xtal_cap(pRTL8711_1_Com,fre_current_Index,param))
				{
				    param = "rtl87xx1 set err";
					result_err = true;
					goto __err;
				}
				pRTL8711_1_Com->x_tal = fre_current_Index;
			}
		}

		for(j=0;j<3;j++)
		{
			if((avpower[j] < pwr_min)||(avpower[j] > pwr_max))
			{
				param.Format("%d:%.2lfdBm",current_Index,avpower);
				result_err = true;
				goto __err;
			}
			else
			{
				if(tset->dev_id == TEST_DEV_ID_B)
				{
					if(channel[j] > 11)
					{
						pRTL8711_2_Com->tx_pwr_h = current_Index[j];
					}
					else if(channel[j] < 6)
					{
						pRTL8711_2_Com->tx_pwr_l = current_Index[j];
					}
					else
					{
						pRTL8711_2_Com->tx_pwr_m = current_Index[j];
					}
					if(write_rtl8711_power_Efuse(pRTL8711_2_Com,(uint8)channel[j], (uint8)index_difference, param) == false)	
					{
						show_message();
						result_err = true;
						goto __err;
					}
				}
				else
				{
					if(channel[j] > 11)
					{
						pRTL8711_1_Com->tx_pwr_h = current_Index[j];
					}
					else if(channel[j] < 6)
					{
						pRTL8711_1_Com->tx_pwr_l = current_Index[j];
					}
					else
					{
						pRTL8711_1_Com->tx_pwr_m = current_Index[j];
					}
					if(write_rtl8711_power_Efuse(pRTL8711_1_Com,(uint8)channel[j], (uint8)index_difference, param) == false)	
					{
						show_message();
						result_err = true;
						goto __err;
					}	
				}
			}
		}

		 param = "RF TEST OK";
	}

__err:
	//Release_N4010A_Instrument();

	// 1、when test ok 2、when test rf fail  3、when test recv expection  stop tx for test sense
	if(tset->dev_id == TEST_DEV_ID_B)
	{
		stop_rtl8711_tx(pRTL8711_2_Com,param);
	}
	else
	{	
		stop_rtl8711_tx(pRTL8711_1_Com,param);
	}
	
	if(result_err) 
		return (false);
	
	result = true;
    return(NO_ERR);

}

#endif

void RF_test_integration::record_header(ofstream &fs)
{
        fs << name << " RF INTEGRATION TEST" << ", "; 
}

void RF_test_integration::record_result(ofstream &fs)
{
#if 0
        CString temp;

        if(!res->valid) 
        {
			temp = param;
			format_record_par(temp);
			if(is_tested) temp += ",/F,/F,";
			else temp += ",NC,NC,";
        }
        else
        {
			temp.Format("%.2lf,%.2lf,%.2lf,",res->av,res->av_max,res->av_min);
        }
#endif
        fs << param <<",";
        if(result) fs<<"/P,";
        else if(!is_tested) fs<<"NC,";
        else fs<<"/F,";

}
void RF_test_integration::save_adjust_freqoffset(int index,double freqoffset)
{
	if(tset->dev_id == TEST_DEV_ID_B)
	{
		CTestItem::fre_index[1] = index;
		CTestItem::freoffset_para[1] = freqoffset;	
	}
	else
	{
		CTestItem::fre_index[0] = index;
		CTestItem::freoffset_para[0] = freqoffset;
	}
}


void RF_test_integration::save_adjust_power(int index,double avpower_para,int flag)
{
	if(flag == 0)
	{
		if(tset->dev_id == TEST_DEV_ID_B)
		{
			CTestItem::pwr_low_index[1] = index;
			CTestItem::low_avpower[1] = avpower_para;
		}
		else
		{
			CTestItem::pwr_low_index[0] = index;
			CTestItem::low_avpower[0] = avpower_para;
		}
	}else if(flag == 1)
	{
		if(tset->dev_id == TEST_DEV_ID_B)
		{
			CTestItem::pwr_middle_index[1] = index;
			CTestItem::middle_avpower[1] = avpower_para;
		}
		else
		{
			CTestItem::pwr_middle_index[0] = index;
			CTestItem::middle_avpower[0] = avpower_para;
		}
		
	}else if(flag == 2)
	{
		if(tset->dev_id == TEST_DEV_ID_B)
		{
			CTestItem::pwr_high_index[1] = index;
			CTestItem::high_avpower[1] = avpower_para;
		}
		else
		{
			CTestItem::pwr_high_index[0] = index;
			CTestItem::high_avpower[0] = avpower_para;
		}
		
	}
}

void RF_test_integration::save_read_evm(double evm_para,int flag)
{
	if(flag == 0)
	{
		if(tset->dev_id == TEST_DEV_ID_B)
			CTestItem::low_average_evm[1] = evm_para;
		else
			CTestItem::low_average_evm[0] = evm_para;
	}
	else if(flag == 1)
	{
		if(tset->dev_id == TEST_DEV_ID_B)
			CTestItem::middle_average_evm[1] = evm_para;
		else
			CTestItem::middle_average_evm[0] = evm_para;
	}
	else if(flag == 2)
	{
		if(tset->dev_id == TEST_DEV_ID_B)
			CTestItem::high_average_evm[1] = evm_para;
		else
			CTestItem::high_average_evm[0] = evm_para;
	}

}


//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
CTObj_N4010A_fre::CTObj_N4010A_fre(const char* pname,testSet *Tset,int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
		
	center_fre = 2412;//cfg.VDouble(ParN4010A_TxFre);// 2412MHz
	//fre_offset = 3000;//cfg.VDouble(ParN4010A_freqOffset);// 2000Hz
	freq_compensate = 0;//cfg.VDouble(ParN4010A_freqOffset);//10000Hz
	
	fre_offset = cfg.VDouble(ParN4010A_freqOffset);// 2000Hz
	
	err_min = ERR_MT8852_DATA;
	err_max = ERR_MT8852_DATA;

	par_is_gloden = false;

}


void CTObj_N4010A_fre::gen_prompt(void)
{
	char itemstr[8];
       itoa(Index(),itemstr,10);

       CDutGlodenPar *par = pMainWnd->dutGloden[DUT_PAR_IDX];
        
       //neg_max = par->rf_max_neg_foffset;
	//pos_max = par->rf_max_pos_foffset;

       CString range;
       //range.Format("offset:%.2lfK~%.2lfK", neg_max, pos_max);	
	range.Format("%.lfMHz +/- %.lfHz",center_fre,fre_offset);
       tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}


void CTObj_N4010A_fre::update_range(void)
{
        ConfigFile &cfg = pCfgf;
        CDutGlodenPar *pgloden = pMainWnd->dutGloden[DUT_PAR_IDX];
        bool update = false;
#if 0
        int gloden = tset->gloden_idx();
        if(gloden)
        { 
                par_is_gloden = true;
                update = true;
                pgloden = pMainWnd->dutGloden[gloden];
        }
        else if(par_is_gloden)
        { 
                par_is_gloden = false;
                update = true;   
        }

		neg_max = pgloden->rf_max_neg_foffset;
	    pos_max = pgloden->rf_max_pos_foffset;

        if(update)
        {
                item_range.Format("offset:%.2lfK~%.2lfK", neg_max, pos_max);
                TSET_MSG(MSG_SHOW_RANGE,tset);
        }
#endif
}

int CTObj_N4010A_fre::ItemTestReady(ConfigFile &cfg)
{   
	//mutex_hodle1 = false;			//fu change it
	if (!pMainWnd->glbResource_get(GRESOURCE_MCU1))
	{
		param = "u18 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_hodle1 = true;

	//mutex_hodle2 = false;
	
	if(!pMainWnd->glbResource_get(GRESOURCE_BT_TSET))
	{
		param = "get gsource err";
		return(ERR_STOP);
	}
	mutex_hodle2 = true;
	
	return(NO_ERR);
}

int CTObj_N4010A_fre::ItemTestClean(ConfigFile &cfg)
{
	
	if(mutex_hodle2)
	{ 
		pMainWnd->glbResource_put(GRESOURCE_BT_TSET);
		mutex_hodle2 = false;
	}
	
	if (mutex_hodle1)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU1);
        mutex_hodle1 = false;
	}
	   
	return(NO_ERR);
}

//yhl 2018/10/23
int CTObj_N4010A_fre::TestOp(ConfigFile &cfg)
{
//	struct tSetResult *tresult = tset->mt8852_res;
//	CBsDevice *pobj = tset->pSpiObj;
    result = false;
	bool result_err = false;
    is_tested = true;
	uint8 temp = 0;
	//int upperlimi_Index = cfg.Vint(ParFreqMaxIndex);
	//int lowerlimit_Index = cfg.Vint(ParFreqMinIndex);
	//uint8 upperlimi_Index = 63;
	//uint8 lowerlimit_Index = 0;
	//CString m_szN4010Address = cfg.VCString(ParN4010A_Port);
	uint8 current_Index;		//cfg.Vint(ParFreqIndex);
	double	freqoffsetlimit = fre_offset;
	double freqoffset = 0;
	double avpower = 0;
	CString err_str = "null ESP8266";
	//double instrument_compensate=0;
	//CString cmd_str;	
	//CString answer_str;
	//char rx_buf[128] = {0};
	//BOOL a,b;
	//int ret;
	//int i=0;
	//double	PowerRange = 0;
	//double	TriggerLevel = 0;
	//long	MaxSymbolsUsed = 0;
	//double	MaxPacketLength = 0;
	if(tset->dev_id == TEST_DEV_ID_B)
	{
		//set_rtl8711_channel(pRTL8711_2_Com,1,err_str);
		current_Index = fre_index[1];
		freqoffset = freoffset_para[1];
		
	}
	else
	{
		//set_rtl8711_channel(pRTL8711_1_Com,1,err_str);
		current_Index = fre_index[0];
		freqoffset = freoffset_para[0];
		
	}
	//current_Index = fre_index;
	//freqoffset = freoffset_para;
	if(fabs(freqoffset)>freqoffsetlimit)
	{
			param.Format("%d:%.lfHz",current_Index,freqoffset);
			show_message();
			//pMainWnd->SendMessage(WM_FLASH_CUR_ITEM_NO_RESUL,(WPARAM)NULL,(LPARAM)this);

			//for record resuit so cloae this
			result_err = true;
			goto __err;
	}
	else
	{
		param.Format("%d:%.lfHz",current_Index,freqoffset);
		show_message();
	}
#if 0
	if(!pBtTestSet)
	{
		param = "null device";
		return(ERR_NOMAL);
	}
	
	if(!pMCU1_Com)
	{
		param = "null com";
		return(ERR_NOMAL);
	}
/*	
	if(!pInst_N4010A)
	{
		param = "null N4010A";
		return(ERR_NOMAL);
	}
*/	
       bt_TSET *tSet= pBtTestSet;
	if(!tSet || !tSet->dev_is_ready())
	{
		param = "N4010A err";
		return(ERR_NOMAL);
	}
	
	//path route to N4010A
	if(!pMCU1_Com->set_rout_4010_on(0, MCU_PIO_DUT_4010_METER, MCU_PIO_NUM_DUT_4010_METER))
	{
		param ="switch route to 4010 err";
        return (ERR_NOMAL);
	}
	//dut1 route to N4010A
	if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pRTL8711_1_Com )
		{
		   param = "null RTL87xx1 COM";
		   return (false);
		}
		if(!pMCU1_Com->set_4010_route_dut1(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut1 err";
			return (ERR_NOMAL);
		}	
	}
	else
	{
		if(!pRTL8711_2_Com)
		{
		   param = "null RTL87xx2 COM";
		   return (false);
		}
		
		if(!pMCU1_Com->set_4010_route_dut2(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut2 err";
			return (ERR_NOMAL);
		}
	}
	Sleep(200);

/*
	//初始化N4010A接口	
	if(Inst.state == false)
	{
		if(Create_N4010A_Instrument(m_szN4010Address,param)==false)
		{
			return(false);
		}
	}
*/
#if 0
		Release_N4010A_Instrument();
	
#else	
		//Inst.SetChannel(2,channel);//IEEE802_11g=2//enum RadioFormatEnum Format
		//center_fre = 2.412e9;
		Inst.SetFrequency(2.412e9);
#endif

	
	
	if(tset->dev_id == TEST_DEV_ID_B)
	{
		init_rtl8711_tx_test(pRTL8711_2_Com,TEST_DEV_ID_B,param);
		//set_rtl8711_xtal_cap(pRTL8711_2_Com,current_Index,param);
		start_rtl8711_tx_power(pRTL8711_2_Com,42,param);
	}
	else
	{		
		init_rtl8711_tx_test(pRTL8711_1_Com,TEST_DEV_ID_A,param);		
		//set_rtl8711_xtal_cap(pRTL8711_1_Com,current_Index,param);
		start_rtl8711_tx_power(pRTL8711_1_Com,42,param);
	}
	
	Sleep(100);
	//instrument compensate
	instrument_compensate = cfg.VDouble(Par_instrument_compensate);
	if(((N4010A_DEV*)pBtTestSet)->setPathOffset(instrument_compensate)==false)	
	{
		param = "write rf path loss err";
		return(false);
	}
	
#if 1
	Inst.IEEE80211a_Autorange_Run();
	PowerRange = Inst.GetIEEE80211a_Autorange_PowerRange();
	TriggerLevel = Inst.GetIEEE80211a_Autorange_TriggerLevel();
	MaxSymbolsUsed = Inst.GetIEEE80211a_Autorange_MaxSymbolsUsed();
	//MaxPacketLength = Inst.GetIEEE80211a_Autorange_MaxPacketLength();
	
	Inst.SetPowerRange(PowerRange);
	//Inst.SetMaxPacketLength(MaxPacketLength);
	Inst.SetMaxSymbolsUsed(MaxSymbolsUsed);//802.11a/g
	Inst.SetTriggerStyle(2);
	Inst.SetTriggerLevel(TriggerLevel);
	Inst.SetTriggerDelay(0);
	Inst.SetTriggerHoldoff(1e-6);	
#else	
	Inst.SetPowerRange(20.0);
	Inst.SetMaxSymbolsUsed(60);//802.11a/g
	Inst.SetTriggerStyle(2);
	Inst.SetTriggerLevel(0.1);
	Inst.SetTriggerDelay(0);
	Inst.SetTriggerHoldoff(1e-6);
#endif
	Inst.SetIEEE80211a_Demod_MeasureGatedPower(true);
	
	
	

	//判断是否是金机
	if((tset->is_golden == false) && (tset->r_addr.is_valid == false))
	{		
		if(tset->dev_id == TEST_DEV_ID_B)
		{				   
			tset->is_golden = read_rtl8711_golden_flag(pRTL8711_2_Com,current_Index,param);

		}	
		else
		{
			tset->is_golden = read_rtl8711_golden_flag(pRTL8711_1_Com,current_Index,param);			   				   
		}
									
		if(tset->is_golden)
		{
			tset->r_addr.is_valid = true;
			param = "";
			if(tset->dev_id == TEST_DEV_ID_B) param.Format("rtl87xx2 is golden");
			else param.Format("rtl87xx1 is golden");
		}	
	}

	if(tset->is_golden)
	{
		if(tset->dev_id == TEST_DEV_ID_B)
		{
			read_rtl8711_xtal_cap(pRTL8711_2_Com,pRTL8711_2_Com->x_tal,param);
			set_rtl8711_xtal_cap(pRTL8711_2_Com,pRTL8711_2_Com->x_tal,param);
			//current_Index = pRTL8711_2_Com->x_tal;
		}
		else
		{   			
			read_rtl8711_xtal_cap(pRTL8711_1_Com,pRTL8711_1_Com->x_tal,param);
			set_rtl8711_xtal_cap(pRTL8711_1_Com,pRTL8711_1_Com->x_tal,param);
			//current_Index = pRTL8711_2_Com->x_tal;
		}
		Sleep(100);
		Inst.IEEE80211a_Demod_Initiate();
		freqoffset = Inst.GetIEEE80211a_Demod_FrequencyError();
		if(fabs(freqoffset)>100000)
		{
			param = "read freq err";
			result_err = true;
			goto __err;
		}

		param.Format("golden:%d:%.lfHz",current_Index,freqoffset);

		if(tset->dev_id == TEST_DEV_ID_B)
		{
			stop_rtl8711_tx(pRTL8711_2_Com,param);
		}
		else
		{   
			stop_rtl8711_tx(pRTL8711_1_Com,param);
		}		
	}
	else		
	{	
		
		do			
		{	
			if(tset->dev_id == TEST_DEV_ID_B)
			{
				set_rtl8711_xtal_cap(pRTL8711_2_Com,current_Index,param);
				//start_rtl8711_tx_power(pRTL8711_2_Com,42,param);
			}
			else
			{   
				set_rtl8711_xtal_cap(pRTL8711_1_Com,current_Index,param);
				//start_rtl8711_tx_power(pRTL8711_1_Com,42,param);
			}
			//Sleep(1000);		
			Sleep(100);//这里一定要有延时
			//Sleep(100);
			for(i=0;i<10;i++)
			{
				//Hz//GetIEEE80211a_Demod_FrequencyError();//Transmitter "Demod" Test (DSSS/OFDM)
				// Set up the instrument parameters
/*				
				Inst.SetFrequency(2.412e9);
				Inst.SetPowerRange(20.0);
				Inst.SetMaxSymbolsUsed(60);//802.11a/g
				Inst.SetTriggerDelay(0);
				Inst.SetTriggerHoldoff(1e-6);
				Inst.SetTriggerLevel(0.1);
				Inst.SetTriggerStyle(2);
				//Inst.SetIEEE80211a_Demod_MeasureGatedPower(true);
				//Sleep(200);
*/				
				Inst.IEEE80211a_Demod_Initiate();
//*/				
				//Sleep(100);
				freqoffset = Inst.GetIEEE80211a_Demod_FrequencyError();
				//avpower = Inst.GetIEEE80211a_Demod_GatedPower();
				//param.Format("%d:%.lfHz",current_Index,freqoffset);
				//show_message();
				
				if(fabs(freqoffset)<100000)
					break;
				//Sleep(500);
			}		
			param.Format("%d:%.lfHz",current_Index,freqoffset);
			show_message();
///*			
			if(fabs(freqoffset)>100000)
			{
				param = "read freq err";
				result_err = true;
				goto __err;
			}
//*/		
/*
			freqoffset += freq_compensate;
*/
			if((freqoffset>0) && (freqoffset>freqoffsetlimit))
			{
				//大概 4KHz对应一个等级
				if(freqoffset > 10000)
					temp = (uint8)(freqoffset/3000);
				else
					temp = (uint8)(freqoffset/4000);
				if(temp > 0)
					current_Index -= temp;
				else
					current_Index --;
			}
			else if((freqoffset<0) && (fabs(freqoffset)>freqoffsetlimit))
			{
				if(fabs(freqoffset) > 10000)
					temp = (uint8)(fabs(freqoffset)/3000);
				else
					temp = (uint8)(fabs(freqoffset)/4000);
				if(temp > 0)
					current_Index += temp;
				else
					current_Index ++;
			}
			else
			{
				break;
			}		
		}while(fabs(freqoffset)>freqoffsetlimit);//while((upperlimi_Index - lowerlimit_Index) > 1);//


		if(tset->dev_id == TEST_DEV_ID_B)
		{			
			stop_rtl8711_tx(pRTL8711_2_Com,param);
		}
		else
		{ 			 
			stop_rtl8711_tx(pRTL8711_1_Com,param);
		}		
		
		if(fabs(freqoffset)>freqoffsetlimit)
		{
			param.Format("%d:%.lfHz",current_Index,freqoffset);
			//pMainWnd->SendMessage(WM_FLASH_CUR_ITEM_NO_RESUL,(WPARAM)NULL,(LPARAM)this);
			result_err = true;
			goto __err;
		}
		else
		{		
			if(tset->dev_id == TEST_DEV_ID_B)
			{
				if(!write_rtl8711_xtal_cap(pRTL8711_2_Com,current_Index,param))
				{
				    param = "rtl87xx2 set err";
					result_err = true;
					goto __err;
				}
				pRTL8711_2_Com->x_tal = current_Index;
			}	
			else
			{
				if(!write_rtl8711_xtal_cap(pRTL8711_1_Com,current_Index,param))
				{
				    param = "rtl87xx1 set err";
					result_err = true;
					goto __err;
				}
				pRTL8711_1_Com->x_tal = current_Index;
			}
		}
	}

#endif

__err:
	//Release_N4010A_Instrument();

	if(result_err) 
		return (false);
	
	result = true;
    return(NO_ERR);
}


void CTObj_N4010A_fre::record_header(ofstream &fs)
{
        fs<<name<<" Average Offset(Hz)"<<", ";
}

void CTObj_N4010A_fre::record_result(ofstream &fs)
{
#if 0
        CString temp;

        if(!res->valid) 
        {
                temp = param;
                format_record_par(temp);
                if(is_tested) temp += ",/F,/F,";
                else temp += ",NC,NC,";
        }
        else
        {
                temp.Format("%.2lf,%.2lf,%.2lf,",res->av,res->pos_max,res->neg_max);
        }
#endif
        fs << param <<",";
        if(result) fs<<"/P,";
        else if(!is_tested) fs<<"NC,";
        else fs<<"/F,";
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//yhl_pwr
CTObj_N4010A_pwr::CTObj_N4010A_pwr(const char* pname,int ch,testSet *Tset,bool adjustpowerflag,int ID,ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
		int dw20_difference_dw40;
		int ofdm_difference_dw40;

		err_min = ERR_MT8852_DATA;
        err_max = ERR_MT8852_DATA;
		
		//input_power = cfg.VDouble(ParN4010A_Inputpower);
		adjustflag = adjustpowerflag;
	
#if 0
        if(ch == 2)
		{
			if(tset->dev_id == TEST_DEV_ID_A)
				path_loss = 5.5;//cfg.VDouble(s_DevA_MCH_pathLoss);
			else
				path_loss = 5.5;//cfg.VDouble(s_DevB_MCH_pathLoss);
			channel = 7;//cfg.Vint(ParN4010A_PWMTxCh);
		}
        else if(ch == 3)
		{	
			if(tset->dev_id == TEST_DEV_ID_A)
			path_loss = 5;//cfg.VDouble(s_DevA_HCH_pathLoss);
		    else
			path_loss = 5;//cfg.VDouble(s_DevB_HCH_pathLoss);
			channel =13;// cfg.Vint(ParN4010A_PWHTxCh);
		}
        else
		{	
			if(tset->dev_id == TEST_DEV_ID_A)
			path_loss = 7.5;//cfg.VDouble(s_DevA_LCH_pathLoss);
		    else
			path_loss = 7.5;//cfg.VDouble(s_DevB_LCH_pathLoss);
			channel = 1;//cfg.Vint(ParN4010A_PWLTxCh);
		}

		index_difference = 2;//cfg.Vint(ParN4010A_MinPW);

		pwr_min = 12.5;//cfg.Vint(ParN4010A_MinPW);
        pwr_max = 13.5;//cfg.Vint(ParN4010A_MaxPW);
#else        
        if(ch == 2)
		{
			if(tset->dev_id == TEST_DEV_ID_A)
				path_loss = cfg.VDouble(s_DevA_MCH_pathLoss);
			else
				path_loss = cfg.VDouble(s_DevB_MCH_pathLoss);
			channel = 7;//cfg.Vint(ParN4010A_PWMTxCh);
		}
        else if(ch == 3)
		{	
			if(tset->dev_id == TEST_DEV_ID_A)
			path_loss = cfg.VDouble(s_DevA_HCH_pathLoss);
		    else
			path_loss = cfg.VDouble(s_DevB_HCH_pathLoss);
			channel =13;// cfg.Vint(ParN4010A_PWHTxCh);
		}
        else
		{	
			if(tset->dev_id == TEST_DEV_ID_A)
			path_loss = cfg.VDouble(s_DevA_LCH_pathLoss);
		    else
			path_loss = cfg.VDouble(s_DevB_LCH_pathLoss);
			channel = 1;//cfg.Vint(ParN4010A_PWLTxCh);
		}

		dw20_difference_dw40 = cfg.Vint(Par_bw20_bw40_df);
		ofdm_difference_dw40 = cfg.Vint(Par_ofdm_bw40_df);		
		index_difference = (dw20_difference_dw40&0x0f)<<4;
		index_difference |= (ofdm_difference_dw40&0x0f);

		pwr_min = cfg.VDouble(ParN4010A_MinPW);
        pwr_max = cfg.VDouble(ParN4010A_MaxPW);
#endif        
        par_is_gloden = false;
//        terminate_test_if_err_accur = false;
}

void CTObj_N4010A_pwr::gen_prompt(void)
{

		char itemstr[8];
        itoa(Index(),itemstr,10);
		
        CString range;
        //range.Format("Pwr[%.2lf,%.2lf] Pk:%.1lf,Los:%.2fdB", pwr_min, pwr_max, pwr_pk, path_loss);
        range.Format("Pwr[%.2lf,%.2lf]", pwr_min, pwr_max);
		tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");

}


void CTObj_N4010A_pwr::update_range(void)
{
        ConfigFile &cfg = pCfgf;
        CDutGlodenPar *pgloden = pMainWnd->dutGloden[DUT_PAR_IDX];
        bool update = false;

}

int CTObj_N4010A_pwr::ItemTestReady(ConfigFile &cfg)
{   
	//mutex_hodle1 = false;
	if (!pMainWnd->glbResource_get(GRESOURCE_MCU1))
	{
		param = "u18 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_hodle1 = true;

	//mutex_hodle2 = false;
	
	if(!pMainWnd->glbResource_get(GRESOURCE_BT_TSET))
	{
		param = "get gsource err";
		return(ERR_STOP);
	}
	mutex_hodle2 = true; 
	
	return(NO_ERR);
}

int CTObj_N4010A_pwr::ItemTestClean(ConfigFile &cfg)
{
	
	if(mutex_hodle2)
	{ 
		pMainWnd->glbResource_put(GRESOURCE_BT_TSET);
		mutex_hodle2 = false;
	}
	
	if (mutex_hodle1)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU1);
        mutex_hodle1 = false;
	}
	   
	return(NO_ERR);
}

int CTObj_N4010A_pwr::TestOp(ConfigFile &cfg)
{
    result = false;
	bool result_err = false;
	is_tested = true;
	//int upperlimit_Index = cfg.Vint(ParN4010A_MaxPWLEL);
	//int lowerlimit_Index = cfg.Vint(ParN4010A_MinPWLEL);
	uint8 current_Index;
	double avpower;
	int i;
	//uint8 temp,fre_offset_Index;
	//CString m_szN4010Address = cfg.VCString(ParN4010A_Port);
	//double center_fre = 2.412e9;
	//int i=0,get_range=0;
	//double	PowerRange1 = 0;
	//double	PowerRange2 = 0;
	//double	TriggerLevel = 0;
	//long	MaxSymbolsUsed = 0;
	//double	MaxPacketLength = 0;
	if(channel == 1)
	{
		if(tset->dev_id == TEST_DEV_ID_B)
		{
			current_Index = pwr_low_index[1];
			avpower = low_avpower[1];
		}
		else
		{
			current_Index = pwr_low_index[0];
			avpower = low_avpower[0];
		}
	}
	else if(channel == 7)
	{
		if(tset->dev_id == TEST_DEV_ID_B)
		{
			current_Index = pwr_middle_index[1];
			avpower =	middle_avpower[1];
		}
		else
		{
			current_Index = pwr_middle_index[0];
			avpower = middle_avpower[0];
		}
	}
	else
	{
		if(tset->dev_id == TEST_DEV_ID_B)
		{
			current_Index = pwr_high_index[1];
			avpower = high_avpower[1];
		}
		else
		{
			current_Index = pwr_high_index[0];
			avpower = high_avpower[0];
		}
	}

	if((avpower < pwr_min)||(avpower > pwr_max))
	{
		param.Format("%d:%.2lfdBm",current_Index,avpower);
		show_message();
	
		//for record resuit so cloae this
//#ifndef ONLY_READ
		result_err = true;
		goto __err;
//#endif
	}
	else
	{
		param.Format("%d:%.2lfdBm",current_Index,avpower);
		show_message();
	}
__err:
	//Release_N4010A_Instrument();

	if(result_err) 
		return (false);
	//OutputLog("Log.txt", "CTObj_N4010A_pwr.TestOp_9");

	result = true;
    return(NO_ERR);
}


void CTObj_N4010A_pwr::record_header(ofstream &fs)
{
        fs << name << " Average Power(dBm),Max Power(dBm),Min Power(dBm)" << ", "; 
}

void CTObj_N4010A_pwr::record_result(ofstream &fs)
{
#if 0
        CString temp;

        if(!res->valid) 
        {
			temp = param;
			format_record_par(temp);
			if(is_tested) temp += ",/F,/F,";
			else temp += ",NC,NC,";
        }
        else
        {
			temp.Format("%.2lf,%.2lf,%.2lf,",res->av,res->av_max,res->av_min);
        }
#endif
        fs << param <<",";
        if(result) fs<<"/P,";
        else if(!is_tested) fs<<"NC,";
        else fs<<"/F,";

}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_N4010A_EVM::CTObj_N4010A_EVM(const char* pname,int ch,testSet *Tset,int ID,ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
		err_min = ERR_MT8852_DATA;
        err_max = ERR_MT8852_DATA;
		
		//input_power = cfg.VDouble(ParN4010A_Inputpower);
		//adjustflag = adjustpowerflag;
#if 0		
        if(ch == 2)
		{
			if(tset->dev_id == TEST_DEV_ID_A)
				path_loss = -3;//cfg.VDouble(s_DevA_MCH_pathLoss);
			else
				path_loss = -3;//cfg.VDouble(s_DevB_MCH_pathLoss);
			channel = 7;//cfg.Vint(ParN4010A_PWMTxCh);
		}
        else if(ch == 3)
		{	
			if(tset->dev_id == TEST_DEV_ID_A)
			path_loss = -4;//cfg.VDouble(s_DevA_HCH_pathLoss);
		    else
			path_loss = -4;//cfg.VDouble(s_DevB_HCH_pathLoss);
			channel =13;// cfg.Vint(ParN4010A_PWHTxCh);
		}
        else
		{	
			if(tset->dev_id == TEST_DEV_ID_A)
			path_loss = -3;//cfg.VDouble(s_DevA_LCH_pathLoss);
		    else
			path_loss = -3;//cfg.VDouble(s_DevB_LCH_pathLoss);
			channel = 1;//cfg.Vint(ParN4010A_PWLTxCh);
		}
        evm_max = -32;//cfg.Vint(ParN4010A_MinPW);
 #else       
        if(ch == 2)
		{
			if(tset->dev_id == TEST_DEV_ID_A)
				path_loss = cfg.VDouble(Par_a_evm_lossm);
			else
				path_loss = cfg.VDouble(Par_b_evm_lossm);
			channel = 7;//cfg.Vint(ParN4010A_PWMTxCh);
		}
        else if(ch == 3)
		{	
			if(tset->dev_id == TEST_DEV_ID_A)
			path_loss = cfg.VDouble(Par_a_evm_lossh);
		    else
			path_loss = cfg.VDouble(Par_b_evm_lossh);
			channel =13;// cfg.Vint(ParN4010A_PWHTxCh);
		}
        else
		{	
			if(tset->dev_id == TEST_DEV_ID_A)
			path_loss = cfg.VDouble(Par_a_evm_loss);
		    else
			path_loss = cfg.VDouble(Par_b_evm_loss);
			channel = 1;//cfg.Vint(ParN4010A_PWLTxCh);
		}
        evm_max = cfg.Vint(Par_evm_max);
 #endif       
        //par_is_gloden = false;
}

void CTObj_N4010A_EVM::gen_prompt(void)
{

		char itemstr[8];
        itoa(Index(),itemstr,10);

		
        CString range;
        //range.Format("Pwr[%.2lf,%.2lf] Pk:%.1lf,Los:%.2fdB", pwr_min, pwr_max, pwr_pk, path_loss);
        range.Format("EVM[ < %.2lfdBm]", evm_max);
		tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");

}


void CTObj_N4010A_EVM::update_range(void)
{
        ConfigFile &cfg = pCfgf;
        CDutGlodenPar *pgloden = pMainWnd->dutGloden[DUT_PAR_IDX];
        bool update = false;
}

int CTObj_N4010A_EVM::ItemTestReady(ConfigFile &cfg)
{   
	//mutex_hodle1 = false;
	if (!pMainWnd->glbResource_get(GRESOURCE_MCU1))
	{
		param = "u18 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_hodle1 = true;

	//mutex_hodle2 = false;
	
	if(!pMainWnd->glbResource_get(GRESOURCE_BT_TSET))
	{
		param = "get gsource err";
		return(ERR_STOP);
	}
	mutex_hodle2 = true; 
	
	return(NO_ERR);
}

int CTObj_N4010A_EVM::ItemTestClean(ConfigFile &cfg)
{
	
	if(mutex_hodle2)
	{ 
		pMainWnd->glbResource_put(GRESOURCE_BT_TSET);
		mutex_hodle2 = false;
	}
	
	if (mutex_hodle1)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU1);
        mutex_hodle1 = false;
	}
	   
	return(NO_ERR);
}

#define EVM_TEST_TIMES		 10
//yhl_evm
int CTObj_N4010A_EVM::TestOp(ConfigFile &cfg)
{
    //struct tSetResult *tresult = tset->mt8852_res;
    //CBsDevice *pobj = tset->pSpiObj;
    result = false;
	bool result_err = false;
	//is_tested = true;

	//double evm_sum = 0;
	double average_evm = 0;
	//double current_evm = 0;
	//double current_evm_dBm = 0;
	//CString m_szN4010Address = cfg.VCString(ParN4010A_Port);
	//double current_power = 0;
	//double center_fre = 2.412e9;
	//int i=0;
	//double	PowerRange = 0;
	//double	TriggerLevel = 0;
	//long	MaxSymbolsUsed = 0;
	//double	MaxPacketLength = 0;


	
	if(channel == 1)
	{
		//current_Index = pwr_low_index;
		if(tset->dev_id == TEST_DEV_ID_B)
			average_evm = low_average_evm[1];
		else
			average_evm = low_average_evm[0];
	}
	else if(channel == 7)
	{
		//current_Index = pwr_middle_index;
		if(tset->dev_id == TEST_DEV_ID_B)
			average_evm = middle_average_evm[1];
		else
			average_evm = middle_average_evm[0];
		//average_evm = middle_average_evm;
	#if 0
		if(tset->dev_id == TEST_DEV_ID_A)
		{
			start_esp8266_tx(pRTL8711_1_Com,14,param);
		}
		else
		{
			start_esp8266_tx(pRTL8711_2_Com,14,param);
		}
	#endif
	}
	else
	{
		//current_Index = pwr_high_index;
		if(tset->dev_id == TEST_DEV_ID_B)
			average_evm = high_average_evm[1];
		else
			average_evm = high_average_evm[0];
		//average_evm = high_average_evm;
	}

	if(average_evm > evm_max)
	{
		param.Format("aversge EVM:%.2lfdbm",average_evm);
		show_message();
		
		//for record resuit so cloae this
//#ifndef ONLY_READ
				result_err = true;
				goto __err;
//#endif

	}
	else
	{
		param.Format("aversge EVM:%.2lfdbm",average_evm);
		show_message();
	}
		
	
#if 0
	if(!pBtTestSet)
	{
		param = "null device";
		return(ERR_NOMAL);
	}
	
	if(!pMCU1_Com)
	{
		param = "null com";
		return(ERR_NOMAL);
	}
	
	bt_TSET *tSet= pBtTestSet;
	if(!tSet || !tSet->dev_is_ready())
	{
		param = "N4010A err";
		return(ERR_NOMAL);
	}
	
	//------------------------------------------------------------------------------------------------		
//	mutex_hodle1 = false;
//	if (!pMainWnd->glbResource_get(GRESOURCE_MCU1))
//	{
//		param = "u18 gsource err";
//		return(ERR_TERMINAL);
//	}
//	mutex_hodle1 = true;
	//------------------------------------------------------------------------------------------------
	
	//path route to N4010A
	if(!pMCU1_Com->set_rout_4010_on(0, MCU_PIO_DUT_4010_METER, MCU_PIO_NUM_DUT_4010_METER))
	{
		param ="switch route to 4010 err";
		return (ERR_NOMAL);
	}
	//dut1 route to N4010A
	if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pMCU1_Com->set_4010_route_dut1(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut1 err";
			return (ERR_NOMAL);
		}	
	}
	else
	{
		if(!pMCU1_Com->set_4010_route_dut2(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut2 err";
			return (ERR_NOMAL);
		}
	}
	Sleep(200);
	//OutputLog("Log.txt", "CTObj_N4010A_EVM.TestOp_1");
	//----------------------------------------------------------------------------------		
/*
	//初始化N4010A接口	
	if(Inst.state == false)
	{
		if(Create_N4010A_Instrument(m_szN4010Address,param)==false)
		{
			return(false);
		}
	}
*/
	//Release_N4010A_Instrument();

//	center_fre = 2.412e9 + (channel-1)*0.005e9;	
	//Inst.SetChannel(2,channel);//IEEE802_11g=2//enum RadioFormatEnum Format
//	Inst.SetFrequency(center_fre);

	


	//OutputLog("Log.txt", "CTObj_N4010A_EVM.TestOp_2");
	if(tset->dev_id == TEST_DEV_ID_B)
	{		
		init_rtl8711_tx_test(pRTL8711_2_Com,TEST_DEV_ID_B,param);
		//read_rtl8711_xtal_cap(pRTL8711_2_Com,pRTL8711_2_Com->x_tal,param);
		set_rtl8711_xtal_cap(pRTL8711_2_Com,pRTL8711_2_Com->x_tal,param);
		set_rtl8711_channel(pRTL8711_2_Com,(uint8)channel,param);
		//start_rtl8711_tx_power(pRTL8711_2_Com,current_Index,param);
	}
	else
	{
		init_rtl8711_tx_test(pRTL8711_1_Com,TEST_DEV_ID_A,param);
		//read_rtl8711_xtal_cap(pRTL8711_1_Com,pRTL8711_1_Com->x_tal,param);
		set_rtl8711_xtal_cap(pRTL8711_1_Com,pRTL8711_1_Com->x_tal,param);
		set_rtl8711_channel(pRTL8711_1_Com,(uint8)channel,param);
		//start_rtl8711_tx_power(pRTL8711_1_Com,current_Index,param);
	}
	//OutputLog("Log.txt", "CTObj_N4010A_EVM.TestOp_3");

	if(tset->dev_id == TEST_DEV_ID_B)
	{
		if(read_rtl8711_power_Efuse(pRTL8711_2_Com,(uint8)channel,param) == false)
		{
			result_err = true;
			goto __err;
		}
		if(channel > 11)
		{
			current_power = pRTL8711_2_Com->tx_pwr_h;
		}
		else if(channel < 6)
		{
			current_power = pRTL8711_2_Com->tx_pwr_l;
		}
		else
		{
			current_power = pRTL8711_2_Com->tx_pwr_m;
		}

		start_rtl8711_tx_power(pRTL8711_2_Com,current_power,param);
	}
	else
	{ 
		if(read_rtl8711_power_Efuse(pRTL8711_1_Com,(uint8)channel,param) == false)
		{
			result_err = true;
			goto __err;
		}
		if(channel > 11)
		{
			current_power = pRTL8711_1_Com->tx_pwr_h;
		}
		else if(channel < 6)
		{
			current_power = pRTL8711_1_Com->tx_pwr_l;
		}
		else
		{
			current_power = pRTL8711_1_Com->tx_pwr_m;
		}
		start_rtl8711_tx_power(pRTL8711_1_Com,current_power,param);
	}
	//Sleep(1000);
	Sleep(100);
	//Sleep(200);
	center_fre = 2.412e9 + (channel-1)*0.005e9;	
	//Inst.SetChannel(2,channel);//IEEE802_11g=2//enum RadioFormatEnum Format
	Inst.SetFrequency(center_fre);
/*	
	if(((N4010A_DEV*)pBtTestSet)->setPathOffset(path_loss)==false)	
	{
		param = "write rf path loss err";
		return(false);
	}
*/	
#if 1
	Inst.IEEE80211a_Autorange_Run();
	PowerRange = Inst.GetIEEE80211a_Autorange_PowerRange();
	TriggerLevel = Inst.GetIEEE80211a_Autorange_TriggerLevel();
	MaxSymbolsUsed = Inst.GetIEEE80211a_Autorange_MaxSymbolsUsed();
	//MaxPacketLength = Inst.GetIEEE80211a_Autorange_MaxPacketLength();
	
	Inst.SetPowerRange(PowerRange);
	//Inst.SetMaxPacketLength(MaxPacketLength);
	Inst.SetMaxSymbolsUsed(MaxSymbolsUsed);//802.11a/g
	Inst.SetTriggerStyle(2);
	Inst.SetTriggerLevel(TriggerLevel);
	Inst.SetTriggerDelay(0);
	Inst.SetTriggerHoldoff(1e-6);			
	Inst.SetIEEE80211a_Demod_MeasureGatedPower(true);
#else
	Inst.SetPowerRange(20.0);
	//Inst.SetMaxPacketLength(970e-6);
	Inst.SetMaxSymbolsUsed(60);//802.11a/g
	Inst.SetTriggerStyle(2);
	Inst.SetTriggerLevel(0.1);
	Inst.SetTriggerDelay(0);
	Inst.SetTriggerHoldoff(1e-6);	
	Inst.SetIEEE80211a_Demod_MeasureGatedPower(true);
#endif
#if 1
	for(i=0;i<EVM_TEST_TIMES;i++)
	{
		Inst.IEEE80211a_Demod_Initiate();
		//Sleep(100);			
		current_evm = Inst.GetIEEE80211a_Demod_RmsEVM();
		//EVM(dB)=20*log10（evm(%)）
		current_evm_dBm = 20*log10(current_evm/100);		
		evm_sum  += current_evm_dBm;
		
		//param.Format("%.2lf%%:%.2lfdbm",current_evm,current_evm_dBm);
		//show_message();
	}
	average_evm = evm_sum/EVM_TEST_TIMES;
#else	
	Inst.IEEE80211a_EVM_Initiate();
	average_evm = Inst.GetIEEE80211a_EVM_Result();//(dB)
#endif	
	average_evm += path_loss;
	param.Format("aversge EVM:%.2lfdbm",average_evm);
	show_message();
	//OutputLog("Log.txt", "CTObj_N4010A_EVM.TestOp_6");

///*
	if(tset->dev_id == TEST_DEV_ID_B)
	{
		stop_rtl8711_tx(pRTL8711_2_Com,param);
	}
	else
	{	
		stop_rtl8711_tx(pRTL8711_1_Com,param);
	}
//*/
	//OutputLog("Log.txt", "CTObj_N4010A_EVM.TestOp_7");
	if(average_evm > evm_max)
	{
		result_err = true;
		goto __err;
	}

#endif

__err:
	//Release_N4010A_Instrument();

	if(result_err) 
		return (false);
	//OutputLog("Log.txt", "CTObj_N4010A_EVM.TestOp_8");

	result = true;
    return(NO_ERR);
}


void CTObj_N4010A_EVM::record_header(ofstream &fs)
{
        fs << name << " Average Power(dBm),Max Power(dBm),Min Power(dBm)" << ", "; 
}

void CTObj_N4010A_EVM::record_result(ofstream &fs)
{
#if 0
        CString temp;

        if(!res->valid) 
        {
			temp = param;
			format_record_par(temp);
			if(is_tested) temp += ",/F,/F,";
			else temp += ",NC,NC,";
        }
        else
        {
			temp.Format("%.2lf,%.2lf,%.2lf,",res->av,res->av_max,res->av_min);
        }
#endif
        fs << param <<",";
        if(result) fs<<"/P,";
        else if(!is_tested) fs<<"NC,";
        else fs<<"/F,";

}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_N4010A_sens::CTObj_N4010A_sens(const char* pname,int ch,testSet *Tset, int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{

		//packtesnum = cfg.Vint(ParN4010A_SSPacktes);
#if 0		
		per_max = 10;//cfg.VDouble(ParN4010A_SSPERMax);
		tx_power = -68;//cfg.VDouble(ParN4010A_SSTxPwr);		
		if(ch == 2)
		{
			if(tset->dev_id == TEST_DEV_ID_A)
				path_loss = 0;//cfg.VDouble(s_DevA_MCH_pathLoss);
			else
				path_loss = 0;//cfg.VDouble(s_DevB_MCH_pathLoss);
			channel = 7;//cfg.Vint(ParN4010A_SSMRxCh);
		}
        else if(ch == 3)
		{	
			if(tset->dev_id == TEST_DEV_ID_A)
				path_loss = 0;//cfg.VDouble(s_DevA_HCH_pathLoss);
			else
				path_loss = 0;//cfg.VDouble(s_DevB_HCH_pathLoss);
			channel = 13;//cfg.Vint(ParN4010A_SSHRxCh);
		}
        else
		{	
			if(tset->dev_id == TEST_DEV_ID_A)
				path_loss = 0;//cfg.VDouble(s_DevA_LCH_pathLoss);
			else
				path_loss = 0;//cfg.VDouble(s_DevB_LCH_pathLoss);
			channel = 1;//cfg.Vint(ParN4010A_SSLRxCh);
		}
#else		
		per_max = cfg.VDouble(Par_sens_err_per_max);
		tx_power = cfg.VDouble(ParN4010A_SSTxPwr);		
		if(ch == 2)
		{
			if(tset->dev_id == TEST_DEV_ID_A)
				path_loss = cfg.VDouble(Par_a_sens_lossm);
			else
				path_loss = cfg.VDouble(Par_b_sens_lossm);
			channel = 7;//cfg.Vint(ParN4010A_SSMRxCh);
		}
        else if(ch == 3)
		{	
			if(tset->dev_id == TEST_DEV_ID_A)
				path_loss = cfg.VDouble(Par_a_sens_lossh);
			else
				path_loss = cfg.VDouble(Par_b_sens_lossh);
			channel = 13;//cfg.Vint(ParN4010A_SSHRxCh);
		}
        else
		{	
			if(tset->dev_id == TEST_DEV_ID_A)
				path_loss = cfg.VDouble(Par_a_sens_loss);
			else
				path_loss = cfg.VDouble(Par_b_sens_loss);
			channel = 1;//cfg.Vint(ParN4010A_SSLRxCh);
		}
#endif
        err_min = ERR_MT8852_DATA;
        err_max = ERR_MT8852_DATA;	
//		terminate_test_if_err_accur = false;
		par_is_gloden = false;
}


void CTObj_N4010A_sens::gen_prompt(void)
{
        char itemstr[8];
        itoa(Index(),itemstr,10);

        CString range;
        range.Format("Per:<=%.2lf%% Pwr:%.2lfdBm",per_max,tx_power);
        tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}


void CTObj_N4010A_sens::update_range(void)
{
        ConfigFile &cfg = pCfgf;
        CDutGlodenPar *pgloden = pMainWnd->dutGloden[DUT_PAR_IDX];
        bool update = false;
#if 0
        int gloden = tset->gloden_idx();
        if(gloden)
        { 
                par_is_gloden = true;
                update = true;
                pgloden = pMainWnd->dutGloden[gloden];
        }
        else if(par_is_gloden)
        { 
                par_is_gloden = false;
                update = true;   
        }

        if(update)
        {
            item_range.Format("Ber:<%.1lf Fer:<%.1lf S:%.1lf",per_max,fer_max,pgloden->sens_pwr);    
			TSET_MSG(MSG_SHOW_RANGE,tset);
        }
#endif
}

int CTObj_N4010A_sens::ItemTestReady(ConfigFile &cfg)
{   
#if 1
	//mutex_hodle1 = false;
	if (!pMainWnd->glbResource_get(GRESOURCE_MCU1))
	{
		param = "u18 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_hodle1 = true;

	//mutex_hodle2 = false;
	
	if(!pMainWnd->glbResource_get(GRESOURCE_BT_TSET))
	{
		param = "get gsource err";
		return(ERR_STOP);
	}
	mutex_hodle2 = true;
#else
	mutex_hodle1 = false;
	mutex_hodle2 = false;
	if (!pMainWnd->glbResource_get(GRESOURCE_MCU2))
	{
		param = "u18 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_hodle1 = true;
	mutex_hodle2 = true;
#endif
	
	return(NO_ERR);
}

int CTObj_N4010A_sens::ItemTestClean(ConfigFile &cfg)
{
	
#if 1
	if(mutex_hodle2)
	{ 
		pMainWnd->glbResource_put(GRESOURCE_BT_TSET);
		mutex_hodle2 = false;
	}
	
	if (mutex_hodle1)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU1);
        mutex_hodle1 = false;
	}
#else
	if(mutex_hodle2 || mutex_hodle1)
	{ 
		pMainWnd->glbResource_put(GRESOURCE_MCU2);
		 mutex_hodle1 = false;
		mutex_hodle2 = false;
	}
#endif
	   
	return(NO_ERR);
}
//yhl_sens
int CTObj_N4010A_sens::TestOp(ConfigFile &cfg)
{
	//struct tSetResult *tresult = tset->mt8852_res;
    //CBsDevice *pobj = tset->pSpiObj;
    result = false;
	bool result_err = false;
	double center_fre = 2.412e9;
	double center_err_per = 0;
	double receive_ok=0;
	double receive_err=0;
    //is_tested = true;
	int i=0,j=0;
	//long ErrorStatus = 0;
	//unsigned long m_hN4010;
	CString m_szN4010Address = cfg.VCString(ParN4010A_Port);	//"TCPIP0::192.168.18.195::inst0::INSTR";
__test:
	if(!pBtTestSet)
	{
		param = "null device";
		return(ERR_NOMAL);
	}
	
	if(!pMCU1_Com)
	{
		param = "null com";
		return(ERR_NOMAL);
	}
	
	bt_TSET *tSet= pBtTestSet;
	if(!tSet || !tSet->dev_is_ready())
	{
		param = "N4010A err";
		return(ERR_NOMAL);
	}

	//path route to N4010A
       #if 0
	if(tset->dev_id == TEST_DEV_ID_B)
	{
		stop_esp8266_rx(pRTL8711_2_Com,param);
	}
	else
	{   
		stop_esp8266_rx(pRTL8711_1_Com,param);
	}
       #endif
	
	if(!pMCU1_Com->set_rout_4010_on(0, MCU_PIO_DUT_4010_METER, MCU_PIO_NUM_DUT_4010_METER))
	{
		param ="switch route to 4010 err";
		return (ERR_NOMAL);
	}

	//dut1 route to N4010A
	if(tset->dev_id == TEST_DEV_ID_A)
	{

		if(!pMCU1_Com->set_4010_route_dut1(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut1 err";
			return (ERR_NOMAL);
		}

	}
	else
	{

		if(!pMCU1_Com->set_4010_route_dut2(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut2 err";
			return (ERR_NOMAL);
		}

	}

	Sleep(200);
	//Sleep(200);
	//从PC上下载文件到仪器
	//Inst.AWG_LoadSegment("54OFDM_64QAM_1000B.WF1","Instrument_Segment.WF1");
	//Inst.AWG_LoadSequence("54OFDM_64QAM_1000B.SEQ","Instrument_Sequence.SEQ");
	center_fre = 2.412e9 + (channel-1)*0.005e9; 
	//Inst.SetChannel(2,channel);//IEEE802_11g=2//enum RadioFormatEnum Format
	Inst.SetFrequency(center_fre);
	
	Inst.AWG_SelectSequenceA("54OFDM_64QAM_1000B.SEQ", tx_power+path_loss, 0 );
	//Inst.AWG_SelectSequenceA("54OFDM_64QAM_1000B.SEQ", path_loss, 0 );

	//rtl8711设置开始接包	
	if(tset->dev_id == TEST_DEV_ID_B)//yhl 2018/10/22
	{
		start_esp8266_rx(pRTL8711_2_Com,(uint8)channel,param);
	}
	else
	{ 
		start_esp8266_rx(pRTL8711_1_Com,(uint8)channel,param);
	}

	Sleep(100);
	//OutputLog("Log.txt", "CTObj_N4010A_sens.TestOp_4");

	Inst.AWG_TriggerSequence();
	
	while(Inst.GetAWG_SequencePlaying())	
	{	
		// Setup timeout in case of problems
		if(i++ > 10)
			break;
		Sleep(50);		
	}
	//OutputLog("Log.txt", "CTObj_N4010A_sens.TestOp_5");
	//Retrieve the successful Received Packets value	
	//检索成功接收的数据包值
	//yhl problem
	if(tset->dev_id == TEST_DEV_ID_B)
	{
		get_rtl8711_rx_data(pRTL8711_2_Com,receive_ok,receive_err,param);
	}
	else
	{ 
		get_rtl8711_rx_data(pRTL8711_1_Com,receive_ok,receive_err,param);
	}
	//OutputLog("Log.txt", "CTObj_N4010A_sens.TestOp_6");
	Inst.AWG_Stop();
	j++;
	if((receive_ok > 500)&& ((receive_ok + receive_err) > 900))
	{
		center_err_per = (receive_err/(receive_ok + receive_err))*100;
		//param.Format("err:%d ok:%d",receive_err,receive_ok);
		//show_message();
	}
	else
	{
		param = "esp8266xx receive err";
		if(j>=2)
		{
			result_err = true;
			goto __err;
		}
		else
		{
			goto __test;
		}
			
		//result_err = true;
		//goto __err;
	}

	//dut1 route to N4010A
	if(tset->dev_id == TEST_DEV_ID_B)
	{

		if(!pMCU1_Com->set_4010_route_dut1(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut1 err";
			return (ERR_NOMAL);
		}

	}
	else
	{

		if(!pMCU1_Com->set_4010_route_dut2(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			param ="switch 4010 to dut2 err";
			return (ERR_NOMAL);
		}

	}
	//Calculate FER / PER is within specification limits	
	//PER = (Sent Packets – Received Packets) / (Sent Packets) * 100 %
	

	//param.Format("err:%d ok:%d per:%.2lf%%",(uint16)receive_err,(uint16)receive_ok,center_err_per);
	param.Format("per:%.2lf%%",center_err_per);
	show_message();
	if(center_err_per > per_max)
	{
		result_err = true;
		goto __err;
	}

__err:
	//Release_N4010A_Instrument();

	if(result_err) 
		return (false);

	//OutputLog("Log.txt", "CTObj_N4010A_sens.TestOp_9");	
	result = true;
    return(NO_ERR);
}


void CTObj_N4010A_sens::record_header(ofstream &fs)
{
        fs<<name<<" Overall BER(%),Overall FER(%)"<<", ";
}

void CTObj_N4010A_sens::record_result(ofstream &fs)
{
#if 0
        CString temp;

        if(!res->valid)
        {	
			temp = param;
			format_record_par(temp);
			if(is_tested) temp += ",/F,";
			else temp += ",NC,";	
        }
        else
        {
			temp.Format("%.2lf,%.2lf,",res->Ber,res->Fer);
        }
#endif
        fs << param <<",";
        if(result) fs<<"/P,";
        else if(!is_tested) fs<<"NC,";
        else fs<<"/F,";
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

CTObj_RFPARASAVE::CTObj_RFPARASAVE(const char* pname,testSet *Tset,int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
#if 0
        err_min = ERR_MT8852_DATA;
        err_max = ERR_MT8852_DATA;

        terminate_test_if_err_accur = false;

        if(ch == 2) res = &tset->mt8852_res->ic[1];
        else if(ch == 3) res = &tset->mt8852_res->ic[2];
        else res = &tset->mt8852_res->ic[0];
        par_is_gloden = false;

		terminate_test_if_err_accur = false;
#endif
}


void CTObj_RFPARASAVE::gen_prompt(void)
{
		char itemstr[8];
        itoa(Index(),itemstr,10);

        CDutGlodenPar *par = pMainWnd->dutGloden[DUT_PAR_IDX];
        
        //neg_max = par->rf_max_neg_foffset;
	    //pos_max = par->rf_max_pos_foffset;

        CString range;
        //range.Format("offset:%.2lfK~%.2lfK", neg_max, pos_max);	
		range.Format("--------");
        tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}


void CTObj_RFPARASAVE::update_range(void)
{
        ConfigFile &cfg = pCfgf;
        CDutGlodenPar *pgloden = pMainWnd->dutGloden[DUT_PAR_IDX];
        bool update = false;
#if 0
        int gloden = tset->gloden_idx();
        if(gloden)
        { 
                par_is_gloden = true;
                update = true;
                pgloden = pMainWnd->dutGloden[gloden];
        }
        else if(par_is_gloden)
        { 
                par_is_gloden = false;
                update = true;   
        }

		neg_max = pgloden->rf_max_neg_foffset;
	    pos_max = pgloden->rf_max_pos_foffset;

        if(update)
        {
                item_range.Format("offset:%.2lfK~%.2lfK", neg_max, pos_max);
                TSET_MSG(MSG_SHOW_RANGE,tset);
        }
#endif
}

int CTObj_RFPARASAVE::TestOp(ConfigFile &cfg)
{
	struct tSetResult *tresult = tset->mt8852_res;
    CBsDevice *pobj = tset->pSpiObj;
	CString cmd_str;
    result = false;
    is_tested = true;

	// Write the configuration settings only to SPIFLASH
	if(tset->is_golden == false)
	{
		if(1)
		{
			param.Format("切换正常模式");
		}
	}
	else
	{
		param.Format("golden don't save RF para.");
	}

//让模块回到normal模式
//掉电				
	if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pRTL8711_1_Com)
		{
			param = "null RTL87xx COM";
			return (false);
		}						
		cmd_str.Format("ATSC\r\n");//to normal
		pRTL8711_1_Com->send(cmd_str, 0);
		Sleep(200);
		if(!pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY))
		{
			param = "set dut battery err";
			return (false);
		}
		delete(pRTL8711_1_Com);
		pRTL8711_1_Com = NULL;
	}
	else
	{
		if(!pRTL8711_2_Com)
		{
			param = "null RTL87xx COM";
			return (false);
		}			
		cmd_str.Format("ATSC\r\n");//to normal
		pRTL8711_2_Com->send(cmd_str, 0);
		Sleep(200);
		if(!pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY))
		{
			param = "set dut battery err";
			return (false);
		}
		delete(pRTL8711_2_Com);
		pRTL8711_2_Com = NULL;
	}	
	result = true;
    return(NO_ERR);
}


void CTObj_RFPARASAVE::record_header(ofstream &fs)
{
        fs<<name<<" Average Offset(kHz),Max Offset(kHz),Min Offset(kHz)"<<", ";
}

void CTObj_RFPARASAVE::record_result(ofstream &fs)
{
#if 0
        CString temp;

        if(!res->valid) 
        {
                temp = param;
                format_record_par(temp);
                if(is_tested) temp += ",/F,/F,";
                else temp += ",NC,NC,";
        }
        else
        {
                temp.Format("%.2lf,%.2lf,%.2lf,",res->av,res->pos_max,res->neg_max);
        }
#endif
        fs << param <<",";
        if(result) fs<<"/P,";
        else if(!is_tested) fs<<"NC,";
        else fs<<"/F,";
}


#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------


CTObj_mt8852_test::CTObj_mt8852_test(const char* pname,testSet *Tset,int ID, ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
        mutex_holde = false;
		par_is_gloden = false;
        delay_time = cfg.Vulong(ParMT8852x_Dly_time);

        APathOffset[0] = cfg.VDouble(s_DevA_LCH_pathLoss);  
        APathOffset[1] = cfg.VDouble(s_DevA_MCH_pathLoss);
        APathOffset[2] = cfg.VDouble(s_DevA_HCH_pathLoss);

        BPathOffset[0] = cfg.VDouble(s_DevB_LCH_pathLoss);
        BPathOffset[1] = cfg.VDouble(s_DevB_MCH_pathLoss);
        BPathOffset[2] = cfg.VDouble(s_DevB_HCH_pathLoss);

		RfPwrCenter = cfg.VDouble(s_RFPwrCenter);
        RfPwrMin = cfg.VDouble(s_RFPwrFineMin);  
        RfPwrMax = cfg.VDouble(s_RFPwrFineMax);
        RfPwrAdjMin = cfg.VDouble(s_RFPwrAdjMin); 
        RfPwrAdjMax = cfg.VDouble(s_RFPwrAdjMax);
}

void CTObj_mt8852_test::gen_prompt(void)
{
		char itemstr[8];
        itoa(Index(),itemstr,10);

        CString range;
        //range.Format("delay:%d ",delay_time);
		range = "";
		range.Format("delay:%d ",delay_time);
        //if(rf_power_adjust_sel) range += "pwr-adj:en";
        //else range += "pwr-adj:dis";
        tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}


//#if 0

int CTObj_mt8852_test::ItemTestReady(ConfigFile &cfg)
{
#if 0
        mutex_holde = false;

        bool rout_sw = false;
        if(!pMCU2_Com || !pMCU2_Com->mcu_set_route_mt8852(tset->dev_id,rout_sw)) rf_rout_set_err = true;
        if(!pBtTestSet) rf_par_set_err = true;
        else
        {
                double *pathOffset = APathOffset;
                if(TEST_DEV_ID_B == tset->dev_id) pathOffset = BPathOffset;
                if(!pBtTestSet->setPathOffset(pathOffset[1])) rf_par_set_err = true;

                CDutGlodenPar *par = pMainWnd->dutGloden[tset->gloden_idx()];  //read bt address ?= gdn bt address
                if(!pBtTestSet->setDutPar(par)) rf_par_set_err = true;   //MT8852->setDutPar(par)
        }

        if(rout_sw && pMainWnd->m8852_locked && pBtTestSet && pBtTestSet->idle_timer)
        {
                if(pBtTestSet->idle_timer < rout_sw_time)
                pBtTestSet->idle_timer = rout_sw_time;
        }
        else if(rout_sw)
        {
                param.Format("rout sw delay:%d",rout_sw_time);
                show_message();
                Sleep(rout_sw_time);
        }

        if(!pMainWnd->glbResource_get(GRESOURCE_BT_TSET)) 
        {
                param = "get gsource err";
                return(ERR_STOP);
        }
        mutex_holde = true;
        pMainWnd->m8852_locked = true;
	return(NO_ERR);
#endif

#if 0

	mutex_hodle1 = false;
	mutex_hodle2 = false;
	
	if (!pMainWnd->glbResource_get(GRESOURCE_MCU1))
	{
		param = "u18 gsource err";
		return(ERR_TERMINAL);
	}
	mutex_hodle1 = true;

	if(!pMainWnd->glbResource_get(GRESOURCE_BT_TSET)) 
	{
		param = "get bt gsource err";
		return(ERR_STOP);
	}
	mutex_hodle2 = true;
	pMainWnd->m8852_locked = true;
    return(NO_ERR);
#endif
	return(NO_ERR);
}



int CTObj_mt8852_test::ItemTestClean(ConfigFile &cfg)
{
	if(mutex_hodle2)
    {
		mutex_hodle2 = false;
#if 0		
		bt_TSET *tSet= pBtTestSet;
		if(tSet && delay_time) 
		{
			tSet->idle_timer = delay_time;
			pMainWnd->SetTimer(TIMER_UNLOCK_M8852,100,NULL);
		}
		else
		{
			if(tSet) tSet->idle_timer = 0;
			pMainWnd->m8852_locked = false;
			pMainWnd->glbResource_put(GRESOURCE_BT_TSET);
		}
#endif
		pMainWnd->glbResource_put(GRESOURCE_BT_TSET);
    }
	
	if (mutex_hodle1)
	{
		pMainWnd->glbResource_put(GRESOURCE_MCU1);
        mutex_hodle1 = false;
	}
	   
    return(NO_ERR);
}



void CTObj_mt8852_test::ClearResult(void)
{
        CTestItem::ClearResult();
	    memset(tset->mt8852_res,0,sizeof(struct tSetResult));

        rf_power_cur_par = 0;
        rf_power_adjust_par = 0;
        rf_par_set_err = false;
        rf_rout_set_err = false;
}



int CTObj_mt8852_test::rf_power_is_adjusted(void)
{
#if 0
        const unsigned short pwr_par_tbl[CRF_PWR_STEP_LEN] = { 0x1010,0x1212,0x1414,0x1616,0x1818,0x1A1A,0x1C1C,0x1F1F };
        ConfigFile &cfg = pCfgf;
        CDutDevice *pobj = tset->pObj;
        struct tSetResult *tresult = tset->mt8852_res;
        struct top_res &pwr_res = tresult->op[1];

        if(!rf_power_adjust_sel) return(RF_PWR_ADJ_RES_NULL);
        if(tset->gloden_idx()) return(RF_PWR_ADJ_RES_GOLDEN);
        if(!tset->ic_is_v504) return(RF_PWR_ADJ_RES_NOT_504);
        if(!pwr_res.valid) return(RF_PWR_ADJ_RES_PWR_ERR);

        double pwr = pwr_res.av;
        double *pwr_lv_tbl = cfg.get_pwr_step_tbl();
        if((pwr >= RfPwrMin) && (pwr <= RfPwrMax)) return(RF_PWR_ADJ_RES_PAR_OK);
        if((pwr < RfPwrAdjMin) || (pwr > RfPwrAdjMax)) return(RF_PWR_ADJ_RES_OVER_RANGE);
        
        //get Max Tx Power Gain???
		if(pobj->ReadMaxTxPowerGain(&rf_power_cur_par) != VM_SUCCESS)
        {
                param = pobj->errInfo();
                if(param == "") param = "read power gain";
                if(pobj->ReadMaxTxPowerGain(&rf_power_cur_par) != VM_SUCCESS) return(RF_PWR_ADJ_RES_ERR);
        }
        for(int i=0;i<CRF_PWR_STEP_LEN;i++)
        {
                if(rf_power_cur_par <= pwr_par_tbl[i]) break;
        }
        int cur_idx = i;
        if(cur_idx == CRF_PWR_STEP_LEN) cur_idx -= 1;

        int prev_idx = cur_idx;
        double prev_pwr = pwr,op_pwr = pwr;
        bool op_res = true;
        while(true)
        {
                if(op_pwr == RfPwrCenter) break;
                else if(op_pwr > RfPwrCenter)
                {
                        op_res = rf_power_dec(op_pwr,cur_idx);
                        if(!op_res) break;
                        if(op_pwr <= RfPwrCenter) break;
                }
                else
                {
                        op_res = rf_power_inc(op_pwr,cur_idx);
                        if(!op_res) break;
                        if(op_pwr >= RfPwrCenter) break;
                }
                prev_idx = cur_idx;
                prev_pwr = op_pwr;
        }
        if(fabs(prev_pwr-RfPwrCenter) < fabs(op_pwr-RfPwrCenter)) rf_power_adjust_par = pwr_par_tbl[prev_idx];
        else rf_power_adjust_par = pwr_par_tbl[cur_idx];
        if(rf_power_adjust_par == rf_power_cur_par) return(RF_PWR_ADJ_RES_PAR_OK);

        if(pobj->WriteMaxTxPowerGain(rf_power_adjust_par) != VM_SUCCESS)
        { 
                param = pobj->errInfo();
                if(param == "") param = "write power gain";
                if(pobj->WriteMaxTxPowerGain(rf_power_adjust_par) != VM_SUCCESS) return(RF_PWR_ADJ_RES_ERR);
        }
#endif
        return(RF_PWR_ADJ_RES_PAR_CHG);
}

bool CTObj_mt8852_test::save_pwr_par(void)
{
#if 0
	CDutDevice *pobj = tset->pObj;
	
	if(pobj->SetMaxTxPowerGain(rf_power_adjust_par) != VM_SUCCESS)
	{
		param = pobj->errInfo();
		if(param == "") param = "save power par";
		if(pobj->SetMaxTxPowerGain(rf_power_adjust_par) != VM_SUCCESS) return(false);
	}
#endif
	return(true);
}



bool CTObj_mt8852_test::do_8852_test(struct tSetResult *tresult)
{
     	struct bt_address_s *paddr = tset->pdev_addr;
        CBsDevice *pobj = tset->pSpiObj;
        CString temp;
        bool mt8852_res;

        temp.Format("%.4X%.2X%.6X",paddr->nap,paddr->uap,paddr->lap);      
		mt8852_res = pBtTestSet->run(temp,*tresult,this);

        if(!mt8852_res) 
        {
           param = "mt8852";
           return(false);
        }
        return(true);
}



bool CTObj_mt8852_test::dut_enter_test(void)
{
		uint32 status;
		CBsDevice *pobj = tset->pSpiObj;
		status = bccmdEnableDeviceConnect(pobj->iHandle);
        if(status == TE_OK) status = bccmdEnableDeviceUnderTestMode(pobj->iHandle);
        if(status != TE_OK)
        {
           return(false);
        }
		return(true);
}


//#endif


int CTObj_mt8852_test::TestOp(ConfigFile &cfg)
{
	    uint32 status; 
	    uint16 nap;
	    uint8 uap;
	    uint32 lap;
	    CString temp;
	    unsigned long ltime = 0;    
	    struct tSetResult *tresult = tset->mt8852_res;
	    CBsDevice *pobj = tset->pSpiObj;
        result = false;
		is_tested = true;

		if(!pBtTestSet)
		{
		    param = "null device";
		    return(ERR_NOMAL);
		}

		if(!pMCU1_Com)
		{
		    param = "null com";
			return(ERR_NOMAL);
		}

        memset(tset->mt8852_res,0,sizeof(struct tSetResult));

        bt_TSET *tSet= pBtTestSet;
	    if(!tSet || !tSet->dev_is_ready())
		{
		      param = "mt8852 err";
		      return(ERR_NOMAL);
		}

//------------------------------------------------------------------------------------------------		
		mutex_hodle1 = false;
		if (!pMainWnd->glbResource_get(GRESOURCE_MCU1))
		{
			param = "u18 gsource err";
			return(ERR_TERMINAL);
		}
		mutex_hodle1 = true;
//------------------------------------------------------------------------------------------------
		
		//path route to mt8852
		if(!pMCU1_Com->set_rout_8852_on(0, MCU_PIO_DUT_8852_METER, MCU_PIO_NUM_DUT_8852_METER))
		{
		    param ="switch route to 8852 err";
            return (ERR_NOMAL);
		}
		//dut1 route to mt8852
		if(tset->dev_id == TEST_DEV_ID_A)
		{
			if(!pMCU1_Com->set_8852_route_dut1(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
			{
				param ="switch 8852 to dut1 err";
				return (ERR_NOMAL);
			}	
		}
		else
		{
			if(!pMCU1_Com->set_8852_route_dut2(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
			{
				param ="switch 8852 to dut2 err";
				return (ERR_NOMAL);
			}
		}
		Sleep(100);

//----------------------------------------------------------------------------------		
		mutex_hodle2 = false;
		
		if(!pMainWnd->glbResource_get(GRESOURCE_BT_TSET)) 
		{
			param = "get bluetest gsource err";
			return(ERR_STOP);
		}
		mutex_hodle2 = true;
		//pMainWnd->m8852_locked = true;

		if(pCfgf.ItemSelect(s_BModle_Sel)&& pCfgf.ItemSelect(slaveXtalSel))
		{
			while(true)
			{	
				param = "wait xtal adj done";
				show_message();
				if( pMainWnd->tSetA->xtal_adj_is_tested &&
					pMainWnd->tSetB->xtal_adj_is_tested) 
				{
					break;
				}	
			}
		}

//----------------------------------------------------------------------------------

		status = bccmdEnableDeviceConnect(pobj->iHandle);
        if(status == TE_OK) status = bccmdEnableDeviceUnderTestMode(pobj->iHandle);
        if(status != TE_OK)
        {
            param ="dut enter test mode err";
			return (ERR_NOMAL);
        }		
		
		status = psReadBdAddr(pobj->iHandle,&lap,&uap,&nap);
        if(status != TE_OK) 
        {
	 	    param =  "raddr err";
			return (ERR_NOMAL);

        }
        temp.Format("%.4X%.2X%.6X",nap,uap,lap);
   
		if(!pBtTestSet->run(temp,*tresult,this))
		{
		    //try again
			//wait_continue();
			Sleep(50);
			if(!pBtTestSet->run(temp,*tresult,this))
			{
			   param = "mt8852 run err";
			   return(ERR_MT8852_OP);
			}
		}


#if 0
        if(!do_8852_test(tresult)) 
        { 
			if(retry_delay)
			{
				while(ltime < retry_delay)
				{
					Sleep(100);
					ltime += 100;
					if(ltime%1000)
					{ 
						param = "";
						param.Format("%d s",ltime/1000);
						show_message();
					}
				}
			}
			param = "try again";
			show_message();
			
			param = "do 8852 test fail";
            show_message();
            if(!do_8852_test(tresult))
			{
				if(param == "") param = "mt8852 test err";
				return(ERR_MT8852_OP);
				
			}
        }
#endif


#if 0
        int res = rf_power_is_adjusted();
        if(RF_PWR_ADJ_RES_ERR == res)
        {
                return(ERR_MT8852_OP);
        }
        else if(RF_PWR_ADJ_RES_PAR_CHG == res)
        {
                param = "adjust power retest";
                show_message();
                memset(tresult,0,sizeof(struct tSetResult));
                if(!do_8852_test(tresult)) 
                {
                        param = "try again";
                        show_message();
                        if(!do_8852_test(tresult))
                        {
                                return(ERR_MT8852_OP);
                        }
                }
                if(!save_pwr_par()) return(ERR_MT8852_OP);
        }
        

        if(!rf_power_adjust_sel) param = "OK";
        else
        {
                //if(!tset->ic_is_v504) param = "not 504 IC";
                //else 
				if(RF_PWR_ADJ_RES_OVER_RANGE == res) param = "out of adj range";
                else if(RF_PWR_ADJ_RES_GOLDEN == res) param = tset->gloden_idx_name();
                else param.Format("idx:%d par:%.4X adj:%.4X", rf_power_adjust_value, rf_power_cur_par, rf_power_adjust_par);
        }
#endif

		result = true;
		tset->rf_is_tested = true;
        return(NO_ERR);
//#endif


}
#endif
#if 0

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_mt8852_pwr::CTObj_mt8852_pwr(const char* pname,int ch,testSet *Tset,int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
		err_min = ERR_MT8852_DATA;
        err_max = ERR_MT8852_DATA;

		
        if(ch == 2)
		{
			if(tset->dev_id == TEST_DEV_ID_A)
				path_loss = cfg.VDouble(s_DevA_MCH_pathLoss);
			else
				path_loss = cfg.VDouble(s_DevB_MCH_pathLoss);
			res = &tset->mt8852_res->op[1];
		}
        else if(ch == 3)
		{	
			if(tset->dev_id == TEST_DEV_ID_A)
			path_loss = cfg.VDouble(s_DevA_HCH_pathLoss);
		    else
			path_loss = cfg.VDouble(s_DevB_HCH_pathLoss);
			res = &tset->mt8852_res->op[2];
		}
        else
		{	
			if(tset->dev_id == TEST_DEV_ID_A)
			path_loss = cfg.VDouble(s_DevA_LCH_pathLoss);
		    else
			path_loss = cfg.VDouble(s_DevB_LCH_pathLoss);
			res = &tset->mt8852_res->op[0];
		}
        par_is_gloden = false;
        terminate_test_if_err_accur = false;
}

void CTObj_mt8852_pwr::gen_prompt(void)
{
		char itemstr[8];
        itoa(Index(),itemstr,10);

        CDutGlodenPar *par = pMainWnd->dutGloden[DUT_PAR_IDX];
        //double min,max,pk;
		
		if(channel == 3)
        { 
                pwr_min = par->ch3_rf_pwr_min;
                pwr_max = par->ch3_rf_pwr_max;
                pwr_pk = par->rf_pwr_peak;
        }
		else if(channel == 2)
        { 
                pwr_min = par->ch2_rf_pwr_min;
                pwr_max = par->ch2_rf_pwr_max;
                pwr_pk = par->rf_pwr_peak;
        }
        else
        { 
                pwr_min = par->rf_pwr_min;
                pwr_max = par->rf_pwr_max;
                pwr_pk = par->rf_pwr_peak;
        }

        CString range;
        range.Format("Pwr[%.2lf,%.2lf] Pk:%.1lf,Los:%.2fdB", pwr_min, pwr_max, pwr_pk, path_loss);
        //range.Format("Pwr[%.2lf,%.2lf]", pwr_min, pwr_max);
		tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");

}


void CTObj_mt8852_pwr::update_range(void)
{
        ConfigFile &cfg = pCfgf;
        CDutGlodenPar *pgloden = pMainWnd->dutGloden[DUT_PAR_IDX];
        bool update = false;

        int gloden = tset->gloden_idx();
        if(gloden)
        { 
			par_is_gloden = true;
			update = true;
			pgloden = pMainWnd->dutGloden[gloden];
        }
        else if(par_is_gloden)
        { 
			par_is_gloden = false;
			update = true;   
        }

        //double min,max,pk;
        if(channel == 3)
        { 
			pwr_min = pgloden->ch3_rf_pwr_min;
			pwr_max = pgloden->ch3_rf_pwr_max;
			pwr_pk = pgloden->rf_pwr_peak;
        }
        else if(channel == 2)
        { 
			pwr_min = pgloden->ch2_rf_pwr_min;
			pwr_max = pgloden->ch2_rf_pwr_max;
			pwr_pk = pgloden->rf_pwr_peak;
        }
        else
        { 
			pwr_min = pgloden->rf_pwr_min;
			pwr_max = pgloden->rf_pwr_max;
			pwr_pk = pgloden->rf_pwr_peak;
        }
		
        if(update)
        { 
			item_range.Format("Pwr[%.2lf,%.2lf] Pk:%.1lf,Los:%.2fdB", pwr_min, pwr_max, pwr_pk, path_loss);
			TSET_MSG(MSG_SHOW_RANGE,tset);
        }
}

int CTObj_mt8852_pwr::TestOp(ConfigFile &cfg)
{
        struct tSetResult *tresult = tset->mt8852_res;
        CBsDevice *pobj = tset->pSpiObj;
        result = false;
		is_tested = true;
 
        if(!res->valid) 
        {
            param = "data invalid";
            return(ERR_MT8852_DATA);
        }	

        CString temp;
        param = "";
        temp.Format("av:%.2lf", (res->av + path_loss));
        param += temp;
        temp.Format(" max:%.2lf", (res->av_max + path_loss));
        param += temp;
        temp.Format(" min:%.2lf", (res->av_min + path_loss)); 
        param += temp;
		if((res->av_min + path_loss) >= pwr_min && (res->av_max + path_loss) <= pwr_max)  result = true;
         
        if(!result) return(ERR_MT8852_DATA);
        return(NO_ERR);
}


void CTObj_mt8852_pwr::record_header(ofstream &fs)
{
        fs << name << " Average Power(dBm),Max Power(dBm),Min Power(dBm)" << ", "; 
}

void CTObj_mt8852_pwr::record_result(ofstream &fs)
{
        CString temp;

        if(!res->valid) 
        {
			temp = param;
			format_record_par(temp);
			if(is_tested) temp += ",/F,/F,";
			else temp += ",NC,NC,";
        }
        else
        {
			temp.Format("%.2lf,%.2lf,%.2lf,",res->av,res->av_max,res->av_min);
        }

        fs << temp;
        if(result) fs<<"/P,";
        else if(!is_tested) fs<<"NC,";
        else fs<<"/F,";
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_mt8852_offset::CTObj_mt8852_offset(const char* pname,int ch,testSet *Tset,int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
        err_min = ERR_MT8852_DATA;
        err_max = ERR_MT8852_DATA;

        terminate_test_if_err_accur = false;

        if(ch == 2) res = &tset->mt8852_res->ic[1];
        else if(ch == 3) res = &tset->mt8852_res->ic[2];
        else res = &tset->mt8852_res->ic[0];
        par_is_gloden = false;

		terminate_test_if_err_accur = false;
}


void CTObj_mt8852_offset::gen_prompt(void)
{
		char itemstr[8];
        itoa(Index(),itemstr,10);

        CDutGlodenPar *par = pMainWnd->dutGloden[DUT_PAR_IDX];
        
        neg_max = par->rf_max_neg_foffset;
	    pos_max = par->rf_max_pos_foffset;

        CString range;
        range.Format("offset:%.2lfK~%.2lfK", neg_max, pos_max);
        tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}


void CTObj_mt8852_offset::update_range(void)
{
        ConfigFile &cfg = pCfgf;
        CDutGlodenPar *pgloden = pMainWnd->dutGloden[DUT_PAR_IDX];
        bool update = false;

        int gloden = tset->gloden_idx();
        if(gloden)
        { 
                par_is_gloden = true;
                update = true;
                pgloden = pMainWnd->dutGloden[gloden];
        }
        else if(par_is_gloden)
        { 
                par_is_gloden = false;
                update = true;   
        }

		neg_max = pgloden->rf_max_neg_foffset;
	    pos_max = pgloden->rf_max_pos_foffset;

        if(update)
        {
                item_range.Format("offset:%.2lfK~%.2lfK", neg_max, pos_max);
                TSET_MSG(MSG_SHOW_RANGE,tset);
        }
}

int CTObj_mt8852_offset::TestOp(ConfigFile &cfg)
{
		struct tSetResult *tresult = tset->mt8852_res;
        CBsDevice *pobj = tset->pSpiObj;
        result = false;
        is_tested = true;

        if(!res->valid)
        {
            param = "data invalid";
            return(ERR_MT8852_DATA);
        }

        CString temp;
        param = "";
        temp.Format("Avg:%.2lfK", res->av);
        param += temp;
        temp.Format(" Pos:%.2lfK",res->pos_max);
        param += temp;
        temp.Format(" Neg:%.2lfK",res->neg_max);
        param += temp;
		if((res->av) >= neg_max && (res->av) <= pos_max) result = true;

        if(!result) return(ERR_MT8852_DATA);
        return(NO_ERR);
}


void CTObj_mt8852_offset::record_header(ofstream &fs)
{
        fs<<name<<" Average Offset(kHz),Max Offset(kHz),Min Offset(kHz)"<<", ";
}

void CTObj_mt8852_offset::record_result(ofstream &fs)
{
        CString temp;

        if(!res->valid) 
        {
                temp = param;
                format_record_par(temp);
                if(is_tested) temp += ",/F,/F,";
                else temp += ",NC,NC,";
        }
        else
        {
                temp.Format("%.2lf,%.2lf,%.2lf,",res->av,res->pos_max,res->neg_max);
        }

        fs << temp;
        if(result) fs<<"/P,";
        else if(!is_tested) fs<<"NC,";
        else fs<<"/F,";
}
#endif
#if 0

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CTObj_mt8852_sens::CTObj_mt8852_sens(const char* pname,int ch,testSet *Tset, int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
		per_max = cfg.VDouble(ParMT8852x_SSPERMax);
        fer_max = cfg.VDouble(ParMT8852x_SSFERMax);

        err_min = ERR_MT8852_DATA;
        err_max = ERR_MT8852_DATA;

		terminate_test_if_err_accur = false;

        if(ch == 2) res = &tset->mt8852_res->ss[1];
        else if(ch == 3) res = &tset->mt8852_res->ss[2];
        else res = &tset->mt8852_res->ss[0];
        par_is_gloden = false;
}


void CTObj_mt8852_sens::gen_prompt(void)
{
        char itemstr[8];
        itoa(Index(),itemstr,10);

        CDutGlodenPar *par = pMainWnd->dutGloden[DUT_PAR_IDX];

        CString range;
        range.Format("Ber:%.1lf Fer:%.1lf S:%.1lf",per_max,fer_max,par->sens_pwr);
        tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}


void CTObj_mt8852_sens::update_range(void)
{
        ConfigFile &cfg = pCfgf;
        CDutGlodenPar *pgloden = pMainWnd->dutGloden[DUT_PAR_IDX];
        bool update = false;

        int gloden = tset->gloden_idx();
        if(gloden)
        { 
                par_is_gloden = true;
                update = true;
                pgloden = pMainWnd->dutGloden[gloden];
        }
        else if(par_is_gloden)
        { 
                par_is_gloden = false;
                update = true;   
        }

        if(update)
        {
            item_range.Format("Ber:<%.1lf Fer:<%.1lf S:%.1lf",per_max,fer_max,pgloden->sens_pwr);    
			TSET_MSG(MSG_SHOW_RANGE,tset);
        }
}

int CTObj_mt8852_sens::TestOp(ConfigFile &cfg)
{
		struct tSetResult *tresult = tset->mt8852_res;
        CBsDevice *pobj = tset->pSpiObj;
        result = false;
        is_tested = true;

        if(!res->valid)
        {
                param = "data invalid";
                return(ERR_MT8852_DATA);
        }

        CString temp;
        param = "";
        temp.Format("BER:%.2lf", res->Ber);
        param += temp;
        temp.Format(" FER:%.2lf", res->Fer);
        param += temp;
        //if(res->res) result = true;
		if((res->Ber) <= per_max && (res->Fer) <= fer_max) result = true;

        if(!result) return(ERR_MT8852_DATA);
        return(NO_ERR);
}


void CTObj_mt8852_sens::record_header(ofstream &fs)
{
        fs<<name<<" Overall BER(%),Overall FER(%)"<<", ";
}

void CTObj_mt8852_sens::record_result(ofstream &fs)
{
        CString temp;

        if(!res->valid)
        {	
			temp = param;
			format_record_par(temp);
			if(is_tested) temp += ",/F,";
			else temp += ",NC,";	
        }
        else
        {
			temp.Format("%.2lf,%.2lf,",res->Ber,res->Fer);
        }

        fs << temp;
        if(result) fs<<"/P,";
        else if(!is_tested) fs<<"NC,";
        else fs<<"/F,";
}
#endif
#if 0

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
CTObj_lock::CTObj_lock(const char *pname, testSet *Tset, int ID, ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
}

CTObj_lock::TestOp(ConfigFile &cfg)
{
     result = false;
	 is_tested = true;
	   
	    param = "wait idle";
		show_message();

		if(!pMainWnd->glbResource_get(GRESOURCE_TOP))
		{
		       param = "get lock err";
			return (ERR_TERMINAL);
		}

		tset->own_op_lock = true;
		result = true;
		param = "OK";
	return (NO_ERR);
}
#endif
#if 0
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

CTObj_unlock::CTObj_unlock(const char *pname, testSet *Tset, int ID, ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
	    err_min = ERR_MIN;
        err_max = ERR_MAX;
}

int CTObj_unlock::TestOp(ConfigFile &cfg)
{
    if(tset->own_op_lock)
    { 
         tset->own_op_lock = false;
         pMainWnd->glbResource_put(GRESOURCE_TOP);
    }
        
    result = true;
    param = "OK";
	return(NO_ERR);
}
#endif
#if 0


//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
CTObj_sync::CTObj_sync(const char* pname,int Sync_id,testSet *Tset,int ID,ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
        sync_id = Sync_id;
        max_sync_time = 20*1000;

        err_min = ERR_MIN;
        err_max = ERR_MAX;
}

int CTObj_sync::TestOp(ConfigFile &cfg)
{
        result = false;
        is_tested = true;
        
        param = "synchronization";
        show_message();
        if(!pMainWnd->glbSyncObj_release(sync_id)) 
        {
                result = true;
                param = "OK";
                return(NO_ERR);
        }

        unsigned long dtime = max_sync_time;
        while(true)
        {
                Sleep(100);
                if(!pMainWnd) 
                {
                        param = "terminal";
                        return(ERR_TERMINAL);
                }
                if(!pMainWnd->glbSyncObj_state(sync_id))
                {
                        result = true;
                        param = "OK";
                        return(NO_ERR); 
                }

                if(dtime <= 100) break;
                dtime -= 100;
        }

        param = "no sync";
	return(ERR_TERMINAL);
}
#endif


//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
volatile bool tbox_is_open = true;
volatile bool tbox_atset_is_open = true;
volatile bool tbox_btset_is_open = true;

#if 1
CTObj_close_tbox::CTObj_close_tbox(const char* pname,testSet *Tset, int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
	com = cfg.VCString(ParMultimeterPort);
	max_delay_time = cfg.Vulong(s_close_tbox_delay);
    range_show = false;
	
}

void CTObj_close_tbox::gen_prompt(void)
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	tset->tlist->AddItem(itemstr,name,"","","");
}


void CTObj_close_tbox::update_range(void)
{
	//if(!range_show) 
	{
		item_range = pTbox->port_name();
		CString temp;
		temp.Format(" ,wait %d", max_delay_time);
        item_range += temp;
		TSET_MSG(MSG_SHOW_RANGE,tset);
		range_show = true;
	}
}

int CTObj_close_tbox::ItemTestReady(ConfigFile &cfg)
{
        mutex_holde = false;
        if(!pMainWnd->glbResource_get(GRESOURCE_TBOX))
        {
                param = "get gsource err";
                return(ERR_TERMINAL);
        }
        mutex_holde = true;
	return(NO_ERR);
}

int CTObj_close_tbox::ItemTestClean(ConfigFile &cfg)
{
        if(mutex_holde)
        { 
                pMainWnd->glbResource_put(GRESOURCE_TBOX);
                mutex_holde = false;
        }
	return(NO_ERR);
}

int CTObj_close_tbox::TestOp(ConfigFile &cfg)
{
        result = false;
        is_tested = true;
		int i;
        *(tset->ptbox_is_open) = false;
        //if(!tbox_is_open)
        //{
        //      param = "already closed";
        //      result = true;
        //      return(NO_ERR);
        //}
		#if 1
		pMCU2_Com->set_dut_led_off(1,MCU_PIO_LED, MCU_PIO_NUM_LED);
		#else		  
			  //MCU U5 PIO 初始化
			  pMCU2_Com->set_dut_5v_off(1, MCU_PIO_DUT1_5V, MCU_PIO_NUM_DUT1_5V);
			
			  pMCU2_Com->set_dut_vreg_off(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG);
			  pMCU2_Com->set_dut_spi_off(1, MCU_PIO_SPI,MCU_PIO_NUM_SPI);

              //MCU U18 PIO 初始化
			  pMCU1_Com->set_dut_5v_off(0, MCU_PIO_DUT2_5V, MCU_PIO_NUM_DUT2_5V);
			  pMCU1_Com->set_dut_vreg_off(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG);
			  pMCU1_Com->set_dut_spk_amp_off(0, MCU_PIO_DUT_SPK_AMP, MCU_PIO_NUM_DUT_SPK_AMP);
                        #endif

#if 0
		if(!pTbox || !pTbox->close_box())
        { 
             param = "err";
             return(ERR_TERMINAL);
        }
#else
		if(pTbox)
		{
			for(i=0;i<2;i++)
			{
				if(pTbox->close_box())
					break;
			}
			if(i>=2)
			{
				param = "send close command no answer";
				return(ERR_TERMINAL);
			}

		}
		else
		{
			param = "err";
            return(ERR_TERMINAL);
		}
#endif
        Sleep(max_delay_time);

        //tbox_is_open = false;
        result = true;

        param = "tbox closed ok";
	if(!result) return(ERR_TERMINAL);
	return(NO_ERR);
}
#endif
#if 0
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

volatile bool wait_tbox_close_ok = false;

CTObj_tbox_delay::CTObj_tbox_delay(const char* pname,testSet *Tset,int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
    max_delay_time = cfg.Vulong(s_close_tbox_delay);
}

void CTObj_tbox_delay::gen_prompt(void)
{
	char itemstr[8];
	itoa(Index(),itemstr,10);
	CString range;
	range.Format("wait %dms", max_delay_time);

	tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");
}

int CTObj_tbox_delay::ItemTestReady(ConfigFile &cfg)
{
        mutex_holde = false;
        if(!pMainWnd->glbResource_get(GRESOURCE_TBOX))
        {
                param = "get gsource err";
                return(ERR_TERMINAL);
        }
        mutex_holde = true;
	return(NO_ERR);
}

int CTObj_tbox_delay::ItemTestClean(ConfigFile &cfg)
{
        if(mutex_holde)
        { 
                pMainWnd->glbResource_put(GRESOURCE_TBOX);
                mutex_holde = false;
        }
	return(NO_ERR);
}

int CTObj_tbox_delay::TestOp(ConfigFile &cfg)
{
        result = false;
        is_tested = true;

        if(wait_tbox_close_ok)
        {
             param.Format("already wait %dms", max_delay_time);
        }
		else
		{
		   Sleep(max_delay_time);
		   wait_tbox_close_ok = true;
		   param.Format("wait %dms OK", max_delay_time);         
		}
		result = true;
	    return(true);
}
#endif
#if 1
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
CTObj_open_tbox::CTObj_open_tbox(const char* pname,testSet *Tset,int ID, ConfigFile &cfg):CTestItem(pname,Tset,ID)
{
	com = cfg.VCString(ParMultimeterPort);
        range_show = false;

        err_min = ERR_NOMAL;
        err_max = ERR_MAX;

		terminate_test_if_err_accur = false;
}

void CTObj_open_tbox::gen_prompt(void)
{
	char itemstr[8];
	itoa(Index(),itemstr,10);

	tset->tlist->AddItem(itemstr,name,"","","");
}

void CTObj_open_tbox::update_range(void)
{
	if(!range_show) 
	{
		item_range = pTbox->port_name();
		TSET_MSG(MSG_SHOW_RANGE,tset);
		range_show = true;
	}
}

int CTObj_open_tbox::ItemTestReady(ConfigFile &cfg)
{
        mutex_holde = false;
        if(!pMainWnd->glbResource_get(GRESOURCE_TBOX)) 
        {
                param = "get gsource err";
                return(ERR_STOP);
        }
        mutex_holde = true;
	return(NO_ERR);
}

int CTObj_open_tbox::ItemTestClean(ConfigFile &cfg)
{
        if(mutex_holde)
        { 
                pMainWnd->glbResource_put(GRESOURCE_TBOX);
                mutex_holde = false;
        }
	return(NO_ERR);
}

int CTObj_open_tbox::TestOp(ConfigFile &cfg)
{
        result = false;
        is_tested = true;


		*(tset->ptbox_is_open) = true;
#if 0
        if(tbox_is_open)
        {
                param = "already opened";
                result = true;
                return(NO_ERR);
        }
        else if(!tbox_atset_is_open || !tbox_btset_is_open)
        { 
                param = "hold";
                result = true;
                return(NO_ERR);
        }
#else
		if(!tbox_atset_is_open || !tbox_btset_is_open)
        { 
			param = "hold";
			result = true;
			return(NO_ERR);
        }
#endif

        pMCU2_Com->set_dut_pwr_off(1, MCU_PIO_DUT_PWR, MCU_PIO_NUM_DUT_PWR);
		if(!pTbox || !pTbox->open_box() ||
		   !pMCU2_Com || !pMCU2_Com->set_dut_led_on(1,MCU_PIO_LED, MCU_PIO_NUM_LED))
        { 
                tbox_is_open = true;
                param = "err";
                return(ERR_TERMINAL);
        }
		pMCU2_Com->set_dut_pwr_off(1, MCU_PIO_DUT_PWR, MCU_PIO_NUM_DUT_PWR);
        //tbox_is_open = true;
        result = true;

        param = "tbox opened OK";
	    if(!result) return(ERR_STOP);
	    return(NO_ERR);
}
#endif



CString GetPathOrFile(CString folder_file_path)
{
        //CString str;
        //if(format && *format) GEN_UNKOWN_PAR_STR(format, str);

        TCHAR szFilePath[MAX_PATH + 1];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(_tcsrchr(szFilePath, _T('\\')))[0] = 0;

        CString str_url = szFilePath;
#if 0
        if(str != "")
        {
                str_url += '\\';
                str_url += str;
        }
        return(str_url);
#else
	//有问题是如果是嵌套文件夹的话，必须保证文件夹是存在的，否则会有bug
	str_url += "\\" + folder_file_path;
	return(str_url);
#endif
}


void GetAllFile(CStringList* &all_file_list,CString  file_filter)
{
        CString str_url = GetPathOrFile(file_filter);

        CFileFind fileFind;
        BOOL re = fileFind.FindFile(str_url);
        if(re)
        {
                while(re)
                {
                        re = fileFind.FindNextFile();
                        if(fileFind.IsDots()) continue;
                        if(fileFind.IsDirectory()) continue;
                        all_file_list->AddTail(fileFind.GetFileName());
                }
        }
}


void CTObj_wtriad::gen_prompt()
{
	char itemstr[8];
       itoa(Index(),itemstr,10);
       CString range;
	range.Format("write mac......");
       tset->tlist->AddItem(itemstr,name,range.GetBuffer(0),"","");

}
//----------------------------------------------------------------------------------
CTObj_wtriad::CTObj_wtriad(const char* pname,testSet *Tset, int ID, 
								   ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
	//man_input_type = cfg.ItemSelect(s_InputAddress_manual);
	//default_address_write_only = cfg.ItemSelect(slaveDefAddrWOnly);
//	file="triad.txt";
	//file=cfg.VCString(s_triad_file);
	TriadList = new CStringList();
	GetAllFile(TriadList,"TriadFile\\*.txt");
}
//yhl 2018/10/27
bool CTObj_wtriad::getline_info(CString file_path,const char* mac,char *content)
{
	ifstream inFile;
		CString cmd_str;
	CString mac_str;
	char buff[256]={0};
	char temp_mac[12]={0};
	char* p;
	int mac_head;
	int mac_end;
	int	temp;
	int	i;
	int   mac_length;
	strcpy(temp_mac,mac);
       strlwr(temp_mac);//大写字符变小写
       strcpy(esp_mac,mac);
       
	inFile.open(file);
	while( !inFile.eof())
	{
		inFile.getline(buff,255);	
		p= strstr(buff,mac);
		if(p)
		{
			strncpy(content,buff,strlen(buff));
			break;
		}

	}
	inFile.close();
	if(strlen(content))
	{
		mac_str=	buff;
		mac_length=strlen(mac_str);
		#if 1
		char *iindex=strchr(buff,'\t');
		strncpy(venorid_send,buff,iindex-buff);
		strncpy(secret_send,iindex+1,32);
		strncpy(esp_mac,iindex+34,12);
		strncpy(pk_send,iindex+47,12);
		#else
           	mac_head = mac_str.Find("vendorId:")+9;// 3
	   	mac_end = mac_str.Find("secret:");// 11
		//十进制位数
		temp = mac_end - mac_head;
		for(i=0;i<temp;i++)
		{	
			if((mac_str[mac_head+ i]==0x0d)||(mac_str[mac_head+ i]==0x09))
			break;
			venorid_send[i]=mac_str[mac_head+ i];
		}
	   	mac_head = mac_str.Find("secret:")+7;// 3
	   	mac_end = mac_str.Find("pk:");// 11
	   	temp = mac_end - mac_head;
	   	for(i=0;i<temp;i++)
	   	{	
			if((mac_str[mac_head+ i]==0x0d)||(mac_str[mac_head+ i]==0x09))
			break;
			secret_send[i]=mac_str[mac_head+ i];
	  	}

	   	mac_head= mac_str.Find("pk:")+3;// 11
	   	for(i=0;;i++)
	   	{	
			pk_send[i]=mac_str[mac_head+ i];
			if((mac_head+ i)>=(mac_length-1))
			break;
	  	}
		#endif
		return true;
	}
	else
	{
		return false;
	}
}
#if 0
	strcat(mac_send,esp_mac);
	strcat(mac_send,convert_sybmol);
	strcat(mac_send,venorid_send);
	strcat(mac_send,convert_sybmol);
	strcat(mac_send,secret_send);
	strcat(mac_send,convert_sybmol);
	strcat(mac_send,pk_send);
	strcat(mac_send,mac_tail);
#endif
//yhl 2018/10/27
int CTObj_wtriad::TestOp(ConfigFile &cfg)
{
          result = false;
        is_tested = true;
	char *b = NULL;
	char *c = NULL;
	CString cmd_str;
       	char buff[256]={0};
	char rx_buf[512]={0};
	CString tmp_mac;	
	int ret_head,ret_end;	
	char *mac;
	char read_mac[]="read mac\r\n";

	if(tset->dev_id == TEST_DEV_ID_B)
	{
		tmp_mac = pMainWnd->m_bdev_address;
			
	}
	else
	{
		tmp_mac = pMainWnd->m_adev_address;
	}
	mac = (char*)((LPCTSTR)tmp_mac);
	//strupr(mac);
	  strlwr(mac);//大写字符变小写
	file="";
	for(int i=0;i<TriadList->GetCount();i++)
	{
			CString temp=TriadList->GetAt(TriadList->FindIndex(i));
			if(temp.GetLength()==29)
			{
				CString start=temp.Mid(0,12);
				b = (char*)((LPCTSTR)start);
				
				CString end=temp.Mid(13,12);

				c = (char*)((LPCTSTR)end);
				if(((strcmp(mac,b)) >= 0) && ((strcmp(mac,c)) <= 0))
				{
					temp = "TriadFile\\" + temp;
					file = GetPathOrFile(temp);
					break;
				}								
			}
	}

	if(file=="")
	{
		param = "get file err";
            return(ERR_NOMAL);
	}

	if(!getline_info(file,mac,buff))
	{
		param = "get triad err";
            	return(ERR_NOMAL);
	}
	else
	{

	}

	esp_send_mac();
	if(tset->dev_id == TEST_DEV_ID_B)
	{            
			if(pRTL8711_2_Com->send_and_search(mac_send, 0, "ss", 0)) //可用
			//if(pRTL8711_2_Com->send_and_get(mac_send, 0, rx_buf, 0, 2000, 256))
			{
					param = "mac successful";
					result = true;

			}
			else
			{
					param = "write mac fail";
					result = false;
			}
			Sleep(1000);
			if(pRTL8711_2_Com->send_and_get(read_mac, 0, rx_buf, 0, 2000, 256))
			{
				strlwr(rx_buf);//大写字符变小写				
			       if  (strcmp(esp_mac, rx_buf)==0)
			       	{
					param ="read mac successful";
					result = true;
			       	}
				else
					{
						param = "read mac fail";
				result = false;
					}
			}
			else
			{
				param = "read mac command fail";
			}
			
		}
	else
		{
		
			if(pRTL8711_1_Com->send_and_search(mac_send, 0, "ss", 0)) //可用
			//if(pRTL8711_1_Com->send_and_get(mac_send, 0, rx_buf, 0, 2000, 256))
			{
					param = "write mac successful";
					result = true;

			}
			else
			{
					param = "write mac fail";
					result = false;

			}
		

	
			//read mac
			Sleep(1000);
			if(pRTL8711_1_Com->send_and_get(read_mac, 0, rx_buf, 0, 2000, 256))
			{
				strlwr(rx_buf);//大写字符变小写				
			       if  (strcmp(esp_mac, rx_buf)==0)
			       	{
			       		param = "read mac successful";

					result = true;
			       	}
				else
					{
				param = "read mac fail";                                                                    
				result = false;
					}
			}
			else
			{
				param = "read mac command fail";
				result = false;
			}
		
			
		}
	return(true);
}
//-----------------------------------------------------------------------------------
int  parse_mac_txt(void)
{
	FILE *file_macptr;
	char buffer[64];
	long int length,j,k,findnext,istrue,find3a;
    BYTE temp; 
	char venorid[64];
    char secret[64];
    char pk[64];
	if((file_macptr=fopen("78da07c2de87_78da07c2ee8a.txt","rb"))==NULL)
	{
		//printf("mac.txt is missing!");
		return 0;
	}
	
	fseek(file_macptr,-1,SEEK_END);	
	length=ftell(file_macptr);
	fseek(file_macptr,0,SEEK_SET);
	memset(buffer,0,64);
       memset(venorid,0,sizeof(venorid));
       memset(secret,0,sizeof(secret));
	memset(pk,0,sizeof(pk));

       memset(venorid_send,0,sizeof(venorid_send));
       memset(secret_send,0,sizeof(secret_send));
	memset(pk_send,0,sizeof(pk_send));	
	j=0;
	for(long i=0;i<=length;i++)
	{
		if(fread(&temp,sizeof(char),1,file_macptr)!=1) 
		{
			printf("mac.txt bad\n",i);
			fclose(file_macptr);
			return 0;
		}
		if ((temp>0x80)||(temp==0x00)||(temp==0x3a)||(temp==0x2d)||(temp==0x20))  continue;  // - : ignore
		if (temp>0x20)
			{
			 if (j<64)
			 	{
                     buffer[j++]=temp;
			 	}

			}
		else
			{
              //mac compare after 09  
              if  (strcmp(esp_mac, buffer)==0)//if it real gain mac code
              	{
                    //find
					/////////////////////////////
					j=0;
					memset(buffer,0,64);
					istrue=0;
					for (k=0;k<256;k++)
						{

	             	      if(fread(&temp,sizeof(char),1,file_macptr)!=1) 
		                  {
			                printf("mac.txt bad\n",i);
			                fclose(file_macptr);
			                return 0;
		                  }
		                 if ((temp>0x80)||(temp==0x00)||(temp==0x2d)||(temp==0x20))  continue;
		                 if (temp>0x20)
			               {
			                 if (j<64)
			 	              {
                               buffer[j++]=temp;
			 	              }
			                }
		                 else
			               {
			                // vendorId find 09 again; 
			                 istrue=1;
			                 strcpy(venorid,buffer);  
							 break;
			                }   
						}
				     ///////////////////////////////////////////		
                        if (istrue==0) return 0;
					/////////////////////////////
					j=0;
					memset(buffer,0,64);
					istrue=0;
					for (k=0;k<256;k++)
						{

	             	      if(fread(&temp,sizeof(char),1,file_macptr)!=1) 
		                  {
			                printf("mac.txt bad\n",i);
			                fclose(file_macptr);
			                return 0;
		                  }
		                 if ((temp>0x80)||(temp==0x00)||(temp==0x2d)||(temp==0x20))  continue;
		                 if (temp>=0x20)
			               {
			                 if (j<64)
			 	              {
                               buffer[j++]=temp;
			 	              }
			                }
		                 else
			               {
			                // secret find 09 again; 
			                istrue=1;
			                 strcpy(secret,buffer);   
							 break;
			                }   
						}
				     ///////////////////////////////////////////		
                        if (istrue==0) return 0;
					/////////////////////////////
					j=0;
					memset(buffer,0,64);
					istrue=0; 
					for (k=0;k<256;k++)
						{

	             	      if(fread(&temp,sizeof(char),1,file_macptr)!=1) 
		                  {
			                printf("mac.txt bad\n",i);
			                fclose(file_macptr);
			                return 0;
		                  }
		                 if ((temp>0x80)||(temp==0x00)||(temp==0x2d)||(temp==0x20))  continue;
		                 if (temp>0x20)
			               {
			                 if (j<64)
			 	              {
                               buffer[j++]=temp;
			 	              }
			                }
		                 else
			               {
			                // pk find 09 again; 
			                 istrue=1;
			                 strcpy(pk,buffer);         
							 break;
			                }   
						}
				     ///////////////////////////////////////////						
				     if (istrue==0) return 0;
					 else
					 	{
                            j=0; 
							find3a=0;
							 temp=strlen(venorid);
                            for (k=0;k<temp;k++ )
                            {
                              if (venorid[k] ==0x3a) 
                              	{
                                  		find3a=1; //find :
								  continue;
                              	}
                              	if (find3a ==0) continue;
                              venorid_send[j++]=venorid[k];
                            } 
							  venorid_send[j]=':';//yhl 2018/10/25
							j=0; 
							find3a=0;
							temp=strlen(secret);
			    
				  
                            for (k=0;k<temp;k++ )
                            {
                              if (secret[k] ==0x3a) 
                              	{
                                  find3a=1; //find :
								  continue;
                              	}
                              if (find3a ==0) continue;
                            
                              secret_send[j++]=secret[k];
                            }   
							secret_send[j]=':';//yhl 2018/10/25
							j=0;
							find3a=0;
							temp=strlen(pk);
				 
                            for (k=0;k<temp;k++ )
                            {
                              if (pk[k] ==0x3a) 
                              	{
                                  find3a=1;//find :
								  continue;
                              	}
                              if (find3a ==0) continue;
                            
                              pk_send[j++]=pk[k];
                            }    	
							//pk_send[j]='\r';
					 	  return 1;	
					 	}
					
              	}
			  else
			  	{
                  memset(buffer,0,64);
				  j=0;
				  continue;
					
			  	}
			}

	}
	#if 0
	if(fgetc(file_macptr)==EOF)
		printf("complished fail \n");
	else 
		printf("failed\n");
       #endif
	fclose(file_macptr);
    return 0; 


}
//-------------------------------------------------------------------------------
int  esp_send_mac(void)
{
	char mac_head[]="NPST+E";
	char mac_tail[]="\r\n";
	char convert_sybmol[]=":";
	memset(mac_send,0,sizeof(mac_send));
	//memcpy(mac_send,mac_head,sizeof(mac_head));
	//strupr(esp_mac);//小写变大写	
	strcat(mac_send,esp_mac);
	strcat(mac_send,convert_sybmol);
	strcat(mac_send,pk_send);
	strcat(mac_send,convert_sybmol);
	strcat(mac_send,secret_send);
	strcat(mac_send,convert_sybmol);
	strcat(mac_send,venorid_send);
	strcat(mac_send,mac_tail);
	return(1);
}
