// PD218Dlg.h : header file
//

#if !defined(AFX_PD218DLG_H__B10B25B6_A4E5_41EC_910D_F418F466E55B__INCLUDED_)
#define AFX_PD218DLG_H__B10B25B6_A4E5_41EC_910D_F418F466E55B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CommHead.h"
#include "Config.h"
#include "SortListCtrl.h"
#include "Newdevice.h"
#include "soundIOlib.h"
#include "tset.h"
#include "agilent_n4010_wlantest.h"



#define	WM_START_TEST				(WM_USER+100)
#define	WM_TEST_FINISH				(WM_START_TEST+1)
#define	WM_TIMER_SPRING_TEST		(WM_TEST_FINISH+1)
#define	WM_TIMER_NEXT_AUDIO_STATU	(WM_TIMER_SPRING_TEST+1)
#define	WM_INPUT_BT_ADDRESS			(WM_TIMER_NEXT_AUDIO_STATU+1)
#define	WM_ITEM_MSG				    (WM_INPUT_BT_ADDRESS+1)
#define WM_UPDATE_TITLE				(WM_ITEM_MSG+1)
#define WM_UPDATE_STATIC_FILED		(WM_UPDATE_TITLE+1)
#define WM_UPDATE_TITEM_FILED		(WM_UPDATE_STATIC_FILED+1)
#define WM_CNDEV_FINSH				(WM_UPDATE_TITEM_FILED+1)
#define WM_UPDATE_MULTIMETER		(WM_CNDEV_FINSH+1)
#define WM_AUDIO_CONFIG				(WM_UPDATE_MULTIMETER+1)
#define WM_WAVE_SHOW				(WM_AUDIO_CONFIG+1)
#define WM_AUDIO_TEST				(WM_WAVE_SHOW+1)
#define WM_UPDATE_CFG_FILE			(WM_AUDIO_TEST+1)
#define WM_PROMPT_MSG_BOX			(WM_UPDATE_CFG_FILE+1)
#define	WM_FLASH_CUR_ITEM_NO_RESUL	(WM_PROMPT_MSG_BOX+1)
#define	WM_RESULT_CUR_AND_GO_NEXT   (WM_FLASH_CUR_ITEM_NO_RESUL+1)
#define WM_UPDATE_ADDR_INPUT		(WM_RESULT_CUR_AND_GO_NEXT+1)
#define WM_CNDEV_FINSH_SUCCESS      (WM_UPDATE_ADDR_INPUT+1)
#define	WM_INPUT_MACAddr			(WM_CNDEV_FINSH_SUCCESS+1)

/////////////////////////////////////////////////////////////////////////////
// CPD218Dlg dialog
class CTestItem;
class ConfigFile;
class CPD218Dlg;
class selDutCom;
class ofstream;
class CSortListCtrl;

//class CDutDevice;
//class CPwrBoard;
//class CIOCARD;
class CBsDevice;
class bt_TSET;
class CMultimeter;
class CTBOX;
class testSet;
class CBsDevice;

class CDutGlodenPar;
class CMCU_COM;

//-----------------------------------------------------------------------
enum
{
     GRESOURCE_CURRENT = 0,
	 GRESOURCE_CRYSTAL_ADJ,   //fCounter
	 GRESOURCE_SYNC_OBJ,
	 GRESOURCE_WRECORD,
	 GRESOURCE_TOP,
     GRESOURCE_TBOX,
	 GRESOURCE_FLASH_WR,
	 GRESOURCE_Multimeter,
     GRESOURCE_ADDR,
	 GRESOURCE_BT_TSET,
	 GRESOURCE_MCU1,
	 GRESOURCE_MCU2,
	 GRESOURCE_SOUNDKARD,

     GRESOURCE_NUM
};
//#define GRESOURCE_BT_TSET GRESOURCE_TEST_MODE


enum
{
        CSYNC_OBJ_AUDIO_IO = 0,
        //CSYNC_OBJ_SYS_PWR,
        //CSYNC_OBJ_TMODE_PWR,
        //CSYNC_OBJ_PWROFF_CURRENT,
        //CSYNC_OBJ_REPOWER,

        CSYNC_OBJ_MAX
};

#define MAX_AUDIO_TDATA		10
//-----------------------------------------------------------------------
class CPD218Dlg : public CDialog
{
private:
	bool update_cfg_file_realtime;
	///CRITICAL_SECTION update_cfg_file_cs;

private:
	bool ReadConfigFile(void);
	bool rebuild_new_test_item(void);
bool update_audio_cfg(void);
	void show_log_directory(void);
	void setFocus(int idc,int idc_skip = 0);
	void update_addr_input_ctrl(void);

    CRITICAL_SECTION hMutex_glbRsource[GRESOURCE_NUM];
public:
	void submenuEnable(int menu_id,bool en);
	void update_start_butten(bool a_disable,bool b_disable,bool gdisable = false);
	void star_test(LPVOID ptSet);
	//int  Power_on_dut(void);
	//int Switch_on_spi(void);
	void creat_tsets(void);
	void creat_dut_par(void);
        bool start_audio_deal(CAudio_par *par,unsigned long rec_time = 0,unsigned long tone_time = 0);
        bool stop_audio_deal(CAudio_par *par);
	bool get_fine_audio_value(CAudio_par *par);
        bool have_thread_run(void);

		void waitContinueOp(bool enable);

        selDutCom *selDutComDlg;
        volatile int selDutComDlgFinsh;
        //void OnSelDutCom(WPARAM wParam,LPARAM lParam);

	volatile bool cnrouting_running;
        bool all_device_connected;
	bool allDeviceConnected(void){ return(all_device_connected); }
	void setupCnDev(void);

	bool RecodResult(testSet *tset,bool Result,CTime &stTime,CTime &endTime);

        void glbResource_ini(void);
        void glbResource_clr(void);
        bool glbResource_get(int resource_id);
        void glbResource_put(int resource_id);

        volatile int sync_obj[CSYNC_OBJ_MAX];
        void glbSyncObj_ini(void);
        int glbSyncObj_state(int resource_id);
        int glbSyncObj_hold(int resource_id);
        int glbSyncObj_release(int resource_id);
        void glbSyncObj_ini_hold(int resource_id);
		void set_audio_routing_state(void);
// Construction
public:
     
	//CBsDevice *pCBsObj;
	bool addr_input_remind;
	bool m8852_locked;
	//VSpecAnaly *PSpecAnaly;
    bt_TSET *PBtTestSet;
    CMultimeter *Multimeter;
    CTBOX *tbox;
		
	RTL87XX_COM *DUT_A_RTL8711;
	RTL87XX_COM *DUT_B_RTL8711;
	
	//Instrument_Interface *Inst_N4010A; 
	
    CMCU_COM *DUT_A_MCU;
	CMCU_COM *DUT_B_MCU;
	
	FCOUNTER_DEV *FCounter;

 
	volatile bool tone_mute_change;
	volatile bool left_audio_mute;
	volatile bool right_audio_mute;
    volatile bool stop_audio_cfg;
    bool audio_rout_configed;

    CSoundIO *soundio;
	CRITICAL_SECTION rec_cs;
	HANDLE EvenyWaveshow;
	bool rec_terminal;
	bool rec_running;
	int rec_num;
	//bool wave_data_ok;

	CString cur_mac_data;
	int input_mac_lock;
	
	bool rec_data_start;
	bool wave_data_ready;

	short *wave_data;
	//short *wave_data_draw;
	void draw(int IDC,short* lpData,unsigned long len,bool left);
	void draw_op(CDC &pDC,CRect &rc,short* lpData,unsigned long len,bool left);
	void wave_show(CAudio_par *use_par);
        void wait_audio_deal_finish(void);

	CAudio_par audio_obj;
	CAudio_par AAudio_cfg;
	CAudio_par BAudio_cfg;
	CAudio_par pc_cfg;
	CAudio_par audio_tdata[MAX_AUDIO_TDATA];
	CDutGlodenPar *dutGloden[MAX_DUT_GLODEN_NUM+1];
	CDutSensorPar *dutSensor[MAX_DUT_COLOR_SNR_NUM];
	int audio_tdata_idx;
	int audio_tdata_len;

        bool authorization;
        bool terminal_test;
        bool address_input_running;

        bool tSetA_enable;
        bool tSetB_enable;
        testSet *tSetA;
        testSet *tSetB;
        testSet *cur_tSet;
        bool ATestRunning;
	    bool BTestRunning;
	    bool auto_run_enable;
		bool force_close_window;

	int audio_routing_state;
	bool audio_io_ok;
	int bt_address_lock;

	bool wait_continue_enable;
	volatile bool wait_continue;

	CString orderNumber;
	CString workNumber;
    bool sel_record_dir(void);

	char last_keyboard_input;
        int  last_keyboard_input_len;
        bool adev_address_inputed;
        bool bdev_address_inputed;
        CString adev_input_addr;
        CString bdev_input_addr;
		bool addr_is_inputed(void);

	bool start_btn_is_pressed;


// MFC var
public:
	CBrush m_CurTObjbrush;
	CFont m_CurTObjfont;
	
	CBrush m_CurTItembrush;
	CFont m_CurTItemfont;
	
	CBrush m_ConInsbrush;
	CFont m_ConInsfont;
	
	CBrush m_BtAddressBrush;
	CFont m_BtAddressFont;
	
	CBrush m_testNumBrush;
	CFont m_testNumFont;
	
	CBrush m_butonBrush;
	CFont m_butonFont;

	bool single_Aflag;		//fu add for open tbox when only button a or b
	bool single_Bflag;
	
	CPD218Dlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CPD218Dlg();
       BOOL PreTranslateMessage(MSG* pMsg);

// Dialog Data
	//{{AFX_DATA(CPD218Dlg)
	enum { IDD = IDD_PD218 };
	CButton	m_audio_io_pass;
	CButton	m_audio_io_fail;
	CButton	m_StarButten;
	CSortListCtrl	m_list;
	CSortListCtrl	m_blist;
	CString	m_adev_address;
	CString	m_bdev_address;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPD218Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPD218Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMMCurConfig();
	afx_msg void OnStart();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnAudiopiofail();
	afx_msg void OnAudiopiopass();
	afx_msg void OnCnToolDev();
	afx_msg void OnCnAmodle();
	afx_msg void OnCNMultimeter();
	afx_msg void OnCnTbox();
	afx_msg void OnCfgAudioRout();
	afx_msg void OnAutoRun();
	afx_msg void OnClose();
	afx_msg void OnLogDirector();
	afx_msg void OnMcfgDlg();
	afx_msg void OnMselTitem();
	afx_msg void OnMsetDate();
	afx_msg void OnMauthority();
	afx_msg void OnMparPassword();
	afx_msg void OnMtitemPassword();
	afx_msg void OnMcfgPassword();
	afx_msg void OnMdatePassword();
	afx_msg void OnMauthorityPassword();
	afx_msg void OnExitDialog();
	afx_msg void OnKillfocusAdevAddress();
	afx_msg void OnSetfocusAdevAddress();
	afx_msg void OnKillfocusBdevAddress();
	afx_msg void OnSetfocusBdevAddress();
	afx_msg void OnClrTcount();
	afx_msg void OnRfpwrLossAdj();
	afx_msg void OnContinueRun();
	afx_msg void OnConnectFCounter();
	afx_msg void OnBeginLEDAdcCal();
	afx_msg void OnChangeAdevAddress();
	afx_msg void OnClickList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCurTestItem();
	afx_msg void OnCurTestItem2();
	afx_msg void OnCurTestObj();
	afx_msg void OnCurTestObj2();
	afx_msg void OnSbdevAddress();
	afx_msg void OnTestNuma();
	//}}AFX_MSG

	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD dwData);
	afx_msg void OnTestFinish(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTimerSpringTest(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTimerNextAudioStatu(WPARAM wParam,LPARAM lParam);
	afx_msg void OnInputAddress(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTItemMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnUpdateTitle(WPARAM wParam,LPARAM lParam);
	afx_msg void OnUpdateStatic(WPARAM wParam,LPARAM lParam);
	afx_msg void OnUpdateTitemFiled(WPARAM wParam,LPARAM lParam);
	afx_msg void OnCnDevFinish(WPARAM wParam,LPARAM lParam);
	afx_msg void OnUpdateMultimeter(WPARAM wParam,LPARAM lParam);
	afx_msg void OnAudioConfig(WPARAM wParam,LPARAM lParam);
	afx_msg void OnAudioWaveShow(WPARAM wParam,LPARAM lParam);
	afx_msg void OnAudioTest(WPARAM wParam,LPARAM lParam);
	afx_msg void OnUpdateConfig(WPARAM wParam,LPARAM lParam);
	afx_msg void OnPromptMsgbox(WPARAM wParam,LPARAM lParam);
	afx_msg void OnUpdateAddrInput(WPARAM wParam,LPARAM lParam);
	afx_msg void OnCnDevFinishSuccess(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#define	RESTART_TESTTIMER	123
#define	TIMER_NEXT_AUDIO_STATU	(RESTART_TESTTIMER+1)
#define	TIMER_WAIT_THREAD_END	(TIMER_NEXT_AUDIO_STATU+1)
#define	TIMER_UNLOCK_M8852	(TIMER_WAIT_THREAD_END+1)


#define	pMainWnd 		((CPD218Dlg*)(theApp.m_pMainWnd))
#define	pCfgf 			(*(theApp.pCfgfile))
//#define	pPwrboard 		(pMainWnd->pwr_board)
//#define	pSpecAnaly		(pMainWnd->PSpecAnaly)
#define	pBtTestSet 		(pMainWnd->PBtTestSet)
#define	pMultimeter 		(pMainWnd->Multimeter)
#define	gResMutex 		(pMainWnd->hMutex_glbRsource)
#define	pTbox 		        (pMainWnd->tbox)
//#define	piocard 		(pMainWnd->iocard)
//#define	BT_OBJ			(pMainWnd->pCBsObj)
#define	pSPIPortA 		(theApp.Spi_portA)
#define	pSPIPortB 		(theApp.Spi_portB)
//add
//#define BModleSelected   (theApp.BModeSelected)
#define pMCU1_Com       (pMainWnd->DUT_A_MCU)
#define pMCU2_Com       (pMainWnd->DUT_B_MCU)

#define pRTL8711_1_Com       (pMainWnd->DUT_A_RTL8711)
#define pRTL8711_2_Com       (pMainWnd->DUT_B_RTL8711)

//#define pInst_N4010A       (pMainWnd->Inst_N4010A)//Instrument_Interface *Inst_N4010A;

#define	pFCounter		(pMainWnd->FCounter)

#define	MainList			(pMainWnd->m_list)

#define m_button_atest          m_audio_io_fail
#define m_button_btest          m_audio_io_pass
#define AUDIO_IO_PENDING      	0
#define AUDIO_IO_CHECKED	1

#define MENU_CONFIG_POSITION    0
#define MENU_CONNECT_DEVICE     1
#define MENU_LOG_DIR            2
#define MENU_MODIFY_PASSWORD    3

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool OpenRecordFile(const char* pfile,ofstream& fs,const char* url);
static DWORD WINAPI TestDeal(LPVOID lpParameter);

#define UPDATE_TITLE() 				pMainWnd->SendMessage(WM_UPDATE_TITLE,(WPARAM)NULL,(LPARAM)NULL)
#define UPDATE_STATIC(src_id,cstring) 		pMainWnd->SendMessage(WM_UPDATE_STATIC_FILED,(WPARAM)src_id,(LPARAM)&cstring)
#define UPDATE_TITEM(src_id,cstring) 		pMainWnd->SendMessage(WM_UPDATE_TITEM_FILED,(WPARAM)src_id,(LPARAM)&cstring)
#define UPDATE_MULTIMETER() 			pMainWnd->SendMessage(WM_UPDATE_MULTIMETER,(WPARAM)NULL,(LPARAM)NULL)
#define AUDIO_CFG_MSG(msg,data) 		pMainWnd->SendMessage(WM_AUDIO_CONFIG,(WPARAM)msg,(LPARAM)data)
#define PROMPT_MSG_BOX(text) 		        pMainWnd->PostMessage(WM_PROMPT_MSG_BOX,(WPARAM)NULL,(LPARAM)text)
#define CANSEL_MSG_BOX() 		        pMainWnd->SendMessage(WM_PROMPT_MSG_BOX,(WPARAM)NULL,(LPARAM)NULL)
#define UPDATE_ADDRINPUT(src_id,cstring) 	pMainWnd->SendMessage(WM_UPDATE_ADDR_INPUT,(WPARAM)src_id,(LPARAM)&cstring)


#define AUDIO_CFG_MSG_0				0
#define	AUDIO_CFG_MSG_1				1
#define	AUDIO_CFG_MSG_2				2

#endif // !defined(AFX_PD218DLG_H__B10B25B6_A4E5_41EC_910D_F418F466E55B__INCLUDED_)
