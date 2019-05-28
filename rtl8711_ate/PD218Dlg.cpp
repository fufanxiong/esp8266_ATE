// PD218Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "PD218.h"
#include "PD218Dlg.h"
#include "GPIB.h"
#include "err.h"
#include "password.h"
#include "TestItem.h"
#include "config_deal.h"
#include "cderr.h "   		//for   definition   of   FNERR_BUFFERTOOSMALL
#include "input_address.h"
#include "new_password.h"
#include "new_date.h"
#include "selDutCom.h"
#include "Dongle.h"
#include "cpu.h"
#include "soundIOlib.h"
#include "selDir.h"
#include "setpar.h"
#include <fstream.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>
#include <Dbt.h>
#include "audioRouting.h"
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <math.h>
#include "rfpwr_loss_cal.h"
#include "ledAdcCal.h"
#include "Config.h"
#include "config_set.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CString bt_ate_version;
const char *wtitle = "wait terminal";
const char str_title[] = "ESP8266_ATE";
const char str_ATEST[] = "A model 校验";
const char str_BTEST[] = "B model 校验";
CString str_test_st_running = "测试中...";
CString str_test_result_ok = "测试成功";
CString str_test_result_fail = "测试失败";
CString str_audio_fail = "AUDIO IO FAIL";
CString str_audio_pass = "AUDIO IO PASS";
CString str_a_modle_start = "A模块测试";
CString str_b_modle_start = "B模块测试";
CString str_stop_cfg_audio = "停止音频通路调整";
CString str_audio_mute_left = "左声道静音";
CString str_audio_unmute_left = "左声道取消静音";
CString str_audio_mute_right = "右声道静音";
CString str_audio_unmute_right = "右声道取消静音";
CString str_stop_auto_run = "停止自动化测试";
CString str_start_auto_run = "开始自动化测试";

extern CString creat_cfj_pin;
extern CString cfg_file_mlevel;
extern CString cfg_file_hlevel;

extern CString modify_pin;
extern CString valid_date_pin;
extern CString authorization_pin;
extern CString clr_tcount_pin;

//extern volatile bool tbox_operating;

extern volatile bool tbox_atset_is_open;
extern volatile bool tbox_btset_is_open;
extern volatile bool tbox_is_open;

extern volatile bool dut_spi_is_on;
extern volatile bool dut_pwr_is_on;

extern volatile bool dut_spi_atset_is_on;
extern volatile bool dut_spi_btset_is_on;

extern DWORD WINAPI doDeviceConnect(LPVOID pnull);
//AfxMessageBox("DUT电源打开失败");                                     



#define START_AMODLE() do{  \
               if(tSetA_enable){   \
			       ATestRunning = true; \
				   star_test((LPVOID)tSetA);  \
			   }  \
		   }while(0)
#define START_BMODLE() do{  \
         if( pCfgf.ItemSelect(s_BModle_Sel)){  \
		      BTestRunning = true;  \
			  star_test((LPVOID)tSetB);  \
		 }      \
		   }while(0)



#if 0
#define START_AMODLE()  do{                                                     \
        if(tSetA_enable){                                                       \
                tSetA->pwr_opened = false;                                      \
                tSetA->tmode_pwr_opened = false;                                \
                ATestRunning = true;                                            \
                star_test((LPVOID)tSetA);                                       \
        }                                                                       \
}while(0)
#define START_BMODLE()  do{                                                     \
        if(tSetB_enable){                                                       \
                tSetB->pwr_opened = false;                                      \
                tSetB->tmode_pwr_opened = false;                                \
                BTestRunning = true;                                            \
                star_test((LPVOID)tSetB);                                       \
        }                                                                       \
}while(0)
#endif

#pragma comment (lib,"IDlib.lib")
#pragma comment (lib,"soundIOlib.lib")


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int PasswordVerify(CString* code,const char* show)
{
	password *pdlg;
	int nResponse = IDCANCEL;

	CString* pcode = NULL;
	if(code) pcode = code;
	else return(false);

	while(true) 
    {
		pdlg = new password(show);
		nResponse = pdlg->DoModal();
		if(nResponse == IDOK) 
        {
			int i = 0;
			while(true) 
            {
				if(pcode[i] == "") break;
				if(pdlg->m_EData == pcode[i]) 
                                {
					delete(pdlg);
					return(i+1);
				}
				i += 1;
			}

			if(pcode[i] == "") 
            {
				nResponse = IDCANCEL;
				ThrowErrMessage(PASSWORD_ERR);
			}
		}
        else if(nResponse == IDCANCEL) 
        {
			delete(pdlg);
			return(false);
		}
		delete(pdlg);
	}
	return(false);
}



bool PasswordVerify(int code,const char* show)
{
	password *pdlg;
	int nResponse = IDCANCEL;
	CString* pcode = NULL;
	
	if(code >= PASSWORD_ID_MAX) return(false);
	CString pin_temp[PASSWORD_ID_MAX+1];
	for(int i = 0;i < PASSWORD_ID_MAX;i++) pin_temp[i] = creat_cfj_pin;
	/*ConfigFile *cfg = theApp.pCfgfile;
	if(cfg)
	{
		pin_temp[PASSWORD_ID_PAR] = cfg->VCString(ParSetParPassword);
		pin_temp[PASSWORD_ID_SEL_TITEM] = super_password;//cfg->VCString(ParSuperPassword);
		pin_temp[PASSWORD_ID_CFG] = cfg->VCString(ParCfgFilePassword);
		pin_temp[PASSWORD_ID_CFG_MLV] = cfg_file_mlevel;
		pin_temp[PASSWORD_ID_CFG_HLV] = cfg_file_hlevel;
		pin_temp[PASSWORD_ID_VALID_DATE] = cfg->VCString(ParModifyDatePassword);
		pin_temp[PASSWORD_ID_AUTH] = cfg->VCString(ParAuthorizePassword);
		pin_temp[PASSWORD_ID_CREAT] = creat_cfj_pin;
		pin_temp[PASSWORD_ID_CLR_TCOUNT] = clr_tcount_pin;
	}*/
	pcode = pin_temp;
	
	while(true) 
	{
		pdlg = new password(show);
		nResponse = pdlg->DoModal();
		if(nResponse == IDOK) 
		{
			if(pdlg->m_EData == pcode[code])
			{
				delete(pdlg);
				return(true); 
			}
			else
			{
				nResponse = IDCANCEL;
				ThrowErrMessage(PASSWORD_ERR);
			}
		}
		else if(nResponse == IDCANCEL) 
		{
			delete(pdlg);
			return(false);
		}
		delete(pdlg);
	}
	return(false);
}

bool modify_super_password(void)
{
	new_password *pdlg;
	int nResponse = IDCANCEL;
	while(true) 
        {
		pdlg = new new_password();
		nResponse = pdlg->DoModal();
		if(nResponse == IDCANCEL) 
                {
			delete(pdlg);
			return(false);
		}
		if(nResponse == IDOK) 
		{
			if(pdlg->m_old_password != pCfgf.VCString(ParSuperPassword)) 
			{
				nResponse = IDCANCEL;
				AfxMessageBox("密码错误!\n");
			}
			else if(pdlg->m_confirm_password != pdlg->m_new_password) 
			{
				nResponse = IDCANCEL;
				AfxMessageBox("新输入的密码与确认输入码不一至!\n");
			}
			else if(pdlg->m_confirm_password == "") 
			{
				nResponse = IDCANCEL;
				AfxMessageBox("密码不能为空!\n");
			}
			else 
			{
				pCfgf.ItemSet(ParSuperPassword,pdlg->m_confirm_password);
				AfxMessageBox("新密码设置成功!\n");
				return(true);
			}
		}
		delete(pdlg);
	}
	return(false);
}

bool modify_valid_date(void)
{
	new_date *pdlg;
	int nResponse = IDCANCEL;
	bool res = false;
	
	pdlg = new new_date(pCfgf.Vulong(ParNewYear),pCfgf.Vulong(ParNewMonth),pCfgf.Vulong(ParNewDay));
	nResponse = pdlg->DoModal();
	if(nResponse == IDCANCEL) 
    {
		delete(pdlg);
		return(false);
	}
	if(nResponse == IDOK) 
    {
		pCfgf.ItemSet(ParNewYear,pdlg->m_year);
		pCfgf.ItemSet(ParNewMonth,pdlg->m_month);
		pCfgf.ItemSet(ParNewDay,pdlg->m_day);
		pCfgf.ItemSet(ParDateValid,"yes");
		res = true;
	}
	delete(pdlg);
	return(res);
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CPD218Dlg dialog
CPD218Dlg::CPD218Dlg(CWnd* pParent /*=NULL*/) : CDialog(CPD218Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPD218Dlg)
	m_adev_address = _T("");
	m_bdev_address = _T("");
	//}}AFX_DATA_INIT

	addr_input_remind = false;
	selDutComDlg = NULL;    //SPI select
	all_device_connected = 0;
	terminal_test = 0;
	//global resouce init
	glbResource_ini();

	orderNumber = "PO2350120160521-01";
	workNumber = "liu001";
	last_keyboard_input = 0;
	last_keyboard_input_len = 0;
	adev_address_inputed = 0;
	bdev_address_inputed = 0; 

	tSetA = NULL;
	tSetB = NULL;
	cur_tSet = NULL;
	FCounter = NULL;
	DUT_A_MCU = NULL;
	DUT_B_MCU = NULL;
	DUT_A_RTL8711 = NULL;
	DUT_B_RTL8711 = NULL;
	for(int i=0; i<(MAX_DUT_GLODEN_NUM + 1); i++) dutGloden[i] = NULL;
	for(i=0; i <MAX_DUT_COLOR_SNR_NUM; i++) dutSensor[i] = NULL;

	creat_dut_par();
	//#if 0
	PBtTestSet = NULL;
	m8852_locked = false;

	//////////////Instruments//////////////
	//PSpecAnaly = NULL;
	Multimeter = NULL;
	tbox = NULL;    
	///////////////////////////////////////

	audio_tdata_len = 0;
	audio_tdata_idx = 0;
	cnrouting_running = false;
	audio_rout_configed = false;
	rec_num = 0;
	rec_running = false;
	rec_terminal = true;
	wave_data_ready = false;
	rec_data_start = false;
	wave_data = NULL;
	soundio = NULL;

	authorization = false;
	address_input_running = 0;
	ATestRunning = false;
	BTestRunning = false;
	auto_run_enable = false;
	force_close_window = false;

	//add led indication
	start_btn_is_pressed = false;

	single_Aflag = false;		//fu init add flag
	single_Bflag = false;
//#endif

	m_CurTObjbrush.CreateSolidBrush(RGB(0,255,0));
	m_CurTObjfont.CreatePointFont(200,"宋体");
	
	m_CurTItembrush.CreateSolidBrush(RGB(0,255,0));
	m_CurTItemfont.CreatePointFont(300,"宋体");
	
	m_ConInsbrush.CreateSolidBrush(RGB(0,255,0));
	m_ConInsfont.CreatePointFont(200,"宋体");
	
	m_BtAddressBrush.CreateSolidBrush(RGB(0,0,213));
	m_BtAddressFont.CreatePointFont(300,"宋体");
	
	m_testNumBrush.CreateSolidBrush(RGB(0,255,0));
	m_testNumFont.CreatePointFont(150, "宋体");

	m_butonBrush.CreateSolidBrush(RGB(0,255,0));
	m_butonFont.CreatePointFont(200,"宋体");

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CPD218Dlg::creat_dut_par(void)
{
	   ConfigFile &cfg = pCfgf;
	   for(int i=0;i<MAX_DUT_GLODEN_NUM+1;i++)
	   {
		   if(dutGloden[i]) delete dutGloden[i];
		   dutGloden[i] = NULL;
	   }
	   
	   dutGloden[0] = new CDutGlodenPar(s_dut_address,cfg);
	   
       dutGloden[1] = new CDutGlodenPar(s_G11_address,cfg);
	   dutGloden[2] = new CDutGlodenPar(s_G12_address,cfg);
	   dutGloden[3] = new CDutGlodenPar(s_G13_address,cfg);
	   
	   dutGloden[4] = new CDutGlodenPar(s_G21_address,cfg);
	   dutGloden[5] = new CDutGlodenPar(s_G22_address,cfg);
	   dutGloden[6] = new CDutGlodenPar(s_G23_address,cfg);
	   
	   dutGloden[7] = new CDutGlodenPar(s_G31_address,cfg);
	   dutGloden[8] = new CDutGlodenPar(s_G32_address,cfg);
	   dutGloden[9] = new CDutGlodenPar(s_G33_address,cfg);
	   
	   dutGloden[10] = new CDutGlodenPar(s_G41_address,cfg);
	   dutGloden[11] = new CDutGlodenPar(s_G42_address,cfg);
	   dutGloden[12] = new CDutGlodenPar(s_G43_address,cfg);
	   
	   dutGloden[13] = new CDutGlodenPar(s_dut_address,cfg);
	   
	   //sensor par
	   for(i=0; i<MAX_DUT_COLOR_SNR_NUM; i++)
	   {
		   if(dutSensor[i]) delete dutSensor[i];
		   dutSensor[i] = NULL;
	   }
#if 0
	   dutSensor[0] = new CDutSensorPar(s_U45_sel, cfg);
	   dutSensor[1] = new CDutSensorPar(s_U49_sel, cfg);
	   dutSensor[2] = new CDutSensorPar(s_U51_sel, cfg);
	   dutSensor[3] = new CDutSensorPar(s_U55_sel, cfg);
	   dutSensor[4] = new CDutSensorPar(s_U56_sel, cfg);
	   dutSensor[5] = new CDutSensorPar(s_U60_sel, cfg);
#endif
}


DWORD WINAPI terminal_running_thread(LPVOID null_par)
{
        CPD218Dlg *pdlg = pMainWnd;
        
		pdlg->force_close_window = true;
        pdlg->auto_run_enable = false;
        pdlg->terminal_test = 1;
        pdlg->stop_audio_cfg = true;
        if(pdlg->Multimeter) pdlg->Multimeter->thread_running = false;
        SetEvent(pdlg->EvenyWaveshow);

        //wait connect routing finish
        if(pdlg->cnrouting_running) 
        {
			Sleep(150);
			pdlg->stop_audio_deal(&pdlg->audio_obj);
        } 
        CANSEL_MSG_BOX();

        CString temp;
        int i = 0;
		unsigned long time = 10000;
        HWND childHwnd;
        while(time)
        {
                if(!pdlg->have_thread_run() && !pCfgf.writing_file) break;
                if(!(i % 10))
                {
                        childHwnd = FindWindowEx(NULL,NULL,NULL,wtitle);
                        temp.Format("Wait thread terminal:%d", time);
                        PROMPT_MSG_BOX(&temp);
                }
                Sleep(100);
                if(childHwnd != NULL) 
                {
                        ::SendMessage(childHwnd,WM_CLOSE,0,0);
                        childHwnd = NULL;
                }
                time -= 100;
                i += 1;
        }
        CANSEL_MSG_BOX();

        ::PostMessage(pdlg->m_hWnd,WM_CLOSE,0,0);
        return(0);
}

CPD218Dlg::~CPD218Dlg()
{
        //wait connect routing finish
        if(cnrouting_running) 
        {
                Sleep(150);
                stop_audio_deal(&audio_obj);
        }

        if(tSetA)
        {     
			    if(tSetA->hthread) TerminateThread(tSetA->hthread, EXIT_FAILURE);
                CloseHandle(tSetA->hthread);
                tSetA->hthread = NULL;
                tSetA->hthreadId = NULL;
           #if 0
				if(tSetA->myProc_Handle) TerminateThread(tSetA->myProc_Handle, EXIT_FAILURE);
				CloseHandle(tSetA->myProc_Handle);
                tSetA->myProc_Handle = NULL;
				tSetA->myProc_HandleId = NULL;
            
                if(DUT_B_MCU)
				{
				  DUT_B_MCU->set_LED_off(1, MCU_PIO_BLUE_LED,MCU_PIO_NUM_BLUE_LED);
				  DUT_B_MCU->set_LED_off(1, MCU_PIO_GREEN_LED,MCU_PIO_NUM_GREEN_LED);
				  DUT_B_MCU->set_LED_off(1, MCU_PIO_RED_LED,MCU_PIO_NUM_RED_LED);
				}
          #endif
        }

        if(tSetB)
        { 
                if(tSetB->hthread) TerminateThread(tSetB->hthread, EXIT_FAILURE);
                CloseHandle(tSetB->hthread);
                tSetB->hthread = NULL;
                tSetB->hthreadId = NULL;
           #if 0
				if(tSetB->myProc_Handle) TerminateThread(tSetB->myProc_Handle, EXIT_FAILURE);	
				CloseHandle(tSetB->myProc_Handle);
                tSetB->myProc_Handle = NULL;
				tSetB->myProc_HandleId = NULL;	
				if(DUT_A_MCU)
				{
				  DUT_A_MCU->set_LED_off(0, MCU_PIO_BLUE_LED,MCU_PIO_NUM_BLUE_LED);
				  DUT_A_MCU->set_LED_off(0, MCU_PIO_GREEN_LED,MCU_PIO_NUM_GREEN_LED);
				  DUT_A_MCU->set_LED_off(0, MCU_PIO_RED_LED,MCU_PIO_NUM_RED_LED);
				}
          #endif
        }
 
		if(Multimeter) delete Multimeter;
        Multimeter = NULL;

		if(tbox)
		{
			DUT_B_MCU->set_dut_led_off(1, MCU_PIO_LED, MCU_PIO_NUM_LED);  //退出程序，关掉照明LED灯
			delete tbox;
		}
        tbox = NULL;
		
		//if(PSpecAnaly) delete PSpecAnaly;
 	    //PSpecAnaly = NULL;
		
		if(PBtTestSet) delete PBtTestSet;
		PBtTestSet = NULL;
		
		//if(pwr_board) delete pwr_board;
		//pwr_board = NULL;

  		if(pSPIPortA) {
			delete pSPIPortA;
		}
		pSPIPortA = NULL;
		
		if(pSPIPortB) delete pSPIPortB;
		pSPIPortB = NULL;
		
        if(tSetA) delete tSetA;
        tSetA = NULL;

        if(tSetB) delete tSetB;
        tSetB = NULL;

        

		if(FCounter) delete FCounter;
	    FCounter = NULL;
		
        //global resource clr
		glbResource_clr();

	    ATestRunning = false;
        BTestRunning = false;
        
        if(soundio) delete soundio;
        soundio = NULL;
        if(wave_data) delete []wave_data;
        wave_data = NULL;
		//if(wave_data_draw) delete []wave_data_draw;
        //wave_data_draw = NULL;
        DeleteCriticalSection(&rec_cs);

        //DeleteCriticalSection(&update_cfg_file_cs);
        CloseHandle(EvenyWaveshow);


		for(int i=0;i<MAX_DUT_GLODEN_NUM+1;i++)
        {
			if(dutGloden[i]) delete dutGloden[i];
            dutGloden[i] = NULL;
        }

		for(i=0; i<MAX_DUT_COLOR_SNR_NUM; i++)
		{
			if(dutSensor[i]) delete dutSensor[i];
			dutSensor[i] = NULL;
		}

		if(DUT_A_MCU)
		{	
			delete DUT_A_MCU;
		}
		DUT_A_MCU = NULL;

		if(DUT_B_MCU)
		{
			delete DUT_B_MCU;
		}
		DUT_B_MCU = NULL;
		
		if(DUT_A_RTL8711)
		{
			delete DUT_A_RTL8711;
		}
		DUT_A_RTL8711 = NULL;
		
		if(DUT_B_RTL8711)
		{
			delete DUT_B_RTL8711;
		}
		DUT_B_RTL8711 = NULL;
}

void CPD218Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPD218Dlg)
	DDX_Control(pDX, IDC_Audio_pio_pass, m_audio_io_pass);
	DDX_Control(pDX, IDC_Audio_pio_fail, m_audio_io_fail);
	DDX_Control(pDX, IDC_Start, m_StarButten);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_LIST2, m_blist);
	DDX_Text(pDX, IDC_ADEV_ADDRESS, m_adev_address);
	DDX_Text(pDX, IDC_BDEV_ADDRESS, m_bdev_address);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPD218Dlg, CDialog)
	//{{AFX_MSG_MAP(CPD218Dlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_MMCurConfig, OnMMCurConfig)
	ON_BN_CLICKED(IDC_Start, OnStart)
	ON_WM_TIMER()
	ON_WM_INITMENUPOPUP()
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_Audio_pio_fail, OnAudiopiofail)
	ON_BN_CLICKED(IDC_Audio_pio_pass, OnAudiopiopass)
	ON_COMMAND(ID_CN_TOOL_DEV, OnCnToolDev)
	ON_COMMAND(ID_CN_AMODLE, OnCnAmodle)
	ON_COMMAND(ID_CN_Multimeter, OnCNMultimeter)
	ON_COMMAND(ID_CN_TBOX, OnCnTbox)
	ON_COMMAND(ID_CFG_AUDIO_ROUT, OnCfgAudioRout)
	ON_BN_CLICKED(IDC_AUTO_RUN, OnAutoRun)
	ON_WM_CLOSE()
	ON_COMMAND(ID_MENUITEM32789, OnLogDirector)
	ON_COMMAND(ID_MCFG_DLG, OnMcfgDlg)
	ON_COMMAND(ID_MSEL_TITEM, OnMselTitem)
	ON_COMMAND(ID_MSET_DATE, OnMsetDate)
	ON_COMMAND(ID_MAUTHORITY, OnMauthority)
	ON_COMMAND(ID_MPAR_PASSWORD, OnMparPassword)
	ON_COMMAND(ID_MTITEM_PASSWORD, OnMtitemPassword)
	ON_COMMAND(ID_MCFG_PASSWORD, OnMcfgPassword)
	ON_COMMAND(ID_MDATE_PASSWORD, OnMdatePassword)
	ON_COMMAND(ID_MAUTHORITY_PASSWORD, OnMauthorityPassword)
	ON_COMMAND(ID_EXIT_DIALOG, OnExitDialog)
	ON_EN_KILLFOCUS(IDC_ADEV_ADDRESS, OnKillfocusAdevAddress)
	ON_EN_SETFOCUS(IDC_ADEV_ADDRESS, OnSetfocusAdevAddress)
	ON_EN_KILLFOCUS(IDC_BDEV_ADDRESS, OnKillfocusBdevAddress)
	ON_EN_SETFOCUS(IDC_BDEV_ADDRESS, OnSetfocusBdevAddress)
	ON_COMMAND(ID_CLR_TCOUNT, OnClrTcount)
	ON_COMMAND(ID_RFPWR_LOSS_ADJ, OnRfpwrLossAdj)
	ON_BN_CLICKED(IDC_CONTINUE_RUN, OnContinueRun)
	ON_COMMAND(ID_CN_FCounter, OnConnectFCounter)
	ON_COMMAND(ID_lLED_ADC_CAL, OnBeginLEDAdcCal)
	ON_EN_CHANGE(IDC_ADEV_ADDRESS, OnChangeAdevAddress)
	ON_NOTIFY(NM_CLICK, IDC_LIST2, OnClickList2)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList1)
	ON_BN_CLICKED(IDC_CurTestItem, OnCurTestItem)
	ON_BN_CLICKED(IDC_CurTestItem2, OnCurTestItem2)
	ON_BN_CLICKED(IDC_CurTestObj, OnCurTestObj)
	ON_BN_CLICKED(IDC_CurTestObj2, OnCurTestObj2)
	ON_BN_CLICKED(IDC_SBDEV_ADDRESS, OnSbdevAddress)
	ON_BN_CLICKED(IDC_TEST_NUMA, OnTestNuma)
	//}}AFX_MSG_MAP

	ON_WM_DEVICECHANGE()
	ON_MESSAGE(WM_ITEM_MSG,OnTItemMsg)
	ON_MESSAGE(WM_TEST_FINISH,OnTestFinish)
	ON_MESSAGE(WM_TIMER_SPRING_TEST,OnTimerSpringTest)
	ON_MESSAGE(WM_TIMER_NEXT_AUDIO_STATU,OnTimerNextAudioStatu)
	ON_MESSAGE(WM_INPUT_BT_ADDRESS,OnInputAddress)
	//ON_MESSAGE(WM_SHOW_SEL_DUT_DLG,OnSelDutCom)
	ON_MESSAGE(WM_UPDATE_TITLE,OnUpdateTitle)
	ON_MESSAGE(WM_UPDATE_STATIC_FILED,OnUpdateStatic)
	ON_MESSAGE(WM_UPDATE_TITEM_FILED,OnUpdateTitemFiled)
	ON_MESSAGE(WM_CNDEV_FINSH,OnCnDevFinish)
	ON_MESSAGE(WM_CNDEV_FINSH_SUCCESS,OnCnDevFinishSuccess)
	ON_MESSAGE(WM_UPDATE_MULTIMETER,OnUpdateMultimeter)
	ON_MESSAGE(WM_AUDIO_CONFIG,OnAudioConfig)
	ON_MESSAGE(WM_WAVE_SHOW,OnAudioWaveShow)
	ON_MESSAGE(WM_AUDIO_TEST,OnAudioTest)
	ON_MESSAGE(WM_UPDATE_CFG_FILE,OnUpdateConfig)
	ON_MESSAGE(WM_PROMPT_MSG_BOX,OnPromptMsgbox)
	ON_MESSAGE(WM_UPDATE_ADDR_INPUT,OnUpdateAddrInput)
    //ON_MESSAGE(WM_ADDR_INPUT_REMIND,OnAddrInputRemind)
    //ADD REFRESH TEST ITEM LIST
	//ON_MESSAGE(WM_REFRESH_TEST_ITEM_LIST,OnRefreshTestItemList)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPD218Dlg message handlers

BOOL CPD218Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
   
	
	// TODO: Add extra initialization here
	m_audio_io_pass.SetFont(&m_butonFont);
	m_audio_io_fail.SetFont(&m_butonFont);
	m_StarButten.SetFont(&m_butonFont);

        ///InitializeCriticalSection(&update_cfg_file_cs);
        InitializeCriticalSection(&rec_cs);

        EvenyWaveshow = CreateEvent(NULL,FALSE,FALSE,"CWaveRecordEvent");
        wave_data = new short[REC_BLOCK_SIZE];
		//wave_data_draw = new short[REC_BLOCK_SIZE];
        soundio = new CSoundIO();
        if(!wave_data || !soundio || !soundio->is_initialed())
        {
                AfxMessageBox("系统内存不足！！\n无法运行程序！！");
                EndDialog(IDCANCEL);
        }


        creat_tsets();   //初始化
  	    tSetA->CreatTestItemList(tSetA,pCfgf);
	    tSetA->ShowTestItemList();
        tSetB->CreatTestItemList(tSetB, pCfgf);
	    tSetB->ShowTestItemList();
        update_cfg_file_realtime = pCfgf.ItemSelect(slaveSaveCfjRealTime);

	    GetDlgItem(IDC_CurTestObj)->SetWindowText(str_ATEST);
	    GetDlgItem(IDC_CurTestItem)->SetWindowText("");
        GetDlgItem(IDC_CurTestObj2)->SetWindowText(str_BTEST);
	    GetDlgItem(IDC_CurTestItem2)->SetWindowText("");

		GetDlgItem(IDC_TEST_NUMA)->SetWindowText("");
        GetDlgItem(IDC_TEST_NUMB)->SetWindowText("");
      
		show_log_directory();

	    submenuEnable(ID_CN_TBOX, pCfgf.ItemSelect(parTestBoxNeed));

        submenuEnable(ID_CN_FCounter, 0);

		submenuEnable(ID_lLED_ADC_CAL, 0);

        submenuEnable(ID_CN_Multimeter,pCfgf.ItemSelect(s_PwrOff_cur_sel)||
			                           pCfgf.ItemSelect(s_PwrOn_cur_sel));

		submenuEnable(ID_CN_TOOL_DEV, pCfgf.ItemSelect(s_N4010A_TEST_Sel));

		submenuEnable(ID_CFG_AUDIO_ROUT,0);

        update_start_butten(ATestRunning,BTestRunning);   //更新“开始”按钮
	    ::SetProp(m_hWnd,MUTEX_TEST_OBJ_PRO,(HANDLE)1);

        CString temp = str_title;
        temp += bt_ate_version;           //在这里设置窗口版本信息V1.0.5.0
        SetWindowText(temp);

        if(pCfgf.ItemSelect(slaveParAutoRun))
        {
			GetDlgItem(IDC_AUTO_RUN)->SetWindowText(str_start_auto_run);
			GetDlgItem(IDC_AUTO_RUN)->ShowWindow(true);
        }

	update_audio_cfg();
        audio_obj = AAudio_cfg;
///*
	if(!sel_record_dir())
        {
		EndDialog(IDCANCEL);
        }
//*/        
	//AAudio_cfg.RShowID = IDC_CurTestObj;
        //AAudio_cfg.LShowID = IDC_CurTestItem;
        //BAudio_cfg.RShowID = IDC_CurTestObj2;
        //BAudio_cfg.LShowID = IDC_CurTestItem2;
        //audio_obj = AAudio_cfg;

		GetDlgItem(IDC_ADEV_ADDRESS)->SetWindowText("");
        ((CEdit*)GetDlgItem(IDC_ADEV_ADDRESS))->SetSel(-1);
        GetDlgItem(IDC_ADEV_ADDRESS)->SetFont(&m_BtAddressFont,TRUE);
        GetDlgItem(IDC_BDEV_ADDRESS)->SetWindowText("");
        ((CEdit*)GetDlgItem(IDC_BDEV_ADDRESS))->SetSel(-1);
        GetDlgItem(IDC_BDEV_ADDRESS)->SetFont(&m_BtAddressFont,TRUE);
		

        GetDlgItem(IDC_Start)->SetFocus();

	

#if 0
        //显示分配的测试项地址
		CdNode<CTestItem> *pItem = tSetA->TItemList; 
		CString addr;
		CString tmp = "";
		while(pItem)
		{
		    addr.Format("0x%x", pItem);
			tmp += addr;
            tmp += "\n";
			pItem = pItem->next;
			if(pItem == tSetA->TItemList) break;
		
		}
        AfxMessageBox(tmp);    
#endif
        return TRUE;  // return TRUE  unless you set the focus to a control
}

bool CPD218Dlg::update_audio_cfg(void)
{
        int spk_ch_mode = pCfgf.get_spk_ch_mode();
        int ARShowID = 0;
        int ALShowID = 0;
        int BRShowID = 0;
        int BLShowID = 0;

        if(spk_ch_mode == 3)
        { 
                ARShowID = IDC_CurTestObj;
                ALShowID = IDC_CurTestItem;

                BRShowID = IDC_CurTestObj2;
                BLShowID = IDC_CurTestItem2;
        }
        else if(spk_ch_mode == 2)
        {
                ARShowID = IDC_CurTestObj;
                ALShowID = 0;

                BRShowID = IDC_CurTestObj2;
                BLShowID = 0;
        }
        else if(spk_ch_mode == 1)
        {
                ARShowID = 0;
                ALShowID = IDC_CurTestItem;

                BRShowID = 0;
                BLShowID = IDC_CurTestItem2;
        }

        bool rvalue = false;
        if((AAudio_cfg.LShowID != ALShowID) || (AAudio_cfg.RShowID != ARShowID)) rvalue = true;
        AAudio_cfg.LShowID = ALShowID;
        AAudio_cfg.RShowID = ARShowID;
        BAudio_cfg.LShowID = BLShowID;
        BAudio_cfg.RShowID = BRShowID;
        return(rvalue);
}


void CPD218Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPD218Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPD218Dlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
	// TODO: Add your message handler code here
	KillTimer(RESTART_TESTTIMER);
}

HBRUSH CPD218Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO: Change any attributes of the DC here

	CString text;
	CFont *pfont = &m_CurTItemfont;
    int id = pWnd->GetDlgCtrlID();

	switch(id) 
    {
        case IDC_CurTestObj:
        case IDC_CurTestObj2:
		{
			GetDlgItem(id)->GetWindowText(text);
			if((text == str_ATEST) || (text == str_BTEST))
			{ 
				pDC->SelectObject(&m_CurTObjfont);
			}
			else
			{
				pDC->SetTextColor(RGB(15,15,255));
				pfont = &m_BtAddressFont;
				pDC->SetBkMode(TRANSPARENT);
				pDC->SelectObject(pfont);
			}
		}
		break;

        case IDC_CurTestItem:
        case IDC_CurTestItem2:
		{
			GetDlgItem(id)->GetWindowText(text);
			if(text.Find(str_test_result_fail) >= 0) pDC->SetTextColor(RGB(255,0,0));
			else if(text.Find(str_test_result_ok) >= 0) pDC->SetTextColor(RGB(15,15,255));
			else if(text.Find(str_test_st_running) >= 0) pDC->SetTextColor(RGB(15,15,255));
			else 
			{
				pDC->SetTextColor(RGB(15,15,255));
				pfont = &m_ConInsfont;
			}
			pDC->SetBkMode(TRANSPARENT);
			pDC->SelectObject(pfont);
		}
		break;

		case IDC_ADEV_ADDRESS:
		case IDC_BDEV_ADDRESS:
		{
				HBRUSH b;
				CWnd *pFocusWnd = GetFocus();
				if(pFocusWnd && 
					( ((id == IDC_ADEV_ADDRESS) && (pFocusWnd == GetDlgItem(IDC_ADEV_ADDRESS))) || 
					((id == IDC_BDEV_ADDRESS) && (pFocusWnd == GetDlgItem(IDC_BDEV_ADDRESS))) ))
				{
					b = CreateSolidBrush(RGB(255,0,0));
				}
				else 
				{
					if( ((id == IDC_ADEV_ADDRESS) && adev_address_inputed) || 
						((id == IDC_BDEV_ADDRESS) && bdev_address_inputed) )
					{
						b = CreateSolidBrush(RGB(0,255,0));
					}
					else
					{
						b = CreateSolidBrush(RGB(220,220,220));
					}
				}
				
				pDC->SetTextColor(RGB(0,0,255));
				pfont = &m_BtAddressFont;
				pDC->SetBkMode(TRANSPARENT);
				pDC->SelectObject(pfont);
				return(b);
			}
		    break;

        case IDC_SBDEV_ADDRESS:
			{
				pDC->SetTextColor(RGB(130,130,130));
                pfont = &m_BtAddressFont;
                pDC->SetBkMode(TRANSPARENT);
                pDC->SelectObject(pfont);
                HBRUSH b = CreateSolidBrush(RGB(210,210,210));
                return(b);	
			}
			
			break;

		case IDC_TEST_NUMA:
        case IDC_TEST_NUMB:
			{
				pDC->SelectObject(&m_testNumFont);
			}
			break;
		case IDC_Audio_pio_pass:
        case IDC_Start:
        case IDC_Audio_pio_fail:
			break;
	    default:
			break;
	}

	// TODO: Return a different brush if the default is not desired
	return hbr;
}


int CPD218Dlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	return 0;
}

void CPD218Dlg::OnClose() 
{
	if(!force_close_window && have_thread_run())
	{ 
		AfxBeginThread((AFX_THREADPROC)terminal_running_thread,NULL);
		return;
	}
	
	CDialog::OnClose();
}

void CPD218Dlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	::RemoveProp(m_hWnd,MUTEX_TEST_OBJ_PRO);
}

void CPD218Dlg::OnMMCurConfig()
{  
		int password = PasswordVerify(PASSWORD_ID_CFG,"");
        if(!password) return;
		
        int op_lv = CFG_FILE_OP_LEVEL2;
        if(password == PASSWORD_ID_CFG_MLV) op_lv = CFG_FILE_OP_LEVEL1;
        else if(password == PASSWORD_ID_CFG_HLV) op_lv = CFG_FILE_OP_LEVEL0;
		
		config_deal dlg(op_lv);
        dlg.DoModal();
        rebuild_new_test_item();
}

void CPD218Dlg::setupCnDev(void)
{
	cnrouting_running = true;
	all_device_connected = false;
	update_start_butten(true,true,true);
	AfxBeginThread((AFX_THREADPROC)doDeviceConnect,NULL);
}

void CPD218Dlg::OnCnToolDev() 
{
	GetDlgItem(IDC_CurTestObj)->SetWindowText(str_ATEST);
    	GetDlgItem(IDC_CurTestObj2)->SetWindowText(str_BTEST);
	GetDlgItem(IDC_CurTestItem)->SetWindowText("");
	GetDlgItem(IDC_CurTestItem2)->SetWindowText("");

	if(PBtTestSet) delete PBtTestSet;
	PBtTestSet = NULL;
	all_device_connected = false;
	setupCnDev();     
}

void CPD218Dlg::OnCnAmodle() 
{
    GetDlgItem(IDC_CurTestObj)->SetWindowText(str_ATEST);
    GetDlgItem(IDC_CurTestObj2)->SetWindowText(str_BTEST);
	GetDlgItem(IDC_CurTestItem)->SetWindowText("");
	GetDlgItem(IDC_CurTestItem2)->SetWindowText("");

	setupCnDev();
}


void CPD218Dlg::OnCNMultimeter() 
{	
	if(pCfgf.ItemSelect(s_PwrOn_cur_sel)||pCfgf.ItemSelect(s_PwrOff_cur_sel))
	{
		GetDlgItem(IDC_CurTestObj)->SetWindowText(str_ATEST);
        GetDlgItem(IDC_CurTestObj2)->SetWindowText(str_BTEST);
		GetDlgItem(IDC_CurTestItem)->SetWindowText("");
	    GetDlgItem(IDC_CurTestItem2)->SetWindowText("");

		if(Multimeter && Multimeter->pWinThread && Multimeter->thread_running) return; 
		delete Multimeter;
		Multimeter = NULL;
		setupCnDev();
	}
}


void CPD218Dlg::OnConnectFCounter() 
{
	// TODO: Add your command handler code here
	if(0)
	{
		GetDlgItem(IDC_CurTestObj)->SetWindowText(str_ATEST);
        GetDlgItem(IDC_CurTestObj2)->SetWindowText(str_BTEST);
		GetDlgItem(IDC_CurTestItem)->SetWindowText("");
	    GetDlgItem(IDC_CurTestItem2)->SetWindowText("");

		if(pFCounter) delete pFCounter;
		pFCounter = NULL;
		setupCnDev();
	}
}



void CPD218Dlg::OnCnTbox()
{
    //no code
	if(pCfgf.ItemSelect(parTestBoxNeed))
	{
		GetDlgItem(IDC_CurTestObj)->SetWindowText(str_ATEST);
        GetDlgItem(IDC_CurTestObj2)->SetWindowText(str_BTEST);
		GetDlgItem(IDC_CurTestItem)->SetWindowText("");
	    GetDlgItem(IDC_CurTestItem2)->SetWindowText("");

		if(tbox) delete tbox;
		tbox = NULL;
		setupCnDev();
	}
}

void CPD218Dlg::OnCfgAudioRout() 
{
	GetDlgItem(IDC_CurTestObj)->SetWindowText(str_ATEST);
    GetDlgItem(IDC_CurTestObj2)->SetWindowText(str_BTEST);
	GetDlgItem(IDC_CurTestItem)->SetWindowText("");
	GetDlgItem(IDC_CurTestItem2)->SetWindowText("");
	
	audio_rout_configed = false;
    setupCnDev();
}


bool CPD218Dlg::sel_record_dir(void)
{
	selDir sdlg;
	
	int nResponse = sdlg.DoModal();
	if(nResponse != IDOK) return(false);
	if((sdlg.str_order == "") || (sdlg.str_wknum == "")) return(false);
	
	orderNumber = sdlg.str_order;
	workNumber = sdlg.str_wknum;
	show_log_directory();
	return(true);
}



BOOL CPD218Dlg::OnDeviceChange(UINT nEventType,DWORD dwData)
{
		//tSetA->pSpiObj->usb_dongle->put_app_mesg(MSG_CONNECT_USB_DONGLE,NULL,0);
		//tSetB->pSpiObj->usb_dongle->put_app_mesg(MSG_CONNECT_USB_DONGLE,NULL,0);
	return TRUE;	
}

bool CPD218Dlg::have_thread_run(void)
{
        if(ATestRunning || BTestRunning) return(true);
        if(address_input_running) return(true);
        if(cnrouting_running) return(true);
	return(false);
}



void CPD218Dlg::OnPromptMsgbox(WPARAM wParam,LPARAM lParam)
{
        if(lParam != NULL)
        { 
                CString text = *(CString *)lParam;
                MessageBox(TEXT(text),wtitle);
        }
        else
        {
                HWND childHwnd = FindWindowEx(NULL,NULL,NULL,wtitle);
                if(childHwnd != NULL) ::PostMessage(childHwnd,WM_CLOSE,0,0);
        }
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void CPD218Dlg::OnTItemMsg(WPARAM wParam,LPARAM lParam)
{
        ((testSet*)lParam)->DoSelfMsg(wParam);
}

void CPD218Dlg::OnUpdateTitle(WPARAM wParam,LPARAM lParam)
{
        CString title = str_title;
        ConfigFile &cfg = pCfgf;
        //CString temp;

        title += bt_ate_version;
        //set_title_show(&temp);
        //title += temp;

		if(pFCounter)
		{
		        title += "  53131A:";
                title += pFCounter->port_name();
		}

		if(pBtTestSet)
		{
		        title += "  N4010A:";
                title += pBtTestSet->port_name();
		}
		
		if(pMultimeter)
        {
                title += "  Multimeter:";
                title += pMultimeter->port.key.PortName;
        }

		if(cfg.ItemSelect(parTestBoxNeed) && pTbox)
		{
		        title += "  TBox:";
                title += pTbox->port.key.PortName;
		}

		if(pMCU1_Com)
        {
                title += "  U18:";
                title += pMCU1_Com->port.key.PortName;
        }

		if(pMCU2_Com)
        {
                title += "  U5:";
                title += pMCU2_Com->port.key.PortName;
        }
		
		if(pRTL8711_1_Com)
        {
                title += "  ESP8266_A:";
                title += pRTL8711_1_Com->port.key.PortName;
        }

		if(pRTL8711_2_Com)
        {
                title += "  ESP8266_B:";
                title += pRTL8711_2_Com->port.key.PortName;
        }
        SetWindowText(title);
}

void CPD218Dlg::OnUpdateStatic(WPARAM wParam,LPARAM lParam)
{
        GetDlgItem((int)wParam)->SetWindowText(((CString*)lParam)->GetBuffer(0));
}


void CPD218Dlg::OnUpdateTitemFiled(WPARAM wParam,LPARAM lParam)
{

}




void CPD218Dlg::OnCnDevFinish(WPARAM wParam,LPARAM lParam)
{
    cnrouting_running = false;  

	update_start_butten(ATestRunning,BTestRunning);
    GetDlgItem(IDC_CurTestObj)->SetWindowText(str_ATEST);
    
}


//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
static DWORD WINAPI myProc1(LPVOID lpParam)
{
    pMCU2_Com->set_LED_flash(1, MCU_PIO_BLUE_LED, MCU_PIO_NUM_BLUE_LED, 500);  //flash last time 500ms
    return 0;

}

static DWORD WINAPI myProc2(LPVOID lpParam)
{
    pMCU1_Com->set_LED_flash(0, MCU_PIO_BLUE_LED, MCU_PIO_NUM_BLUE_LED, 500); //flash last time 500ms
    return 0;

}

//#if 0
void CPD218Dlg::OnCnDevFinishSuccess(WPARAM wParam,LPARAM lParam)
{
        //MCU U5 初始化
        pMCU2_Com->set_dut_5v_off(1, MCU_PIO_DUT1_5V, MCU_PIO_NUM_DUT1_5V);
        pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY);
		if(pCfgf.ItemSelect(parTestBoxNeed))
		{
		    pMCU2_Com->set_dut_led_on(1, MCU_PIO_LED, MCU_PIO_NUM_LED);
		}
		else
		{
		    pMCU2_Com->set_dut_led_off(1, MCU_PIO_LED, MCU_PIO_NUM_LED);
		}
		pMCU2_Com->set_dut_vreg_off(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG);
		pMCU2_Com->set_dut_spi_off(1, MCU_PIO_SPI,MCU_PIO_NUM_SPI);
        pMCU2_Com->set_dut_current_on(1, MCU_PIO_DUT1_CURRENT,MCU_PIO_NUM_DUT1_CURRENT);
		pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY);
        pMCU2_Com->set_dut_pwr_off(1, MCU_PIO_DUT_PWR, MCU_PIO_NUM_DUT_PWR);
        pMCU2_Com->set_dut1_rout_meter_on(1, MCU_PIO_DUT2_METER, MCU_PIO_NUM_DUT2_METER);
		//green led flash indicate ready for test

#ifdef LED_DEBUG_TO_TEST
		
		tSetA->myProc_Handle = CreateThread(NULL, 0, myProc1, NULL, 0, &tSetA->myProc_HandleId);
#endif
		//MCU U18 初始化
        pMCU1_Com->set_dut_5v_off(0, MCU_PIO_DUT2_5V, MCU_PIO_NUM_DUT2_5V);
        pMCU1_Com->set_dut_current_on(0, MCU_PIO_DUT2_CURRENT,MCU_PIO_NUM_DUT2_CURRENT);
		pMCU1_Com->set_dut_vreg_off(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG);
        pMCU1_Com->set_dut_spk_amp_off(0, MCU_PIO_DUT_SPK_AMP, MCU_PIO_NUM_DUT_SPK_AMP);
        pMCU1_Com->set_8852_route_dut2(0, MCU_PIO_DUT2_RF_ROUTE,MCU_PIO_NUM_DUT2_RF_ROUTE);
        pMCU1_Com->set_rout_4010_on(0, MCU_PIO_DUT_4010_METER,MCU_PIO_NUM_DUT_4010_METER);
        pMCU1_Com->set_dut1_rout_audio_on(0, MCU_PIO_DUT2_AUDIO, MCU_PIO_NUM_DUT2_AUDIO);
        pMCU1_Com->set_test_board_short_pc_off(0, MCU_PIO_AUDIO_CHK, MCU_PIO_NUM_AUDIO_CHK);
		
		if(tSetB_enable)
		{
#ifdef LED_DEBUG_TO_TEST

			tSetB->myProc_Handle = CreateThread(NULL, 0, myProc2, NULL, 0, &tSetB->myProc_HandleId);
#endif
		}
}




void CPD218Dlg::OnUpdateMultimeter(WPARAM wParam,LPARAM lParam)
{
    // IDC_MultimeterVal?   
	if(pCfgf.ItemSelect(s_PwrOff_cur_sel)||pCfgf.ItemSelect(s_PwrOn_cur_sel)) 
		GetDlgItem(IDC_MultimeterVal)->ShowWindow(true);
    else 
		GetDlgItem(IDC_MultimeterVal)->ShowWindow(false);
}

void CPD218Dlg::OnAudioConfig(WPARAM wParam,LPARAM lParam)
{
        int text_idx = 0;     
        CString *Text = (CString *)lParam;
#if 0
        if(wParam == 0)
        {
                GetDlgItem(IDC_CurTestObj)->GetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_CurTestItem)->GetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_CurTestItem2)->GetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_CurTestObj2)->GetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_Start)->GetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_Audio_pio_fail)->GetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_Audio_pio_pass)->GetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_Start)->SetWindowText(str_stop_cfg_audio);
                GetDlgItem(IDC_Audio_pio_fail)->SetWindowText(str_audio_mute_left);
                GetDlgItem(IDC_Audio_pio_pass)->SetWindowText(str_audio_mute_right);
                m_StarButten.EnableWindow(true);
                m_audio_io_pass.EnableWindow(true);
                m_audio_io_fail.EnableWindow(true);
        }
        else
        {
                m_StarButten.EnableWindow(false);
                m_audio_io_pass.EnableWindow(false);
                m_audio_io_fail.EnableWindow(false);
                GetDlgItem(IDC_CurTestObj)->SetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_CurTestItem)->SetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_CurTestItem2)->SetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_CurTestObj2)->SetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_Start)->SetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_Audio_pio_fail)->SetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_Audio_pio_pass)->SetWindowText(Text[text_idx++]);
        }
#endif


		if((wParam == AUDIO_CFG_MSG_0) || (AUDIO_CFG_MSG_2 == wParam))
        {
                GetDlgItem(IDC_CurTestObj)->GetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_CurTestItem)->GetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_CurTestItem2)->GetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_CurTestObj2)->GetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_Start)->GetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_Audio_pio_fail)->GetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_Audio_pio_pass)->GetWindowText(Text[text_idx++]);

                GetDlgItem(IDC_Start)->SetWindowText(str_stop_cfg_audio);
                GetDlgItem(IDC_Audio_pio_fail)->SetWindowText(str_audio_mute_left);
                GetDlgItem(IDC_Audio_pio_pass)->SetWindowText(str_audio_mute_right);

                if(AUDIO_CFG_MSG_2 == wParam)
                { 
                        m_StarButten.EnableWindow(false);
                        m_audio_io_pass.EnableWindow(false);
                        m_audio_io_fail.EnableWindow(false);
                }
                else
                {
                        m_StarButten.EnableWindow(true);
                        m_audio_io_pass.EnableWindow(true);
                        m_audio_io_fail.EnableWindow(true);
                }
        }
        else if(wParam == AUDIO_CFG_MSG_1)
        {
                m_StarButten.EnableWindow(false);
                m_audio_io_pass.EnableWindow(false);
                m_audio_io_fail.EnableWindow(false);
                GetDlgItem(IDC_CurTestObj)->SetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_CurTestItem)->SetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_CurTestItem2)->SetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_CurTestObj2)->SetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_Start)->SetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_Audio_pio_fail)->SetWindowText(Text[text_idx++]);
                GetDlgItem(IDC_Audio_pio_pass)->SetWindowText(Text[text_idx++]);
        }
}

void CPD218Dlg::OnAudioWaveShow(WPARAM wParam,LPARAM lParam)
{
        wave_show((CAudio_par *)lParam);
}

void CPD218Dlg::OnAudioTest(WPARAM wParam,LPARAM lParam)
{
       
#if 0
	    struct static_obj_t *back = (struct static_obj_t *)lParam;
        if(wParam == 0)
        {
            GetDlgItem(back[0].id_idx)->GetWindowText(back[0].cap);
            GetDlgItem(back[1].id_idx)->GetWindowText(back[1].cap);
        }
        else
        {
            GetDlgItem(back[0].id_idx)->SetWindowText(back[0].cap);
            GetDlgItem(back[1].id_idx)->SetWindowText(back[1].cap);
        }
#endif

		struct static_obj_t *back = (struct static_obj_t *)lParam;
		if(wParam == 0)
        {
		      if(back[0].id_idx) GetDlgItem(back[0].id_idx)->GetWindowText(back[0].cap);
              if(back[1].id_idx) GetDlgItem(back[1].id_idx)->GetWindowText(back[1].cap);
		}
        else
        {
              if(back[0].id_idx) GetDlgItem(back[0].id_idx)->SetWindowText(back[0].cap);
              if(back[1].id_idx) GetDlgItem(back[1].id_idx)->SetWindowText(back[1].cap);
        }
}

void CPD218Dlg::OnUpdateConfig(WPARAM wParam,LPARAM lParam)
{
     if(update_cfg_file_realtime)
     {
	    pCfgf.update_config_to_disk();
              
		/* if(!pCfgf.writing_file && TryEnterCriticalSection(&update_cfg_file_cs))                
		{
		   pCfgf.update_config_to_disk();
		   LeaveCriticalSection(&update_cfg_file_cs);
	    } */
	 }
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
void CPD218Dlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(nIDEvent);
//	PostMessage(WM_COMMAND,((WPARAM)BN_CLICKED)<<8|(WPARAM)IDC_Start,0L);
	switch(nIDEvent)
    {
	case RESTART_TESTTIMER:
		PostMessage(WM_TIMER_SPRING_TEST,0L,0L);
		break;
	case TIMER_NEXT_AUDIO_STATU:
		PostMessage(WM_TIMER_NEXT_AUDIO_STATU,0L,0L);
		break;
	case TIMER_WAIT_THREAD_END:
		{
			HWND childHwnd = FindWindowEx(m_hWnd,NULL,NULL,NULL);
			if(childHwnd != NULL) ::SendMessage(childHwnd,BM_CLICK,0,0);
			break;
		}
	case TIMER_UNLOCK_M8852:
		{
			if(m8852_locked)
			{ 
				m8852_locked = false;
				glbResource_put(GRESOURCE_BT_TSET);
			}
		}
		break;
		
	default:
		break;
	}

//	CDialog::OnTimer(nIDEvent);
}



void CPD218Dlg::OnTimerSpringTest(WPARAM wParam,LPARAM lParam)
{
	KillTimer(RESTART_TESTTIMER);
	
	//add led indication,start test and cancel led flash
	start_btn_is_pressed = true;

	
	GetDlgItem(IDC_CurTestObj)->SetWindowText(str_ATEST);
	GetDlgItem(IDC_CurTestItem)->SetWindowText("");
	GetDlgItem(IDC_CurTestObj2)->SetWindowText(str_BTEST);
	GetDlgItem(IDC_CurTestItem2)->SetWindowText("");
	
	START_AMODLE();
	START_BMODLE();
	update_start_butten(ATestRunning,BTestRunning);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool CPD218Dlg::addr_is_inputed(void)
{
	
	if(tSetA_enable && tSetA->need_input_addr_first && !adev_address_inputed) return(false);
	if(tSetB_enable && tSetB->need_input_addr_first && !bdev_address_inputed) return(false);
	return(true);
}


void CPD218Dlg::update_start_butten(bool a_disable,bool b_disable,bool gdisable)
{
        bool g_disable = false;
        a_disable = ATestRunning;
        b_disable = BTestRunning;

        if(a_disable) g_disable = true;
        if(b_disable) g_disable = true;
        if(!all_device_connected)
        {
                a_disable = true;
                b_disable = true;
        }
        if(cnrouting_running) g_disable = true;
        if(!addr_is_inputed()) g_disable = true;
        if(g_disable)
        {
                a_disable = true;
                b_disable = true;
        }


        CString gTest = "开始";
        if(!all_device_connected && !cnrouting_running)  
        {
                gTest = "连接仪器";
                g_disable = false;
        }
        GetDlgItem(IDC_Start)->SetWindowText(gTest);
        if(g_disable) 
        {
                //if(all_device_connected) AB_RF_Route_SW_enable = false;
                m_StarButten.EnableWindow(false);
                setFocus(IDC_LIST2);
        }
        else 
        {
                //if(all_device_connected) AB_RF_Route_SW_enable = true;

                m_StarButten.EnableWindow(true);
                setFocus(IDC_Start,IDC_ADEV_ADDRESS);

                if(auto_run_enable)
                { 
                        KillTimer(RESTART_TESTTIMER);
                        SetTimer(RESTART_TESTTIMER,3000,NULL);
                }
        }

        // A & B sync deal
        if(!all_device_connected || a_disable || g_disable || !tSetA_enable) m_button_atest.EnableWindow(false);
        else m_button_atest.EnableWindow(true);
        if(!all_device_connected || b_disable || g_disable || !tSetB_enable) m_button_btest.EnableWindow(false);
        else m_button_btest.EnableWindow(true);

        if(all_device_connected && pCfgf.ItemSelect(s_N4010A_TEST_Sel)) submenuEnable(ID_RFPWR_LOSS_ADJ,true);
        else submenuEnable(ID_RFPWR_LOSS_ADJ,false);

        if(g_disable && (ATestRunning || BTestRunning || cnrouting_running))
        {
                GetMenu()->EnableMenuItem(MENU_CONFIG_POSITION,MF_BYPOSITION|MF_GRAYED);
                GetMenu()->EnableMenuItem(MENU_CONNECT_DEVICE,MF_BYPOSITION|MF_GRAYED);
                GetMenu()->EnableMenuItem(MENU_LOG_DIR,MF_BYPOSITION|MF_GRAYED); 
        }
        else
        {
                GetMenu()->EnableMenuItem(MENU_CONFIG_POSITION,MF_BYPOSITION|MF_ENABLED);
                GetMenu()->EnableMenuItem(MENU_CONNECT_DEVICE,MF_BYPOSITION|MF_ENABLED);
                GetMenu()->EnableMenuItem(MENU_LOG_DIR,MF_BYPOSITION|MF_ENABLED);
        }


	DrawMenuBar();
}

void CPD218Dlg::submenuEnable(int menu_id,bool en)
{ 
        if(en) GetMenu()->EnableMenuItem(menu_id, MF_BYCOMMAND | MF_ENABLED);
        else GetMenu()->EnableMenuItem(menu_id, MF_BYCOMMAND | MF_GRAYED);
}


void CPD218Dlg::show_log_directory(void)
{
	CString mtemp = "";
	mtemp += orderNumber;
	mtemp += " ";
	mtemp += workNumber;
	
	GetMenu()->ModifyMenu(MENU_LOG_DIR,MF_BYPOSITION, ID_MENUITEM32789, mtemp);
	DrawMenuBar();
}


void CPD218Dlg::OnStart()
{
        CString Text1;
        pMainWnd->GetDlgItem(IDC_Start)->GetWindowText(Text1);
        if(Text1 == str_stop_cfg_audio)
        {
                stop_audio_cfg = 1;
                m_StarButten.EnableWindow(false);
        }
        else if(!all_device_connected)
        {
                setupCnDev();
        }
        else if(!BTestRunning && !ATestRunning)
        {

#ifdef LED_DEBUG_TESTED
		if(pMCU1_Com && pMCU2_Com)
		{
		    	pMCU2_Com->set_LED_off(1, MCU_PIO_GREEN_LED,MCU_PIO_NUM_GREEN_LED);
			pMCU1_Com->set_LED_off(0, MCU_PIO_GREEN_LED,MCU_PIO_NUM_GREEN_LED);

			pMCU2_Com->set_LED_off(1, MCU_PIO_RED_LED,MCU_PIO_NUM_RED_LED);
			pMCU1_Com->set_LED_off(0, MCU_PIO_RED_LED,MCU_PIO_NUM_RED_LED);
		}
#endif
		GetDlgItem(IDC_CurTestObj)->SetWindowText(str_ATEST);
		GetDlgItem(IDC_CurTestObj2)->SetWindowText(str_BTEST);
		SendMessage(WM_TIMER_SPRING_TEST,0L,0L);
        }
}

volatile bool is_atset_finish = false;
volatile bool is_btset_finish = false;

void CPD218Dlg::OnTestFinish(WPARAM wParam,LPARAM lParam)
{
        testSet* tset = (testSet*)lParam;

        *(tset->testRunning) = false;
        update_start_butten(ATestRunning,BTestRunning);

		CString temp;
        unsigned long total = tset->pass_num + tset->fail_num;
        double ratial = 0;
        if(total != 0) ratial = 100*tset->pass_num/total;
        if(tset->dev_id == TEST_DEV_ID_A) 
        {
			temp.Format(" T:%d (P:%d F:%d) R:%.0lf%%",total,tset->pass_num,tset->fail_num,ratial);
			GetDlgItem(IDC_TEST_NUMA)->SetWindowText(temp);
   
			is_atset_finish = true;
        }
        else 
        {
			temp.Format("T:%d (P:%d F:%d) R:%.0lf%% ",total,tset->pass_num,tset->fail_num,ratial);
			GetDlgItem(IDC_TEST_NUMB)->SetWindowText(temp);

		    is_btset_finish = true;
        }
#if 1	//by long
		if(tSetB_enable)
		{
		  if((is_atset_finish && is_btset_finish) || (is_atset_finish && single_Aflag) || (is_btset_finish && single_Bflag))
		  {
		  		if(single_Aflag == true)
					single_Aflag = false;
				if(single_Bflag == true)
					single_Bflag = false;
#if 1
			  //if(!tbox_is_open)
			  //{
				  //打开屏蔽箱
				  if(pMCU2_Com && pTbox && pCfgf.ItemSelect(parTestBoxNeed))
				  {
					  pTbox->open_box();	
					  pMCU2_Com->set_dut_led_on(1,MCU_PIO_LED, MCU_PIO_NUM_LED);
					  //tbox_atset_is_open = true;
					  //tbox_btset_is_open = true;
					  //tbox_is_open = true;
				  }
			  //}
#endif
			  #if 1
			  //MCU U5 PIO 初始化
			  pMCU2_Com->set_dut_5v_off(1, MCU_PIO_DUT1_5V, MCU_PIO_NUM_DUT1_5V);
			  pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY);
			  if(!pCfgf.ItemSelect(parTestBoxNeed))
			  {
			     pMCU2_Com->set_dut_led_off(1, MCU_PIO_LED, MCU_PIO_NUM_LED);
			  }
			  pMCU2_Com->set_dut_vreg_off(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG);
			  pMCU2_Com->set_dut_spi_off(1, MCU_PIO_SPI,MCU_PIO_NUM_SPI);
			  pMCU2_Com->set_dut_current_on(1, MCU_PIO_DUT1_CURRENT,MCU_PIO_NUM_DUT1_CURRENT);
			  pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY);
			  pMCU2_Com->set_dut_pwr_off(1, MCU_PIO_DUT_PWR, MCU_PIO_NUM_DUT_PWR);
			  pMCU2_Com->set_dut1_rout_meter_on(1, MCU_PIO_DUT2_METER, MCU_PIO_NUM_DUT2_METER);

              //MCU U18 PIO 初始化
			  pMCU1_Com->set_dut_5v_off(0, MCU_PIO_DUT2_5V, MCU_PIO_NUM_DUT2_5V);
			  pMCU1_Com->set_dut_current_on(0, MCU_PIO_DUT2_CURRENT,MCU_PIO_NUM_DUT2_CURRENT);
			  pMCU1_Com->set_dut_vreg_off(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG);
			  pMCU1_Com->set_dut_spk_amp_off(0, MCU_PIO_DUT_SPK_AMP, MCU_PIO_NUM_DUT_SPK_AMP);
			  pMCU1_Com->set_8852_route_dut2(0, MCU_PIO_DUT2_RF_ROUTE,MCU_PIO_NUM_DUT2_RF_ROUTE);
			  pMCU1_Com->set_rout_4010_on(0, MCU_PIO_DUT_4010_METER,MCU_PIO_NUM_DUT_4010_METER);
			  pMCU1_Com->set_dut1_rout_audio_on(0, MCU_PIO_DUT2_AUDIO, MCU_PIO_NUM_DUT2_AUDIO);
			  pMCU1_Com->set_test_board_short_pc_off(0, MCU_PIO_AUDIO_CHK, MCU_PIO_NUM_AUDIO_CHK);
                        #endif
			  is_atset_finish = false;
              is_btset_finish = false;
			  dut_pwr_is_on = false;
			  dut_spi_is_on = false;
		  }
		}
		else
		{
		    if(is_atset_finish)
			{
				if(single_Aflag == true)	// no matter button a module or button start
					single_Aflag = false;
#if 1
			  //if(!tbox_is_open)
			  //{
				  //打开屏蔽箱
				  if(pMCU2_Com && pTbox && pCfgf.ItemSelect(parTestBoxNeed))
				  {
					  pTbox->open_box();	
					  pMCU2_Com->set_dut_led_on(1,MCU_PIO_LED, MCU_PIO_NUM_LED);
					  //tbox_atset_is_open = true;
					  //tbox_is_open = true;
				  }
			  //}
#endif
			  //MCU U5 PIO 初始化
			  pMCU2_Com->set_dut_5v_off(1, MCU_PIO_DUT1_5V, MCU_PIO_NUM_DUT1_5V);
			  pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY);
			  if(!pCfgf.ItemSelect(parTestBoxNeed))
			  {
			    pMCU2_Com->set_dut_led_off(1, MCU_PIO_LED, MCU_PIO_NUM_LED);
			  }
			  pMCU2_Com->set_dut_vreg_off(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG);
			  pMCU2_Com->set_dut_spi_off(1, MCU_PIO_SPI,MCU_PIO_NUM_SPI);
			  pMCU2_Com->set_dut_current_on(1, MCU_PIO_DUT1_CURRENT,MCU_PIO_NUM_DUT1_CURRENT);
			  pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY);
			  pMCU2_Com->set_dut_pwr_off(1, MCU_PIO_DUT_PWR, MCU_PIO_NUM_DUT_PWR);
			  pMCU2_Com->set_dut1_rout_meter_on(1, MCU_PIO_DUT2_METER, MCU_PIO_NUM_DUT2_METER);

              //MCU U18 PIO 初始化
			  pMCU1_Com->set_dut_5v_off(0, MCU_PIO_DUT2_5V, MCU_PIO_NUM_DUT2_5V);
			  pMCU1_Com->set_dut_current_on(0, MCU_PIO_DUT2_CURRENT,MCU_PIO_NUM_DUT2_CURRENT);
			  pMCU1_Com->set_dut_vreg_off(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG);
			  pMCU1_Com->set_dut_spk_amp_off(0, MCU_PIO_DUT_SPK_AMP, MCU_PIO_NUM_DUT_SPK_AMP);
			  pMCU1_Com->set_8852_route_dut2(0, MCU_PIO_DUT2_RF_ROUTE,MCU_PIO_NUM_DUT2_RF_ROUTE);
			  pMCU1_Com->set_rout_4010_on(0, MCU_PIO_DUT_4010_METER,MCU_PIO_NUM_DUT_4010_METER);
			  pMCU1_Com->set_dut1_rout_audio_on(0, MCU_PIO_DUT2_AUDIO, MCU_PIO_NUM_DUT2_AUDIO);
			  pMCU1_Com->set_test_board_short_pc_off(0, MCU_PIO_AUDIO_CHK, MCU_PIO_NUM_AUDIO_CHK);
			
			  is_atset_finish = false;
			  dut_spi_is_on = false;
			  dut_pwr_is_on = false;
			}
		}
#endif	//by long

}

void CPD218Dlg::OnAutoRun() 
{
        CString Text1;
        pMainWnd->GetDlgItem(IDC_AUTO_RUN)->GetWindowText(Text1);
        if(Text1 == str_stop_auto_run)
        {
                GetDlgItem(IDC_AUTO_RUN)->SetWindowText(str_start_auto_run);
                auto_run_enable = false;
                KillTimer(RESTART_TESTTIMER);
        }
        else
        { 
                GetDlgItem(IDC_AUTO_RUN)->SetWindowText(str_stop_auto_run);
                if(pCfgf.ItemSelect(slaveParAutoRun)) auto_run_enable = true;
        }
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
void CPD218Dlg::OnAudiopiofail()
{
	// TODO: Add your control notification handler code here
#if 0
	audio_io_ok = false;
	m_audio_io_fail.ShowWindow(false);
	m_audio_io_pass.ShowWindow(false);
	audio_routing_state = AUDIO_IO_CHECKED;
#endif

	CString text;
	m_audio_io_fail.GetWindowText(text);
	if(text == str_a_modle_start) 
	{
		GetDlgItem(IDC_CurTestObj)->SetWindowText(str_ATEST);
	    GetDlgItem(IDC_CurTestItem)->SetWindowText("");

		tSetB->xtal_adj_is_tested = true;  //A,B都测试，只点击‘a开始测试’
		single_Aflag = true;		//fu add for a single button for tbox
		START_AMODLE();
		update_start_butten(ATestRunning,BTestRunning);
	}
	else if(text.Find(str_audio_mute_left) >= 0)
	{ 
		audio_obj.left_mute = true;
		GetDlgItem(IDC_Audio_pio_fail)->SetWindowText(str_audio_unmute_left);
	}
	else if(text.Find(str_audio_unmute_left) >= 0)
	{ 
		audio_obj.left_mute = false;
		GetDlgItem(IDC_Audio_pio_fail)->SetWindowText(str_audio_mute_left);
	}

}

void CPD218Dlg::OnAudiopiopass() 
{
	// TODO: Add your control notification handler code here
#if 0
	audio_io_ok = true;
	m_audio_io_fail.ShowWindow(false);
	m_audio_io_pass.ShowWindow(false);
	audio_routing_state = AUDIO_IO_CHECKED;
#endif

	CString text;
	m_audio_io_pass.GetWindowText(text);
	if(text == str_b_modle_start) 
	{
        GetDlgItem(IDC_CurTestObj2)->SetWindowText(str_BTEST);
	    GetDlgItem(IDC_CurTestItem2)->SetWindowText("");
		
		tSetA->xtal_adj_is_tested = true;  //A,B都测试，只点击‘b开始测试’
		single_Bflag = true;		//fu add for a single button for tbox
		START_BMODLE();
		update_start_butten(ATestRunning,BTestRunning);
	}
	else if(text.Find(str_audio_mute_right) >= 0)
	{
		audio_obj.right_mute = true; 
		GetDlgItem(IDC_Audio_pio_pass)->SetWindowText(str_audio_unmute_right);
	}
	else if(text.Find(str_audio_unmute_right) >= 0)
	{
		audio_obj.right_mute = false; 
		GetDlgItem(IDC_Audio_pio_pass)->SetWindowText(str_audio_mute_right);
	}
}



void CPD218Dlg::set_audio_routing_state(void)
{
	m_audio_io_pass.ShowWindow(true);
	m_audio_io_fail.ShowWindow(true);
}

void CPD218Dlg::OnTimerNextAudioStatu(WPARAM wParam,LPARAM lParam)
{

}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

/*
void CPD218Dlg::OnSelDutCom(WPARAM wParam,LPARAM lParam)
{
        ConfigFile &cfg = pCfgf;
        int nResponse = IDCANCEL;
        bool res = false;

        if(!selDutComDlg)
        {
                selDutCom *dlg = new selDutCom(&cfg, cfg.ItemSelect(slaveUsbConnectEn));
                selDutComDlg = dlg;
                dlg->DoModal();
                selDutComDlg = NULL;
                delete(dlg);
                dlg = NULL;
        }
        selDutComDlgFinsh = 1;
}
*/


//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
void CPD218Dlg::OnInputAddress(WPARAM wParam,LPARAM lParam)
{
	unsigned long man_input_type = (unsigned long)wParam;
	struct bt_address_s* p_addr = (struct bt_address_s*)lParam;

    if(!man_input_type) 
	{
        if(!p_addr->is_valid) 
		{
			p_addr->is_valid = pCfgf.get_bt_address(true,p_addr->nap,p_addr->uap,p_addr->lap);
			
		}
		address_input_running = 0;
        return;	
	}

	/*no use code*/
	input_address *pdlg;
	int nResponse = IDCANCEL;
	bool try_flag = false;
	struct bt_address_s addr_temp;// = {0,0,0,0,""};
	while(true) 
    {
		pdlg = new input_address();
		nResponse = pdlg->DoModal();

		if(nResponse == IDOK) 
        {
			char edata[13] = {0};
			int len = pdlg->m_address.GetLength();
			for (int i=0;i<len;i++) edata[i] = pdlg->m_address[i];
			if(len <= 6) 
            {
				addr_temp.lap = a16ToUint(edata);
				pCfgf.get_tx_nap_uap(addr_temp.nap,addr_temp.uap);
				addr_temp.is_valid = true;
				*p_addr = addr_temp;
				delete(pdlg);
				break;
			}
            else if(12 == len)
            {
				addr_temp.lap = a16ToUint(&edata[6]);
				edata[6] = 0;
				unsigned long temp = a16ToUint(edata);
				pCfgf.get_tx_nap_uap(temp,addr_temp.nap,addr_temp.uap);
				addr_temp.is_valid = true;
				*p_addr = addr_temp;
				delete(pdlg);
				break;
			}
		}
        else if(nResponse == IDCANCEL)
        {
           p_addr->is_valid = false;
			delete(pdlg);
			break;
		}
		delete(pdlg);
		try_flag = true;
	}
	address_input_running = 0;


#if 0
	CBsDevice *obj = (CBsDevice *)lParam;
	
	input_address *pdlg;
	int nResponse = IDCANCEL;
	bool try_flag = false;
	struct bt_address_s addr_temp = {0,0,0};
	struct bt_address_s* p_addr = &obj->addr;
	
	while(true) {
		pdlg = new input_address();
		nResponse = pdlg->DoModal();
		
		if(nResponse == IDOK) {
			char edata[13] = {0};
			int len = pdlg->m_address.GetLength();
			for (int i=0;i<len;i++) edata[i] = pdlg->m_address[i];
			if(len <= 6) {
				addr_temp.lap = a16ToUint(edata);
				pCfgf.get_tx_nap_uap(addr_temp.nap,addr_temp.uap);
				addr_temp.is_valid = true;
				*p_addr = addr_temp;
				delete(pdlg);
				break;
			}else if(12 == len){
				addr_temp.lap = a16ToUint(&edata[6]);
				edata[6] = 0;
				unsigned long temp = a16ToUint(edata);
				pCfgf.get_tx_nap_uap(temp,addr_temp.nap,addr_temp.uap);
				addr_temp.is_valid = true;
				*p_addr = addr_temp;
				delete(pdlg);
				break;
			}
		}else if(nResponse == IDCANCEL) {
			delete(pdlg);
			break;
		}
		delete(pdlg);
		try_flag = true;
	}
	bt_address_lock = 0;

#endif

}

//============================================================================================================
//============================================================================================================
void CPD218Dlg::glbResource_ini(void)
{
        for(int i=0;i<GRESOURCE_NUM;i++)
        {
                InitializeCriticalSection(&hMutex_glbRsource[i]);
        }
}

void CPD218Dlg::glbResource_clr(void)
{
        for(int i=0;i<GRESOURCE_NUM;i++)
        {
                DeleteCriticalSection(&hMutex_glbRsource[i]);
        }
}

bool CPD218Dlg::glbResource_get(int resource_id)
{
        if(resource_id >= GRESOURCE_NUM) return(false);
        EnterCriticalSection(&hMutex_glbRsource[resource_id]);
		return(true);
}

void CPD218Dlg::glbResource_put(int resource_id)
{
        if(resource_id >= GRESOURCE_NUM) return;
        LeaveCriticalSection(&hMutex_glbRsource[resource_id]);
}

//--------------------------------------------------------------------------------------------------------------
void CPD218Dlg::glbSyncObj_ini(void)
{
        for(int i=0;i<CSYNC_OBJ_MAX;i++)
        {
           sync_obj[i] = 0;
        }
}


int CPD218Dlg::glbSyncObj_state(int resource_id)
{
        int val = 0;

        if(resource_id >= CSYNC_OBJ_MAX) return(0);
        if(!glbResource_get(GRESOURCE_SYNC_OBJ)) return(0);
        val = sync_obj[resource_id];
        glbResource_put(GRESOURCE_SYNC_OBJ);
        return(val);
}


int CPD218Dlg::glbSyncObj_hold(int resource_id)
{
        int val = 0;

        if(resource_id >= CSYNC_OBJ_MAX) return(0);
        if(!glbResource_get(GRESOURCE_SYNC_OBJ)) return(0);
        sync_obj[resource_id] += 1;
        val = sync_obj[resource_id];
        glbResource_put(GRESOURCE_SYNC_OBJ);
        return(val);
}


int CPD218Dlg::glbSyncObj_release(int resource_id)
{
        int val = 0;

        if(resource_id >= CSYNC_OBJ_MAX) return(0);
        if(!glbResource_get(GRESOURCE_SYNC_OBJ)) return(0);
        if(sync_obj[resource_id]) sync_obj[resource_id] -= 1;
        val = sync_obj[resource_id];
        glbResource_put(GRESOURCE_SYNC_OBJ);
        return(val);
}

void CPD218Dlg::glbSyncObj_ini_hold(int resource_id)
{
        if(resource_id >= CSYNC_OBJ_MAX) return;
        if(tSetA_enable) glbSyncObj_release(resource_id);
        if(tSetB_enable) glbSyncObj_release(resource_id);
        if(tSetA_enable) glbSyncObj_hold(resource_id);
        if(tSetB_enable) glbSyncObj_hold(resource_id);
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
void CPD218Dlg::creat_tsets(void)
{
        tSetA_enable = true;
        tSetA = new testSet();
        tSetA->tlist = &m_blist;
        tSetA->dev_id = TEST_DEV_ID_A;
        //tSetA->dev_id = TEST_DEV_ID_B;
		tSetA->pSpiObj = new CBsDevice();
		tSetA->pSpiObj->spi_port = pSPIPortA;
		//tSetA->pSpiObj->com_port = pRTL8711_1_Com;
		tSetA->pSpiObj->spi_allocated = true;

        tSetA->idc_CurTestObj = IDC_CurTestObj;
        tSetA->idc_CurTestItem = IDC_CurTestItem;
        tSetA->prompt_test_name = str_ATEST;
        tSetA->testRunning = &ATestRunning;
        tSetA->ptbox_is_open = &tbox_atset_is_open;
        //add CSR

    
        tSetB_enable = pCfgf.ItemSelect(s_BModle_Sel);
        tSetB = new testSet();
        tSetB->tlist = &m_list;
        tSetB->dev_id = TEST_DEV_ID_B;
        //tSetB->dev_id = TEST_DEV_ID_A;
		tSetB->pSpiObj = new CBsDevice();
		tSetB->pSpiObj->spi_port = pSPIPortB;
		//tSetB->pSpiObj->com_port = pRTL8711_2_Com;
		if(tSetB_enable) tSetB->pSpiObj->spi_allocated = true;
        tSetB->idc_CurTestObj = IDC_CurTestObj2;
        tSetB->idc_CurTestItem = IDC_CurTestItem2;
        tSetB->prompt_test_name = str_BTEST;
        tSetB->testRunning = &BTestRunning;
        tSetB->ptbox_is_open = &tbox_btset_is_open;
		//add CSR

		
		wait_continue_enable = pCfgf.ItemSelect(sWaitContinueRun);

		update_addr_input_ctrl();
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
bool CPD218Dlg::rebuild_new_test_item(void)
{	
	    creat_dut_par();
#if 0
	    if(pCfgf.ItemSelect(s_audio_mic_spk_sel) && !pMainWnd->audio_rout_configed)
		   all_device_connected = false;
#endif
        if(update_audio_cfg()) all_device_connected = false;
	
	    tSetA->addr.is_valid = false;
	    tSetA->DeleteTestItemList();
	    tSetA->CreatTestItemList(tSetA,pCfgf);
	    tSetA->ShowTestItemList();
	    tSetA->golden_group = pCfgf.VCString(slaveGoldenNapUapGroup);
	    tSetA->golden_group.MakeUpper();
	
        tSetB->DeleteTestItemList();
	    tSetB->CreatTestItemList(tSetB,pCfgf);
	    tSetB->ShowTestItemList();
	    tSetB->golden_group = tSetA->golden_group;
	
	    pCfgf.update_sn_value(slaveParSn,slaveParOld_Sn);
		update_addr_input_ctrl();

        if( !pCfgf.ItemSelect(s_PwrOff_cur_sel)&&
			!pCfgf.ItemSelect(s_PwrOn_cur_sel))
		{
             GetDlgItem(IDC_MultimeterVal)->ShowWindow(false);
		}
        else 
		{
            if(!Multimeter)
			{
			   pMainWnd->all_device_connected = false;
			}
			GetDlgItem(IDC_MultimeterVal)->ShowWindow(true);
			submenuEnable(ID_CN_Multimeter, true);
		}
        

		submenuEnable(ID_CN_TBOX, pCfgf.ItemSelect(parTestBoxNeed));
		if(pCfgf.ItemSelect(parTestBoxNeed) && !tbox)
		{
		    pMainWnd->all_device_connected = false;
		}

        submenuEnable(ID_CN_BMODLE,tSetB_enable);
#if 0
		submenuEnable(ID_lLED_ADC_CAL, pCfgf.ItemSelect(s_check_led_use_fiber));
#endif
		submenuEnable(ID_lLED_ADC_CAL, 0);

        submenuEnable(ID_CN_Multimeter,pCfgf.ItemSelect(s_PwrOn_cur_sel)||
			                           pCfgf.ItemSelect(s_PwrOff_cur_sel));  
		
		submenuEnable(ID_CN_TOOL_DEV, pCfgf.ItemSelect(s_N4010A_TEST_Sel));
		if(pCfgf.ItemSelect(s_N4010A_TEST_Sel) && !PBtTestSet)
		{
		    pMainWnd->all_device_connected = false;
		}
#if 0
        submenuEnable(ID_CFG_AUDIO_ROUT,pCfgf.ItemSelect(s_audio_mic_spk_sel));
#endif
		submenuEnable(ID_CFG_AUDIO_ROUT,0);

        if(0 && !FCounter)
		{
		    pMainWnd->all_device_connected = false;
		}


//#if 0
        if(pCfgf.ItemSelect(slaveParAutoRun))
        {
                auto_run_enable = false;
                GetDlgItem(IDC_AUTO_RUN)->SetWindowText(str_start_auto_run);
                GetDlgItem(IDC_AUTO_RUN)->ShowWindow(true);
        }
        else
        {
                auto_run_enable = false;
                GetDlgItem(IDC_AUTO_RUN)->ShowWindow(false);
        }
//#endif
        update_start_butten(ATestRunning,BTestRunning);
	    GetDlgItem(IDC_CurTestObj)->SetWindowText(str_ATEST);
	    GetDlgItem(IDC_CurTestItem)->SetWindowText("");
        GetDlgItem(IDC_CurTestObj2)->SetWindowText(str_BTEST);
	    GetDlgItem(IDC_CurTestItem2)->SetWindowText("");
	
		return(true);
}

void CPD218Dlg::star_test(LPVOID ptSet)
{
        testSet *tset = (testSet *)ptSet;

	/*clear test result && gen prompt*/
	tset->ShowTestItemList();
	tset->hthread = CreateThread(NULL,0,TestDeal,ptSet,0,&tset->hthreadId);

}


//============================================================================================================
//============================================================================================================
static DWORD WINAPI TestDeal(LPVOID ptSet)
{
	testSet *tset = (testSet *)ptSet;
	CTime stTime = CTime::GetCurrentTime();  //Record: startTime
	bool TestResult = false;
	CdNode<CTestItem> *pItem = tset->TItemList; 
try
{
	//testSet *tset = (testSet *)ptSet;
	//CTime stTime = CTime::GetCurrentTime();  //Record: startTime
	unsigned long delayofftime = 500;
	//CdNode<CTestItem> *pItem = tset->TItemList; 
	int ErrSort = NO_ERR;
	//bool TestResult = false;

	CString temp = "测试中...";
	UPDATE_STATIC(tset->idc_CurTestItem,temp);
	TSET_MSG(MSG_READY,tset);
	if(!tset->tlist->GetItemCount() || !tset->pSpiObj) 
		goto __TestFail;
	
        ///////////////gloden//////////////
	tset->reset_times = 0;
	tset->fail_titem_id = 0;
	tset->is_golden = false;
	tset->r_addr.is_valid = false;
	if(tset->dev_id == TEST_DEV_ID_A)
	{
		dut_spi_atset_is_on = false;
	}
	else
	{
		dut_spi_btset_is_on = false;
	}
	ErrSort = NO_ERR;
	while(pItem) 
	{
		if(pMainWnd->terminal_test) break;

		ErrSort = tset->DoItemTest(pItem->cur, NO_ERR);
		//ErrSort = tset->DoItemTest(pItem->cur, ErrSort);

		//modified
		if(ErrSort != NO_ERR)
		{
			if(pItem->cur->disable_run_when_err())
			{

				if(tset->dev_id == TEST_DEV_ID_A)
				{
					dut_spi_atset_is_on = false;
				}
				else
				{
					dut_spi_btset_is_on = false;
				}
			}
			tset->fail_titem_id = pItem->cur->identifier;
			tset->xtal_adj_is_tested = true; //测试中dut A或dut B出现fail，保证另外一个可以继续测试
			break;
		}			

		pItem = pItem->next;
		if(pItem == tset->TItemList) 
		{
			break;
		}
	}


        //do item whenever a err happen     
	while(pItem)
	{
		if(pMainWnd->terminal_test) break;
		if(pItem == tset->TItemList) {
			break;
		}
		{
		       tset->DoItemTest(pItem->cur,ErrSort);
		       pItem = pItem->next;
		}
	}



__TestFail:
        if(!pMainWnd->terminal_test)
        {
		pMainWnd->PostMessage(WM_UPDATE_CFG_FILE,(WPARAM)NULL,(LPARAM)NULL);
		TestResult = tset->AllTestItemOk();
		tset->test_result = TestResult;
		if(TestResult)
		{
			tset->pass_num += 1;
		}
		else
		{	
			tset->fail_num += 1;
		}
		TSET_MSG(MSG_FINISH,tset);   ///OnTItemMsg ->MSG_FINISH
        }
     
	CTime endTime = CTime::GetCurrentTime();  //Record: endTime
	pMainWnd->RecodResult(tset,TestResult,stTime,endTime);
	pMainWnd->SendMessage(WM_TEST_FINISH,(WPARAM)TestResult,(LPARAM)tset);

}
catch (CMemoryException* e)
{
	AfxMessageBox("mem err");	
}
catch (CFileException* e)
{
	AfxMessageBox("cfile err");
}
//yhl 2018/11/19
catch (CException* e)
{
	//if(tset->dev_id == TEST_DEV_ID_A)
		//AfxMessageBox("module A get cException err");
	//else
		//AfxMessageBox("module B get cException err");
#if 1
	if(!pMainWnd->terminal_test)
	{
		//一般是仪器读不到数据的时候抛出这个异常，所以要stop tx 以免影响另外的一个通道

		CString param;
		if(tset->dev_id == TEST_DEV_ID_B)
		{
			stop_rtl8711_tx(pRTL8711_2_Com,param);
		}
		else
		{	
			stop_rtl8711_tx(pRTL8711_1_Com,param);
		}
		pItem->cur->ItemTestClean();		//release mux otherwise another will wait 
		TSET_MSG(MSG_SHOW_RESULT,pItem->cur->tset);  //显示测试结果
		tset->fail_titem_id = pItem->cur->identifier;
		pMainWnd->PostMessage(WM_UPDATE_CFG_FILE,(WPARAM)NULL,(LPARAM)NULL);
		//TestResult = tset->AllTestItemOk();
		TestResult = false;
		tset->test_result = TestResult;
		if(TestResult)
		{
			tset->pass_num += 1;
		}
		else
		{	
			tset->fail_num += 1;
		}
		TSET_MSG(MSG_FINISH,tset);	 ///OnTItemMsg ->MSG_FINISH
		}
		
		CTime endTime = CTime::GetCurrentTime();  //Record: endTime
		pMainWnd->RecodResult(tset,TestResult,stTime,endTime);
		pMainWnd->SendMessage(WM_TEST_FINISH,(WPARAM)TestResult,(LPARAM)tset);
#endif
	}

	return(0);
}



//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
bool CPD218Dlg::RecodResult(testSet *tset,bool Result,CTime &stTime,CTime &endTime)
{
	ConfigFile &cfg = pCfgf;

	CString RecorSel;
	CString Dir;
	CString filename;
	CString new_sn,sn;
	bool NewRecord = false;

#define	INI_REC_VAR(T_OBJ)									\
do{	RecorSel = cfg.VCString(T_OBJ##ParRecord);						\
	Dir = cfg.VCString(T_OBJ##ParRecordDir);						\
	filename = cfg.VCString(T_OBJ##ParRecordfile);						\
}while (0)

	    if(!cfg.ItemSelect(slaveParRecord)) return(true);
        if(!pMainWnd->glbResource_get(GRESOURCE_WRECORD)) return(false);
        INI_REC_VAR(slave);
        CString ate_sn = cfg.VCString(slaveParSn); //ate_sn is use time-->format string
        CString file_ate_sn = " "+ate_sn+" ";
        sn = cfg.VCString(ParATEVersion);
        sn += "(";
        sn += ate_sn;
        sn += ")";  

	TCHAR szFilePath[MAX_PATH + 1];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
	CString str_url =  szFilePath;
	str_url += Dir;
	
        DWORD st_rec = GetFileAttributes(str_url);
        if((st_rec == -1) || !(GetFileAttributes(str_url) & FILE_ATTRIBUTE_DIRECTORY))
        { 
                mkdir(str_url);
                st_rec = GetFileAttributes(str_url);
				if((st_rec == -1) || !(GetFileAttributes(str_url) & FILE_ATTRIBUTE_DIRECTORY))
				{
					pMainWnd->glbResource_put(GRESOURCE_WRECORD);
					return(false);
				}
       }

        bool record_cofig_op = true;             //need record config file
        CTime fTime = CTime::GetCurrentTime();
       // if(tset->is_golden) filename = "golden";
        CString tdir = orderNumber;
        tdir += " ";
        tdir += workNumber;
        CString file_suffixes = ".csv";
	str_url += "\\";
        str_url += tdir;
        str_url += "\\";  // '\order_number\worker_number\ bt_log/golden .csv'
	CString file = str_url+filename+file_suffixes;
	ifstream ifs(file,ios::nocreate|ios::in);
	if(ifs.is_open()) 
    {
                char buf[1024] = {0};
                ifs.getline(buf,1024);
                CString temp = buf;
                if(temp.Find(sn) < 0) NewRecord = true;
                record_cofig_op = false;

		ifs.seekg(0,ios::end);
		streampos ps = ifs.tellg();
		if((ps >= 0x20000) || NewRecord) 
        {
			ifs.close();
			CString fileBack,temp;
                        temp = fTime.Format("%Y-%m-%d_%H-%M-%S");
                        fileBack = filename+" "+cfg.VCString(slaveParOld_Sn); 
                        fileBack += " ";
                        fileBack += temp;  //file back: bt_log yyyy-mm-dd
			fileBack = str_url+fileBack+file_suffixes;
			rename(file,fileBack);
		}
                else ifs.close();
	}


	CdNode<CTestItem> *pItem;
	ofstream fs;
	fs.open(file,ios::app|ios::nocreate|ios::out);
	if(!fs.is_open()) 
    {
		fs.clear();
		fs.open(file,ios::app|ios::out);   // first creat new file
		if(!fs.is_open()) 
		{
			pMainWnd->glbResource_put(GRESOURCE_WRECORD);
			return(false);
         }
		
		pItem = tset->TItemList;
		while(pItem)
        {
			fs<<pItem->cur->name<<",";
			pItem = pItem->next;
			if(pItem == tset->TItemList) break;
			fs<<",";
		}
		fs<<",st time,end time,";
		fs<<"Result,"<<sn<<endl;

		if(NewRecord || record_cofig_op)  //always record config file
		{ 
			bool record_config = true;
			CString cfg_url = str_url;
			cfg_url += "\\config";
			DWORD st_rec = GetFileAttributes(cfg_url);
			if((st_rec == -1) || !(GetFileAttributes(cfg_url) & FILE_ATTRIBUTE_DIRECTORY))
			{ 
				mkdir(cfg_url);
				st_rec = GetFileAttributes(cfg_url);
				if((st_rec == -1) || !(GetFileAttributes(cfg_url) & FILE_ATTRIBUTE_DIRECTORY))
				{
					record_config = false;
				}
			}
			if(record_config) 
			{
				cfg_url += "\\";
				CString cfgfile;
				cfgfile = fTime.Format("%Y-%m-%d_%H-%M-%S");
				cfgfile = " " + cfgfile;
				cfgfile = ate_sn + cfgfile;
				cfgfile += ".txt";
				cfgfile = "config " + cfgfile;
				cfgfile = cfg_url + cfgfile;
				cfg.record_config(cfgfile);
			}
		}
	}

    
	pItem = tset->TItemList;
	while(pItem) //yhl 2018/10/23
    {
		/*CString format_str = pItem->cur->format_result();
		fs<<format_str<<",";
	    if(pItem->cur->result) fs<<"/P,";
	    else fs<<"/F,";*/

        pItem->cur->record_result(fs);
		pItem = pItem->next;
		if(pItem == tset->TItemList) break;
	}
	fs<<stTime.Format("%Y-%m-%d %H:%M:%S")<<","<<endTime.Format("%Y-%m-%d %H:%M:%S")<<",";
	if(Result) fs<<"Ok,";
	else fs<<"Fail,";

        if(tset->dev_id == TEST_DEV_ID_A) fs<<"A"<<endl;
        else fs<<"B"<<endl;
        fs.flush();
	fs.close();
        
        pMainWnd->glbResource_put(GRESOURCE_WRECORD);
	return(true);
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
void CPD218Dlg::draw_op(CDC &pDC,CRect &rc,short* lpData,unsigned long len,bool left)
{
        //创建画笔并选中到pDC
        CPen rpen,lpen,xpen,*pOldPen;
        rpen.CreatePen(PS_SOLID,1,RGB(255,0,0)); //1个像素，红色
        lpen.CreatePen(PS_SOLID,1,RGB(0,255,0)); //1个像素，绿色
        xpen.CreatePen(PS_SOLID,1,RGB(0,255,255)); //1个像素，天蓝色
        pOldPen = pDC.SelectObject(&xpen);

        SetTextColor(pDC,RGB(255,255,255));  //1个像素，白色
        SetBkColor(pDC,RGB(0,0,0));
        if(left) pDC.TextOut(rc.left,rc.top+16," L");
        else pDC.TextOut(rc.left,rc.top+16," R");
       
        //绘制x轴
        pDC.MoveTo(rc.left,(rc.bottom+rc.top)/2);
        pDC.LineTo(rc.right,(rc.bottom+rc.top)/2);
        if(left) pOldPen = pDC.SelectObject(&lpen);
        else pOldPen = pDC.SelectObject(&rpen);

        double coef = rc.bottom - rc.top - 4;
        coef = coef/65536;
        int center = (rc.bottom - rc.top)/2;
        int xrey_step = 2;
        int xrey = rc.left;
        int yrey = 0;
        int points = len/2;
        if(points > (rc.right - rc.left)/xrey_step) points = (rc.right - rc.left)/xrey_step;
        int da_idx = 0;
        if(!left) da_idx = 1;

        yrey = (int)(lpData[da_idx]*coef) + center;
        pDC.MoveTo(rc.left,yrey);
        for(int i=0;i<points;i++)
        {
                yrey = (int)(lpData[da_idx]*coef) + center;
                pDC.LineTo(xrey,yrey);
                da_idx += 2;
                xrey += xrey_step;
        }
        pDC.SelectObject(pOldPen);
}

void CPD218Dlg::draw(int IDC,short* lpData,unsigned long len,bool left)
{  
	   //得到IDC_STATIC的句柄  
        CStatic* pWnd = (CStatic*)GetDlgItem(IDC);

        //根据句柄得到dc  
        CDC* pDC = pWnd->GetDC();

        //得到dc的客户区域，并创建画刷填充这个区域
        CRect rc;
        pWnd->GetClientRect(&rc);

        CBitmap bmp;
        bmp.CreateCompatibleBitmap(pDC,rc.Width(),rc.Height());

        CDC dcMem;
        dcMem.CreateCompatibleDC(pDC);
        dcMem.SelectObject(&bmp);

        CBrush brush;
        brush.CreateSolidBrush(RGB(0,0,0));
        dcMem.FillRect(&rc,&brush);

        //设置dc的裁减区域为客户区域，因为STATIC并不具有真正的dc  
        //而是其父窗口的dc，所以用裁减来避免画到客户区域外面去
        CRgn rgn;
        rgn.CreateRectRgn(rc.left,rc.top,rc.right,rc.bottom);
        dcMem.SelectClipRgn(&rgn);

        draw_op(dcMem,rc,lpData,len,left);
        pDC->BitBlt(0,0,rc.Width(),rc.Height(),&dcMem,0,0,SRCCOPY);

        dcMem.DeleteDC();
        bmp.DeleteObject();
        pWnd->ReleaseDC(pDC);
}


//int audio_gain;
void CPD218Dlg::wave_show(CAudio_par *use_par)
{
        CAudio_par &par = *use_par;
        CPD218Dlg &dlg = *pMainWnd;
        int draw_pos,align,temp_pos;
        short prev_data,cur_data,min_offset;
        bool res = false;

        EnterCriticalSection(&dlg.rec_cs);

		//add
		if(par.stop)
        {
                dlg.wave_data_ready = false;
                LeaveCriticalSection(&dlg.rec_cs);
                return; 
        }

        if(dlg.wave_data_ready) res = dlg.soundio->audio_analyse(wave_data,par);
 
        //int record_num = par.record_num;

        //deterin if record can finish
		if(dlg.wave_data_ready && res && par.valid)
        { 
			if(!rec_data_start)
			{ 
				bool lvalid = false,rvalid = false;
				
				if(!par.LShowID) lvalid = true;
				else if(par.Llv > par.valid_lv) lvalid = true;
				
				if(!par.RShowID) rvalid = true;
				else if(par.Rlv > par.valid_lv) rvalid = true;
				if(lvalid && rvalid) 
				{
					rec_data_start = true;
					if(par.record_num && (par.record_num < MAX_AUDIO_TDATA)) 
					{
						par.record_num = MAX_AUDIO_TDATA;
					}
				}
			}
			
			if(rec_data_start) 
			{
				audio_tdata[audio_tdata_idx] = par;
				audio_tdata_idx += 1;
				if(audio_tdata_len < 3000) audio_tdata_len += 1;
				if(audio_tdata_idx  >= MAX_AUDIO_TDATA) audio_tdata_idx = 0;
				par.record_num -= 1;
			}
			if(!par.record_num) par.stop = true;
        }

        // get wave show start pos
        draw_pos = 0;
        align = 0;
        if(par.LShowID && par.RShowID)
        { 
                if(par.left_mute) align = 1;
                else align = 0;
        }
        else if(par.RShowID) align = 1;
        else if(par.LShowID) align = 0;
        temp_pos = align;

        min_offset = 0x7fff;
        prev_data = dlg.wave_data[temp_pos];
        
		for (int i=0;i<64;i++)
        {
			cur_data = dlg.wave_data[temp_pos];
			if(cur_data == 0) 
			{
				draw_pos = i;
				break;
			}
			
			if((prev_data < 0) && (cur_data > 0))
			{
				draw_pos = i;
				break;
			}
			
			prev_data = cur_data;
			temp_pos += 2;
        }
        par.show_pos = draw_pos;
		

        if(par.LShowID) dlg.draw(par.LShowID, &dlg.wave_data[draw_pos * 2], (REC_BLOCK_SIZE/4-draw_pos)* 2, 0);
        if(par.RShowID) dlg.draw(par.RShowID, &dlg.wave_data[draw_pos * 2], (REC_BLOCK_SIZE/4-draw_pos)* 2, 1);

		//if(par.LShowID) dlg.draw(par.LShowID, &dlg.wave_data_draw[draw_pos * 2], (REC_BLOCK_SIZE/4-draw_pos)* 2, 0);
        //if(par.RShowID) dlg.draw(par.RShowID, &dlg.wave_data_draw[draw_pos * 2], (REC_BLOCK_SIZE/4-draw_pos)* 2, 1);
        
        dlg.wave_data_ready = false;
        LeaveCriticalSection(&dlg.rec_cs);
}


//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
bool rec_data_deal(short* lpData,unsigned long len,void* use_par,int widx,int ridx)
{
    CAudio_par &par = *(CAudio_par*)use_par;
    CPD218Dlg* pdlg = pMainWnd;

    if(!pdlg) return(false);
    pdlg->rec_num += 1;
    if(!par.stop && TryEnterCriticalSection(&pdlg->rec_cs))
    {
            for(unsigned long i=0;i<len;i++)
			{
				pdlg->wave_data[i] = lpData[i];
				//pdlg->wave_data_draw[i] = lpData[i]*audio_gain;
			}

            pdlg->wave_data_ready = true;
            LeaveCriticalSection(&pdlg->rec_cs);
            pdlg->PostMessage(WM_WAVE_SHOW,(WPARAM)NULL,(LPARAM)use_par);
    }
    return(true);
}

bool CPD218Dlg::start_audio_deal(CAudio_par *par,unsigned long rec_time,unsigned long tone_time)
{
        audio_tdata_idx = 0;
        audio_tdata_len = 0;
        rec_num = 0;
		wave_data_ready = 0;
        rec_data_start = 0;

		return(soundio->start_audio_deal(rec_data_deal,*par,rec_time,tone_time));
}

bool CPD218Dlg::stop_audio_deal(CAudio_par *par)
{
        soundio->stop_audio_deal(*par);
        wave_data_ready = false;
        return(true);
}


bool CPD218Dlg::get_fine_audio_value(CAudio_par *par)
{
#if 0
	par->Llv = 0;
	par->Lthd = 100;
	par->Rlv = 0;
	par->Rthd = 100;
        if(!rec_data_start) return(false);
        
        int i = 0;
        double data[MAX_AUDIO_TDATA];
        for(i=0;i<MAX_AUDIO_TDATA;i++)
        {
                if(i >= audio_tdata_len) break;
                data[i] = audio_tdata[i].Llv;
        }
        if(i) par->Llv = get_wave_data_avg(data,i);

        for(i=0;i<MAX_AUDIO_TDATA;i++)
        {
                if(i >= audio_tdata_len) break;
                data[i] = audio_tdata[i].Lthd;
        }
        if(i) par->Lthd = get_wave_data_avg(data,i);

        for(i=0;i<MAX_AUDIO_TDATA;i++)
        {
                if(i >= audio_tdata_len) break;
                data[i] = audio_tdata[i].Rlv;
        }
        if(i) par->Rlv = get_wave_data_avg(data,i);

        for(i=0;i<MAX_AUDIO_TDATA;i++)
        {
                if(i >= audio_tdata_len) break;
                data[i] = audio_tdata[i].Rthd;
        }
        if(i) par->Rthd = get_wave_data_avg(data,i);

        for(i=0;i<MAX_AUDIO_TDATA;i++)
        {
                if(i >= audio_tdata_len) break;
                data[i] = audio_tdata[i].show_pos;
        }
        if(i) par->show_pos = (int)get_pos_offset(data,i);
#endif
        return(true);
}


//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------


void CPD218Dlg::OnLogDirector() 
{
	// TODO: Add your command handler code here
	if(!sel_record_dir()) return;
}

void CPD218Dlg::OnMcfgDlg() 
{
	// TODO: Add your command handler code here
	
	if(!PasswordVerify(PASSWORD_ID_PAR,"")) return;
	setpar sdlg;
        sdlg.DoModal();
    rebuild_new_test_item();
}



void CPD218Dlg::OnMselTitem() 
{
	// TODO: Add your command handler code here
	if(!PasswordVerify(PASSWORD_ID_SEL_TITEM,"")) return;

	//config_deal dlg(CFG_FILE_OP_LEVEL3);
	//config_set dlg;
	//dlg.DoModal();
	//rebuild_new_test_item();
}

void CPD218Dlg::OnMsetDate() 
{
	// TODO: Add your command handler code here
	if(!PasswordVerify(PASSWORD_ID_VALID_DATE,"")) return;
	modify_valid_date();
}

void CPD218Dlg::OnMauthority() 
{
	// TODO: Add your command handler code here
	if(!PasswordVerify(PASSWORD_ID_AUTH,"")) return;
	m_StarButten.EnableWindow(false);
	bool res = genIdFile();
	if(!res) AfxMessageBox("无法授权此计算机！！");
	else AfxMessageBox("此计算机已授权！！");
        m_StarButten.EnableWindow(true);
}

void CPD218Dlg::OnMparPassword() 
{
	// TODO: Add your command handler code here
	if(!PasswordVerify(PASSWORD_ID_PAR,"")) return;
        modify_super_password();
}

void CPD218Dlg::OnMtitemPassword() 
{
	// TODO: Add your command handler code here
	if(!PasswordVerify(PASSWORD_ID_SEL_TITEM,"")) return;
        modify_super_password();
}

void CPD218Dlg::OnMcfgPassword() 
{
	// TODO: Add your command handler code here
	if(!PasswordVerify(PASSWORD_ID_CFG,"")) return;
        modify_super_password();
}

void CPD218Dlg::OnMdatePassword() 
{
	// TODO: Add your command handler code here
	if(!PasswordVerify(PASSWORD_ID_VALID_DATE,"")) return;
        modify_super_password();
}

void CPD218Dlg::OnMauthorityPassword() 
{
	// TODO: Add your command handler code here
	if(!PasswordVerify(PASSWORD_ID_AUTH,"")) return;
        modify_super_password();
}

void CPD218Dlg::OnExitDialog() 
{
	// TODO: Add your command handler code here
	EndDialog(IDCANCEL);
}


void CPD218Dlg::OnContinueRun()
{
        wait_continue = false;
        GetDlgItem(IDC_CONTINUE_RUN)->ShowWindow(false);
}

void CPD218Dlg::waitContinueOp(bool enable) 
{
        if(!wait_continue_enable || !enable)
        { 
                wait_continue = false;
                GetDlgItem(IDC_CONTINUE_RUN)->ShowWindow(false);
        }
        else
        {
                wait_continue = true;
                GetDlgItem(IDC_CONTINUE_RUN)->ShowWindow(true);
        }
}




//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//#if 0
//扫描枪扫描地址
BOOL CPD218Dlg::PreTranslateMessage(MSG* pMsg)   
{ 
	
#if 0
	//if((WM_KEYDOWN == pMsg->message) && (VK_SHIFT == pMsg->wParam)) return TRUE;
	if((WM_KEYDOWN == pMsg->message) && (VK_ESCAPE == pMsg->wParam)) 
	{
		int idc = ::GetDlgCtrlID(::GetFocus());
		if(idc == IDC_ADEV_ADDRESS)
		{
			last_keyboard_input = 0;
			last_keyboard_input_len = 0;
			OnSetfocusAdevAddress();
			setFocus(IDC_Start);
		}
		else if(idc == IDC_BDEV_ADDRESS)
		{ 
			last_keyboard_input = 0;
			last_keyboard_input_len = 0;
			OnSetfocusBdevAddress();
			setFocus(IDC_Start);
		}
		return TRUE;
	}
	
	if((WM_KEYDOWN == pMsg->message) && (VK_RETURN == pMsg->wParam))
	{
		int idc = ::GetDlgCtrlID(::GetFocus());
		if(idc == IDC_ADEV_ADDRESS)
		{
			CString text;
			GetDlgItem(IDC_ADEV_ADDRESS)->GetWindowText(text);
			int len = text.GetLength();
			if(len != 12)
			{
				OnSetfocusAdevAddress();
				if(len && ((text[len-1] == 'B') || (text[len-1] == 'b')))
				{
					if(!tSetB_enable) setFocus(IDC_Start);
					else
					{
						OnSetfocusBdevAddress();
						setFocus(IDC_BDEV_ADDRESS);
					}
					
				}
			}
			else
			{
				if(!adjust_hexdigital(text)) OnSetfocusAdevAddress();
				else   //A DEV address input ok
				{
					if(bdev_address_inputed && (text == bdev_input_addr)) OnSetfocusBdevAddress();
					adev_address_inputed = true;
					adev_input_addr = text;
					GetDlgItem(IDC_ADEV_ADDRESS)->SetWindowText(text);
					((CEdit*)GetDlgItem(IDC_ADEV_ADDRESS))->SetSel(-1);
					setFocus(IDC_Start);
					update_start_butten(ATestRunning,BTestRunning);
				}
			}
		}
		else if(idc == IDC_BDEV_ADDRESS)
		{
			CString text;
			GetDlgItem(IDC_BDEV_ADDRESS)->GetWindowText(text);
			int len = text.GetLength();
			if(len != 12)
			{
				OnSetfocusBdevAddress();
				if(len && ((text[len-1] == 'A') || (text[len-1] == 'a')))
				{
					if(!tSetA_enable) setFocus(IDC_Start);
					else
					{
						OnSetfocusAdevAddress();
						setFocus(IDC_ADEV_ADDRESS);
					}  
				}
			}
			else
			{
				if(!adjust_hexdigital(text)) OnSetfocusBdevAddress();
				else    //B DEV address input ok
				{
					if(adev_address_inputed && (text == adev_input_addr)) OnSetfocusAdevAddress();
					bdev_address_inputed = true;
					bdev_input_addr = text;
					GetDlgItem(IDC_BDEV_ADDRESS)->SetWindowText(text);
					((CEdit*)GetDlgItem(IDC_BDEV_ADDRESS))->SetSel(-1);
					setFocus(IDC_Start);
					update_start_butten(ATestRunning,BTestRunning);
				}
			}
		}
		else
		{
			if((last_keyboard_input == 'A')||(last_keyboard_input == 'a'))
			{ 
				OnSetfocusAdevAddress();
				setFocus(IDC_ADEV_ADDRESS);
			}
			else if(((last_keyboard_input == 'B') || (last_keyboard_input == 'b')) && tSetB_enable)
			{
				OnSetfocusBdevAddress();
				setFocus(IDC_BDEV_ADDRESS);
			}  
		}
		
		last_keyboard_input = 0;
		last_keyboard_input_len = 0;
		return TRUE;
	}
	else if(WM_KEYDOWN == pMsg->message)
	{ 
		if((pMsg->wParam == '0x0d') || (pMsg->wParam == '0x0a'))
		{ 
			last_keyboard_input = 0;
			last_keyboard_input_len = 0;
		}
		else
		{
			last_keyboard_input = pMsg->wParam;
			last_keyboard_input_len += 1;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
#endif

	if((WM_KEYDOWN == pMsg->message) && (VK_ESCAPE == pMsg->wParam)) 
	{
		int idc = ::GetDlgCtrlID(::GetFocus());
		if(idc == IDC_ADEV_ADDRESS)
		{
			last_keyboard_input = 0;
			last_keyboard_input_len = 0;
			OnSetfocusAdevAddress();
			setFocus(IDC_Start);
		}
		else if(idc == IDC_BDEV_ADDRESS)
		{ 
			last_keyboard_input = 0;
			last_keyboard_input_len = 0;
			OnSetfocusBdevAddress();
			setFocus(IDC_Start);
		}
		return TRUE;
	}
	
	if((WM_KEYDOWN == pMsg->message) && (VK_RETURN == pMsg->wParam))
	{
		int idc = ::GetDlgCtrlID(::GetFocus());
		if(idc == IDC_ADEV_ADDRESS)
		{
			CString text;
			GetDlgItem(IDC_ADEV_ADDRESS)->GetWindowText(text);
			int len = text.GetLength();
			if(len != 12)
			{
				OnSetfocusAdevAddress();
				if(len && ((text[len-1] == 'B') || (text[len-1] == 'b')))
				{
					if(!tSetB_enable) setFocus(IDC_Start);
					else
					{
						OnSetfocusBdevAddress();
						setFocus(IDC_BDEV_ADDRESS);
					}
				}
			}
			else
			{
			
				if(!adjust_hexdigital(text)) OnSetfocusAdevAddress();
				else   //A DEV address input ok
				{
					if(bdev_address_inputed && (text == bdev_input_addr)) OnSetfocusBdevAddress();
					adev_address_inputed = true;
					adev_input_addr = text;
					
					GetDlgItem(IDC_ADEV_ADDRESS)->SetWindowText(text);
					((CEdit*)GetDlgItem(IDC_ADEV_ADDRESS))->SetSel(-1);
					setFocus(IDC_Start);
					update_start_butten(ATestRunning,BTestRunning);
				}
			}
		}
		else if(idc == IDC_BDEV_ADDRESS)
		{
			CString text;
			GetDlgItem(IDC_BDEV_ADDRESS)->GetWindowText(text);
			
			int len = text.GetLength();
			if(len != 12)
			{
				OnSetfocusBdevAddress();
				if(len && ((text[len-1] == 'A') || (text[len-1] == 'a')))
				{
					if(!tSetA_enable) setFocus(IDC_Start);
					else
					{
						OnSetfocusAdevAddress();
						setFocus(IDC_ADEV_ADDRESS);
					}  
				}
			}
			else
			{
				if(!adjust_hexdigital(text)) OnSetfocusBdevAddress();
				else    //B DEV address input ok
				{
					if(adev_address_inputed && (text == adev_input_addr)) OnSetfocusAdevAddress();
					bdev_address_inputed = true;
					bdev_input_addr = text;			
					
					GetDlgItem(IDC_BDEV_ADDRESS)->SetWindowText(text);
					((CEdit*)GetDlgItem(IDC_BDEV_ADDRESS))->SetSel(-1);
					setFocus(IDC_Start);
					update_start_butten(ATestRunning,BTestRunning);
				}
			}
		}
		else
		{
			if((last_keyboard_input == 'A')||(last_keyboard_input == 'a'))
			{ 
				OnSetfocusAdevAddress();
				setFocus(IDC_ADEV_ADDRESS);
			}
			else if(((last_keyboard_input == 'B') || (last_keyboard_input == 'b')) && tSetB_enable)
			{
				OnSetfocusBdevAddress();
				setFocus(IDC_BDEV_ADDRESS);
			}  
		}
		
		last_keyboard_input = 0;
		last_keyboard_input_len = 0;
		return TRUE;
	}
	else if(WM_KEYDOWN == pMsg->message)
	{ 
		if((pMsg->wParam == '0x0d') || (pMsg->wParam == '0x0a'))
		{ 
			last_keyboard_input = 0;
			last_keyboard_input_len = 0;
		}
		else
		{
			last_keyboard_input = pMsg->wParam;
			last_keyboard_input_len += 1;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);

}

//#endif

void CPD218Dlg::update_addr_input_ctrl(void)
{


		if(pCfgf.ItemSelect(slaveWAddressSel) &&!pCfgf.ItemSelect(s_InputAddress_man))
		{
		
			GetDlgItem(IDC_SBDEV_ADDRESS)->ShowWindow(true);
			GetDlgItem(IDC_ADEV_ADDRESS)->ShowWindow(true);
			GetDlgItem(IDC_BDEV_ADDRESS)->ShowWindow(true);
			if(!tSetB_enable)
			{
				OnSetfocusBdevAddress();
			}	
				
			
		}
		else
		{

		
			GetDlgItem(IDC_SBDEV_ADDRESS)->ShowWindow(false);
			GetDlgItem(IDC_ADEV_ADDRESS)->ShowWindow(false);
			GetDlgItem(IDC_BDEV_ADDRESS)->ShowWindow(false);
			OnSetfocusAdevAddress();
			OnSetfocusBdevAddress();

		}

}

void CPD218Dlg::setFocus(int idc,int idc_skip)
{
        int cur_focus_id = ::GetDlgCtrlID(::GetFocus());
        if(idc_skip && (idc_skip == cur_focus_id)) return;

        if(((IDC_ADEV_ADDRESS == idc_skip) || (IDC_BDEV_ADDRESS == idc_skip)) &&
           ((IDC_ADEV_ADDRESS == cur_focus_id) || (IDC_BDEV_ADDRESS == cur_focus_id))) return;


        CWnd *pWnd = GetDlgItem(idc);
        if(pWnd->IsWindowEnabled()) 
        {
                pWnd->SetFocus();
                return;
        }

        pWnd = GetDlgItem(IDC_Start);
        if(pWnd->IsWindowEnabled())
        {
                pWnd->SetFocus();
                return;
        }
        GetDlgItem(IDC_LIST2)->SetFocus();
}

void CPD218Dlg::OnKillfocusAdevAddress() 
{
	// TODO: Add your control notification handler code here
	if(!adev_address_inputed) OnSetfocusAdevAddress();
    ((CEdit *)GetDlgItem(IDC_ADEV_ADDRESS))->Invalidate();
   // GetDlgItem(IDC_TEST_NUMA)->SetWindowText("");  /*add to cancel the remind string, wjf,2016/09/14*/
}

void CPD218Dlg::OnSetfocusAdevAddress() 
{
	// TODO: Add your control notification handler code here
	adev_address_inputed = false;
        GetDlgItem(IDC_ADEV_ADDRESS)->SetWindowText("");
        ((CEdit*)GetDlgItem(IDC_ADEV_ADDRESS))->SetSel(-1);
        if(!tSetA_enable) setFocus(IDC_Start);
	
}

void CPD218Dlg::OnKillfocusBdevAddress() 
{
	// TODO: Add your control notification handler code here
	if(!bdev_address_inputed) OnSetfocusBdevAddress();
        ((CEdit*)GetDlgItem(IDC_BDEV_ADDRESS))->Invalidate();
}

void CPD218Dlg::OnSetfocusBdevAddress() 
{
	// TODO: Add your control notification handler code here
	bdev_address_inputed = false;
        GetDlgItem(IDC_BDEV_ADDRESS)->SetWindowText("");
        ((CEdit*)GetDlgItem(IDC_BDEV_ADDRESS))->SetSel(-1);
        if(!tSetB_enable) setFocus(IDC_Start);
	
}

void CPD218Dlg::OnClrTcount() 
{
	// TODO: Add your command handler code here
    if(!PasswordVerify(PASSWORD_ID_CLR_TCOUNT,"")) return;

	tSetA->pass_num = 0;
	tSetA->fail_num = 0;
	tSetB->pass_num = 0;
	tSetB->fail_num = 0;
	
	GetDlgItem(IDC_TEST_NUMB)->SetWindowText("");
	GetDlgItem(IDC_TEST_NUMA)->SetWindowText("");	
}


void CPD218Dlg::OnUpdateAddrInput(WPARAM wParam,LPARAM lParam)
{
        GetDlgItem((int)wParam)->SetWindowText(((CString*)lParam)->GetBuffer(0));
        ((CEdit*)GetDlgItem((int)wParam))->SetSel(-1);
}

void CPD218Dlg::OnRfpwrLossAdj() 
{
	// TODO: Add your command handler code here
    rfpwr_loss_cal sdlg;
    int nResponse = sdlg.DoModal();
    rebuild_new_test_item();
}


void CPD218Dlg::OnBeginLEDAdcCal() 
{
	// TODO: Add your command handler code here
	ledAdcCal sdlg;
    int nResponse = sdlg.DoModal();
    rebuild_new_test_item();
	
}


void CPD218Dlg::OnChangeAdevAddress() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CPD218Dlg::OnClickList2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CPD218Dlg::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CPD218Dlg::OnCurTestItem() 
{
	// TODO: Add your control notification handler code here
	
}

void CPD218Dlg::OnCurTestItem2() 
{
	// TODO: Add your control notification handler code here
	
}

void CPD218Dlg::OnCurTestObj() 
{
	// TODO: Add your control notification handler code here
	
}

void CPD218Dlg::OnCurTestObj2() 
{
	// TODO: Add your control notification handler code here
	
}

void CPD218Dlg::OnSbdevAddress() 
{
	// TODO: Add your control notification handler code here
	
}

void CPD218Dlg::OnTestNuma() 
{
	// TODO: Add your control notification handler code here
	
}
