// rfpwr_loss_cal.cpp : implementation file
//

#include "stdafx.h"
#include "pd218.h"
#include "Newdevice.h"
#include "Config.h"
#include "PD218Dlg.h"
#include "err.h"
#include "tset.h"
#include "TestItem.h"
#include "TestHeader.h"
#include "tset.h"
#include "rfpwr_loss_cal.h"
#include "uEnergyTest.h"


#include <math.h>

#include "agilent_n4010_wlantest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


DWORD WINAPI doRfPwrTest(LPVOID input_par);

extern int start_esp8266_tx(RTL87XX_COM* com_A_B, uint8 current_channel, CString &param);
extern int stop_rtl8711_tx( RTL87XX_COM* com_A_B, CString &param);
extern char gpio[];
extern char rtl87xx_gpio1[];
extern char rtl87xx_gpio2[];
extern int  igpio[];

#define MAX_TGLODEN_IDX          3

struct tSetResult ARFResult[MAX_TGLODEN_IDX];
struct tSetResult BRFResult[MAX_TGLODEN_IDX];

int ARFGoldenIdx[MAX_TGLODEN_IDX];
int BRFGoldenIdx[MAX_TGLODEN_IDX];

CString mac_addr_A;
CString mac_addr_B;

int idx_distribut[MAX_TGLODEN_IDX][2] = {
        {1,2},          // 0 idx
        {1,2},      	// 1 idx
        {2,1},       	// 2 idx
		/*
        {1,2},          // 3 idx
        {1,2},          // 4 idx
        {1,2},          // 5 idx
        {1,2},          // 6 idx
		*/
};


/////////////////////////////////////////////////////////////////////////////
// rfpwr_loss_cal dialog

rfpwr_loss_cal::rfpwr_loss_cal(CWnd* pParent /*=NULL*/)
	: CDialog(rfpwr_loss_cal::IDD, pParent)
{
	//{{AFX_DATA_INIT(rfpwr_loss_cal)
	m_agold_mac = _T("");
	m_bgold_mac = _T("");
	//}}AFX_DATA_INIT

	m_prompt_font.CreatePointFont(200, "宋体");
    m_button_font.CreatePointFont(200, "宋体");

	
	hthread = NULL;
    hthreadId = NULL;
	
	rf_test_idx = 1;
	golden_idx = 0;

	last_keyboard_input = 0;
	last_keyboard_input_len = 0;
	adev_address_inputed = 0;
	bdev_address_inputed = 0;
	adev_input_addr = " ";
	bdev_input_addr = " ";
	
	for(int i=0;i<MAX_TGLODEN_IDX;i++)
	{
		memset(&ARFResult[i], 0, sizeof(struct tSetResult));
		memset(&BRFResult[i], 0, sizeof(struct tSetResult));
		ARFGoldenIdx[i] = 0;
		BRFGoldenIdx[i] = 0;
	}
}


void rfpwr_loss_cal::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(rfpwr_loss_cal)
	DDX_Control(pDX, IDCANCEL, m_cancel_button);
	DDX_Control(pDX, IDOK, m_ok_button);
	DDX_Control(pDX, IDC_DO_PWRCAL, m_rfpwr_button);
	DDX_Text(pDX, IDC_AGOLD_ADDR, m_agold_mac);
	DDX_Text(pDX, IDC_BGOLD_ADDR, m_bgold_mac);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(rfpwr_loss_cal, CDialog)
	//{{AFX_MSG_MAP(rfpwr_loss_cal)
	ON_BN_CLICKED(IDC_DO_PWRCAL, OnDoPwrcal)
	ON_WM_CTLCOLOR()
	ON_EN_SETFOCUS(IDC_AGOLD_ADDR, OnSetfocusAgoldAddress)
	ON_EN_SETFOCUS(IDC_BGOLD_ADDR, OnSetfocusBgoldAddress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// rfpwr_loss_cal message handlers


BOOL rfpwr_loss_cal::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    ConfigFile &cfg = pCfgf;

	CDialog::OnInitDialog();	
	m_rfpwr_button.SetFont(&m_button_font);

	m_CurEditfont.CreatePointFont(300, _T("宋体"));
	GetDlgItem(IDC_AGOLD_ADDR)->SetFont(&m_CurEditfont);
	GetDlgItem(IDC_BGOLD_ADDR)->SetFont(&m_CurEditfont);
	
	mac_addr_A.Empty();
	mac_addr_B.Empty();

	rf_pwr_test_finish();
	
	APathOffset[0] = cfg.VDouble(s_DevA_LCH_pathLoss);
	APathOffset[1] = cfg.VDouble(s_DevA_MCH_pathLoss);
	APathOffset[2] = cfg.VDouble(s_DevA_HCH_pathLoss);
	
	BPathOffset[0] = cfg.VDouble(s_DevB_LCH_pathLoss);
	BPathOffset[1] = cfg.VDouble(s_DevB_MCH_pathLoss);
	BPathOffset[2] = cfg.VDouble(s_DevB_HCH_pathLoss);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void rfpwr_loss_cal::OnDoPwrcal() 
{
	// TODO: Add your control notification handler code here
    ConfigFile &cfg = pCfgf;
	
	bool need_box = pCfgf.ItemSelect(parTestBoxNeed);
	bool op_ok = true;
	CString temp;
#if 1
	if(need_box && !pTbox)
	{
		AfxMessageBox("屏蔽箱未连接!!");
		EndDialog(IDCANCEL);
		return;
	}
#endif	
	if(!pMCU1_Com || !pMCU2_Com)
	{
		AfxMessageBox("MCU未分配COM !!");
		EndDialog(IDCANCEL);
		return;
	}
	
	if(!pBtTestSet || !pBtTestSet->dev_is_ready())
	{
		AfxMessageBox("N4010A 未连接!!");
		EndDialog(IDCANCEL);
		return;
	}
	
	if(rf_test_idx >= MAX_TGLODEN_IDX)
	{
		AfxMessageBox("测试已完成，请点击\"确认\"按键完成校正!!");
		return;
	}
	
	m_ok_button.EnableWindow(false);
	m_cancel_button.EnableWindow(false);
	m_rfpwr_button.EnableWindow(false);
	OnSetfocusAgoldAddress();
	OnSetfocusBgoldAddress();
	
	hthread = CreateThread(NULL,0,doRfPwrTest,this,0,&hthreadId);
    	
}

//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
#define PROMP(str) do{ dlg->GetDlgItem(IDC_PROMPT2)->SetWindowText(str); } while(0)

void rfpwr_loss_cal::show_prompt(CString str)
{
        GetDlgItem(IDC_PROMPT2)->SetWindowText(str);
}

//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

static DWORD WINAPI doRfPwrTest(LPVOID input_par)
{
        bool a_res = true;
	    bool b_res = true;    
	    ConfigFile &cfg = pCfgf;
        rfpwr_loss_cal *dlg = (rfpwr_loss_cal*)input_par;

        bool need_box = pCfgf.ItemSelect(parTestBoxNeed);
        bool op_ok = true;
        CString temp;
		CString cmd_str;
		CString cmd_set_mode;
		CString cmd_set_value;
		int i=0;

        //cfg dut routing
#if 1
        if(need_box)
        {
			if(!pTbox->close_box())
			{
			   PROMP("tbox close err");
			   op_ok = false;
			}
            else
			{
			   if(!pMCU2_Com->set_dut_led_off(1, MCU_PIO_LED, MCU_PIO_NUM_LED))
			   {
			      PROMP("led off err");
				  op_ok = false;
			   }
			}	
        }
#endif		
		if(op_ok)
        { 
#if 0
			//wait dut connect tbox
		    Sleep(pCfgf.Vulong(s_close_tbox_delay));
#endif
			//MCU set dut pwr on to connect spi
			if(!pMCU2_Com->set_dut_pwr_on(1, MCU_PIO_DUT_PWR, MCU_PIO_NUM_DUT_PWR))
			{
				PROMP("set dut pwr err");
				return(false);
			}
			if(pMainWnd->tSetA_enable)
			{
				if(!pMCU2_Com->set_dut_vreg_on(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG))
				{
					PROMP("DUT1 burning feet short circuit err");
					return(false);
				}
			}
			if(pMainWnd->tSetB_enable)
			{
				if(!pMCU1_Com->set_dut_vreg_on(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG))
				{
					PROMP("DUT2 burning feet short circuit err");
					return (ERR_STOP);
				}
			}
			if(!pMCU2_Com->set_dut_spi_on(1, MCU_PIO_SPI,MCU_PIO_NUM_SPI))
			{
				PROMP("set dut spi on err");
				return(false);
			}
			
			if(dlg->rf_test_idx < MAX_TGLODEN_IDX)
			{
				dlg->tset = pMainWnd->tSetA;
				if(pMainWnd->tSetA_enable)
				{

#if 0
					//off/on into mp
					pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY);
					pMCU2_Com->set_dut_current_on(1, MCU_PIO_DUT1_CURRENT,MCU_PIO_NUM_DUT1_CURRENT);
					pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY);
					//Sleep(500);
					//pMCU2_Com->set_dut_vreg_on(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG);	
					if(pRTL8711_1_Com)
					{					
						delete(pRTL8711_1_Com);
						pRTL8711_1_Com = NULL;
					}
					// 5次重连机会
					for(i=0;i<5;i++)
					{
						Sleep(500);
						//串口线连接到板上的COM口。
						//temp = "R8711_A 正在分配COM口...";
						//DEV_OP_PROMP(temp);
						pRTL8711_1_Com = new RTL87XX_COM("R8711_A"); 
						if(pRTL8711_1_Com->connect("", 0)) 
						{	
							//UPDATE_TITLE();
							//正常连接后要一定延时,让8711运行稳定再操作
							Sleep(1000);
							break;
						}
						else
						{
							delete(pRTL8711_1_Com);
							pRTL8711_1_Com = NULL;
							if(i==4)
							{
								PROMP("R8711_A 分配COM口失败");
								goto __err;
							}
						}			
					}	
#endif
#if 0
					/*  ensure into mp mode for goiden module*/
						Sleep(2000);
						cmd_str.Format("ATSR\r\n");//to MP	
						if(pRTL8711_1_Com->send_and_search(cmd_str, 0, "Recover OTA signature success.", 0))
						{//如果切换成功表示当前在MP模式，需要重启模块
									
									delete(pRTL8711_1_Com);
									pRTL8711_1_Com = NULL;	
									if(!pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY))
									{
										//param = "set dut battery err";
										return (false);
									}
									Sleep(200);
									if(!pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY))
									{
										//param = "set dut battery err";
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
				

					/*end ensure into mp for golden module*/
#endif
					a_res = dlg->do_4010_test(dlg->rf_test_idx);
				}
				dlg->tset = pMainWnd->tSetB;
				if(pMainWnd->tSetB_enable)
				{
					if(a_res) 
					{
#if 0
						//off/on into mp
						pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY);
						pMCU1_Com->set_dut_current_on(0, MCU_PIO_DUT2_CURRENT,MCU_PIO_NUM_DUT2_CURRENT);
						pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY);
						//Sleep(500);
						//pMCU1_Com->set_dut_vreg_on(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG);
						if(pRTL8711_2_Com)
						{					
							delete(pRTL8711_2_Com);
							pRTL8711_2_Com = NULL;
						}
						
						// 5次重连机会
						for(i=0;i<5;i++)
						{
							Sleep(500);
							//串口线连接到板上的COM口。
							//temp = "R8711_B 正在分配COM口...";
							//DEV_OP_PROMP(temp);
							pRTL8711_2_Com = new RTL87XX_COM("R8711_B"); 
							if(pRTL8711_2_Com->connect("", 0)) 
							{	
								//UPDATE_TITLE();
								//正常连接后要一定延时,让8711运行稳定再操作
								Sleep(1000);
								break;
							}
							else
							{
								delete(pRTL8711_2_Com);
								pRTL8711_2_Com = NULL;
								if(i==4)
								{
									PROMP("R8711_B 分配COM口失败");
									goto __err;
								}
							}
						}
#endif
#if 0
						/*ensure into mp mode*/
						Sleep(2000);
						cmd_str.Format("ATSR\r\n");//to MP	
						if(pRTL8711_2_Com->send_and_search(cmd_str, 0, "Recover OTA signature success.", 0))
						{//如果切换成功表示当前在MP模式，需要重启模块
								delete(pRTL8711_2_Com);
								pRTL8711_2_Com = NULL;
								if(!pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY))
								{
									//param = "set dut battery err";
									return (false);
								}
								Sleep(200);
								if(!pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY))
								{
									//param = "set dut battery err";
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
						/*ensure end into mp mode*/
#endif
						b_res = dlg->do_4010_test(dlg->rf_test_idx);
					}
					else
					{
						b_res = false;
					}
				}
				if(a_res && b_res) dlg->rf_test_idx += 1;
			}
        }
#if 1
        if(need_box)
        {
			if(!pTbox->open_box())
			{
			   PROMP("tbox open err");
			}
            else
			{
			   if(!pMCU2_Com->set_dut_led_on(1, MCU_PIO_LED, MCU_PIO_NUM_LED))
			   {
			      PROMP("Led on err");
			   }
			}	
        }
#endif	
__err:
		pMCU2_Com->set_dut_current_off(1, MCU_PIO_DUT1_CURRENT,MCU_PIO_NUM_DUT1_CURRENT);
		pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY);
		pMCU2_Com->set_dut_vreg_off(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG);
				
		pMCU1_Com->set_dut_current_off(0, MCU_PIO_DUT2_CURRENT,MCU_PIO_NUM_DUT2_CURRENT);
		pMCU1_Com->set_dut_bat_off(0, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY);
		pMCU1_Com->set_dut_vreg_off(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG);
				
	    pMCU2_Com->set_dut_spi_off(1, MCU_PIO_SPI,MCU_PIO_NUM_SPI);
		pMCU2_Com->set_dut_pwr_off(1, MCU_PIO_DUT_PWR, MCU_PIO_NUM_DUT_PWR);

        dlg->rf_pwr_test_finish();
        
		return(0);
}


extern int hexCharToValue(char ch);
extern int hexStr2Hex( char chH, char chL);
extern int read_rtl8711_xtal_cap(RTL87XX_COM* com_A_B, uint8 &current_xcap, CString &param);
extern int set_rtl8711_xtal_cap(RTL87XX_COM* com_A_B, uint8 current_xcap, CString &param);
extern int read_rtl8711_power_Efuse(RTL87XX_COM* com_A_B, uint8 current_channel, CString &param);
extern int set_rtl8711_channel(RTL87XX_COM* com_A_B, uint8 current_channel, CString &param);
extern int init_rtl8711_tx_test( RTL87XX_COM* com_A_B, int tset_dev_id, CString &param);
extern int start_rtl8711_tx_power( RTL87XX_COM* com_A_B, uint8 current_pow, CString &param);
extern int Release_N4010A_Instrument();
extern int stop_rtl8711_tx( RTL87XX_COM* com_A_B, CString &param);



//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
unsigned short pio_loss = 0xffff;
bool rfpwr_loss_cal::dut_enter_rfmode(int test_golden_idx)
{
    ConfigFile &cfg = pCfgf;
    CString temp;
    CBsDevice *pobj;
    CString Dev;
	//bool return_val;
	uint16 io = 0;
	uint16 ver_len = 0;
	int i = 0;
	//int32 retVal;
    CString cmd_str;
    CString cmd_set_mode;
    CString cmd_set_value;
	
    pobj = tset->pSpiObj;
	tset->r_addr.is_valid = false;
    if(!tset || !pobj)
    {
         err_str = "invalid dut\n";
         return(false);
    }

    struct test_sel_t &tsel = tset->tsel;
    Dev = (tset->dev_id == TEST_DEV_ID_A)?"DUT A":"DUT B";
    pobj = tset->pSpiObj;
    
	//path route to N4010A
	if(!pMCU1_Com->set_rout_4010_on(0, MCU_PIO_DUT_4010_METER, MCU_PIO_NUM_DUT_4010_METER))
	{
	    err_str ="switch route to 4010 err";
        return (false);
	}

	temp = "RF通路切换到 ";
    temp += Dev;
    show_prompt(temp);

	if(tset->dev_id == TEST_DEV_ID_A)
	{
		if(!pMCU1_Com->set_4010_route_dut1(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			err_str ="switch 4010 to dut1 err";
			return (false);
		}	
	}
	else
	{
		if(!pMCU1_Com->set_4010_route_dut2(0, MCU_PIO_DUT2_RF_ROUTE, MCU_PIO_NUM_DUT2_RF_ROUTE))
		{
			err_str ="switch 4010 to dut2 err";
			return (false);
		}
	}

	if(tset->dev_id == TEST_DEV_ID_A)
	{
/****************************************************/
		//set into mp
		{

				//set pio21 推挽输出
				cmd_set_mode.Format("MCU+SET(1,%c,%04x,5)\r\n", gpio[0], (1 << igpio[21]));//arg2=A~G,arg3=0001~FFFF
				if(!pMCU2_Com->send_and_verify(cmd_set_mode, 0, "MCU+OK\r\n", 0))
				{
				  	//param.Format("mcu1 set PIO");
					return (false);
				}
				Sleep(50);

				cmd_set_value.Format("MCU+WR(1,%c,%04x,%04x)\r\n", gpio[0], (1 << igpio[21]),(0000));

				if(!pMCU2_Com->send_and_verify(cmd_set_value, 0, "MCU+OK\r\n", 0))
				{
				   	//param.Format("mcu1 set PIO");
					return (false);
				}
				Sleep(50);


		}
		
		
		if(!pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY))
		{
			err_str = "set dut battery err";
			show_prompt(err_str);
			return (false);
		}
		if(pRTL8711_1_Com)
		{
			delete(pRTL8711_1_Com);
			pRTL8711_1_Com = NULL;
		}
		
		Sleep(200);
		if(!pMCU2_Com->set_dut_current_on(1, MCU_PIO_DUT1_CURRENT,MCU_PIO_NUM_DUT1_CURRENT))
		{
			err_str = "a set dut current err";
			show_prompt(err_str);
			return (false);
		}
		
		if(!pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY))
		{
			err_str = "set dut battery err";
			show_prompt(err_str);
			return (false);
		}
		
		// 5次重连机会
		for(i=0;i<5;i++)
		{
			Sleep(500);
			//串口线连接到板上的COM口。
			//temp = "R8711_A 正在分配COM口...";
			//DEV_OP_PROMP(temp);
			pRTL8711_1_Com = new RTL87XX_COM("ESP8266_A"); 
			if(pRTL8711_1_Com->connect("", 0)) 
			{	
				UPDATE_TITLE();
				//正常连接后要一定延时,让8711运行稳定再操作
				Sleep(1000);
				break;
			}
			else
			{
				delete(pRTL8711_1_Com);
				pRTL8711_1_Com = NULL;
				if(i==4)
				{
					err_str += "ESP8266_A 分配COM口失败!!\n";
					show_prompt(err_str);
					return (false);
				}
			}			
		}		
	}
	else
	{

						
/**********************************************/
		//set mp mode
		//set pio21 推挽输出
		{
							
				cmd_set_mode.Format("MCU+SET(0,%c,%04x,5)\r\n", gpio[0], (1 << igpio[21]));//arg2=A~G,arg3=0001~FFFF
				if(!pMCU1_Com->send_and_verify(cmd_set_mode, 0, "MCU+OK\r\n", 0))
				{
					 //param.Format("mcu1 set PIO21 err");
					return (false);
				}
				Sleep(50);

				cmd_set_value.Format("MCU+WR(0,%c,%04x,%04x)\r\n", gpio[0], (1 << igpio[21]),(0000));

				if(!pMCU1_Com->send_and_verify(cmd_set_value, 0, "MCU+OK\r\n", 0))
				{
					//param.Format("mcu1 set PIO21 write err");
					return (false);
				}
					Sleep(50);			
		}
		
		if(!pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY))
		{
			err_str = "set dut battery err";
			show_prompt(err_str);
			return (false);
		}
		if(pRTL8711_2_Com)
		{
			delete(pRTL8711_2_Com);
			pRTL8711_2_Com = NULL;
		}
		Sleep(200);
		
		if(!pMCU1_Com->set_dut_current_on(0, MCU_PIO_DUT2_CURRENT,MCU_PIO_NUM_DUT2_CURRENT))
		{
			err_str = " b set dut current err";
			show_prompt(err_str);
			return (false);
		}
		if(!pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY))
		{
			err_str = "set dut battery err";
			show_prompt(err_str);
			return (false);
		}
		
		// 5次重连机会
		for(i=0;i<5;i++)
		{
			Sleep(500);
			//串口线连接到板上的COM口。
			//temp = "R8711_B 正在分配COM口...";
			//DEV_OP_PROMP(temp);
			pRTL8711_2_Com = new RTL87XX_COM("ESP8266_B"); 
			if(pRTL8711_2_Com->connect("", 0)) 
			{	
				UPDATE_TITLE();
				//正常连接后要一定延时,让8711运行稳定再操作
				Sleep(1000);
				break;
			}
			else
			{
				delete(pRTL8711_2_Com);
				pRTL8711_2_Com = NULL;
				if(i==4)
				{
					err_str += "ESP8266_B 分配COM口失败!!\n";
					show_prompt(err_str);
					return (false);
				}
			}
		}

	}
				
	return(true);				
}

//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//yhl 2018/10/24 jingji
bool rfpwr_loss_cal::do_4010_test(int trf_idx)
{
try
{
        //uint16 nap;
	    //uint8 uap;
	    //uint32 lap;
	    //CString mac_addr;    
	    ConfigFile &cfg = pCfgf;
        CString Dev;
		CString temp;
		CString param;
		CBsDevice *pobj; 
		int i=0,j=0;
		CString cmd_str;
		CString answer_str;
		char rx_buf[512] = {0};
		int ret;
		double center_fre = 2.412e9;
		uint8 current_power = 0;
		double evm_sum = 0;
		double power_sum = 0;
		double max_evm = 0;
		double current_evm = 0;
		double current_evm_dBm = 0;
		double instrument_compensate=0;

		double	PowerRange = 0;
		double	TriggerLevel = 0;
		long	MaxSymbolsUsed = 0;
		double	MaxPacketLength = 0;

		//static const uint8 TEST_CHANNEL = 19;
		//static const uint8 TEST_PAYLOAD_LEN = 37;
		//static const uint8 TEST_PAYLOAD_TYPE = 0;
		//static const uint32 RX_TEST_DURATION = 1000;
		//static const CString TX_POWER_LEVEL_CS_NAME = "tx_power_level";
		
		double avpower;
		//double avpower_sum;
		uint8 channel;

        pobj = tset->pSpiObj;

        Dev = (tset->dev_id == TEST_DEV_ID_A)?"DUT A":"DUT B";
		if(!dut_enter_rfmode(rf_test_idx))
        {
			show_prompt(err_str);
			return(false);
		}	
		
		temp = "开始读 ";
		temp += Dev;
		temp += " 地址";
		show_prompt(temp);
		Sleep(1000);
#if 0
		#if 0//yhl_read 2018/10/26
		cmd_str.Format("+R\r\n");//mac
		#endif
		cmd_str.Format("iwpriv config_get rmap,11a,6\r\n");//mac	
		if(tset->dev_id == TEST_DEV_ID_A)
		{
			if(!pRTL8711_1_Com)
			{
			   	err_str = "null RTL87xx1 COM";
				show_prompt(err_str);
			   	return (false);
			}
			if(!pRTL8711_1_Com->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
			{
		    	err_str = "rtl87xx1 get err";
				show_prompt(err_str);
				return (false);
			}
			//if(rx_buf)//"Private Message: 0x00 0x26 0x29 0x03 0xA2 0xB6"
			{
				answer_str.Empty();
				answer_str = rx_buf;//2018/10/22
				ret = answer_str.Find("Private Message: 0x");
				if(ret)
				{
					mac_addr_A.Empty();
					mac_addr_A = answer_str.Mid(ret+19,27);
					mac_addr_A.Replace(" 0x",":");//替换
					tset->r_addr.mac_addr = mac_addr_A;
					tset->r_addr.mac_addr.Remove(':');//删除指定字符
					tset->r_addr.is_valid = true;
					golden_idx = tset->gloden_idx();
					if(!golden_idx)
					{
						err_str = "A 上不是金机";
						show_prompt(err_str);
						return(false);
					}
					show_prompt(mac_addr_A);
					Sleep(1000);
				}
				else 
				{
					err_str = "read mac addr err";
					show_prompt(err_str);
					return (false);
				}
			}

		}
		else
		{
			if(!pRTL8711_2_Com)
			{
			   	err_str = "null RTL87xx2 COM";
				show_prompt(err_str);
			   	return (false);
			}		
			if(!pRTL8711_2_Com->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
			{
		    	err_str = "rtl87xx2 get err";
				show_prompt(err_str);
				return (false);
			}	
			//if(rx_buf)//"Private Message: 0x00 0x26 0x29 0x03 0xA2 0xB6"
			{
				answer_str.Empty();
				answer_str = rx_buf;
				ret = answer_str.Find("Private Message: 0x");
				if(ret)
				{
					mac_addr_B.Empty();
					mac_addr_B = answer_str.Mid(ret+19,27);
					mac_addr_B.Replace(" 0x",":");
					tset->r_addr.mac_addr = mac_addr_B;
					tset->r_addr.mac_addr.Remove(':');//删除指定字符
					tset->r_addr.is_valid = true;
					golden_idx = tset->gloden_idx();
					if(!golden_idx)
					{
						err_str = "B 上不是金机";
						show_prompt(err_str);
						return(false);
					}
					show_prompt(mac_addr_B);
					Sleep(1000);
				}
				else 
				{
					err_str = "read mac addr err";
					show_prompt(err_str);
					return (false);
				}
			}			
		}

#endif
		if(tset->dev_id == TEST_DEV_ID_A)
		{
			if(adev_input_addr != " ")
				m_agold_mac = adev_input_addr;
			else
				AfxMessageBox("a golden mac not input");

			tset->r_addr.mac_addr = m_agold_mac;
			
			tset->r_addr.is_valid = true;
			golden_idx = tset->gloden_idx();
			if(!golden_idx)
			{
				err_str = "A 上不是金机";
				show_prompt(err_str);
				return(false);
			}
			show_prompt(m_agold_mac);
			Sleep(1000);
		}
		else
		{
			if(bdev_input_addr != " ")
				m_bgold_mac = bdev_input_addr;
			else
				AfxMessageBox("b golden mac not input");

			tset->r_addr.mac_addr = m_bgold_mac;
			tset->r_addr.is_valid = true;
			golden_idx = tset->gloden_idx();
			if(!golden_idx)
			{
				err_str = "B 上不是金机";
				show_prompt(err_str);
				return(false);
			}
			show_prompt(m_bgold_mac);
			Sleep(1000);
		}


/*
#if 1//判断是否是金机
		cmd_str.Format("iwpriv config_get rmap,c7,1\r\n");	
		if(tset->dev_id == TEST_DEV_ID_A)
		{
			if(!pRTL8711_1_Com->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
			{
		    	err_str = "rtl87xx1 get err";
				show_prompt(err_str);
				return (false);
			}
			//if(rx_buf)//"Private Message: 0xFF"
			{
				answer_str.Empty();
				answer_str = rx_buf;
				ret = answer_str.Find("Private Message: 0x");
				temp = hexStr2Hex(rx_buf[ret+19],rx_buf[ret+20]);
				if(temp != 0x55) 
				{
					err_str = "";
					err_str.Format("%s 不是 金机\n",mac_addr_A);
					show_prompt(err_str);
					return(false);
				}
			}
		}
		else
		{		
			if(!pRTL8711_2_Com->send_and_get(cmd_str, 0, rx_buf, 0, 2000, 256))//可用
			{
		    	err_str = "rtl87xx2 get err";
				show_prompt(err_str);
				return (false);
			}	
			//if(rx_buf)//"Private Message: 0xFF"
			{
				answer_str.Empty();
				answer_str = rx_buf;
				ret = answer_str.Find("Private Message: 0x");
				temp = hexStr2Hex(rx_buf[ret+19],rx_buf[ret+20]);
				if(temp != 0x55) 
				{
					err_str = "";
					err_str.Format("%s 不是 金机\n",mac_addr_B);
					show_prompt(err_str);
					return(false);
				}
			}
		}	
#endif
*/
	//when test golden mechina ,wo need set instrument compensate
	//instrument compensate
	instrument_compensate = cfg.VDouble(Par_instrument_compensate);
	if(((N4010A_DEV*)pBtTestSet)->setPathOffset(instrument_compensate)==false)	
	{
		err_str = "write rf path loss err";
		show_prompt(err_str);
		return(false);
	}

	//Release_N4010A_Instrument();


//金机补偿固定值，直接测试读出功率，EVM，灵敏度
//再计算与预期值之差，就是补偿值
		int dev_AB_idx = 0;
		if(tset->dev_id != TEST_DEV_ID_A) 
			dev_AB_idx = 1;
        
		show_prompt("开始 RF 测试. . .");
		struct tSetResult n4010a_res;   
		memset(&n4010a_res,0,sizeof(struct tSetResult));

		for(i=0;i<3;i++)
		{
			if(i==0)
			{
				show_prompt("开始 RF low fre/evm测试. . .");
				channel = 1;//cfg.Vint(ParN4010A_PWLTxCh);
			}
			else if(i==1)
			{
				show_prompt("开始 RF mid fre/evm测试. . .");
				channel = 7;//cfg.Vint(ParN4010A_PWMTxCh);
			}
			else if(i==2)
			{
				show_prompt("开始 RF high fre/evm测试. . .");
				channel = 13;//cfg.Vint(ParN4010A_PWHTxCh);
			}
						
			if(tset->dev_id == TEST_DEV_ID_B)
			{	
#if 0
			       #if 0//yhl
				read_rtl8711_xtal_cap(pRTL8711_2_Com,pRTL8711_2_Com->x_tal,err_str);
				init_rtl8711_tx_test(pRTL8711_2_Com,TEST_DEV_ID_B,err_str);
				set_rtl8711_xtal_cap(pRTL8711_2_Com,pRTL8711_2_Com->x_tal,err_str);
				set_rtl8711_channel(pRTL8711_2_Com,channel,err_str);
				#else
				set_rtl8711_channel(pRTL8711_2_Com,channel,err_str);
				#endif
				#if 0//yhl 2018/10/23
				if(read_rtl8711_power_Efuse(pRTL8711_2_Com,channel,err_str) == false)
				{
					err_str = "rtl87xx2 get err";
					show_prompt(err_str);
					return (false);
				}
				#endif
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

				start_rtl8711_tx_power(pRTL8711_2_Com,current_power,err_str);
#else
			start_esp8266_tx(pRTL8711_2_Com,channel,param);

#endif
			}
			else
			{ 
#if 0
			       #if 0
				//init_rtl8711_tx_test(pRTL8711_1_Com,TEST_DEV_ID_A,err_str);
				read_rtl8711_xtal_cap(pRTL8711_1_Com,pRTL8711_1_Com->x_tal,err_str);
				init_rtl8711_tx_test(pRTL8711_1_Com,TEST_DEV_ID_A,err_str);
				set_rtl8711_xtal_cap(pRTL8711_1_Com,pRTL8711_1_Com->x_tal,err_str);
				set_rtl8711_channel(pRTL8711_1_Com,channel,err_str);
				#else
				set_rtl8711_channel(pRTL8711_1_Com,channel,err_str);
				#endif
				#if 0//yhl 2018/10/23
				if(read_rtl8711_power_Efuse(pRTL8711_1_Com,channel,err_str) == false)
				{
					err_str = "rtl87xx1 get err";
					show_prompt(err_str);
					return (false);
				}
				#endif
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
				start_rtl8711_tx_power(pRTL8711_1_Com,current_power,err_str);
#else
				start_esp8266_tx(pRTL8711_1_Com,channel,param);
#endif
			}
			
			Sleep(100);

#if 0
			//Release_N4010A_Instrument();
			Sleep(100);
#else
			//Inst.SetWlanMode();
			//Inst.SetChannel(2,channel);//IEEE802_11g=2//enum RadioFormatEnum Format
			center_fre = 2.412e9 + (channel-1)*0.005e9;
			Inst.SetFrequency(center_fre);
			Inst.IEEE80211a_Autorange_Run();
		#if 1	
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
			max_evm = -50; 
			power_sum = 0;
			for(j=0;j<10;j++)
			{						
				//这里获取仪器数据有时候会不成功			
				Inst.IEEE80211a_Demod_Initiate();	
				//当IEEE80211a_Demod_Initiate获取数据不成功时候去调用GetIEEE80211a_Demod_RmsEVM不会抛出异常
				current_evm = Inst.GetIEEE80211a_Demod_RmsEVM();
				//EVM(dB)=20*log10（evm(%)）
				current_evm_dBm = 20*log10(current_evm/100);				
				evm_sum  += current_evm_dBm;
				//max_evm = max(max_evm,current_evm_dBm);
				//if(current_evm_dBm < -20)
				//if(current_evm < 0)
				//	break;
				//当IEEE80211a_Demod_Initiate获取数据不成功时候去调用GetIEEE80211a_Demod_GatedPower就会抛出异常				
				avpower = Inst.GetIEEE80211a_Demod_GatedPower();
				power_sum  += avpower;
			}
			current_evm_dBm = evm_sum/10;
			avpower = power_sum/10;
		#else			
			//这里获取仪器数据有时候会不成功
			Inst.IEEE80211a_EVM_Initiate();
			//当IEEE80211a_EVM_Initiate获取数据不成功时候去调用GetIEEE80211a_EVM_Result不会抛出异常
			current_evm_dBm = Inst.GetIEEE80211a_EVM_Result();//(dB)			
			Inst.AveragePower_Initiate();
			avpower = Inst.GetAveragePower_Result();
		#endif			
#endif	
			n4010a_res.op[i].valid = true;
			n4010a_res.op[i].av = avpower;
			n4010a_res.op[i].evm = current_evm_dBm;
			//n4010a_res.op[i].evm = max_evm;
			
			if(tset->dev_id == TEST_DEV_ID_B)
			{
				stop_rtl8711_tx(pRTL8711_2_Com,err_str);
			}
			else
			{	
				stop_rtl8711_tx(pRTL8711_1_Com,err_str);
			}

		}
		
		#if 1
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
		#endif

		if(rf_test_idx < MAX_TGLODEN_IDX)
		{
			if(tset->dev_id == TEST_DEV_ID_A)
			{
				//if(0)
				if(!n4010a_res.op[0].valid && !n4010a_res.op[1].valid && !n4010a_res.op[2].valid)
				{
					show_prompt("RF 数据未测出");
					return (false);
				}
				else 
				{
					//result_adjust(n4010a_res,APathOffset);

					ARFResult[rf_test_idx] = n4010a_res;
					ARFGoldenIdx[rf_test_idx] = tset->gloden_idx(); 
				}
			}
			else
			{
				//if(0)
				if(!n4010a_res.op[0].valid && !n4010a_res.op[1].valid && !n4010a_res.op[2].valid)
				{
					show_prompt("RF 数据未测出");
					return (false);
				}
				else 
				{
					//result_adjust(n4010a_res,BPathOffset);

					BRFResult[rf_test_idx] = n4010a_res;
					BRFGoldenIdx[rf_test_idx] = tset->gloden_idx();
				}
			}
		}
		
        show_prompt("RF 测试结束");
        return(true);
}
catch (CMemoryException* e)
{
	AfxMessageBox(" golden rf mem err");	
}
catch (CFileException* e)
{
	AfxMessageBox("golden rf cfile err");
}
catch (CException* e)
{
	show_prompt("RF测试故障");
	return (false);
}

}


//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

void rfpwr_loss_cal::result_adjust(struct tSetResult &n4010a_res,double *pathOffset)
{
#if 0
        if(n4010a_res.op[0].valid)
        {
                double lch_loss = pathOffset[1] - pathOffset[0];
                n4010a_res.op[0].av += lch_loss;
                n4010a_res.op[0].av_max += lch_loss;
                n4010a_res.op[0].av_min += lch_loss;
                n4010a_res.op[0].av_pk += lch_loss;
                n4010a_res.op[0].res = true;
        }
        if(n4010a_res.op[2].valid)
        {
                double lch_loss = pathOffset[1] - pathOffset[2];
                n4010a_res.op[2].av += lch_loss;
                n4010a_res.op[2].av_max += lch_loss;
                n4010a_res.op[2].av_min += lch_loss;
                n4010a_res.op[2].av_pk += lch_loss;
                n4010a_res.op[2].res = true;
        }
#endif
}


//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

void rfpwr_loss_cal::rf_pwr_test_finish(void)
{
        m_ok_button.EnableWindow(true);
        m_cancel_button.EnableWindow(true);
        m_rfpwr_button.EnableWindow(true);

        CString title;
        if(rf_test_idx >= MAX_TGLODEN_IDX) title = "测试已完成\n点击确认完成校验";
        else title.Format("请在 A 上放入金机%d\nB 上放入金机%d",idx_distribut[rf_test_idx][0],idx_distribut[rf_test_idx][1]);
        GetDlgItem(IDC_PROMPT)->SetWindowText(title);

        CString temp,a_temp,b_temp;

		if(mac_addr_A.GetLength() == 0)
        	a_temp = "                      A DUT 功率/EVM\n";
		else
			a_temp.Format("             A DUT(%s)功率/EVM\n", mac_addr_A);
        a_temp += "             LCH            MCH            HCH\n";  //10,12,12 space
        
        if(mac_addr_B.GetLength() == 0)
        	b_temp = "                      B DUT 功率/EVM\n";
		else
			b_temp.Format("             B DUT(%s)功率/EVM\n", mac_addr_B);
        b_temp += "             LCH            MCH            HCH\n";  //10,12,12 space
        for(int i=1;i<MAX_TGLODEN_IDX;i++)
        {
                if(ARFResult[i].op[0].valid || ARFResult[i].op[1].valid || ARFResult[i].op[2].valid)
                {
                        if(!ARFGoldenIdx[i]) temp = "dut       ";  //7 space
                        else
						{
							temp = pMainWnd->tSetA->gloden_idx_name(ARFGoldenIdx[i]);
							temp += "   ";  //add 3 space
						}
                        a_temp += temp;

                        if(ARFResult[i].op[0].valid)
                        { 
                                temp = "";
                                temp.Format("%.2lf/%.2lf    ", ARFResult[i].op[0].av,ARFResult[i].op[0].evm);  //6 space
                                a_temp += temp;
                        }
                        else
                        {
                                temp = "NC       ";
                                a_temp += temp;
                        }

                        if(ARFResult[i].op[1].valid)
                        { 
                                temp = "";
                                temp.Format("%.2lf/%.2lf    ", ARFResult[i].op[1].av,ARFResult[i].op[1].evm);  //6 space
                                a_temp += temp;
                        }
                        else
                        {
                                temp = "NC        ";         //8 space
                                a_temp += temp;
                        }
                        if(ARFResult[i].op[2].valid)
                        { 
                                temp = "";
                                temp.Format("%.2lf/%.2lf", ARFResult[i].op[2].av,ARFResult[i].op[2].evm);  
                                a_temp += temp;
                        }
                        else
                        {
                                temp = "NC";
                                a_temp += temp;
                        }
                        a_temp += "\n";
                }
                if(BRFResult[i].op[0].valid || BRFResult[i].op[1].valid || BRFResult[i].op[2].valid)
                {
                        if(!BRFGoldenIdx[i]) temp = "dut       ";//temp = "dut\t\t";
                        else
						{
							temp = pMainWnd->tSetB->gloden_idx_name(BRFGoldenIdx[i]);
							temp += "   ";  //add 3 space
						}
                        b_temp += temp;

                        if(BRFResult[i].op[0].valid)
                        { 
                                temp = "";
                                temp.Format("%.2lf/%.2lf    ", BRFResult[i].op[0].av,BRFResult[i].op[0].evm);
                                b_temp += temp;
                        }
                        else
                        {
                                temp = "NC       ";
                                b_temp += temp;
                        }
                        if(BRFResult[i].op[1].valid)
                        { 
                                temp = "";
                                temp.Format("%.2lf/%.2lf    ", BRFResult[i].op[1].av,BRFResult[i].op[1].evm);
                                b_temp += temp;
                        }
                        else
                        {
                                temp = "NC        ";
                                b_temp += temp;
                        }
                        if(BRFResult[i].op[2].valid)
                        { 
                                temp = "";
                                temp.Format("%.2lf/%.2lf", BRFResult[i].op[2].av,BRFResult[i].op[2].evm);
                                b_temp += temp;
                        }
                        else
                        {
                                temp = "NC";
                                b_temp += temp;
                        }
                        b_temp += "\n";
                }
        }
        GetDlgItem(IDC_ARF_PWR)->SetWindowText(a_temp);
        GetDlgItem(IDC_BRF_PWR)->SetWindowText(b_temp);  
}

void a_adjust_pwr_loss(double &pwr_loss,uint8 l_m_h)
{
	if(l_m_h==0)
		pwr_loss += 0.1;
	if(l_m_h==1)
		pwr_loss += 0.1;
	if(l_m_h==2)
		pwr_loss += 0.1;
}
void b_adjust_pwr_loss(double &pwr_loss,uint8 l_m_h)
{
	if(l_m_h==0)
		pwr_loss += 0.1;
	if(l_m_h==1)
		pwr_loss += 0.1;
	if(l_m_h==2)
		pwr_loss += 0.1;
}

void adjust_evm_loss(double &evm_loss)
{
	if(evm_loss > -1.8)
		evm_loss = -2.2;
	else if(evm_loss < -3.2)
		evm_loss = -2.8;
}

//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

void rfpwr_loss_cal::OnOK() 
{
	    // TODO: Add extra validation here
	    ConfigFile &cfg = pCfgf;
        //bool a_ok = true;
        //bool b_ok = true;
		double test = 0;
		double a_temp[3] = {0};
		double b_temp[3] = {0};
		int i=0;

        if(rf_test_idx < MAX_TGLODEN_IDX)
        {
            AfxMessageBox("数据未测完,要退出请点击\"取消\"按键!!");
            return;
        }

        for(i=1;i<MAX_TGLODEN_IDX;i++)
        {
        	test = pMainWnd->dutGloden[ARFGoldenIdx[i]]->ch1_rf_avpwr;
			ARFResult[0].op[0].av += (pMainWnd->dutGloden[ARFGoldenIdx[i]]->ch1_rf_avpwr - ARFResult[i].op[0].av);
			test = pMainWnd->dutGloden[ARFGoldenIdx[i]]->ch2_rf_avpwr;
			ARFResult[0].op[1].av += (pMainWnd->dutGloden[ARFGoldenIdx[i]]->ch2_rf_avpwr - ARFResult[i].op[1].av);
			test = pMainWnd->dutGloden[ARFGoldenIdx[i]]->ch3_rf_avpwr;
			ARFResult[0].op[2].av += (pMainWnd->dutGloden[ARFGoldenIdx[i]]->ch3_rf_avpwr - ARFResult[i].op[2].av);			
        }
        ARFResult[0].op[0].av /= (MAX_TGLODEN_IDX-1);
        ARFResult[0].op[1].av /= (MAX_TGLODEN_IDX-1);
        ARFResult[0].op[2].av /= (MAX_TGLODEN_IDX-1);
        round_1point(ARFResult[0].op[0].av);
        round_1point(ARFResult[0].op[1].av);
        round_1point(ARFResult[0].op[2].av);
		//a_adjust_pwr_loss(ARFResult[0].op[0].av,0);
		//a_adjust_pwr_loss(ARFResult[0].op[1].av,1);
		//a_adjust_pwr_loss(ARFResult[0].op[2].av,2);
        cfg.ItemSet(s_DevA_LCH_pathLoss,ARFResult[0].op[0].av);
        cfg.ItemSet(s_DevA_MCH_pathLoss,ARFResult[0].op[1].av);
        cfg.ItemSet(s_DevA_HCH_pathLoss,ARFResult[0].op[2].av);
		
        for(i=1;i<(MAX_TGLODEN_IDX-1);i++)
        {
			test = pMainWnd->dutGloden[ARFGoldenIdx[i]]->ch1_rf_evm;
			test = pMainWnd->dutGloden[ARFGoldenIdx[i]]->ch2_rf_evm;
			test = pMainWnd->dutGloden[ARFGoldenIdx[i]]->ch3_rf_evm;
			a_temp[0] = max(ARFResult[i].op[0].evm,ARFResult[i+1].op[0].evm);
			a_temp[1] = max(ARFResult[i].op[1].evm,ARFResult[i+1].op[1].evm);
			a_temp[2] = max(ARFResult[i].op[2].evm,ARFResult[i+1].op[2].evm);
        }
	 for(i=1;i<MAX_TGLODEN_IDX;i++)
	 {
        	ARFResult[0].op[0].evm += (pMainWnd->dutGloden[ARFGoldenIdx[i]]->ch1_rf_evm - a_temp[0] );
        
			ARFResult[0].op[1].evm += (pMainWnd->dutGloden[ARFGoldenIdx[i]]->ch2_rf_evm - a_temp[1] );
        
			ARFResult[0].op[2].evm += (pMainWnd->dutGloden[ARFGoldenIdx[i]]->ch3_rf_evm - a_temp[2] );
	 }
	 ARFResult[0].op[0].evm /= (MAX_TGLODEN_IDX-1);
        ARFResult[0].op[1].evm /= (MAX_TGLODEN_IDX-1);
        ARFResult[0].op[2].evm /= (MAX_TGLODEN_IDX-1);
        round_1point(ARFResult[0].op[0].evm);
        round_1point(ARFResult[0].op[1].evm);
        round_1point(ARFResult[0].op[2].evm);
		//adjust_evm_loss(ARFResult[0].op[0].evm);
		//adjust_evm_loss(ARFResult[0].op[1].evm);
		//adjust_evm_loss(ARFResult[0].op[2].evm);
        cfg.ItemSet(Par_a_evm_loss,ARFResult[0].op[0].evm);
        cfg.ItemSet(Par_a_evm_lossm,ARFResult[0].op[1].evm);
        cfg.ItemSet(Par_a_evm_lossh,ARFResult[0].op[2].evm);

        if(pMainWnd->tSetB_enable)
        {
			for(int i=1;i<MAX_TGLODEN_IDX;i++)
			{
				test = pMainWnd->dutGloden[BRFGoldenIdx[i]]->ch1_rf_avpwr;
				BRFResult[0].op[0].av += (pMainWnd->dutGloden[BRFGoldenIdx[i]]->ch1_rf_avpwr - BRFResult[i].op[0].av);
				test = pMainWnd->dutGloden[BRFGoldenIdx[i]]->ch2_rf_avpwr;
				BRFResult[0].op[1].av += (pMainWnd->dutGloden[BRFGoldenIdx[i]]->ch2_rf_avpwr - BRFResult[i].op[1].av);
				test = pMainWnd->dutGloden[BRFGoldenIdx[i]]->ch3_rf_avpwr;
				BRFResult[0].op[2].av += (pMainWnd->dutGloden[BRFGoldenIdx[i]]->ch3_rf_avpwr - BRFResult[i].op[2].av); 
			}
			
			BRFResult[0].op[0].av /= (MAX_TGLODEN_IDX-1);
			BRFResult[0].op[1].av /= (MAX_TGLODEN_IDX-1);
			BRFResult[0].op[2].av /= (MAX_TGLODEN_IDX-1);
			round_1point(BRFResult[0].op[0].av);
			round_1point(BRFResult[0].op[1].av);
			round_1point(BRFResult[0].op[2].av);			
			//b_adjust_pwr_loss(BRFResult[0].op[0].av,0);
			//b_adjust_pwr_loss(BRFResult[0].op[1].av,1);
			//b_adjust_pwr_loss(BRFResult[0].op[2].av,2);
			cfg.ItemSet(s_DevB_LCH_pathLoss,BRFResult[0].op[0].av);
			cfg.ItemSet(s_DevB_MCH_pathLoss,BRFResult[0].op[1].av);
			cfg.ItemSet(s_DevB_HCH_pathLoss,BRFResult[0].op[2].av);

			for(i=1;i<(MAX_TGLODEN_IDX-1);i++)
	        {
				b_temp[0]= max(BRFResult[i].op[0].evm,BRFResult[i+1].op[0].evm);
				b_temp[1] = max(BRFResult[i].op[1].evm,BRFResult[i+1].op[1].evm);
				b_temp[2] = max(BRFResult[i].op[2].evm,BRFResult[i+1].op[2].evm);			
	        }
		for(i=1;i<MAX_TGLODEN_IDX;i++)
		{
	        BRFResult[0].op[0].evm += (pMainWnd->dutGloden[BRFGoldenIdx[i]]->ch1_rf_evm - b_temp[0]);
	        BRFResult[0].op[1].evm += (pMainWnd->dutGloden[BRFGoldenIdx[i]]->ch2_rf_evm - b_temp[1]);
	        BRFResult[0].op[2].evm += (pMainWnd->dutGloden[BRFGoldenIdx[i]]->ch3_rf_evm - b_temp[2]);
		}

		BRFResult[0].op[0].evm /= (MAX_TGLODEN_IDX-1);
		BRFResult[0].op[1].evm /= (MAX_TGLODEN_IDX-1);
		BRFResult[0].op[2].evm /= (MAX_TGLODEN_IDX-1);
	        round_1point(BRFResult[0].op[0].evm);
	        round_1point(BRFResult[0].op[1].evm);
	        round_1point(BRFResult[0].op[2].evm);
			//adjust_evm_loss(BRFResult[0].op[0].evm);
			//adjust_evm_loss(BRFResult[0].op[1].evm);
			//adjust_evm_loss(BRFResult[0].op[2].evm);
	        cfg.ItemSet(Par_b_evm_loss,BRFResult[0].op[0].evm);
	        cfg.ItemSet(Par_b_evm_lossm,BRFResult[0].op[1].evm);
	        cfg.ItemSet(Par_b_evm_lossh,BRFResult[0].op[2].evm);
        }
	
	CDialog::OnOK();
}


//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
HBRUSH rfpwr_loss_cal::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	//CFont *pfont = &m_CurEditfont;
	int id = pWnd->GetDlgCtrlID();
    switch(id)
	{
	    case IDC_PROMPT:
        {
                pDC->SetTextColor(RGB(255,0,0));
                pDC->SetBkMode(TRANSPARENT);
                pDC->SelectObject(&m_prompt_font);
        }
        break;
        case IDC_PROMPT2:
        {
                pDC->SetTextColor(RGB(0,0,255));
                pDC->SetBkMode(TRANSPARENT);
                pDC->SelectObject(&m_prompt_font);
        }
        break;
		case IDC_AGOLD_ADDR:
		case IDC_BGOLD_ADDR:
			{
				HBRUSH b;
				CWnd *pFocusWnd = GetFocus();
				if(pFocusWnd && 
					( ((id == IDC_AGOLD_ADDR) && (pFocusWnd == GetDlgItem(IDC_AGOLD_ADDR))) || 
					((id == IDC_BGOLD_ADDR) && (pFocusWnd == GetDlgItem(IDC_BGOLD_ADDR))) ))
				{
					b = CreateSolidBrush(RGB(255,0,0));
				}
				else 
				{
					if( ((id == IDC_AGOLD_ADDR) && adev_address_inputed) || 
						((id == IDC_BGOLD_ADDR) && bdev_address_inputed) )
					{
						b = CreateSolidBrush(RGB(0,255,0));
					}
					else
					{
						b = CreateSolidBrush(RGB(220,220,220));
					}
				}
				
				pDC->SetTextColor(RGB(0,0,255));
				//pfont = &m_BtAddressFont;
				pDC->SetBkMode(TRANSPARENT);
				//pDC->SelectObject(pfont);
				return(b);
			}
		break;
        
        default:break;
	}

	// TODO: Return a different brush if the default is not desired
	return hbr;
}


BOOL rfpwr_loss_cal::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	if((WM_KEYDOWN == pMsg->message) && (VK_ESCAPE == pMsg->wParam)) 
	{
		int idc = ::GetDlgCtrlID(::GetFocus());
		if(idc == IDC_AGOLD_ADDR)
		{
			last_keyboard_input = 0;
			last_keyboard_input_len = 0;
			OnSetfocusAgoldAddress();
			setFocus(IDC_DO_PWRCAL);
		}
		else if(idc == IDC_BGOLD_ADDR)
		{ 
			last_keyboard_input = 0;
			last_keyboard_input_len = 0;
			OnSetfocusBgoldAddress();
			setFocus(IDC_DO_PWRCAL);
		}
		return TRUE;
	}
	
	if((WM_KEYDOWN == pMsg->message) && (VK_RETURN == pMsg->wParam))
	{
		int idc = ::GetDlgCtrlID(::GetFocus());
		if(idc == IDC_AGOLD_ADDR)
		{
			CString text;
			GetDlgItem(IDC_AGOLD_ADDR)->GetWindowText(text);
			int len = text.GetLength();
			if(len != 12)
			{
				OnSetfocusAgoldAddress();
				if(len && ((text[len-1] == 'B') || (text[len-1] == 'b')))
				{
					OnSetfocusBgoldAddress();
					setFocus(IDC_BGOLD_ADDR);
				}
			}
			else
			{
			
				if(!adjust_hexdigital(text)) 
				{
					//SET A TEXT -1 , SET FOUC A
					OnSetfocusAgoldAddress();
					//setFocus(IDC_AGOLD_ADDR);
				}
				else   //A DEV address input ok
				{
					if(bdev_address_inputed && (text == bdev_input_addr))
					{
						//SET B TEXT -1 SET FOUC B
						OnSetfocusBgoldAddress();
						//setFocus(IDC_BGOLD_ADDR);
					}
					adev_address_inputed = true;
					adev_input_addr = text;
					
					GetDlgItem(IDC_AGOLD_ADDR)->SetWindowText(text);
					((CEdit*)GetDlgItem(IDC_AGOLD_ADDR))->SetSel(-1);
					setFocus(IDC_DO_PWRCAL);
					
				}
			}
		}
		else if(idc == IDC_BGOLD_ADDR)
		{
			CString text;
			GetDlgItem(IDC_BGOLD_ADDR)->GetWindowText(text);
			
			int len = text.GetLength();
			if(len != 12)
			{
				//OnSetfocusBdevAddress();
				OnSetfocusBgoldAddress();
				if(len && ((text[len-1] == 'A') || (text[len-1] == 'a')))
				{
					//if(!tSetA_enable) setFocus(IDC_Start);
					//else
					//{
					//	OnSetfocusAdevAddress();
						OnSetfocusAgoldAddress();
						setFocus(IDC_AGOLD_ADDR);
					//}  
				}
			}
			else
			{
				if(!adjust_hexdigital(text)) 
				{
					//SET B TEXT -1 ,SET FOUCS B
					OnSetfocusBgoldAddress();
					//setFocus(IDC_BGOLD_ADDR);
				}
				else    //B DEV address input ok
				{
					if(adev_address_inputed && (text == adev_input_addr)) 
					{
						//OnSetfocusAdevAddress();
						//SET FOUCS A
						OnSetfocusAgoldAddress();
						//setFocus(IDC_AGOLD_ADDR);
					}
					bdev_address_inputed = true;
					bdev_input_addr = text;			
					
					GetDlgItem(IDC_BGOLD_ADDR)->SetWindowText(text);
					((CEdit*)GetDlgItem(IDC_BGOLD_ADDR))->SetSel(-1);
					setFocus(IDC_DO_PWRCAL);
					//setFocus(IDC_Start);
					//update_start_butten(ATestRunning,BTestRunning);
				}
			}
		}
		else
		{
			if((last_keyboard_input == 'A')||(last_keyboard_input == 'a'))
			{ 
				OnSetfocusAgoldAddress();
				setFocus(IDC_AGOLD_ADDR);
			}
			else if(((last_keyboard_input == 'B') || (last_keyboard_input == 'b')))
			{
				OnSetfocusBgoldAddress();
				setFocus(IDC_BGOLD_ADDR);
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

#if 0
void rfpwr_loss_cal::OnSetfocusAgoldAddress()
{
	// TODO: Add your control notification handler code here
	adev_address_inputed = false;
        GetDlgItem(IDC_AGOLD_ADDR)->SetWindowText("");
        ((CEdit*)GetDlgItem(IDC_AGOLD_ADDR))->SetSel(-1);
        //if(!tSetA_enable) 
		//setFocus(IDC_Start);
	
}

void rfpwr_loss_cal::OnSetfocusBgoldAddress()
{
	// TODO: Add your control notification handler code here
	bdev_address_inputed = false;
        GetDlgItem(IDC_BGOLD_ADDR)->SetWindowText("");
        ((CEdit*)GetDlgItem(IDC_BGOLD_ADDR))->SetSel(-1);
        //if(!tSetB_enable) 
		//setFocus(IDC_Start);
	
}
#endif

void rfpwr_loss_cal::setFocus(int idc,int idc_skip)
{
        int cur_focus_id = ::GetDlgCtrlID(::GetFocus());
        if(idc_skip && (idc_skip == cur_focus_id)) return;

        if(((IDC_AGOLD_ADDR == idc_skip) || (IDC_BGOLD_ADDR == idc_skip)) &&
           ((IDC_AGOLD_ADDR == cur_focus_id) || (IDC_BGOLD_ADDR == cur_focus_id))) return;


        CWnd *pWnd = GetDlgItem(idc);
        if(pWnd->IsWindowEnabled()) 
        {
                pWnd->SetFocus();
                return;
        }

        //pWnd = GetDlgItem(IDC_Start);
        if(pWnd->IsWindowEnabled())
        {
                pWnd->SetFocus();
                return;
        }
        //GetDlgItem(IDC_LIST2)->SetFocus();
}




void rfpwr_loss_cal::OnSetfocusAgoldAddress() 
{
	// TODO: Add your control notification handler code here
		adev_address_inputed = false;
        GetDlgItem(IDC_AGOLD_ADDR)->SetWindowText("");
        ((CEdit*)GetDlgItem(IDC_AGOLD_ADDR))->SetSel(-1);
}

void rfpwr_loss_cal::OnSetfocusBgoldAddress() 
{
	// TODO: Add your control notification handler code here
		bdev_address_inputed = false;
        GetDlgItem(IDC_BGOLD_ADDR)->SetWindowText("");
        ((CEdit*)GetDlgItem(IDC_BGOLD_ADDR))->SetSel(-1);
}
