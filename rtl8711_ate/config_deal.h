#if !defined(AFX_CONFIG_DEAL_H__C1A6C824_48DD_49FC_999D_D491D1E4D8BD__INCLUDED_)
#define AFX_CONFIG_DEAL_H__C1A6C824_48DD_49FC_999D_D491D1E4D8BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// config_deal.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// config_deal dialog

class config_deal : public CDialog
{
private:
	void UpDataToFile(ConfigFile &ConfigF);
	void UpDataFromFile(ConfigFile &ConfigF);
	int op_level;

// Construction
public:
	//config_deal(CWnd* pParent = NULL);   // standard constructor
	config_deal(int level,CWnd* pParent = NULL);  // standard constructor
	void UpDateFile(ConfigFile &ConfigF,bool flag = true);


// Dialog Data
	//{{AFX_DATA(config_deal)
	enum { IDD = IDD_MCFJ_DEAL };
	CComboBox	m_sel_project;
	int current_project;
	BOOL	m_TxPwrSel;
	long	m_TxCurMin;
	long	m_TxCurMax;
	double	m_TxPwrMin;
	BOOL	m_xtal_adj_sel;
	BOOL	m_waddr_sel;
	BOOL	m_ver_sel;
	BOOL	m_rest_finish;
	BOOL	m_read_addr;
	BOOL	m_N4010A_test;
	BOOL	m_BModleSel;
	BOOL	m_mic_spk;
	BOOL	m_write_file;
	double	m_pwr_step1;
	double	m_pwr_step2;
	double	m_pwr_step3;
	double	m_pwr_step4;
	double	m_pwr_step5;
	double	m_pwr_step6;
	double	m_pwr_step7;
	double	m_pwr_step8;
	double	m_pwr_adj_max;
	double	m_pwr_adj_min;
	double	m_pwr_max;
	double	m_pwr_min;
	BOOL	m_select_ch;
	BOOL	m_charge_adjust;
	BOOL	m_pwr_on_curr;
	BOOL	m_pwr_off_cur;
	BOOL	m_def_addr_verify;
	BOOL	m_Aio_0_chk;
	BOOL	m_mbias_A_chk;
	BOOL	m_mbias_B_chk;
	BOOL	m_pio_0_chk;
	BOOL	m_pio_1_chk;
	BOOL	m_pio_2_chk;
	BOOL	m_pio_3_chk;
	BOOL	m_pio_4_chk;
	BOOL	m_pio_5_chk;
	BOOL	m_pio_6_chk;
	BOOL	m_pio_7_chk;
	BOOL	m_pio_8_chk;
	BOOL	m_pio_9_chk;
	BOOL	m_pio_10_chk;
	BOOL	m_pio_11_chk;
	BOOL	m_pio_12_chk;
	BOOL	m_pio_13_chk;
	BOOL	m_pio_14_chk;
	BOOL	m_pio_15_chk;
	BOOL	m_pcm_in_out_chk;
	BOOL	m_pcm_sync_chk;
	BOOL	m_pcm_clk_chk;
	BOOL	m_read_bt_name;
	BOOL	m_hf_ic_chk;
	BOOL	m_hf_op_chk;
	BOOL	m_hf_ss_chk;
	BOOL	m_lf_ic_chk;
	BOOL	m_lf_op_chk;
	BOOL	m_lf_ss_chk;
	BOOL	m_mf_ic_chk;
	BOOL	m_mf_op_chk;
	BOOL	m_mf_ss_chk;
	BOOL	m_need_tbox;
	BOOL	m_uart_loopback;
	BOOL	m_hf_pw_chk;
	BOOL	m_lf_pw_chk;
	BOOL	m_mf_pw_chk;
	BOOL	m_keep_cspar;
	BOOL	m_waddr_auto_sel;
	BOOL	m_read_soft_ver;
	BOOL	m_mcu_io_chk;
	BOOL	m_mf_evm_chk;
	BOOL	m_lf_evm_chk;
	BOOL	m_hf_evm_chk;
	BOOL	m_8711_io_chk;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(config_deal)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(config_deal)
	virtual void OnOK();
	afx_msg void OnInputCfj();
	afx_msg void OnOutputCfj();
	virtual BOOL OnInitDialog();
	afx_msg void OnTxWriteUid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIG_DEAL_H__C1A6C824_48DD_49FC_999D_D491D1E4D8BD__INCLUDED_)
