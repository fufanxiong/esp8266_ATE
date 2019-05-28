// config_deal.cpp : implementation file
//

#include "stdafx.h"
#include "PD218.h"
#include "PD218Dlg.h"
#include "err.h"
#include "GPIB.h"
#include <time.h>
#include <fstream.h>
#include "Config.h"
#include "config_deal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString Cfj_file_ini;
extern CString Cfj_file_bin;

/////////////////////////////////////////////////////////////////////////////
// config_deal dialog


config_deal::config_deal(int level,CWnd* pParent /*=NULL*/)
	: CDialog(config_deal::IDD, pParent)
{
	//{{AFX_DATA_INIT(config_deal)
	m_xtal_adj_sel = FALSE;
	m_waddr_sel = FALSE;
	m_read_addr = FALSE;
	m_N4010A_test = FALSE;
	m_BModleSel = FALSE;
	m_mic_spk = FALSE;
	m_write_file = FALSE;
	m_pwr_step1 = 0.0;
	m_pwr_step2 = 0.0;
	m_pwr_step3 = 0.0;
	m_pwr_step4 = 0.0;
	m_pwr_step5 = 0.0;
	m_pwr_step6 = 0.0;
	m_pwr_step7 = 0.0;
	m_pwr_step8 = 0.0;
	m_pwr_adj_max = 0.0;
	m_pwr_adj_min = 0.0;
	m_pwr_max = 0.0;
	m_pwr_min = 0.0;
	m_select_ch = FALSE;
	m_charge_adjust = FALSE;
	m_pwr_on_curr = FALSE;
	m_pwr_off_cur = FALSE;
	m_def_addr_verify = FALSE;
	m_Aio_0_chk = FALSE;
	m_mbias_A_chk = FALSE;
	m_mbias_B_chk = FALSE;
	m_pio_0_chk = FALSE;
	m_pio_1_chk = FALSE;
	m_pio_2_chk = FALSE;
	m_pio_3_chk = FALSE;
	m_pio_4_chk = FALSE;
	m_pio_5_chk = FALSE;
	m_pio_6_chk = FALSE;
	m_pio_7_chk = FALSE;
	m_pio_8_chk = FALSE;
	m_pio_9_chk = FALSE;
	m_pio_10_chk = FALSE;
	m_pio_11_chk = FALSE;
	m_pio_12_chk = FALSE;
	m_pio_13_chk = FALSE;
	m_pio_14_chk = FALSE;
	m_pio_15_chk = FALSE;
	m_pcm_in_out_chk = FALSE;
	m_pcm_sync_chk = FALSE;
	m_pcm_clk_chk = FALSE;
	m_read_bt_name = FALSE;
	m_hf_ic_chk = FALSE;
	m_hf_op_chk = FALSE;
	m_hf_ss_chk = FALSE;
	m_lf_ic_chk = FALSE;
	m_lf_op_chk = FALSE;
	m_lf_ss_chk = FALSE;
	m_mf_ic_chk = FALSE;
	m_mf_op_chk = FALSE;
	m_mf_ss_chk = FALSE;
	m_need_tbox = FALSE;
	m_uart_loopback = FALSE;
	m_hf_pw_chk = FALSE;
	m_lf_pw_chk = FALSE;
	m_mf_pw_chk = FALSE;
	m_keep_cspar = FALSE;
	m_waddr_auto_sel = FALSE;
	m_read_soft_ver = FALSE;
	m_mcu_io_chk = FALSE;
	m_mf_evm_chk = FALSE;
	m_lf_evm_chk = FALSE;
	m_hf_evm_chk = FALSE;
	m_8711_io_chk = FALSE;
	//}}AFX_DATA_INIT

	op_level = level;
}


void config_deal::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(config_deal)
	DDX_Control(pDX, IDC_SEL_PROJECT, m_sel_project);
	DDX_Check(pDX, IDC_Tx_Xtal_Adj, m_xtal_adj_sel);
	DDX_Check(pDX, IDC_Tx_WriteUid, m_waddr_sel);
	DDX_Check(pDX, IDC_read_addr, m_read_addr);
	DDX_Check(pDX, IDC_N4010A_TEST, m_N4010A_test);
	DDX_Check(pDX, IDC_BMODLE_SEL, m_BModleSel);
	DDX_Check(pDX, IDC_WRITE_FILE, m_write_file);
	DDX_Text(pDX, IDC_PWR_STEP1, m_pwr_step1);
	DDV_MinMaxDouble(pDX, m_pwr_step1, 0., 2.);
	DDX_Text(pDX, IDC_PWR_STEP2, m_pwr_step2);
	DDV_MinMaxDouble(pDX, m_pwr_step2, 0., 2.);
	DDX_Text(pDX, IDC_PWR_STEP3, m_pwr_step3);
	DDV_MinMaxDouble(pDX, m_pwr_step3, -2., 2.);
	DDX_Text(pDX, IDC_PWR_STEP4, m_pwr_step4);
	DDV_MinMaxDouble(pDX, m_pwr_step4, -2., 0.);
	DDX_Text(pDX, IDC_PWR_STEP5, m_pwr_step5);
	DDV_MinMaxDouble(pDX, m_pwr_step5, -2., 0.);
	DDX_Text(pDX, IDC_PWR_STEP6, m_pwr_step6);
	DDV_MinMaxDouble(pDX, m_pwr_step6, -2., 0.);
	DDX_Text(pDX, IDC_PWR_STEP7, m_pwr_step7);
	DDV_MinMaxDouble(pDX, m_pwr_step7, -2., 0.);
	DDX_Text(pDX, IDC_PWR_STEP8, m_pwr_step8);
	DDV_MinMaxDouble(pDX, m_pwr_step8, -2., 0.);
	DDX_Text(pDX, IDC_PWR_ADJ_MAX, m_pwr_adj_max);
	DDX_Text(pDX, IDC_PWR_ADJ_MIN, m_pwr_adj_min);
	DDX_Text(pDX, IDC_PWR_MAX, m_pwr_max);
	DDX_Text(pDX, IDC_PWR_MIN, m_pwr_min);
	DDX_Check(pDX, IDC_LANGUAGE_SELECT, m_select_ch);
	DDX_Check(pDX, IDC_PWR_ON_CUR, m_pwr_on_curr);
	DDX_Check(pDX, IDC_PWR_OFF_CUR, m_pwr_off_cur);
	DDX_Check(pDX, IDC_Verify_Def_Addr, m_def_addr_verify);
	DDX_Check(pDX, IDC_PIO_09_Chk, m_pio_9_chk);
	DDX_Check(pDX, IDC_HF_SS_CHK, m_hf_ss_chk);
	DDX_Check(pDX, IDC_LF_SS_CHK, m_lf_ss_chk);
	DDX_Check(pDX, IDC_MF_SS_CHK, m_mf_ss_chk);
	DDX_Check(pDX, IDC_USE_TBOX, m_need_tbox);
	DDX_Check(pDX, IDC_UART_LOOPBACK, m_uart_loopback);
	DDX_Check(pDX, IDC_HF_PW_CHK, m_hf_pw_chk);
	DDX_Check(pDX, IDC_LF_PW_CHK, m_lf_pw_chk);
	DDX_Check(pDX, IDC_MF_PW_CHK, m_mf_pw_chk);
	DDX_Check(pDX, IDC_KEEP_CSPAR, m_keep_cspar);
	DDX_Check(pDX, IDC_Tx_WriteUid_Auto, m_waddr_auto_sel);
	DDX_Check(pDX, IDC_READ_SOFT_VER, m_read_soft_ver);
	DDX_Check(pDX, IDC_MCU_IO_CHK, m_mcu_io_chk);
	DDX_Check(pDX, IDC_MF_EVM_CHK, m_mf_evm_chk);
	DDX_Check(pDX, IDC_LF_EVM_CHK, m_lf_evm_chk);
	DDX_Check(pDX, IDC_HF_EVM_CHK, m_hf_evm_chk);
	DDX_Check(pDX, IDC_8711_IO_CHK, m_8711_io_chk);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(config_deal, CDialog)
	//{{AFX_MSG_MAP(config_deal)
	ON_BN_CLICKED(IDC_INPUT_CFJ, OnInputCfj)
	ON_BN_CLICKED(IDC_OUTPUT_CFJ, OnOutputCfj)
	ON_BN_CLICKED(IDC_Tx_WriteUid, OnTxWriteUid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// config_deal message handlers

BOOL config_deal::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
#if 0
	m_sel_project.AddString("不测试");
	m_sel_project.AddString("101-01");
	m_sel_project.AddString("101-02");
	m_sel_project.AddString("101-03");
	current_project = 0;
	m_sel_project.SetCurSel(current_project);
#endif
	if(op_level > CFG_FILE_OP_LEVEL2)
    { 
		GetDlgItem(IDC_INPUT_CFJ)->ShowWindow(false);
		GetDlgItem(IDC_OUTPUT_CFJ)->ShowWindow(false);
		GetDlgItem(IDC_PWR_STEP1)->EnableWindow(false);
		GetDlgItem(IDC_PWR_STEP2)->EnableWindow(false);
		GetDlgItem(IDC_PWR_STEP3)->EnableWindow(false);
		GetDlgItem(IDC_PWR_STEP4)->EnableWindow(false);
		GetDlgItem(IDC_PWR_STEP5)->EnableWindow(false);
		GetDlgItem(IDC_PWR_STEP6)->EnableWindow(false);
		GetDlgItem(IDC_PWR_STEP7)->EnableWindow(false);
		GetDlgItem(IDC_PWR_STEP8)->EnableWindow(false);
		GetDlgItem(IDC_PWR_MIN)->EnableWindow(false);
		GetDlgItem(IDC_PWR_MAX)->EnableWindow(false);
		GetDlgItem(IDC_PWR_ADJ_MIN)->EnableWindow(false);
		GetDlgItem(IDC_PWR_ADJ_MAX)->EnableWindow(false);
	}
	
	UpDateFile(pCfgf,false);
	
	/*if(m_waddr_sel)
	{
		GetDlgItem(IDC_Tx_WriteUid_Auto)->ShowWindow(true);
	}
	else
	{
		GetDlgItem(IDC_Tx_WriteUid_Auto)->ShowWindow(false);
	}*/

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void config_deal::OnOK() 
{
	// TODO: Add extra validation here
	
	UpdateData();
	UpDateFile(pCfgf);
	
	CDialog::OnOK();
}

void config_deal::OnInputCfj() 
{
	// TODO: Add your control notification handler code here
#if 0
	ConfigFile &cfg = pCfgf;
	int res = cfg.readfile(Cfj_file_ini,false);
	if(res == 1) {
		cfg.MarkDirty();
		res = cfg.writefile(Cfj_file_bin);
		if(res) UpDateFile(pCfgf,false);
	}

	CString show = "导入文件 :";
	show += Cfj_file_ini + " ";
	if(res) show += "成功!!";
	else show += "失败!";
	AfxMessageBox(show);
#endif

	ConfigFile &cfg = pCfgf;
	//int res = cfg.readfile(Cfj_file_ini,false,op_level);
	int res = cfg.readfile(Cfj_file_ini,false,0);
	if(res == 1) 
    {
		cfg.MarkDirty();
		cfg.update_config_to_disk();
		UpDateFile(pCfgf,false);
	}

	CString show = "导入文件 :";
	show += Cfj_file_ini + " ";
	if(res) show += "成功!!";
	else show += "失败!";
	AfxMessageBox(show);

}

void config_deal::OnOutputCfj() 
{
	// TODO: Add your control notification handler code here
#if 0	
	ConfigFile &cfg = pCfgf;
	int res = cfg.writefile(Cfj_file_ini,false);
	
	CString show = "导出文件 :";
	show += Cfj_file_ini + " ";
	if(res) show += "成功!!";
	else show += "失败!";
	AfxMessageBox(show);
#endif

	ConfigFile &cfg = pCfgf;
	//int res = cfg.writefile(Cfj_file_ini,false,op_level);
	int res = cfg.writefile(Cfj_file_ini,false,0);//全开放写在ini文件
	CString show = "导出文件 :";
	show += Cfj_file_ini + " ";
	if(res) show += "成功!!";
	else show += "失败!";
	AfxMessageBox(show);
}

void config_deal::UpDateFile(ConfigFile &ConfigF, bool flag)
{
	if(flag == true)
	{//退出
		UpDataToFile(ConfigF);   //click 'OK', save cfg to file
		pCfgf.MarkDirty();
	}
	else
	{//进入
		UpDataFromFile(ConfigF);
		UpdateData(false);
	}
}



//--------------------------------------------------------------------------------------
//CSR
//--------------------------------------------------------------------------------------

void config_deal::UpDataFromFile(ConfigFile &ConfigF)
{
	ConfigFile &cfg = ConfigF;

	
    if(cfg.VCString(ParLanguage) == "ch") m_select_ch = true;
	else m_select_ch = false;

	if(cfg.ItemSelect(parTestBoxNeed)) m_need_tbox = true;
	else m_need_tbox = false;

	if(cfg.ItemSelect(s_write_file_sel)) m_write_file = true;
	else m_write_file = false;

	if(cfg.ItemSelect(s_keep_cspar_sel)) m_keep_cspar = true;
	else m_keep_cspar = false;

//	if(cfg.ItemSelect(s_psk_write_sel)) m_wr_pskey_chk = true;
//	else m_wr_pskey_chk = false; 

//	if(cfg.ItemSelect(s_wpsKey1_sel)) m_cvc_wr_chk = true;
//	else m_cvc_wr_chk = false; 
   	
	if(cfg.ItemSelect(s_PwrOn_cur_sel)) m_pwr_on_curr = true;
	else m_pwr_on_curr = false;
	
	if(cfg.ItemSelect(s_PwrOff_cur_sel)) m_pwr_off_cur = true;
	else m_pwr_off_cur = false;

	//if(cfg.ItemSelect(s_pio_test_sel)) m_io_check = true;
	//else m_io_check = false;


	if(cfg.ItemSelect(s_pio_check9_sel)) m_pio_9_chk = true;
	else m_pio_9_chk = false;

	if(cfg.ItemSelect(s_uart_loopback_test_sel)) m_uart_loopback = true;
	else m_uart_loopback = false;
	
	if(cfg.ItemSelect(s_readAddress_sel)) m_read_addr = true;
	else m_read_addr = false;
	
	if(cfg.ItemSelect(s_defaddr_sel)) m_def_addr_verify = true;
	else m_def_addr_verify = false;
	
	if(cfg.ItemSelect(s_InputAddress_man)) m_waddr_auto_sel = true;
	else m_waddr_auto_sel = false;

	if(cfg.ItemSelect(slaveWAddressSel)) m_waddr_sel = true;
	else m_waddr_sel = false;	

	if(cfg.ItemSelect(s_readName_sel)) m_read_bt_name = true;
	else m_read_bt_name = false;
        

	//if(cfg.ItemSelect(s_RstAfterTest_sel)) m_rest_finish = true;
	//else m_rest_finish = false;

    if(cfg.ItemSelect(s_N4010A_TEST_Sel)) 
		m_N4010A_test = true;
	else 
		m_N4010A_test = false;
	
	if(cfg.ItemSelect(ParN4010A_FreTestSel)) 
		m_xtal_adj_sel = true;
	else 
		m_xtal_adj_sel = false;
	
	if(cfg.ItemSelect(ParN4010A_PWLChTestSel)) m_lf_pw_chk = true;
	else m_lf_pw_chk = false;

	if(cfg.ItemSelect(ParN4010A_PWMChTestSel)) m_mf_pw_chk = true;
	else m_mf_pw_chk = false;

	if(cfg.ItemSelect(ParN4010A_PWHChTestSel)) m_hf_pw_chk = true;
	else m_hf_pw_chk = false;
	
	if(cfg.ItemSelect(ParN4010A_SSLChTestSel)) m_lf_ss_chk = true;
	else m_lf_ss_chk = false;
	
	if(cfg.ItemSelect(ParN4010A_SSMChTestSel)) m_mf_ss_chk = true;
	else m_mf_ss_chk = false;
	
	if(cfg.ItemSelect(ParN4010A_SSHChTestSel)) m_hf_ss_chk = true;
	else m_hf_ss_chk = false;

	if(cfg.ItemSelect(s_BModle_Sel)) m_BModleSel = true;
	else m_BModleSel = false;





//20171122
	if(cfg.ItemSelect(Par_read_soft_ver)) m_read_soft_ver = true;
	else m_read_soft_ver = false;

	if(cfg.ItemSelect(Par_mcu_io_chk)) m_mcu_io_chk = true;
	else m_mcu_io_chk = false;

	if(cfg.ItemSelect(Par_8711_io_chk)) m_8711_io_chk = true;
	else m_8711_io_chk = false;

	if(cfg.ItemSelect(Par_mf_evm_chk)) m_mf_evm_chk = true;
	else m_mf_evm_chk = false;

	if(cfg.ItemSelect(Par_lf_evm_chk)) m_lf_evm_chk = true;
	else m_lf_evm_chk = false;

	if(cfg.ItemSelect(Par_hf_evm_chk)) m_hf_evm_chk = true;
	else m_hf_evm_chk = false;




//#if 0
	//new added


//#endif
#if 0
        m_pwr_min = cfg.VDouble(s_RFPwrFineMin);
        m_pwr_max = cfg.VDouble(s_RFPwrFineMax);
        m_pwr_adj_min = cfg.VDouble(s_RFPwrAdjMin);
        m_pwr_adj_max = cfg.VDouble(s_RFPwrAdjMax);

        const double* pwr_step_tbl = cfg.get_pwr_step_tbl();
        const double* pwr_tbl = cfg.get_pwr_tbl();
        const int* pwr_idx = cfg.get_pwr_idx_tbl();

		m_pwr_step1 = pwr_step_tbl[7];
        m_pwr_step2 = pwr_step_tbl[6];
        m_pwr_step3 = pwr_step_tbl[5];
        m_pwr_step4 = pwr_step_tbl[4];
        m_pwr_step5 = pwr_step_tbl[3];
        m_pwr_step6 = pwr_step_tbl[2];
        m_pwr_step7 = pwr_step_tbl[1];
        m_pwr_step8 = pwr_step_tbl[0];
#endif

}

void config_deal::UpDataToFile(ConfigFile &ConfigF)
{
	ConfigFile &cfg = ConfigF;

	if(m_select_ch) cfg.ItemSet(ParLanguage,"ch");
	else cfg.ItemSet(ParLanguage,"en");

	if(m_need_tbox) cfg.ItemSet(parTestBoxNeed,"yes");
	else cfg.ItemSet(parTestBoxNeed,"no");

	if(m_write_file)  cfg.ItemSet(s_write_file_sel,"yes");
	else cfg.ItemSet(s_write_file_sel,"no"); 
	
	if(m_keep_cspar)  cfg.ItemSet(s_keep_cspar_sel,"yes");
	else cfg.ItemSet(s_keep_cspar_sel,"no"); 

//	if(m_spi_lock_chk) cfg.ItemSet(slaveSpiLockSel,"yes");
//	else cfg.ItemSet(slaveSpiLockSel,"no");

//	if(m_spi_unlock_chk) cfg.ItemSet(slaveSpiunLockSel,"yes");
//	else cfg.ItemSet(slaveSpiunLockSel,"no");

	if(m_pwr_on_curr) cfg.ItemSet(s_PwrOn_cur_sel,"yes");
	else cfg.ItemSet(s_PwrOn_cur_sel,"no");

//	if(m_wr_pskey_chk) cfg.ItemSet(s_psk_write_sel,"yes");
//	else cfg.ItemSet(s_psk_write_sel,"no");

//	if(m_cvc_wr_chk) cfg.ItemSet(s_wpsKey1_sel,"yes");
//	else cfg.ItemSet(s_wpsKey1_sel,"no"); 

    if(m_pwr_off_cur) cfg.ItemSet(s_PwrOff_cur_sel,"yes");
	else cfg.ItemSet(s_PwrOff_cur_sel,"no");
 
	//if(m_io_check) cfg.ItemSet(s_pio_test_sel,"yes");
	//else cfg.ItemSet(s_pio_test_sel,"no");


	if(m_pio_9_chk) cfg.ItemSet(s_pio_check9_sel, "yes");
	else cfg.ItemSet(s_pio_check9_sel, "no");
	
	if(m_uart_loopback) cfg.ItemSet(s_uart_loopback_test_sel, "yes");
	else cfg.ItemSet(s_uart_loopback_test_sel, "no");

	if(m_read_addr) cfg.ItemSet(s_readAddress_sel,"yes");
	else cfg.ItemSet(s_readAddress_sel,"no");

	if(m_read_bt_name) cfg.ItemSet(s_readName_sel,"yes");
	else  cfg.ItemSet(s_readName_sel,"no");

	if(m_def_addr_verify) cfg.ItemSet(s_defaddr_sel,"yes");
    else cfg.ItemSet(s_defaddr_sel,"no");

	if(m_waddr_auto_sel) cfg.ItemSet(s_InputAddress_man,"yes");
	else cfg.ItemSet(s_InputAddress_man,"no");

	if(m_waddr_sel) cfg.ItemSet(slaveWAddressSel,"yes");
	else cfg.ItemSet(slaveWAddressSel,"no");

	//if(m_rest_finish) cfg.ItemSet(s_RstAfterTest_sel,"yes");
	//else cfg.ItemSet(s_RstAfterTest_sel,"no");

    if(m_N4010A_test) 
		cfg.ItemSet(s_N4010A_TEST_Sel,"yes");
	else 
		cfg.ItemSet(s_N4010A_TEST_Sel,"no");
	
	if(m_xtal_adj_sel) 
		cfg.ItemSet(ParN4010A_FreTestSel,"yes");
	else 
		cfg.ItemSet(ParN4010A_FreTestSel,"no");
	
	if(m_lf_pw_chk) cfg.ItemSet(ParN4010A_PWLChTestSel,"yes");
	else cfg.ItemSet(ParN4010A_PWLChTestSel,"no");
	
	if(m_mf_pw_chk) cfg.ItemSet(ParN4010A_PWMChTestSel,"yes");
	else cfg.ItemSet(ParN4010A_PWMChTestSel,"no");
	
	if(m_hf_pw_chk) cfg.ItemSet(ParN4010A_PWHChTestSel,"yes");
	else cfg.ItemSet(ParN4010A_PWHChTestSel,"no");
	
	if(m_lf_ss_chk) cfg.ItemSet(ParN4010A_SSLChTestSel,"yes");
	else cfg.ItemSet(ParN4010A_SSLChTestSel,"no");
	
	if(m_mf_ss_chk) cfg.ItemSet(ParN4010A_SSMChTestSel,"yes");
	else cfg.ItemSet(ParN4010A_SSMChTestSel,"no");
	
	if(m_hf_ss_chk) cfg.ItemSet(ParN4010A_SSHChTestSel,"yes");
	else cfg.ItemSet(ParN4010A_SSHChTestSel,"no");

	if(m_BModleSel) cfg.ItemSet(s_BModle_Sel,"yes");
	else cfg.ItemSet(s_BModle_Sel,"no");




//20171122
	if(m_read_soft_ver) cfg.ItemSet(Par_read_soft_ver,"yes");
	else cfg.ItemSet(Par_read_soft_ver,"no");

	if(m_mcu_io_chk) cfg.ItemSet(Par_mcu_io_chk,"yes");
	else cfg.ItemSet(Par_mcu_io_chk,"no");

	if(m_8711_io_chk) cfg.ItemSet(Par_8711_io_chk,"yes");
	else cfg.ItemSet(Par_8711_io_chk,"no");

	if(m_mf_evm_chk) cfg.ItemSet(Par_mf_evm_chk,"yes");
	else cfg.ItemSet(Par_mf_evm_chk,"no");

	if(m_lf_evm_chk) cfg.ItemSet(Par_lf_evm_chk,"yes");
	else cfg.ItemSet(Par_lf_evm_chk,"no");

	if(m_hf_evm_chk) cfg.ItemSet(Par_hf_evm_chk,"yes");
	else cfg.ItemSet(Par_hf_evm_chk,"no");





//#if 0
    //new added

//#endif
#if 0

        cfg.ItemSet(s_RFPwrFineMin,m_pwr_min);
        cfg.ItemSet(s_RFPwrFineMax,m_pwr_max);
        cfg.ItemSet(s_RFPwrAdjMin,m_pwr_adj_min);
        cfg.ItemSet(s_RFPwrAdjMax,m_pwr_adj_max);

        CString RfPwrTable;
		CString RfPwrStep;
		CString pwr_temp;

        pwr_temp.Format("%.2lf",m_pwr_step1);
        RfPwrStep += pwr_temp;
        pwr_temp.Format(" %.2lf",m_pwr_step2);
        RfPwrStep += pwr_temp;
        pwr_temp.Format(" %.2lf",m_pwr_step3);
        RfPwrStep += pwr_temp;
        pwr_temp.Format(" %.2lf",m_pwr_step4);
        RfPwrStep += pwr_temp;
        pwr_temp.Format(" %.2lf",m_pwr_step5);
        RfPwrStep += pwr_temp;
        pwr_temp.Format(" %.2lf",m_pwr_step6);
        RfPwrStep += pwr_temp;
        pwr_temp.Format(" %.2lf",m_pwr_step7);
        RfPwrStep += pwr_temp;
        pwr_temp.Format(" %.2lf",m_pwr_step8);
        RfPwrStep += pwr_temp;

        cfg.update_pwr_tbl(RfPwrTable,RfPwrStep);
#endif
}

void config_deal::OnTxWriteUid() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	/*if(m_waddr_sel)
	{
		GetDlgItem(IDC_Tx_WriteUid_Auto)->ShowWindow(true);
	}
	else
	{
		GetDlgItem(IDC_Tx_WriteUid_Auto)->ShowWindow(false);
	}*/
}
