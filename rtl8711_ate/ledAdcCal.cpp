// ledAdcCal.cpp : implementation file
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
#include "ledAdcCal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


DWORD WINAPI doLEDCalTest(LPVOID input_par);


#define  MAX_TGLODEN_IDX   4

struct ledAdcResult ALedResult[MAX_TGLODEN_IDX][2];
struct ledAdcResult BLedResult[MAX_TGLODEN_IDX][2];

int ARFGoldenIdx_led[MAX_TGLODEN_IDX];
int BRFGoldenIdx_led[MAX_TGLODEN_IDX];


int idx_distribut_led[MAX_TGLODEN_IDX][2] = {
        {1,2},          //0 idx
        {1,2},          //1 idx
        {2,3},          //2 idx
        {3,1}           //3 idx
};


/////////////////////////////////////////////////////////////////////////////
// ledAdcCal dialog


ledAdcCal::ledAdcCal(CWnd* pParent /*=NULL*/)
	: CDialog(ledAdcCal::IDD, pParent)
{
	//{{AFX_DATA_INIT(ledAdcCal)
	//}}AFX_DATA_INIT


	m_prompt_font.CreatePointFont(200,"宋体");
    m_button_font.CreatePointFont(200,"宋体");

	hthread = NULL;
    hthreadId = NULL;
	iHandle = 0;

	led_test_idx = 1;
    golden_idx = 0;

	for(int i=0;i<MAX_TGLODEN_IDX;i++)
	{
		memset(&ALedResult[i][0],0,sizeof(struct ledAdcResult));
		memset(&ALedResult[i][1],0,sizeof(struct ledAdcResult));
		memset(&BLedResult[i][0],0,sizeof(struct ledAdcResult));
		memset(&BLedResult[i][1],0,sizeof(struct ledAdcResult));
		ARFGoldenIdx_led[i] = 0;
		BRFGoldenIdx_led[i] = 0;
	}       
}




void ledAdcCal::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ledAdcCal)
	DDX_Control(pDX, IDOK, m_ok_button);
	DDX_Control(pDX, IDCANCEL, m_cancel_button);
	DDX_Control(pDX, IDC_LED_CAL_BTN, m_led_cal_btn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ledAdcCal, CDialog)
	//{{AFX_MSG_MAP(ledAdcCal)
	ON_BN_CLICKED(IDC_LED_CAL_BTN, OnLedCal)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_PROMPT, OnPrompt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ledAdcCal message handlers

BOOL ledAdcCal::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CDialog::OnInitDialog();	
	m_led_cal_btn.SetFont(&m_button_font);

	led_test_finish();

	ConfigFile &cfg = pCfgf;
#if 0
	if(pCfgf.ItemSelect(s_U45_sel))
	{
	   dutA_snr_sel_id = s_U45_sel;

	   dutA_mcu_r_grp =     MCU_PIO_DUT1_RED_0;
	   dutA_mcu_r_pio_num = MCU_PIO_NUM_DUT1_RED_0;
	   dutA_mcu_g_grp =     MCU_PIO_DUT1_GREEN_0;
	   dutA_mcu_g_pio_num = MCU_PIO_NUM_DUT1_GREEN_0;
	   dutA_mcu_b_grp =     MCU_PIO_DUT1_BLUE_0;
	   dutA_mcu_b_pio_num = MCU_PIO_NUM_DUT1_BLUE_0;
	}
	else if(pCfgf.ItemSelect(s_U51_sel))
	{
		dutA_snr_sel_id = s_U51_sel;

       dutA_mcu_r_grp =     MCU_PIO_DUT1_RED_1;
	   dutA_mcu_r_pio_num = MCU_PIO_NUM_DUT1_RED_1;
	   dutA_mcu_g_grp =     MCU_PIO_DUT1_GREEN_1;
	   dutA_mcu_g_pio_num = MCU_PIO_NUM_DUT1_GREEN_1;
	   dutA_mcu_b_grp =     MCU_PIO_DUT1_BLUE_1;
	   dutA_mcu_b_pio_num = MCU_PIO_NUM_DUT1_BLUE_1;
	}
	else if(pCfgf.ItemSelect(s_U56_sel))
	{
	   dutA_snr_sel_id = s_U56_sel;

	   dutA_mcu_r_grp =     MCU_PIO_DUT1_RED_2;
	   dutA_mcu_r_pio_num = MCU_PIO_NUM_DUT1_RED_2;
	   dutA_mcu_g_grp =     MCU_PIO_DUT1_GREEN_2;
	   dutA_mcu_g_pio_num = MCU_PIO_NUM_DUT1_GREEN_2;
	   dutA_mcu_b_grp =     MCU_PIO_DUT1_BLUE_2;
	   dutA_mcu_b_pio_num = MCU_PIO_NUM_DUT1_BLUE_2;
	}
	else
	{
	    AfxMessageBox("请先选择DUT A使用的颜色传感器!!");
		EndDialog(IDCANCEL);
		return TRUE;
	}
#endif
	ApathLedAdc[0] = cfg.Vint((dutA_snr_sel_id+1));
	ApathLedAdc[1] = cfg.Vint((dutA_snr_sel_id+2));
	ApathLedAdc[2] = cfg.Vint((dutA_snr_sel_id+3));
	ApathLedAdc[3] = cfg.Vint((dutA_snr_sel_id+4));
	ApathLedAdc[4] = cfg.Vint((dutA_snr_sel_id+5));
	ApathLedAdc[5] = cfg.Vint((dutA_snr_sel_id+6));
	ApathLedAdc[6] = cfg.Vint((dutA_snr_sel_id+13));
	ApathLedAdc[7] = cfg.Vint((dutA_snr_sel_id+14));
	ApathLedAdc[8] = cfg.Vint((dutA_snr_sel_id+15));
	ApathLedAdc[9] = cfg.Vint((dutA_snr_sel_id+16));
	ApathLedAdc[10] = cfg.Vint((dutA_snr_sel_id+17));
	ApathLedAdc[11] = cfg.Vint((dutA_snr_sel_id+18));
	

	if(pCfgf.ItemSelect(s_BModle_Sel))
	{
#if 0
		if(pCfgf.ItemSelect(s_U49_sel))
		{
			dutB_snr_sel_id = s_U49_sel;
			
			dutB_mcu_r_grp =     MCU_PIO_DUT2_RED_0;
			dutB_mcu_r_pio_num = MCU_PIO_NUM_DUT2_RED_0;
			dutB_mcu_g_grp =     MCU_PIO_DUT2_GREEN_0;
			dutB_mcu_g_pio_num = MCU_PIO_NUM_DUT2_GREEN_0;
			dutB_mcu_b_grp =     MCU_PIO_DUT2_BLUE_0;
			dutB_mcu_b_pio_num = MCU_PIO_NUM_DUT2_BLUE_0;
			
		}
		else if(pCfgf.ItemSelect(s_U55_sel))
		{
			dutB_snr_sel_id = s_U55_sel;
			
			dutB_mcu_r_grp =     MCU_PIO_DUT2_RED_1;
			dutB_mcu_r_pio_num = MCU_PIO_NUM_DUT2_RED_1;
			dutB_mcu_g_grp =     MCU_PIO_DUT2_GREEN_1;
			dutB_mcu_g_pio_num = MCU_PIO_NUM_DUT2_GREEN_1;
			dutB_mcu_b_grp =     MCU_PIO_DUT2_BLUE_1;
			dutB_mcu_b_pio_num = MCU_PIO_NUM_DUT2_BLUE_1;
		}
		else if(pCfgf.ItemSelect(s_U60_sel))
		{
			dutB_snr_sel_id = s_U60_sel;
			
			dutB_mcu_r_grp =     MCU_PIO_DUT2_RED_2;
			dutB_mcu_r_pio_num = MCU_PIO_NUM_DUT2_RED_2;
			dutB_mcu_g_grp =     MCU_PIO_DUT2_GREEN_2;
			dutB_mcu_g_pio_num = MCU_PIO_NUM_DUT2_GREEN_2;
			dutB_mcu_b_grp =     MCU_PIO_DUT2_BLUE_2;
			dutB_mcu_b_pio_num = MCU_PIO_NUM_DUT2_BLUE_2;
		}
		else
		{
			AfxMessageBox("请先选择DUT B使用的颜色传感器!!");
			EndDialog(IDCANCEL);
			return TRUE;
		}
#endif		
		BpathLedAdc[0] = cfg.Vint((dutB_snr_sel_id+1));
		BpathLedAdc[1] = cfg.Vint((dutB_snr_sel_id+2));
		BpathLedAdc[2] = cfg.Vint((dutB_snr_sel_id+3));
		BpathLedAdc[3] = cfg.Vint((dutB_snr_sel_id+4));
		BpathLedAdc[4] = cfg.Vint((dutB_snr_sel_id+5));
		BpathLedAdc[5] = cfg.Vint((dutB_snr_sel_id+6));
		BpathLedAdc[6] = cfg.Vint((dutB_snr_sel_id+13));
		BpathLedAdc[7] = cfg.Vint((dutB_snr_sel_id+14));
		BpathLedAdc[8] = cfg.Vint((dutB_snr_sel_id+15));
		BpathLedAdc[9] = cfg.Vint((dutB_snr_sel_id+16));
		BpathLedAdc[10] = cfg.Vint((dutB_snr_sel_id+17));
		BpathLedAdc[11] = cfg.Vint((dutB_snr_sel_id+18));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ledAdcCal::OnLedCal() 
{
	// TODO: Add your control notification handler code here

	if(!pMCU1_Com || !pMCU2_Com)
	{
		AfxMessageBox("MCU未分配COM,请确认是否连接仪器!!");
		EndDialog(IDCANCEL);
		return;
	}
	
	
	if(led_test_idx >= MAX_TGLODEN_IDX)
	{
		AfxMessageBox("测试已完成，请点击\"确认\"按键完成校正!!");
		return;
	}
	
	m_ok_button.EnableWindow(false);
	m_cancel_button.EnableWindow(false);
	m_led_cal_btn.EnableWindow(false);	
	
	hthread = CreateThread(NULL,0,doLEDCalTest,this,0,&hthreadId);
	
}


//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
#define PROMP(str) do{ dlg->GetDlgItem(IDC_PROMPT2)->SetWindowText(str); } while(0)

void ledAdcCal::show_prompt(CString str)
{
        GetDlgItem(IDC_PROMPT2)->SetWindowText(str);
}


DWORD WINAPI doLEDCalTest(LPVOID input_par)
{
        bool a_res = true;
	    bool b_res = true;    
	    ConfigFile &cfg = pCfgf;
        ledAdcCal *dlg = (ledAdcCal*)input_par;

        bool need_box = pCfgf.ItemSelect(parTestBoxNeed);
        bool op_ok = true;
        CString temp;

        //cfg dut routing
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
			      PROMP("Led off err");
				  op_ok = false;
			   }
			}	
        }

        if(op_ok)
        {
			//wait dut connect tbox
		    Sleep(pCfgf.Vulong(s_close_tbox_delay));

			PROMP("set dut pwr on");
			//MCU set dut pwr on to connect spi
			if(!pMCU2_Com->set_dut_pwr_on(1, MCU_PIO_DUT_PWR, MCU_PIO_NUM_DUT_PWR))
			{
				PROMP("set dut pwr err");
				return (false);
			}
			PROMP("set dut spi on");
			if(!pMCU2_Com->set_dut_spi_on(1, MCU_PIO_SPI,MCU_PIO_NUM_SPI))
			{
				PROMP("set dut spi on err");
				return (false);
			}
			
			if(dlg->led_test_idx < MAX_TGLODEN_IDX)
			{
				dlg->tset = pMainWnd->tSetA;
				if(pMainWnd->tSetA_enable)
				{
					pMCU2_Com->set_dut_current_on(1, MCU_PIO_DUT1_CURRENT,MCU_PIO_NUM_DUT1_CURRENT);
					pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY);
					Sleep(500);
					pMCU2_Com->set_dut_vreg_on(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG);	
					
					a_res = dlg->do_LED_test(dlg->led_test_idx);
				}
				dlg->tset = pMainWnd->tSetB;
				if(pMainWnd->tSetB_enable)
				{
					if(a_res) 
					{	
						pMCU1_Com->set_dut_current_on(0, MCU_PIO_DUT2_CURRENT,MCU_PIO_NUM_DUT2_CURRENT);
						//dut2 battery也是用U5
						pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY);
						Sleep(500);
						pMCU1_Com->set_dut_vreg_on(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG);
						
						b_res = dlg->do_LED_test(dlg->led_test_idx);
					}
					else
					{
						b_res = false;
					}
				}
				if(a_res && b_res) dlg->led_test_idx += 1;
			}
        }

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
				
		pMCU2_Com->set_dut_current_off(1, MCU_PIO_DUT1_CURRENT,MCU_PIO_NUM_DUT1_CURRENT);
		pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY);
		pMCU2_Com->set_dut_vreg_off(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG);
				
		pMCU1_Com->set_dut_current_off(0, MCU_PIO_DUT2_CURRENT,MCU_PIO_NUM_DUT2_CURRENT);
		pMCU1_Com->set_dut_bat_off(0, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY);
		pMCU1_Com->set_dut_vreg_off(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG);
				
	    pMCU2_Com->set_dut_spi_off(1, MCU_PIO_SPI,MCU_PIO_NUM_SPI);
		pMCU2_Com->set_dut_pwr_off(1, MCU_PIO_DUT_PWR, MCU_PIO_NUM_DUT_PWR);

		dlg->led_test_finish();
  
	return 0;
}


unsigned short pio_led = 0xffff;
bool ledAdcCal::do_LED_test(int trf_idx)
{	    
	    uint16 nap;
	    uint8 uap;
	    uint32 lap;
		uint32 status;
		//int iHandle;
		uint16 io = 0;
		uint16 ver_len = 0;
	    CString bt_addr;    
	    ConfigFile &cfg = pCfgf;
        CString Dev;
		CString temp;
		CBsDevice *pobj; 
        pobj = tset->pSpiObj;
		int i = 0;
		int ENABLE = 1;
		int DISABLE = 0;
		int red,green,blue;
		bool led0_gt_zero = false;
		bool led1_gt_zero = false;


	   Dev = (tset->dev_id == TEST_DEV_ID_A)?"DUT A":"DUT B";	
//#if 0
		do
		{
			iHandle = openTestEngineSpiTrans(pobj->spi_port->port_num(),0);
			i++;
			Sleep(50);
		}while(iHandle == 0 &&(i < 5));
		
		i = 0;
		do
		{
		  status = psReadBdAddr(iHandle,&lap,&uap,&nap);	
		  if(status == TE_OK) break;
          i++;
		  Sleep(50);
		} while(i < 5);

        if(status != TE_OK)
		{
			closeTestEngine(iHandle);
			err_str = Dev + " read bt addr err";
			show_prompt(err_str);
			return(false);
		}

		bt_addr.Format("%.4X%.2X%.6X",nap,uap,lap);
		
		tset->r_addr.nap = nap;
		tset->r_addr.uap = uap;
		tset->r_addr.lap = lap;
		tset->r_addr.is_valid = true;
		golden_idx = tset->gloden_idx();
		if(!golden_idx)
		{ 
			closeTestEngine(iHandle);
			err_str = "";
			temp = Dev;
			err_str.Format(": %.4X%.2X %.6X 不是金机\n",tset->r_addr.nap,tset->r_addr.uap,tset->r_addr.lap);
			temp += err_str;
			show_prompt(temp);
			return(false);
		}
		
		int dev_AB_idx = 0;
		if(tset->dev_id != TEST_DEV_ID_A) dev_AB_idx = 1;
		int group_golden_idx = tset->golden_idx_in_group(golden_idx);
		if(group_golden_idx != idx_distribut_led[led_test_idx][dev_AB_idx])
		{
			closeTestEngine(iHandle);
			err_str = "";
			temp = Dev;
			err_str.Format(": %.4X%.2X %.6X 是金机%d, 不是金机%d,请重新放置!\n",tset->r_addr.nap,tset->r_addr.uap,
				tset->r_addr.lap,group_golden_idx,idx_distribut_led[led_test_idx][dev_AB_idx]);
			temp += err_str;
			show_prompt(temp);
			return(false);
		}

		//vm disable, 1
		i = 0;
		do
		{
		  status = psWriteVmDisable(iHandle, VM_DISABLE);
		  if(status == TE_OK) break;
		  i++;
		  Sleep(50);
		}while(i<5);

		if(status != TE_OK)
		{
			closeTestEngine(iHandle);
			temp = Dev;
			err_str =  " Vm Disable Error";
            temp += err_str;
			show_prompt(temp);
			return(false);			
		}
		else 
		{
			Sleep(1000);
			uint8 cur_st;
			i = 0;
			do
			{
			   status = psReadVmDisable(iHandle,&cur_st);
			   if(status == TE_OK) break;
			   i++;
			   Sleep(50);
			}while(i<5);

			if((status != TE_OK) || (cur_st != 1))
			{
				closeTestEngine(iHandle);
				temp = Dev;
				err_str = " Vm Disable Not Ok";
				temp += err_str;
				show_prompt(temp);
				return(false);
			}
		}		
		//bc reset
		for(i = 0; i < 5; i++)
		{
			status = bccmdSetWarmReset(iHandle,1000);
			if(status == TE_OK)
			{
				Sleep(1000);
				break;
			}
			else if(i == 4)
			{
				closeTestEngine(iHandle);
				temp = Dev;
				err_str = " bc warm reset err";
				temp += err_str;
				show_prompt(temp);
				return(false);
			}
			
			Sleep(500);
		}
		
		//vm enable, 0
		i = 0;
		do 
		{
			status = psWriteVmDisable(iHandle, 0);
			if(status == TE_OK) break;
			i++;
			Sleep(50);
		}while(i<5);

		if(status != TE_OK)
		{
			closeTestEngine(iHandle);
			err_str =  "vm enable error";
			show_prompt(err_str);
			return(false);
			
		}
		else 
		{
			Sleep(1000);
			uint8 cur_st;
			i = 0;
			do
			{
			   status = psReadVmDisable(iHandle,&cur_st);
			   if(status == TE_OK) break;
			   i++;
			   Sleep(50);
			}while(i<5);

			if((status != TE_OK) || (cur_st != 0))
			{
				closeTestEngine(iHandle);
				temp = Dev;
				err_str = " vm enable not ok";
				temp += err_str;
				show_prompt(temp);
			    return(false);
			}
		}
			
        temp = "先点亮 ";
		temp += Dev;
		temp += " 蓝灯";
		show_prompt(temp);
		Sleep(1000);		
		
		status = bccmdLedEnable(iHandle, 0, ENABLE);
		if(status != TE_OK)
		{
			closeTestEngine(iHandle);
			temp = Dev;
			err_str= " LED0 on error";
			temp += err_str;
			show_prompt(temp);
			return(false);
		}	

		temp = "开始读 ";
		temp += Dev;
		temp += " 蓝灯的RGB值";
		show_prompt(temp);
		Sleep(2000);

		//struct ledAdcResult mt_res;   
		//memset(&mt_res, 0, sizeof(struct ledAdcResult));
		red = 0;
		green = 0;
		blue = 0;
		if(!get_led_val(&red, &green, &blue))
		{
			closeTestEngine(iHandle);
			temp = "read ";
			temp += Dev;
			err_str= " LED0 RGB data error";
			temp += err_str;
			show_prompt(temp);
			return(false);
		}

		//if(red&&green&&blue)
		//{
			//led0_gt_zero = true;
		//}

        if(led_test_idx < MAX_TGLODEN_IDX)
		{
			if(tset->dev_id == TEST_DEV_ID_A)
			{
				if( 
					(red < ApathLedAdc[0])  || (red > ApathLedAdc[1])  ||
					(green < ApathLedAdc[2])|| (green > ApathLedAdc[3])||
					(blue < ApathLedAdc[4]) || (blue > ApathLedAdc[5]) 
					)
					
				{
					closeTestEngine(iHandle);
					temp = Dev;
			        err_str= " LED0(蓝灯)亮度太低，请检查测试架!!";
			        temp += err_str;
			        show_prompt(temp);
					return (false);
				}
				else 
				{
					ALedResult[led_test_idx][0].led_ad.B = blue;
					ALedResult[led_test_idx][0].led_ad.G = green;
					ALedResult[led_test_idx][0].led_ad.R = red;

					ARFGoldenIdx_led[led_test_idx] = tset->gloden_idx();
				}
			}
			else
			{
				if( (red < BpathLedAdc[0])  || (red > BpathLedAdc[1]) ||
					(green < BpathLedAdc[2])|| (green > BpathLedAdc[3])||
					(blue < BpathLedAdc[4]) || (blue > BpathLedAdc[5]) )
				{
					closeTestEngine(iHandle);
					temp = Dev;
			        err_str= " LED0(蓝灯)亮度太低，请检查测试架!!";
			        temp += err_str;
			        show_prompt(temp);
					return (false);
				}
				else
				{				

					BLedResult[led_test_idx][0].led_ad.B = blue;
					BLedResult[led_test_idx][0].led_ad.G = green;
					BLedResult[led_test_idx][0].led_ad.R = red;

					BRFGoldenIdx_led[led_test_idx] = tset->gloden_idx();
				}
			}
		}

		
		status = bccmdLedEnable(iHandle, 0, DISABLE);
		if(status != TE_OK)
		{
			closeTestEngine(iHandle);
			temp = Dev;
			err_str= " LED0 off error";
			temp += err_str;
			show_prompt(temp);
			return(false);
		}

		temp += " 熄灭蓝灯,再点亮白灯...";
		show_prompt(temp);
		Sleep(1000);		
		
		status = bccmdLedEnable(iHandle, 1, ENABLE);
		if(status != TE_OK)
		{
			closeTestEngine(iHandle);
			temp = Dev;
			err_str= " LED1 on error";
			temp += err_str;
			show_prompt(temp);
			return(false);
		}	

		temp = "开始读 ";
		temp += Dev;
		temp += " 白灯的RGB值";
		show_prompt(temp);
		Sleep(2000);

		red = 0;
		green = 0;
		blue = 0;
		if(!get_led_val(&red, &green, &blue))
		{
			closeTestEngine(iHandle);
			temp = "read ";
			temp += Dev;
			err_str= " LED1 RGB data err";
			temp += err_str;
			show_prompt(temp);
		    return(false);
		}

		//if(red&&green&&blue)
		//{
			//led1_gt_zero = true;
		//}

        if(led_test_idx < MAX_TGLODEN_IDX)
		{
			if(tset->dev_id == TEST_DEV_ID_A)
			{
				if( 
					(red < ApathLedAdc[6])  || (red > ApathLedAdc[7]) ||
					(green < ApathLedAdc[8])|| (green > ApathLedAdc[9])||
					(blue < ApathLedAdc[10])|| (blue > ApathLedAdc[11])
					)
				{
					closeTestEngine(iHandle);
					temp = Dev;
			        err_str= " LED1(白灯)亮度太低，请检查测试架!!";
			        temp += err_str;
			        show_prompt(temp);
					return (false);
				}
				else 
				{
					
					ALedResult[led_test_idx][1].led_ad.B = blue;
					ALedResult[led_test_idx][1].led_ad.G = green;
					ALedResult[led_test_idx][1].led_ad.R = red;
					ARFGoldenIdx_led[led_test_idx] = tset->gloden_idx();
				}
			}
			else
			{
				if( 
					(red < BpathLedAdc[6])  || (red > BpathLedAdc[7]) ||
					(green < BpathLedAdc[8])|| (green > BpathLedAdc[9])|| 
					(blue < BpathLedAdc[10]) || (blue > BpathLedAdc[11]) 
					)
				{
					closeTestEngine(iHandle);
					temp = Dev;
			        err_str= " LED1(白灯)亮度太低，请检查测试架!!";
			        temp += err_str;
			        show_prompt(temp);
					return (false);
				}
				else 
				{
					BLedResult[led_test_idx][1].led_ad.B = blue;
					BLedResult[led_test_idx][1].led_ad.G = green;
					BLedResult[led_test_idx][1].led_ad.R = red;

					BRFGoldenIdx_led[led_test_idx] = tset->gloden_idx();
				}
			}
		}

		status = bccmdLedEnable(iHandle, 1, DISABLE);
		if(status != TE_OK)
		{
			closeTestEngine(iHandle);
			temp = Dev;
			err_str= " LED1 off error";
			temp += err_str;
			show_prompt(temp);
			return(false);
		}

		closeTestEngine(iHandle);
		temp = Dev;
		temp += " LED0(蓝灯)和LED1(白灯)测到的RGB数据有效,测试成功!";
		show_prompt(temp);
		return(true);
		
#if 0
		if(led0_gt_zero && led1_gt_zero)
		{
            closeTestEngine(iHandle);
			temp = Dev;
			temp += " LED0(蓝灯)和LED1(白灯)测到的RGB数据有效,测试成功!";
			show_prompt(temp);
			return(true);
		}

		else
		{
		    closeTestEngine(iHandle);
			if(!led0_gt_zero)
			{
				if(led1_gt_zero)
				{
				    temp = Dev;
					temp += " 测试数据无效,LED0(蓝灯)测到的RGB值中出现了0值,请检查机架后重新测试!";
					show_prompt(temp);
				}
			    else
				{
				    temp = Dev;
                    temp += " 测试数据无效,LED0(蓝灯)和LED1(白灯)测到的RGB值中都出现了0值,请检查机架后重新测试!";
					show_prompt(temp);
				}
			}
			else
			{
				temp = Dev;
				temp += " 测试数据无效,LED1(白灯)测到的RGB值中出现了0值,请检查机架后重新测试!";
				show_prompt(temp);
    
			}
			return(false);
		}
#endif
}


bool ledAdcCal::get_led_val(int *red, int *green, int *blue)
{
    char rx_buf[30] = {0};
	char *pbuf = NULL;
	char recv;
	int i;
	
	if(tset->dev_id == TEST_DEV_ID_A)
		{
			if(!pMCU2_Com->get_dut_rgb(1, dutA_mcu_r_grp, dutA_mcu_r_pio_num, &rx_buf[0]))
			{
			    err_str = "DUT A u5 read Red data error";
			    show_prompt(err_str);
				return false;
			}
			
			pbuf = &rx_buf[11];    //MCU+ADC_RD()
			i = 4;
			while(i--)
			{
				recv = *pbuf++;
				if(recv >= '0' && recv <= '9')
				{
					*red = *red*16 + (recv - '0');
				}
				else if(recv >= 'A' && recv <= 'F')
				{
					*red = *red*16 + (recv - 'A' + 10);
				}
			}
			
			memset(rx_buf, 0, 30);
			i = 4;
			//dut1 read green ADC value
			if(!pMCU2_Com->get_dut_rgb(1, dutA_mcu_g_grp, dutA_mcu_g_pio_num, &rx_buf[0]))
			{
				err_str = "DUT A u5 read Green data error";
			    show_prompt(err_str);
				return false;
			}
			
			pbuf = &rx_buf[11];  //MCU+ADC_RD()
			while(i--)
			{  
				recv = *pbuf++;
				if(recv >= '0' && recv <= '9')
				{
					*green = *green*16 + (recv - '0');
				}
				else if(recv >= 'A' && recv <= 'F')
				{
					*green = *green*16 + (recv - 'A' + 10);
				}
			}
			
			memset(rx_buf, 0, 30);
			i = 4;
			//dut1 read blue ADC value
			if(!pMCU2_Com->get_dut_rgb(1, dutA_mcu_b_grp, dutA_mcu_b_pio_num, &rx_buf[0]))
			{
				err_str = "DUT A u5 read Blue data error";
			    show_prompt(err_str);
				return false;
			}
			
			pbuf = &rx_buf[11];
			while(i--)
			{	
				recv = *pbuf++;
				if(recv >= '0' && recv <= '9')
				{
					*blue = *blue*16 + (recv - '0');
				}
				else if(recv >= 'A' && recv <= 'F')
				{
					*blue = *blue*16 + (recv - 'A' + 10);
				}
			}
		}
		else
		{
			if(!pMCU1_Com->get_dut_rgb(0, dutB_mcu_r_grp, dutB_mcu_r_pio_num, &rx_buf[0]))
			{
				err_str = "DUT B u18 read Red data error";
			    show_prompt(err_str);
				return false;
			}
			
			pbuf = &rx_buf[11];
			i = 4;
			while(i--)
			{
				recv = *pbuf++;
				if(recv >= '0' && recv <= '9')
				{
					*red = *red*16 + (recv - '0');
				}
				else if(recv >= 'A' && recv <= 'F')
				{
					*red = *red*16 + (recv - 'A' + 10);
				}
			}
			
			
			memset(rx_buf, 0, 30);
			i = 4;
			//dut2 read green ADC value
			if(!pMCU1_Com->get_dut_rgb(0, dutB_mcu_g_grp, dutB_mcu_g_pio_num, &rx_buf[0]))
			{
				err_str = "DUT B u18 read Green data error";
			    show_prompt(err_str);
				return false;
			}
			
			pbuf = &rx_buf[11];
			while(i--)
			{  
				recv = *pbuf++;
				if(recv >= '0' && recv <= '9')
				{
					*green = *green*16 + (recv - '0');
				}
				else if(recv >= 'A' && recv <= 'F')
				{
					*green = *green*16 + (recv - 'A' + 10);
				}
			}
			
			memset(rx_buf, 0, 30);
			i = 4;
			//dut2 read blue ADC value
			if(!pMCU1_Com->get_dut_rgb(0, dutB_mcu_b_grp, dutB_mcu_b_pio_num, &rx_buf[0]))
			{
				err_str = "DUT B u18 read Blue data error";
			    show_prompt(err_str);
				return false;
			}
			
			pbuf = &rx_buf[11];
			while(i--)
			{	
				recv = *pbuf++;
				if(recv >= '0' && recv <= '9')
				{
					*blue = *blue*16 + (recv - '0');
				}
				else if(recv >= 'A' && recv <= 'F')
				{
					*blue = *blue*16 + (recv - 'A' + 10);
				}
			}	
		}
   
		return (true);
}


void ledAdcCal::led_test_finish(void)
{
        m_ok_button.EnableWindow(true);
        m_cancel_button.EnableWindow(true);
        m_led_cal_btn.EnableWindow(true);

        CString title;
        if(led_test_idx >= MAX_TGLODEN_IDX) title = "测试已完成\n点击确认完成校验";
        else title.Format("请在 A 上放入金机%d\nB 上放入金机%d",
			  idx_distribut_led[led_test_idx][0],idx_distribut_led[led_test_idx][1]);
        GetDlgItem(IDC_PROMPT)->SetWindowText(title);

        CString temp,a_temp,b_temp;
        a_temp = "             A DUT 亮度\n";
        a_temp += "             LED0           LED1\n";  //10,7,7 space
        b_temp = "             B DUT 亮度\n";
        b_temp += "             LED0           LED1\n";  //10,7,7 space
        for(int i=1;i < led_test_idx;i++)
        {	
			//dut A
			if(!ARFGoldenIdx_led[i]) temp = "dut       ";  //8 space
			else
			{
				temp = pMainWnd->tSetA->gloden_idx_name(ARFGoldenIdx_led[i]);
				temp += "  ";  //add 3 space
			}
			a_temp += temp;
			temp = "";
			
			if(ALedResult[i][0].led_ad.R)
			{
			   temp.Format("R  %04d           ", ALedResult[i][0].led_ad.R);  //6 space
			   a_temp += temp;
			   if(ALedResult[i][1].led_ad.R)
			   {
			      temp.Format("%04d", ALedResult[i][1].led_ad.R); 
			   }
			   else
			   {
			      temp.Format("NC(000)");
			   }
			   a_temp += temp;
			}
			else
			{
			   temp.Format("R NC(000)       ");  //6 space
			   a_temp += temp;
			   if(ALedResult[i][1].led_ad.R)
			   {
			      temp.Format("%04d", ALedResult[i][1].led_ad.R); 
			   }
			   else
			   {
			      temp.Format("NC(000)");
			   }
			   a_temp += temp;
			}
			a_temp += "\n";

			if(ALedResult[i][0].led_ad.G)
			{
			   temp.Format("          G  %04d           ", ALedResult[i][0].led_ad.G);  //6 space
			   a_temp += temp;
			   if(ALedResult[i][1].led_ad.G)
			   {
			      temp.Format("%04d", ALedResult[i][1].led_ad.G); 
			   }
			   else
			   {
			      temp.Format("NC(000)");
			   }
			   a_temp += temp;
			}
			else
			{
			   temp.Format("          G NC(000)       ");  //6 space
			   a_temp += temp;
			   if(ALedResult[i][1].led_ad.G)
			   {
			      temp.Format("%04d", ALedResult[i][1].led_ad.G); 
			   }
			   else
			   {
			      temp.Format("NC(000)");
			   }
			   a_temp += temp;
			}
            a_temp += "\n";

			if(ALedResult[i][0].led_ad.B)
			{
			   temp.Format("          B  %04d           ", ALedResult[i][0].led_ad.B);  //6 space
			   a_temp += temp;
			   if(ALedResult[i][1].led_ad.B)
			   {
			      temp.Format("%04d", ALedResult[i][1].led_ad.B); 
			   }
			   else
			   {
			      temp.Format("NC(000)");
			   }
			   a_temp += temp;
			}
			else
			{
			   temp.Format("          B NC(000)       ");  //6 space
			   a_temp += temp;
			   if(ALedResult[i][1].led_ad.B)
			   {
			      temp.Format("%04d", ALedResult[i][1].led_ad.B); 
			   }
			   else
			   {
			      temp.Format("NC(000)");
			   }
			   a_temp += temp;
			}
            a_temp += "\n";
			
			
			//dut B
			if(!BRFGoldenIdx_led[i]) temp = "dut       ";  //8 space
			else
			{
				temp = pMainWnd->tSetB->gloden_idx_name(BRFGoldenIdx_led[i]);
				temp += "  ";  //add 3 space
			}
			b_temp += temp;	
			temp = "";


			if(BLedResult[i][0].led_ad.R)
			{
			   temp.Format("R  %04d           ", BLedResult[i][0].led_ad.R);  //6 space
			   b_temp += temp;
			   if(BLedResult[i][1].led_ad.R)
			   {
			      temp.Format("%04d", BLedResult[i][1].led_ad.R); 
			   }
			   else
			   {
			      temp.Format("NC(000)");
			   }
			   b_temp += temp;
			}
			else
			{
			   temp.Format("R NC(000)       ");  //6 space
			   b_temp += temp;
			   if(BLedResult[i][1].led_ad.R)
			   {
			      temp.Format("%04d", BLedResult[i][1].led_ad.R); 
			   }
			   else
			   {
			      temp.Format("NC(000)");
			   }
			   b_temp += temp;
			}
			b_temp += "\n";

			if(BLedResult[i][0].led_ad.G)
			{
			   temp.Format("          G  %04d           ", BLedResult[i][0].led_ad.G);  //6 space
			   b_temp += temp;
			   if(BLedResult[i][1].led_ad.G)
			   {
			      temp.Format("%04d", BLedResult[i][1].led_ad.G); 
			   }
			   else
			   {
			      temp.Format("NC(000)");
			   }
			   b_temp += temp;
			}
			else
			{
			   temp.Format("          G NC(000)       ");  //6 space
			   b_temp += temp;
			   if(BLedResult[i][1].led_ad.G)
			   {
			      temp.Format("%04d", BLedResult[i][1].led_ad.G); 
			   }
			   else
			   {
			      temp.Format("NC(000)");
			   }
			   b_temp += temp;
			}
            b_temp += "\n";

			if(BLedResult[i][0].led_ad.B)
			{
			   temp.Format("          B  %04d           ", BLedResult[i][0].led_ad.B);  //6 space
			   b_temp += temp;
			   if(BLedResult[i][1].led_ad.B)
			   {
			      temp.Format("%04d", BLedResult[i][1].led_ad.B); 
			   }
			   else
			   {
			      temp.Format("NC(000)");
			   }
			   b_temp += temp;
			}
			else
			{
			   temp.Format("          B NC(000)       ");  //6 space
			   b_temp += temp;
			   if(BLedResult[i][1].led_ad.B)
			   {
			      temp.Format("%04d", BLedResult[i][1].led_ad.B); 
			   }
			   else
			   {
			      temp.Format("NC(000)");
			   }
			   b_temp += temp;
			}
            b_temp += "\n";
        }
        GetDlgItem(IDC_A_LED)->SetWindowText(a_temp);
        GetDlgItem(IDC_B_LED)->SetWindowText(b_temp); 
}

void ledAdcCal::OnOK() 
{
	// TODO: Add extra validation here
	    unsigned long  A_led0_r=0, A_led0_g=0, A_led0_b=0;
		unsigned long  A_led1_r=0, A_led1_g=0, A_led1_b=0;
		
		unsigned long  B_led0_r=0, B_led0_g=0, B_led0_b=0;
		unsigned long  B_led1_r=0, B_led1_g=0, B_led1_b=0;
	    ConfigFile &cfg = pCfgf;

        if(led_test_idx < MAX_TGLODEN_IDX)
        {
            AfxMessageBox("数据未测完,要退出请点击\"取消\"按键!!");
            return;
        }

        for(int i=1;i<MAX_TGLODEN_IDX;i++)
        {	
			A_led0_r += ALedResult[i][0].led_ad.R; 
			A_led0_g += ALedResult[i][0].led_ad.G;
			A_led0_b += ALedResult[i][0].led_ad.B; 

			A_led1_r += ALedResult[i][1].led_ad.R; 
			A_led1_g += ALedResult[i][1].led_ad.G;
			A_led1_b += ALedResult[i][1].led_ad.B; 

        }
        A_led0_r /= (MAX_TGLODEN_IDX-1);
		A_led0_g /= (MAX_TGLODEN_IDX-1);
		A_led0_b /= (MAX_TGLODEN_IDX-1);
		
		A_led1_r /= (MAX_TGLODEN_IDX-1);
		A_led1_g /= (MAX_TGLODEN_IDX-1);
		A_led1_b /= (MAX_TGLODEN_IDX-1);
        
        cfg.ItemSet((dutA_snr_sel_id+7),  A_led0_r);
        cfg.ItemSet((dutA_snr_sel_id+8),  A_led0_g);
        cfg.ItemSet((dutA_snr_sel_id+9),  A_led0_b);
		cfg.ItemSet((dutA_snr_sel_id+19), A_led1_r);
        cfg.ItemSet((dutA_snr_sel_id+20), A_led1_g);
        cfg.ItemSet((dutA_snr_sel_id+21), A_led1_b);

        if(pMainWnd->tSetB_enable)
        {
			for(int i=1;i<MAX_TGLODEN_IDX;i++)
			{
				B_led0_r += BLedResult[i][0].led_ad.R; 
				B_led0_g += BLedResult[i][0].led_ad.G;
				B_led0_b += BLedResult[i][0].led_ad.B; 
				
				B_led1_r += BLedResult[i][1].led_ad.R; 
				B_led1_g += BLedResult[i][1].led_ad.G;
				B_led1_b += BLedResult[i][1].led_ad.B; 
			}
			
			
			B_led0_r /= (MAX_TGLODEN_IDX-1);
			B_led0_g /= (MAX_TGLODEN_IDX-1);
			B_led0_b /= (MAX_TGLODEN_IDX-1);
			
			B_led1_r /= (MAX_TGLODEN_IDX-1);
			B_led1_g /= (MAX_TGLODEN_IDX-1);
			B_led1_b /= (MAX_TGLODEN_IDX-1);

			cfg.ItemSet((dutB_snr_sel_id+7),  B_led0_r);
			cfg.ItemSet((dutB_snr_sel_id+8),  B_led0_g);
			cfg.ItemSet((dutB_snr_sel_id+9),  B_led0_b);
			cfg.ItemSet((dutB_snr_sel_id+19), B_led1_r);
			cfg.ItemSet((dutB_snr_sel_id+20), B_led1_g);
			cfg.ItemSet((dutB_snr_sel_id+21), B_led1_b);
        }
	
	CDialog::OnOK();
}

HBRUSH ledAdcCal::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
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
        
        default:break;
	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void ledAdcCal::OnCancel() 
{
	// TODO: Add extra cleanup here

	CDialog::OnCancel();
}

void ledAdcCal::OnPrompt() 
{
	// TODO: Add your control notification handler code here
	
}
