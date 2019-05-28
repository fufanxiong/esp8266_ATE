// PD218.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PD218.h"
#include "PD218Dlg.h"
#include "config_deal.h"
#include "Newdevice.h"
#include "tset.h"
#include "selDutCom.h"

#include <TLHELP32.H>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString Cfj_file_ini = "config.ini";
CString Cfj_file_bin = "config.bin";
#if 0
CString super_password = "12345+54321";
CString creat_cfj_pin = "54321*12345";
CString modify_pin = "987654321+123456789";
CString valid_date_pin = "jx-bt-12-08-23";
CString authorization_pin = "jx-bt-16-03-29";
CString setpar_password = "123+321";
CString configfile_password = "12345*54321";
CString cfg_file_mlevel = "jx-bt-12345*54321";
CString cfg_file_hlevel = "jx-bt-987651234*432156789";
CString clr_tcount_pin = "123456";
#else
CString super_password = "123456";
CString creat_cfj_pin = "123456";
CString modify_pin = "123456";
CString valid_date_pin = "123456";
CString authorization_pin = "123456";
CString setpar_password = "123456";
CString configfile_password = "123456";
CString cfg_file_mlevel = "123456";
CString cfg_file_hlevel = "123456";
CString clr_tcount_pin = "123456";
#endif

int PasswordVerify(CString* code,const char* show);
bool PasswordVerify(int code,const char* show);
bool modify_valid_date(void);

/////////////////////////////////////////////////////////////////////////////
// CPD218App

BEGIN_MESSAGE_MAP(CPD218App, CWinApp)
	//{{AFX_MSG_MAP(CPD218App)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPD218App construction

CPD218App::CPD218App()
{
	// TODO: add construction code here
	// Place all significant initialization in InitInstance

	m_pMainWnd = NULL;
	pCfgfile = NULL;
}

CPD218App::~CPD218App()
{
	if(pCfgfile) delete pCfgfile;
	pCfgfile = NULL;


}


/////////////////////////////////////////////////////////////////////////////
// The one and only CPD218App object

CPD218App theApp;
Instrument_Interface Inst; 

CString PortMutexString;

/////////////////////////////////////////////////////////////////////////////
// CPD218App initialization

BOOL CPD218App::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	//_CrtSetBreakAlloc(8926);

	if(!ini_config_par()) return FALSE;
	
	if(!valid_date_verify()) return FALSE;
	
	//if(!selectSpiPort()) return FALSE;   //SPI
	
	//BModeSelected = pCfgfile->ItemSelect(s_BModle_Sel);  //B模块是否选择?
	
	//PortMutexString = "JX_BT_ATE";
	HANDLE hMutex = CreateMutex(NULL,true,MUTEX_TEST_OBJ_PRO);
	if(hMutex && (ERROR_ALREADY_EXISTS == GetLastError())) 
	{
		CloseHandle(hMutex);
		ActiveMyInstance();
		return FALSE;
	}

	CPD218Dlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{

	}
	else if (nResponse == IDCANCEL)
	{
	}

    

	// Since the dialog has been closed, return FALSE so that we exit the
	// application, rather than start the application's message pump.
	CloseHandle(hMutex);
	return FALSE;
}



bool CPD218App::selectSpiPort()
{
	//if(!pMainWnd->terminal_test && !is_bt_spi_allocate())

	
//#if 0	
	//selDutCom *dlg = new selDutCom(pCfgfile, pCfgfile->ItemSelect(slaveUsbConnectEn));
	selDutCom *dlg = new selDutCom(pCfgfile, false);
	int Response = dlg->DoModal();
	//bool is_null_port_list = (dlg->SPIPorts->hlist == NULL);
//#if 0

    if (Response != IDOK)
	{
		//if(is_null_port_list) AfxMessageBox("no spi port find!");
		return FALSE;
	}


	SPISource *tport = dlg->spi_portA;
	if(!tport) {
		AfxMessageBox("invalid port selected \n"
			"program terminal now!");
		return FALSE;
	}

	PortMutexString = tport->port_name();
	PortMutexString += tport->port_num();
	if(PortMutexString == "") {
		AfxMessageBox("port is null \n"
			"program terminal now!");
		return FALSE;
	}

	if(tport) Spi_portA = new SPISource(0,tport->port_name(),tport->port_num(),tport->port_type());

	if(pCfgfile->ItemSelect(s_BModle_Sel)){
	  tport = dlg->spi_portB;
	  if(!tport) {
		 AfxMessageBox("invalid port selected \n"
			"program terminal now!");
		 return FALSE;
	  }

	  PortMutexString = tport->port_name();
	  PortMutexString += tport->port_num();
	  if(PortMutexString == "") {
		  AfxMessageBox("port is null \n"
			  "program terminal now!");
		  return FALSE;
	  }

      if(tport) Spi_portB = new SPISource(0,tport->port_name(),tport->port_num(),tport->port_type());
	}

	delete(dlg);
	dlg = NULL;

	return(TRUE);
}


void CPD218App::ActiveMyInstance(void)
{
	HWND hWndPrevious = ::GetWindow(::GetDesktopWindow(),GW_CHILD);     
	while(::IsWindow(hWndPrevious)) 
	{
		if(::GetProp(hWndPrevious,MUTEX_TEST_OBJ_PRO)) 
		{
			if(::IsIconic(hWndPrevious)) 
			{
				::ShowWindow(hWndPrevious,SW_RESTORE);        
				::SetForegroundWindow(hWndPrevious);        
			}
			else ::SetForegroundWindow(::GetLastActivePopup(hWndPrevious));    
			return;   
		}
		hWndPrevious = ::GetWindow(hWndPrevious,GW_HWNDNEXT);   
	}
	AfxMessageBox("已有一个实例在运行,请先关掉!");
}



bool CPD218App::ini_config_par(void)
{
#if 0
	pCfgfile = new ConfigFile();
	if(pCfgfile->readfile("config.bin") != 1) 
	{
		delete(pCfgfile);

		int nResponse = IDCANCEL;
		CString pin_temp[] = { creat_cfj_pin, "" };
		if(!PasswordVerify(pin_temp,"请输入密码进入配置文件创建页!")) 
		{
			AfxMessageBox("读配置文件出错,请提供有效配置文件!");
			return FALSE;
		}
		pCfgfile = new ConfigFile();
		//config_deal dlg;
		config_deal dlg(CFG_FILE_OP_LEVEL3);
		dlg.DoModal();
	}
	return(true);
#endif
   
	pCfgfile = new ConfigFile();
	if(pCfgfile->readfile("config.bin",true,CFG_FILE_OP_LEVEL0) != 1) 
	{
		delete(pCfgfile);
		pCfgfile = NULL;

		int nResponse = IDCANCEL;
		if(!PasswordVerify(PASSWORD_ID_CREAT,"请输入密码进入配置文件创建页!")) 
		{
			AfxMessageBox("读配置文件出错,请提供有效配置文件!");
			return FALSE;
		}
		pCfgfile = new ConfigFile();
		config_deal dlg(CFG_FILE_OP_LEVEL3);
		dlg.DoModal();

		CTime Time = CTime::GetCurrentTime();
		CString temp = Time.Format("%Y-%m-%d %H:%M:%S");
		int pos = temp.Find("-");
		temp = temp.Left(pos);
		pCfgfile->ItemSet(ParNewYear,temp);
		
		pCfgfile->update_config_to_disk();
	}
	return(true);
}

bool CPD218App::valid_date_verify(void)
{
	CTime endTime = CTime::GetCurrentTime();
	bool can_adj = false;
___try_veriy:
	bool date_valid = pCfgfile->ItemSelect(ParDateValid);
	bool valid2 = true;

	int yearCfg = pCfgf.Vulong(ParNewYear);
	int yearCur = endTime.GetYear();
	
	int monthCfg = pCfgf.Vulong(ParNewMonth);
    int monthCur = endTime.GetMonth();

	int dayCfg = pCfgf.Vulong(ParNewDay);
	int dayCur = endTime.GetDay();

	if(valid2)
	{
		/*
		if(pCfgf.Vulong(ParNewYear) > (unsigned long)endTime.GetYear());
		else if(pCfgf.Vulong(ParNewYear) < (unsigned long)endTime.GetYear()) valid2 = false;
		else if(pCfgf.Vulong(ParNewMonth) > (unsigned long)endTime.GetMonth());
		else if(pCfgf.Vulong(ParNewMonth) < (unsigned long)endTime.GetMonth()) valid2 = false;
		else if(pCfgf.Vulong(ParNewDay) >= (unsigned long)endTime.GetDay());
		else if(pCfgf.Vulong(ParNewDay) < (unsigned long)endTime.GetDay()) valid2 = false;
		*/

		if((yearCfg < yearCur) || (monthCfg < monthCur) || (dayCfg < dayCur))
		{
			valid2 = false;
		}
	}
	if(date_valid != valid2) 
	{
		if(can_adj) 
		{
			date_valid = valid2;
			if(date_valid) pCfgfile->ItemSet(ParDateValid,"yes");
		}
		else if(date_valid) 
		{
			date_valid = false;
			pCfgfile->ItemSet(ParDateValid,"no");
		}
	}

	can_adj = false;
	if(!date_valid) AfxMessageBox("ATE date is invalid!");
	else return(true);


    if (!PasswordVerify(PASSWORD_ID_VALID_DATE,"")) return false;
	can_adj = true;
	modify_valid_date();

	/*
	CString pin_temp[] = { 
		pCfgf.VCString(ParSuperPassword), 
		modify_pin, 
		valid_date_pin,
		"" };

	int pin_res = PasswordVerify(pin_temp,"");
	if(!pin_res) return FALSE;
	if(3 == pin_res) {
		can_adj = true;
		modify_valid_date();
	} */
	goto ___try_veriy;
}
