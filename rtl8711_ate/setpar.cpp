// setpar.cpp : implementation file
//

#include "stdafx.h"
#include "pd218.h"
#include "Config.h"
#include "PD218Dlg.h"
#include "err.h"
#include "TestItem.h"
#include "tset.h"
#include "setpar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// setpar dialog


setpar::setpar(CWnd* pParent /*=NULL*/)
	: CDialog(setpar::IDD, pParent)
{
	//{{AFX_DATA_INIT(setpar)
	m_RF_Arouting_loss = 0.0;
	m_RF_Brouting_loss = 0.0;
	m_curent_address = _T("");
	m_end_address = _T("");
	m_gloden_address = _T("");
	m_amplitude = 0.0;
	m_gloden_lv_max = 0.0;
	m_gloden_lv_min = 0.0;
	m_gloden_thd = 0.0;
	m_gloden_name = _T("");
	m_gloden_sens = 0.0;
	m_gloden_ver = _T("");
	m_wfile = _T("");
	m_RF_Arouting_loss_MCH = 0.0;
	m_RF_Arouting_loss_HCH = 0.0;
	m_RF_Brouting_loss_HCH = 0.0;
	m_RF_Brouting_loss_MCH = 0.0;
	m_n4010a_foffset = 0.0;
	m_pwron_cur_min = 0.0;
	m_pwron_cur_max = 0.0;
	m_xtal_par = 0;
	m_xtal_min = 0;
	m_xtal_max = 0;
	m_power_par = 0;
	m_power_max = 0;
	m_power_min = 0;
	m_dut1_uart_port = _T("");
	m_dut1_uart_baudrate = 0;
	m_dut2_uart_port = _T("");
	m_dut2_uart_baudrate = 0;
	m_gloden_foffset = 0;
	m_avpwr_ch3 = 0.0;
	m_avpwr_ch1 = 0.0;
	m_avpwr_ch2 = 0.0;
	m_gloden_exp_power = 0.0;
	m_dut_foffset = 0;
	m_dut_inputpower = 0;
	m_dut_senspower = 0;
	m_dut_senspackets = 0;
	m_a_evm_loss = 0.0;
	m_a_evm_lossh = 0.0;
	m_a_evm_lossm = 0.0;
	m_b_evm_loss = 0.0;
	m_b_evm_lossh = 0.0;
	m_b_evm_lossm = 0.0;
	m_bw20_bw40_df = 0;
	m_evm_max = 0.0;
	m_sens_err_per_max = 0;
	m_dut_avpowermax = 0.0;
	m_dut_avpowermin = 0.0;
	m_b_sens_loss = 0.0;
	m_a_sens_lossm = 0.0;
	m_a_sens_lossh = 0.0;
	m_a_sens_loss = 0.0;
	m_b_sens_lossh = 0.0;
	m_b_sens_lossm = 0.0;
	m_gloden_exp_evm = 0.0;
	m_gloden_exp_evm_h = 0.0;
	m_gloden_exp_evm_l = 0.0;
	m_gloden_exp_evm_m = 0.0;
	m_ofdm_bw40_df = 0;
	m_instrument_compensate = 0.0;
	m_set_soft_ver = _T("");
	//}}AFX_DATA_INIT
}


void setpar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(setpar)
	DDX_Control(pDX, IDC_SEL_GOLDEN, m_sel_gloden);
	DDX_Text(pDX, IDC_ARF_LOSS, m_RF_Arouting_loss);
	DDX_Text(pDX, IDC_BRF_LOSS, m_RF_Brouting_loss);
	DDX_Text(pDX, IDC_CUR_ADDR, m_curent_address);
	DDV_MaxChars(pDX, m_curent_address, 12);
	DDX_Text(pDX, IDC_END_ADDR, m_end_address);
	DDV_MaxChars(pDX, m_end_address, 12);
	DDX_Text(pDX, IDC_G_ADDR, m_gloden_address);
	DDV_MaxChars(pDX, m_gloden_address, 12);
	DDX_Text(pDX, IDC_G_NAME, m_gloden_name);
	DDV_MaxChars(pDX, m_gloden_name, 32);
	DDX_Text(pDX, IDC_G_SENS, m_gloden_sens);
	DDX_Text(pDX, IDC_G_VER, m_gloden_ver);
	DDV_MaxChars(pDX, m_gloden_ver, 16);
	DDX_Text(pDX, IDC_WFILE, m_wfile);
	DDX_Text(pDX, IDC_ARF_LOSSM, m_RF_Arouting_loss_MCH);
	DDX_Text(pDX, IDC_ARF_LOSSH, m_RF_Arouting_loss_HCH);
	DDX_Text(pDX, IDC_BRF_LOSSH, m_RF_Brouting_loss_HCH);
	DDX_Text(pDX, IDC_BRF_LOSSM, m_RF_Brouting_loss_MCH);
	DDX_Text(pDX, IDC_ARF_FLOSSH, m_n4010a_foffset);
	DDX_Text(pDX, IDC_PWRON_CUR_MIN, m_pwron_cur_min);
	DDX_Text(pDX, IDC_PWRON_CUR_MAX, m_pwron_cur_max);
	DDX_Text(pDX, IDC_DEF_XTAL_PAR, m_xtal_par);
	DDV_MinMaxInt(pDX, m_xtal_par, 0, 63);
	DDX_Text(pDX, IDC_XTAL_MIN, m_xtal_min);
	DDV_MinMaxInt(pDX, m_xtal_min, 0, 63);
	DDX_Text(pDX, IDC_XTAL_MAX, m_xtal_max);
	DDV_MinMaxInt(pDX, m_xtal_max, 0, 63);
	DDX_Text(pDX, IDC_DEF_POWER_PAR, m_power_par);
	DDV_MinMaxInt(pDX, m_power_par, 0, 63);
	DDX_Text(pDX, IDC_POWER_MAX, m_power_max);
	DDV_MinMaxInt(pDX, m_power_max, 0, 63);
	DDX_Text(pDX, IDC_POWER_MIN, m_power_min);
	DDV_MinMaxInt(pDX, m_power_min, 0, 63);
	DDX_Text(pDX, IDC_DUT1_UART_PORT, m_dut1_uart_port);
	DDX_Text(pDX, IDC_DUT1_UART_RATE, m_dut1_uart_baudrate);
	DDX_Text(pDX, IDC_DUT2_UART_PORT, m_dut2_uart_port);
	DDX_Text(pDX, IDC_DUT2_UART_RATE, m_dut2_uart_baudrate);
	DDX_Text(pDX, IDC_G_FOFFSET, m_gloden_foffset);
	DDX_Text(pDX, IDC_G_RF_AVPWR_H, m_avpwr_ch3);
	DDX_Text(pDX, IDC_G_RF_AVPWR_L, m_avpwr_ch1);
	DDX_Text(pDX, IDC_G_RF_AVPWR_M, m_avpwr_ch2);
	DDX_Text(pDX, IDC_G_EXP_POWER, m_gloden_exp_power);
	DDX_Text(pDX, IDC_DUT_FOFFSET, m_dut_foffset);
	DDX_Text(pDX, IDC_DUT_INPUTPOWER, m_dut_inputpower);
	DDX_Text(pDX, IDC_DUT_SENSPOWER, m_dut_senspower);
	DDX_Text(pDX, IDC_DUT_SENSPACKETS, m_dut_senspackets);
	DDX_Text(pDX, IDC_A_EVM_LOSS, m_a_evm_loss);
	DDX_Text(pDX, IDC_A_EVM_LOSSH, m_a_evm_lossh);
	DDX_Text(pDX, IDC_A_EVM_LOSSM, m_a_evm_lossm);
	DDX_Text(pDX, IDC_B_EVM_LOSS, m_b_evm_loss);
	DDX_Text(pDX, IDC_B_EVM_LOSSH, m_b_evm_lossh);
	DDX_Text(pDX, IDC_B_EVM_LOSSM, m_b_evm_lossm);
	DDX_Text(pDX, IDC_BW20_BW40_DF, m_bw20_bw40_df);
	DDV_MinMaxInt(pDX, m_bw20_bw40_df, -8, 7);
	DDX_Text(pDX, IDC_EVM_MAX, m_evm_max);
	DDX_Text(pDX, IDC_SENS_ERR_PER_MAX, m_sens_err_per_max);
	DDX_Text(pDX, IDC_DUT_AVPOWERMAX, m_dut_avpowermax);
	DDX_Text(pDX, IDC_DUT_AVPOWERMIN, m_dut_avpowermin);
	DDX_Text(pDX, IDC_B_SENS_LOSS, m_b_sens_loss);
	DDX_Text(pDX, IDC_A_SENS_LOSSM, m_a_sens_lossm);
	DDX_Text(pDX, IDC_A_SENS_LOSSH, m_a_sens_lossh);
	DDX_Text(pDX, IDC_A_SENS_LOSS, m_a_sens_loss);
	DDX_Text(pDX, IDC_B_SENS_LOSSH, m_b_sens_lossh);
	DDX_Text(pDX, IDC_B_SENS_LOSSM, m_b_sens_lossm);
	DDX_Text(pDX, IDC_G_EXP_EVM, m_gloden_exp_evm);
	DDX_Text(pDX, IDC_G_EXP_EVM_H, m_gloden_exp_evm_h);
	DDX_Text(pDX, IDC_G_EXP_EVM_L, m_gloden_exp_evm_l);
	DDX_Text(pDX, IDC_G_EXP_EVM_M, m_gloden_exp_evm_m);
	DDX_Text(pDX, IDC_OFDM_BW40_DF, m_ofdm_bw40_df);
	DDV_MinMaxInt(pDX, m_ofdm_bw40_df, -8, 7);
	DDX_Text(pDX, IDC_INSTRUMENT_COMPENSATE, m_instrument_compensate);
	DDV_MinMaxDouble(pDX, m_instrument_compensate, 0., 40.);
	DDX_Text(pDX, IDC_SET_SOFT_VER, m_set_soft_ver);
	DDV_MaxChars(pDX, m_set_soft_ver, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(setpar, CDialog)
	//{{AFX_MSG_MAP(setpar)
	ON_CBN_SELCHANGE(IDC_SEL_GOLDEN, OnSelchangeSelGolden)
	ON_EN_CHANGE(IDC_CUR_ADDR, OnChangeCurAddr)
	ON_EN_CHANGE(IDC_DEF_ADDR, OnChangeDefAddr)
	ON_EN_CHANGE(IDC_END_ADDR, OnChangeEndAddr)
	ON_EN_CHANGE(IDC_G_ADDR, OnChangeGAddr)
	ON_EN_CHANGE(IDC_WFILE_CRC32, OnChangeWfileCrc32)
	ON_CBN_SELCHANGE(IDC_COMBO_SNR_SEL, OnSelchangeComboSnrSel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// setpar message handlers



BOOL setpar::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	//golden par
	for(int i=0;i<MAX_DUT_GLODEN_NUM;i++) gloden[i] = *pMainWnd->dutGloden[i];
	m_sel_gloden.AddString("DUT");
	m_sel_gloden.AddString("金机11");
	m_sel_gloden.AddString("金机12");
	m_sel_gloden.AddString("金机13");
	m_sel_gloden.AddString("金机21");
	m_sel_gloden.AddString("金机22");
	m_sel_gloden.AddString("金机23");
	m_sel_gloden.AddString("金机31");
	m_sel_gloden.AddString("金机32");
	m_sel_gloden.AddString("金机33");
	m_sel_gloden.AddString("金机41");
	m_sel_gloden.AddString("金机42");
	m_sel_gloden.AddString("金机43");
	current_gloden = 0;
	m_sel_gloden.SetCurSel(current_gloden);

    //audio mode select
//	m_dut_audio_mode.AddString("0-MicL to SpkL, MicR to SpkR");
//	m_dut_audio_mode.AddString("1-MicR to SpkL and SpkR");
//	m_dut_audio_mode.AddString("2-MicL to SpkL and SpkR");
//	m_dut_audio_mode.AddString("3-MicL to SpkR, MicR to SpkL");
#if 0
	current_audio_mode = pCfgf.Vint(s_audio_mic_spk_mode);
#endif
//	m_dut_audio_mode.SetCurSel(current_audio_mode);

    //color sensor par
//	for(i=0;i<MAX_DUT_COLOR_SNR_NUM;i++) sensor[i] = *pMainWnd->dutSensor[i];
//	m_dut_color_snr.AddString("U45 Sensor");
//	m_dut_color_snr.AddString("U49 Sensor");
//    m_dut_color_snr.AddString("U51 Sensor");
//	m_dut_color_snr.AddString("U55 Sensor");
//	m_dut_color_snr.AddString("U60 Sensor");
//	current_sensor = 0;
//	m_dut_color_snr.SetCurSel(current_sensor);
	
	update_from_sys();
	UpdateData(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
void setpar::update_from_gdut(void)
{
	int i = m_sel_gloden.GetCurSel();
	if(i < 0) i = 0;
	if(i >= MAX_DUT_GLODEN_NUM) i = MAX_DUT_GLODEN_NUM - 1;
	current_gloden = i;
	
	if(!current_gloden) GetDlgItem(IDC_SG_ADDR)->SetWindowText("默认的金机地址");
	else GetDlgItem(IDC_SG_ADDR)->SetWindowText("金机地址");
	
	CDutGlodenPar *pgloden = &gloden[current_gloden]; 
	m_gloden_address = pgloden->addr;

	m_gloden_name = pgloden->name;
	m_gloden_foffset = pgloden->rf_fre_offset;
	m_avpwr_ch1 = pgloden->ch1_rf_avpwr;
	m_avpwr_ch2 = pgloden->ch2_rf_avpwr; 
	m_avpwr_ch3 = pgloden->ch3_rf_avpwr;
	m_gloden_exp_power = pgloden->rf_inputpwr;
	m_gloden_exp_evm = pgloden->rf_inputevm;
	m_gloden_exp_evm_l = pgloden->ch1_rf_evm;
	m_gloden_exp_evm_m = pgloden->ch2_rf_evm;
	m_gloden_exp_evm_h = pgloden->ch3_rf_evm;
	m_gloden_sens = pgloden->sens_pwr;
	m_gloden_ver = pgloden->ver;
}

void setpar::update_to_gdut(void)
{
	CDutGlodenPar *pgloden = &gloden[current_gloden];
	
	m_gloden_address.MakeUpper();
	pgloden->addr = m_gloden_address;

	pgloden->name = m_gloden_name;
	pgloden->rf_fre_offset = m_gloden_foffset;
	pgloden->ch1_rf_avpwr = m_avpwr_ch1;
	pgloden->ch2_rf_avpwr = m_avpwr_ch2;
	pgloden->ch3_rf_avpwr = m_avpwr_ch3;
	pgloden->rf_inputpwr = m_gloden_exp_power;
	pgloden->rf_inputevm = m_gloden_exp_evm;
	pgloden->ch1_rf_evm = m_gloden_exp_evm_l;
	pgloden->ch2_rf_evm = m_gloden_exp_evm_m;
	pgloden->ch3_rf_evm = m_gloden_exp_evm_h;
	pgloden->sens_pwr = m_gloden_sens;
	pgloden->ver = m_gloden_ver;

}


//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
void setpar::update_from_sensor(void)
{
#if 0
	int i = m_dut_color_snr.GetCurSel();
	if(i < 0) i = 0;
	else if(i >= MAX_DUT_COLOR_SNR_NUM) i = MAX_DUT_COLOR_SNR_NUM - 1;
	current_sensor = i;
	
	CDutSensorPar *pSensor = &sensor[current_sensor]; 
#endif

}

void setpar::update_to_sensor(void)
{
	CDutSensorPar *pSensor = &sensor[current_sensor];

}


//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
void setpar::update_from_sys(void)
{	
	ConfigFile &cfg = pCfgf;
	
	cfg.get_bt_address(m_curent_address);
	cfg.get_end_address(m_end_address);
	//cfg.get_default_address(m_default_address);
	
	//m_n4010a_foffset = 0;//cfg.VDouble(ParMT8852x_freqOffset);
	
	m_RF_Arouting_loss =     cfg.VDouble(s_DevA_LCH_pathLoss);
	m_RF_Arouting_loss_MCH = cfg.VDouble(s_DevA_MCH_pathLoss);
	m_RF_Arouting_loss_HCH = cfg.VDouble(s_DevA_HCH_pathLoss);
	
	m_RF_Brouting_loss =     cfg.VDouble(s_DevB_LCH_pathLoss);
	m_RF_Brouting_loss_MCH = cfg.VDouble(s_DevB_MCH_pathLoss);
	m_RF_Brouting_loss_HCH = cfg.VDouble(s_DevB_HCH_pathLoss);
	
	m_wfile = cfg.VCString(s_write_flash_filename);
	
	//m_mic_routing.SetCurSel(cfg.get_mic_ch_mode()-1);
	//m_spk_routing.SetCurSel(cfg.get_spk_ch_mode()-1);
	
	m_xtal_par = cfg.Vint(ParFreqIndex);
	m_xtal_min = cfg.Vint(ParFreqMinIndex);
	m_xtal_max = cfg.Vint(ParFreqMaxIndex);
	
	m_power_par = cfg.Vint(ParN4010A_PWLEL);
	m_power_min = cfg.Vint(ParN4010A_MinPWLEL);
	m_power_max = cfg.Vint(ParN4010A_MaxPWLEL);

	m_dut1_uart_port = cfg.VCString(s_uart_dut1_portname);
	m_dut1_uart_baudrate = cfg.Vint(s_uart_dut1_baudrate);
	m_dut2_uart_port = cfg.VCString(s_uart_dut2_portname);
	m_dut2_uart_baudrate = cfg.Vint(s_uart_dut2_baudrate);
	
	m_dut_foffset = cfg.Vint(ParN4010A_freqOffset);
	m_dut_inputpower = cfg.Vint(ParN4010A_Inputpower);
	m_dut_avpowermin = cfg.VDouble(ParN4010A_MinPW);
	m_dut_avpowermax = cfg.VDouble(ParN4010A_MaxPW);
	m_dut_senspower = cfg.Vint(ParN4010A_SSTxPwr);
	m_dut_senspackets = cfg.Vint(ParN4010A_SSPacktes);

	m_pwron_cur_min = cfg.VDouble(s_PwrOn_cur_min);
	m_pwron_cur_max = cfg.VDouble(s_PwrOn_cur_max);


	//20171122
	m_a_evm_loss = cfg.VDouble(Par_a_evm_loss);
	m_a_evm_lossh = cfg.VDouble(Par_a_evm_lossh);
	m_a_evm_lossm = cfg.VDouble(Par_a_evm_lossm);
	m_b_evm_loss = cfg.VDouble(Par_b_evm_loss);
	m_b_evm_lossh = cfg.VDouble(Par_b_evm_lossh);
	m_b_evm_lossm = cfg.VDouble(Par_b_evm_lossm);
	
	m_a_sens_loss = cfg.VDouble(Par_a_sens_loss);
	m_a_sens_lossh = cfg.VDouble(Par_a_sens_lossh);
	m_a_sens_lossm = cfg.VDouble(Par_a_sens_lossm);
	m_b_sens_loss = cfg.VDouble(Par_b_sens_loss);
	m_b_sens_lossh = cfg.VDouble(Par_b_sens_lossh);
	m_b_sens_lossm = cfg.VDouble(Par_b_sens_lossm);
	
	m_instrument_compensate = cfg.VDouble(Par_instrument_compensate);
	m_evm_max = cfg.VDouble(Par_evm_max);
	m_bw20_bw40_df = cfg.Vint(Par_bw20_bw40_df);	
	m_ofdm_bw40_df = cfg.Vint(Par_ofdm_bw40_df);
	m_sens_err_per_max = cfg.Vint(Par_sens_err_per_max);

	//2018 7 12
	m_set_soft_ver = cfg.VCString(Par_set_soft_ver);
	m_n4010a_foffset = cfg.VDouble(Par_instru_foff_loss);
	
	update_from_gdut();
//	update_from_sensor();
//	m_wfile_crc32.MakeUpper();
	m_curent_address.MakeUpper();
	m_end_address.MakeUpper();

}

void setpar::update_to_sys(void)
{
	ConfigFile &cfg = pCfgf;
	
//	cfg.ItemSet(ParMT8852x_freqOffset, m_mt8852_foffset);
	cfg.ItemSet(s_DevA_LCH_pathLoss,m_RF_Arouting_loss);
	cfg.ItemSet(s_DevA_MCH_pathLoss,m_RF_Arouting_loss_MCH);
	cfg.ItemSet(s_DevA_HCH_pathLoss,m_RF_Arouting_loss_HCH);

	cfg.ItemSet(s_DevB_LCH_pathLoss,m_RF_Brouting_loss);
	cfg.ItemSet(s_DevB_MCH_pathLoss,m_RF_Brouting_loss_MCH);
	cfg.ItemSet(s_DevB_HCH_pathLoss,m_RF_Brouting_loss_HCH);
	
	cfg.set_bt_address(m_curent_address);
	cfg.set_end_address(m_end_address);
	//cfg.set_default_address(m_default_address);
	
	cfg.ItemSet(s_write_flash_filename,m_wfile);
		
	int data = m_xtal_par;
	cfg.ItemSet(ParFreqIndex,data);
	data = m_xtal_min;
	cfg.ItemSet(ParFreqMinIndex,data);
	data = m_xtal_max;
	cfg.ItemSet(ParFreqMaxIndex,data);
	
	data = m_power_par;
	cfg.ItemSet(ParN4010A_PWLEL,data);
	data = m_power_min;
	cfg.ItemSet(ParN4010A_MinPWLEL,data);
	data = m_power_max;
	cfg.ItemSet(ParN4010A_MaxPWLEL,data);





	//20171122
	data = m_bw20_bw40_df;
	cfg.ItemSet(Par_bw20_bw40_df,data);
	data = m_ofdm_bw40_df;
	cfg.ItemSet(Par_ofdm_bw40_df,data);
	data = m_sens_err_per_max;
	cfg.ItemSet(Par_sens_err_per_max,data);
	
	cfg.ItemSet(Par_a_evm_loss,m_a_evm_loss);
	cfg.ItemSet(Par_a_evm_lossh,m_a_evm_lossh);
	cfg.ItemSet(Par_a_evm_lossm,m_a_evm_lossm);
	cfg.ItemSet(Par_b_evm_loss,m_b_evm_loss);	
	cfg.ItemSet(Par_b_evm_lossh,m_b_evm_lossh);
	cfg.ItemSet(Par_b_evm_lossm,m_b_evm_lossm);
	
	cfg.ItemSet(Par_a_sens_loss,m_a_sens_loss);
	cfg.ItemSet(Par_a_sens_lossh,m_a_sens_lossh);
	cfg.ItemSet(Par_a_sens_lossm,m_a_sens_lossm);
	cfg.ItemSet(Par_b_sens_loss,m_b_sens_loss);	
	cfg.ItemSet(Par_b_sens_lossh,m_b_sens_lossh);
	cfg.ItemSet(Par_b_sens_lossm,m_b_sens_lossm);	
	
	cfg.ItemSet(Par_instrument_compensate,m_instrument_compensate);
	cfg.ItemSet(Par_evm_max,m_evm_max);



	//2018 7 12
	cfg.ItemSet(Par_set_soft_ver, m_set_soft_ver);
	cfg.ItemSet(Par_instru_foff_loss, m_n4010a_foffset);


	cfg.ItemSet(s_uart_dut1_portname,m_dut1_uart_port);
	cfg.ItemSet(s_uart_dut1_baudrate,m_dut1_uart_baudrate);
	cfg.ItemSet(s_uart_dut2_portname,m_dut2_uart_port);
	cfg.ItemSet(s_uart_dut2_baudrate,m_dut2_uart_baudrate);

	cfg.ItemSet(ParN4010A_freqOffset,m_dut_foffset);
	cfg.ItemSet(ParN4010A_Inputpower,m_dut_inputpower);
	cfg.ItemSet(ParN4010A_MinPW,m_dut_avpowermin);
	cfg.ItemSet(ParN4010A_MaxPW,m_dut_avpowermax);
	cfg.ItemSet(ParN4010A_SSTxPwr,m_dut_senspower);
	cfg.ItemSet(ParN4010A_SSPacktes,m_dut_senspackets);
	
	cfg.ItemSet(s_PwrOn_cur_min,m_pwron_cur_min);
	cfg.ItemSet(s_PwrOn_cur_max,m_pwron_cur_max);
	
	//golden par
	for(int i=0;i<MAX_DUT_GLODEN_NUM;i++) 
	{
		*pMainWnd->dutGloden[i] = gloden[i];
		pMainWnd->dutGloden[i]->update_to_cfg(cfg);
	}

}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
bool setpar::adjust_hexdigital(CString &address)
{
	CString temp;
	for(int i=0;i<address.GetLength();i++)
	{
		char ctemp = address[i];
		if(('0' <= ctemp) && (ctemp <= '9')) temp += ctemp;
		else if(('a' <= ctemp) && (ctemp <= 'f')) temp += ctemp;
		else if(('A' <= ctemp) && (ctemp <= 'F')) temp += ctemp;
		else
		{ 
			address = temp;
			return(false);
		}
	}
	return(true);
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------


void setpar::OnSelchangeSelGolden() 
{
	// TODO: Add your control notification handler code here

	UpdateData(true);
	update_to_gdut();
	update_from_gdut();
    UpdateData(false);
	
}


void setpar::OnSelchangeComboSnrSel() 
{
	// TODO: Add your control notification handler code here
    UpdateData(true);
	update_to_sensor();
	update_from_sensor();
    UpdateData(false);
	
}



void setpar::OnChangeCurAddr() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	UpdateData(true);
	if(!adjust_hexdigital(m_curent_address)) 
	{
		UpdateData(false);
		((CEdit*)GetDlgItem(IDC_CUR_ADDR))->SetSel(-1);
        }
	
}

void setpar::OnChangeDefAddr() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	//UpdateData(true);
	//if(!adjust_hexdigital(m_default_address)) 
	//{
		//UpdateData(false);
		//((CEdit*)GetDlgItem(IDC_DEF_ADDR))->SetSel(-1);
        //}
	
}

void setpar::OnChangeEndAddr()
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	UpdateData(true);
	if(!adjust_hexdigital(m_end_address)) 
	{
		UpdateData(false);
		((CEdit*)GetDlgItem(IDC_END_ADDR))->SetSel(-1);
        }
	
}

void setpar::OnChangeGAddr() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	UpdateData(true);
	if(!adjust_hexdigital(m_gloden_address)) 
	{
		UpdateData(false);
		((CEdit*)GetDlgItem(IDC_G_ADDR))->SetSel(-1);
        }
	
}

void setpar::OnChangeWfileCrc32() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	UpdateData(true);
#if 0
	if(!adjust_hexdigital(m_wfile_crc32)) 
	{
		UpdateData(false);
		((CEdit*)GetDlgItem(IDC_WFILE_CRC32))->SetSel(-1);
        }
#endif	
}



void setpar::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);
	update_to_gdut();
//	update_to_sensor();
    update_to_sys();
	pCfgf.MarkDirty();
	pCfgf.update_config_to_disk();
	CDialog::OnOK();
}

