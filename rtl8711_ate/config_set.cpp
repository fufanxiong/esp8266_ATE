// config_set.cpp : implementation file
//

#include "stdafx.h"
#include "pd218.h"
#include "PD218Dlg.h"
#include "config_set.h"
#include "Config.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// config_set dialog


config_set::config_set(CWnd* pParent /*=NULL*/)
	: CDialog(config_set::IDD, pParent)
{
	//{{AFX_DATA_INIT(config_set)
	m_w_mac_addr_sel = FALSE;
	m_show_mac_addr_sel = FALSE;
	m_rf_send_test_sel = FALSE;
	m_rf_recv_test_sel = FALSE;
	m_pwr_cvr_sel = FALSE;
	m_rf_power_test_sel = FALSE;
	m_pio_chk_sel = FALSE;
	m_rf_frequency_test_sel = FALSE;
	//m_burn_software_sel = FALSE;
	m_auto_write_mac_sel = FALSE;
	m_adc_chk_sel = FALSE;
	//}}AFX_DATA_INIT
}



void config_set::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(config_set)
	DDX_Check(pDX, IDC_WRITE_MAC, m_w_mac_addr_sel);
	DDX_Check(pDX, IDC_SHOW_MAC, m_show_mac_addr_sel);
	DDX_Check(pDX, IDC_SEND_TEST, m_rf_send_test_sel);
	DDX_Check(pDX, IDC_RECV_TEST, m_rf_recv_test_sel);
	DDX_Check(pDX, IDC_PWR_CUR, m_pwr_cvr_sel);
	DDX_Check(pDX, IDC_POWER_TEST, m_rf_power_test_sel);
	DDX_Check(pDX, IDC_PIO_CHK, m_pio_chk_sel);
	DDX_Check(pDX, IDC_FREQUENCY_TEST, m_rf_frequency_test_sel);
	//DDX_Check(pDX, IDC_BurnSoftware, m_burn_software_sel);
	DDX_Check(pDX, IDC_AUTO_W_MAC, m_auto_write_mac_sel);
	DDX_Check(pDX, IDC_ADC_CHK, m_adc_chk_sel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(config_set, CDialog)
	//{{AFX_MSG_MAP(config_set)
	ON_BN_CLICKED(IDC_WRITE_MAC, OnWriteMac)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// config_set message handlers

BOOL config_set::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	UpDateFile(pCfgf,false);
	
	if(m_w_mac_addr_sel)
	{
		GetDlgItem(IDC_AUTO_W_MAC)->ShowWindow(true);
	}
	else
	{
		GetDlgItem(IDC_AUTO_W_MAC)->ShowWindow(false);
	}	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void config_set::UpDateFile(ConfigFile &ConfigF, bool flag)
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

void config_set::UpDataFromFile(ConfigFile &ConfigF)
{
	ConfigFile &cfg = ConfigF;
		
	//选择烧录软件，就对应开启显示版本
	//if(cfg.ItemSelect(cfg_burn_software_sel)) m_burn_software_sel = true;
	//else m_burn_software_sel = false;
	
	if(cfg.ItemSelect(cfg_pwr_cvr_sel)) m_pwr_cvr_sel = true;
	else m_pwr_cvr_sel = false;
	
	if(cfg.ItemSelect(cfg_pio_chk_sel)) m_pio_chk_sel = true;
	else m_pio_chk_sel = false;
	
	if(cfg.ItemSelect(cfg_adc_chk_sel)) m_adc_chk_sel = true;
	else m_adc_chk_sel = false;
	
	if(cfg.ItemSelect(cfg_w_mac_addr_sel)) m_w_mac_addr_sel = true;
	else m_w_mac_addr_sel = false;	
	
	if(cfg.ItemSelect(cfg_auto_write_mac_sel)) m_auto_write_mac_sel = true;
	else m_auto_write_mac_sel = false;
	
	if(cfg.ItemSelect(cfg_rf_frequency_test_sel)) m_rf_frequency_test_sel = true;
	else m_rf_frequency_test_sel = false;
	
	if(cfg.ItemSelect(cfg_rf_power_test_sel)) m_rf_power_test_sel = true;
	else m_rf_power_test_sel = false;

	if(cfg.ItemSelect(cfg_rf_send_test_sel)) m_rf_send_test_sel = true;
	else m_rf_send_test_sel = false;
   	
	if(cfg.ItemSelect(cfg_rf_recv_test_sel)) m_rf_recv_test_sel = true;
	else m_rf_recv_test_sel = false;	

	if(cfg.ItemSelect(cfg_show_mac_addr_sel)) m_show_mac_addr_sel = true;
	else m_show_mac_addr_sel = false;
}

void config_set::UpDataToFile(ConfigFile &ConfigF)
{
	ConfigFile &cfg = ConfigF;
	
	//if(m_burn_software_sel) cfg.ItemSet(cfg_burn_software_sel,"yes");
	//else cfg.ItemSet(cfg_burn_software_sel,"no");

	if(m_pwr_cvr_sel)  cfg.ItemSet(cfg_pwr_cvr_sel,"yes");
	else cfg.ItemSet(cfg_pwr_cvr_sel,"no"); 
	
	if(m_pio_chk_sel)  cfg.ItemSet(cfg_pio_chk_sel,"yes");
	else cfg.ItemSet(cfg_pio_chk_sel,"no"); 

	if(m_adc_chk_sel) cfg.ItemSet(cfg_adc_chk_sel, "yes");
	else cfg.ItemSet(cfg_adc_chk_sel, "no");
	
	if(m_w_mac_addr_sel) cfg.ItemSet(cfg_w_mac_addr_sel, "yes");
	else cfg.ItemSet(cfg_w_mac_addr_sel, "no");

	if(m_auto_write_mac_sel) cfg.ItemSet(cfg_auto_write_mac_sel,"yes");
	else cfg.ItemSet(cfg_auto_write_mac_sel,"no");

	if(m_rf_frequency_test_sel) cfg.ItemSet(cfg_rf_frequency_test_sel,"yes");
	else  cfg.ItemSet(cfg_rf_frequency_test_sel,"no");

	if(m_rf_power_test_sel) cfg.ItemSet(cfg_rf_power_test_sel,"yes");
	else cfg.ItemSet(cfg_rf_power_test_sel,"no");

	if(m_rf_send_test_sel) cfg.ItemSet(cfg_rf_send_test_sel,"yes");
	else cfg.ItemSet(cfg_rf_send_test_sel,"no");
	
	if(m_rf_recv_test_sel) cfg.ItemSet(cfg_rf_recv_test_sel,"yes");
	else cfg.ItemSet(cfg_rf_recv_test_sel,"no");

	if(m_show_mac_addr_sel) cfg.ItemSet(cfg_show_mac_addr_sel,"yes");
	else cfg.ItemSet(cfg_show_mac_addr_sel,"no");
}


void config_set::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	UpDateFile(pCfgf);
	
	CDialog::OnOK();
}

void config_set::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void config_set::OnWriteMac() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if(m_w_mac_addr_sel)
	{
		GetDlgItem(IDC_AUTO_W_MAC)->ShowWindow(true);
	}
	else
	{
		GetDlgItem(IDC_AUTO_W_MAC)->ShowWindow(false);
	}	
}
