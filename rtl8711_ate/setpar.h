#if !defined(AFX_SETPAR_H__DD3D73D7_16EF_468D_9D24_0BE0D586965E__INCLUDED_)
#define AFX_SETPAR_H__DD3D73D7_16EF_468D_9D24_0BE0D586965E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// setpar.h : header file
//
#include "tset.h"



/////////////////////////////////////////////////////////////////////////////
// setpar dialog

class setpar : public CDialog
{
// Construction
public:
	setpar(CWnd* pParent = NULL);   // standard constructor

	void update_to_gdut(void);
	void update_from_gdut(void);
	
	void update_to_sensor(void);
	void update_from_sensor(void);
	
	void update_from_sys(void);
	void update_to_sys(void);

	
	bool adjust_hexdigital(CString &address);
	
	int current_gloden;
	int current_audio_mode;
	int current_sensor;
	CDutGlodenPar gloden[MAX_DUT_GLODEN_NUM];
	CDutSensorPar sensor[MAX_DUT_COLOR_SNR_NUM];

// Dialog Data
	//{{AFX_DATA(setpar)
	enum { IDD = IDD_SetPar };
	CComboBox	m_sel_gloden;
	double	m_RF_Arouting_loss;
	double	m_RF_Brouting_loss;
	CString	m_curent_address;
	CString	m_end_address;
	CString	m_gloden_address;
	double	m_amplitude;
	double	m_gloden_lv_max;
	double	m_gloden_lv_min;
	double	m_gloden_thd;
	CString	m_gloden_name;
	double	m_gloden_sens;
	CString	m_gloden_ver;
	CString	m_wfile;
	double	m_RF_Arouting_loss_MCH;
	double	m_RF_Arouting_loss_HCH;
	double	m_RF_Brouting_loss_HCH;
	double	m_RF_Brouting_loss_MCH;
	double	m_n4010a_foffset;
	double	m_pwron_cur_min;
	double	m_pwron_cur_max;
	int		m_xtal_par;
	int		m_xtal_min;
	int		m_xtal_max;
	int		m_power_par;
	int		m_power_max;
	int		m_power_min;
	CString	m_dut1_uart_port;
	int		m_dut1_uart_baudrate;
	CString	m_dut2_uart_port;
	int		m_dut2_uart_baudrate;
	int		m_gloden_foffset;
	double	m_avpwr_ch3;
	double	m_avpwr_ch1;
	double	m_avpwr_ch2;
	double	m_gloden_exp_power;
	int		m_dut_foffset;
	int		m_dut_inputpower;
	int		m_dut_senspower;
	int		m_dut_senspackets;
	double	m_a_evm_loss;
	double	m_a_evm_lossh;
	double	m_a_evm_lossm;
	double	m_b_evm_loss;
	double	m_b_evm_lossh;
	double	m_b_evm_lossm;
	int		m_bw20_bw40_df;
	double	m_evm_max;
	int		m_sens_err_per_max;
	double	m_dut_avpowermax;
	double	m_dut_avpowermin;
	double	m_b_sens_loss;
	double	m_a_sens_lossm;
	double	m_a_sens_lossh;
	double	m_a_sens_loss;
	double	m_b_sens_lossh;
	double	m_b_sens_lossm;
	double	m_gloden_exp_evm;
	double	m_gloden_exp_evm_h;
	double	m_gloden_exp_evm_l;
	double	m_gloden_exp_evm_m;
	int		m_ofdm_bw40_df;
	double	m_instrument_compensate;
	CString	m_set_soft_ver;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(setpar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(setpar)
	afx_msg void OnSelchangeSelGolden();
	afx_msg void OnChangeCurAddr();
	afx_msg void OnChangeDefAddr();
	afx_msg void OnChangeEndAddr();
	afx_msg void OnChangeGAddr();
	afx_msg void OnChangeWfileCrc32();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeComboSnrSel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETPAR_H__DD3D73D7_16EF_468D_9D24_0BE0D586965E__INCLUDED_)
