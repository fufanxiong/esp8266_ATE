#if !defined(AFX_CONFIG_SET_H__A4E426FE_6AB4_4411_B2B8_32F7E611D1F4__INCLUDED_)
#define AFX_CONFIG_SET_H__A4E426FE_6AB4_4411_B2B8_32F7E611D1F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// config_set.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// config_set dialog
//

class config_set : public CDialog
{
private:
	void UpDataToFile(ConfigFile &ConfigF);
	void UpDataFromFile(ConfigFile &ConfigF);

// Construction
public:
	config_set(CWnd* pParent = NULL);   // standard constructor
	void UpDateFile(ConfigFile &ConfigF,bool flag = true);
// Dialog Data
	//{{AFX_DATA(config_set)
	enum { IDD = IDD_SETTING };
	BOOL	m_w_mac_addr_sel;
	BOOL	m_show_mac_addr_sel;
	BOOL	m_rf_send_test_sel;
	BOOL	m_rf_recv_test_sel;
	BOOL	m_pwr_cvr_sel;
	BOOL	m_rf_power_test_sel;
	BOOL	m_pio_chk_sel;
	BOOL	m_rf_frequency_test_sel;
	//BOOL	m_burn_software_sel;
	BOOL	m_auto_write_mac_sel;
	BOOL	m_adc_chk_sel;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(config_set)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(config_set)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnWriteMac();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIG_SET_H__A4E426FE_6AB4_4411_B2B8_32F7E611D1F4__INCLUDED_)
