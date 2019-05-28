#if !defined(AFX_RFPWR_LOSS_CAL_H__EBD658DD_E968_44DD_90E3_D207F6524B49__INCLUDED_)
#define AFX_RFPWR_LOSS_CAL_H__EBD658DD_E968_44DD_90E3_D207F6524B49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// rfpwr_loss_cal.h : header file
//

class ConfigFile;
class CPD218Dlg;
class ofstream;
class CDutDevice;
class testSet;


/////////////////////////////////////////////////////////////////////////////
// rfpwr_loss_cal dialog

class rfpwr_loss_cal : public CDialog
{
// Construction
public:
	rfpwr_loss_cal(CWnd* pParent = NULL);   // standard constructor


public:
	CFont m_prompt_font;
	CFont m_button_font;
	
	HANDLE          hthread;
	DWORD           hthreadId;
	
    //bool            rf_test_mode;
	
	CString		err_str;
	testSet 	*tset;
	CsrHandle_t     iHandle;
	int             rf_test_idx;
	int             golden_idx;
	
	double APathOffset[3];
	double BPathOffset[3];
	
	void result_adjust(struct tSetResult &mt8852_res,double *pathOffset);
	bool do_4010_test(int test_golden_idx);
	bool dut_enter_rfmode(int test_golden_idx);

	void rf_pwr_test_finish(void);
	void show_prompt(CString str);

	//input
	char last_keyboard_input;
    int  last_keyboard_input_len;
    bool adev_address_inputed;
    bool bdev_address_inputed;
    CString adev_input_addr;
    CString bdev_input_addr;
	CFont m_CurEditfont;

	//handle
	//void OnSetfocusAgoldAddress();
	//void OnSetfocusBgoldAddress();
	void setFocus(int idc,int idc_skip = 0);

// Dialog Data
	//{{AFX_DATA(rfpwr_loss_cal)
	enum { IDD = IDD_RFPWR_LOSS };
	CButton	m_cancel_button;
	CButton	m_ok_button;
	CButton	m_rfpwr_button;
	CString	m_agold_mac;
	CString	m_bgold_mac;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(rfpwr_loss_cal)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(rfpwr_loss_cal)
	afx_msg void OnDoPwrcal();
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnSetfocusAgoldAddress();
	afx_msg void OnSetfocusBgoldAddress();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RFPWR_LOSS_CAL_H__EBD658DD_E968_44DD_90E3_D207F6524B49__INCLUDED_)
